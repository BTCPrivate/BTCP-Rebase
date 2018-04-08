// Copyright (c) 2009-2010 Satoshi Nakamoto
// Copyright (c) 2009-2017 The Bitcoin Core developers
// Distributed under the MIT software license, see the accompanying
// file COPYING or http://www.opensource.org/licenses/mit-license.php.

#include <pow.h>

#include <arith_uint256.h>
#include <chain.h>
#include <chainparams.h>
#include <fork.h>
#include <primitives/block.h>
#include <uint256.h>

unsigned int GetNextWorkRequired(const CBlockIndex* pindexLast, const CBlockHeader *pblock, const CChainParams& chainparams)
{
    assert(pindexLast != nullptr);

    const Consensus::Params& params = chainparams.GetConsensus();
    int nHeight = pindexLast->nHeight + 1;

    arith_uint256 proofOfWorkLimit;
    if(!isForkEnabled(nHeight, chainparams.ForkStartHeight()))
        proofOfWorkLimit = UintToArith256(params.prePowLimit);
    else
        proofOfWorkLimit = UintToArith256(params.powLimit);

    unsigned int nProofOfWorkLimit = proofOfWorkLimit.GetCompact();
    unsigned int nProofOfWorkBomb  = UintToArith256(uint256S("000000000000000000000000000000000000000000000000000000000000ffff")).GetCompact();

    bool is_fork = isForkBlock(nHeight, chainparams.ForkStartHeight(), chainparams.ForkHeightRange());
    bool was_fork = isForkBlock(nHeight - params.nPowAveragingWindow, chainparams.ForkStartHeight(), chainparams.ForkHeightRange());

    // Genesis block
    if (pindexLast == NULL)
        return nProofOfWorkLimit;

    // right at fork
    else if(is_fork && !was_fork)
        return nProofOfWorkLimit;

    // right post fork
    else if(!is_fork && was_fork)
        return nProofOfWorkLimit;

    // difficulty bomb
    else if(pindexLast->nHeight > params.nPowDifficultyBombHeight)
        return nProofOfWorkBomb;

    // Find the first block in the averaging interval
    const CBlockIndex* pindexFirst = pindexLast;
    arith_uint256 bnTot {0};
    for (int i = 0; pindexFirst && i < params.nPowAveragingWindow; i++) {
        arith_uint256 bnTmp;
        bnTmp.SetCompact(pindexFirst->nBits);
        bnTot += bnTmp;
        pindexFirst = pindexFirst->pprev;
    }

    // Check we have enough blocks
    if (pindexFirst == NULL)
        return nProofOfWorkLimit;

    arith_uint256 bnAvg {bnTot / params.nPowAveragingWindow};

    bool isFork = isForkBlock(pindexLast->nHeight + 1, chainparams.ForkStartHeight(), chainparams.ForkHeightRange());
    return CalculateNextWorkRequired(bnAvg, pindexLast->GetMedianTimePast(), pindexFirst->GetMedianTimePast(), params, proofOfWorkLimit, isFork);
}

unsigned int CalculateNextWorkRequired(arith_uint256 bnAvg,
                                       int64_t nLastBlockTime, int64_t nFirstBlockTime,
                                       const Consensus::Params& params, const arith_uint256 bnPowLimit, bool isFork)
{
    // Limit adjustment step
    // Use medians to prevent time-warp attacks
    int64_t nActualTimespan = nLastBlockTime - nFirstBlockTime;
    nActualTimespan = params.AveragingWindowTimespan(isFork) + (nActualTimespan - params.AveragingWindowTimespan(isFork))/4;

    if (nActualTimespan < params.MinActualTimespan(isFork))
        nActualTimespan = params.MinActualTimespan(isFork);
    if (nActualTimespan > params.MaxActualTimespan(isFork))
        nActualTimespan = params.MaxActualTimespan(isFork);

    // Retarget
    arith_uint256 bnNew {bnAvg};
    bnNew /= params.AveragingWindowTimespan(isFork);
    bnNew *= nActualTimespan;

    if (bnNew > bnPowLimit)
        bnNew = bnPowLimit;

    return bnNew.GetCompact();
}

bool CheckProofOfWork(uint256 hash, unsigned int nBits, const Consensus::Params& params)
{
    bool fNegative;
    bool fOverflow;
    arith_uint256 bnTarget;

    bnTarget.SetCompact(nBits, &fNegative, &fOverflow);

    // Check range
    if (fNegative || bnTarget == 0 || fOverflow || bnTarget > UintToArith256(params.powLimit))
        return false;

    // Check proof of work matches claimed amount
    if (UintToArith256(hash) > bnTarget)
        return false;

    return true;
}
