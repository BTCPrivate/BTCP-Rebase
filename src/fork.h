// Copyright (c) 2018 The Bitcoin Private developers
// Distributed under the MIT software license, see the accompanying
// file COPYING or http://www.opensource.org/licenses/mit-license.php.

#ifndef BITCOIN_FORK_H
#define BITCOIN_FORK_H

#include <consensus/validation.h>
#include <primitives/block.h>

static const uint256 forkExtraHashSentinel = uint256S("f0f0f0f0fafafafaffffffffffffffffffffffffffffffffafafafaf0f0f0f0f");
static constexpr unsigned int FORK_CB_PER_BLOCK = 10000;

inline bool isForkBlock(int nHeight, int forkStartHeight, int forkHeightRange)
{
    return (nHeight > forkStartHeight && nHeight <= forkStartHeight + forkHeightRange);
}

inline bool looksLikeForkBlockHeader(const CBlockHeader& header)
{
    return header.hashReserved == forkExtraHashSentinel;
}

inline bool isForkEnabled(int nHeight, int forkStartHeight)
{
    return nHeight > forkStartHeight;
}

bool ContextualCheckBlockFork(const CBlock& block, CValidationState& state,
                              const CChainParams& chainparams, const CBlockIndex* pindex);

std::string GetUTXOFileName(int);

#endif // BITCOIN_FORK_H
