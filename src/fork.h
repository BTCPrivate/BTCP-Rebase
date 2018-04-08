// Copyright (c) 2009-2010 Satoshi Nakamoto
// Copyright (c) 2009-2014 The Bitcoin Core developers
// Copyright (c) 2018      The Bitcoin Private developers
// Distributed under the MIT software license, see the accompanying
// file COPYING or http://www.opensource.org/licenses/mit-license.php.

#ifndef BTCP_FORK_H
#define BTCP_FORK_H

static const uint256 forkExtraHashSentinel = uint256S("f0f0f0f0fafafafaffffffffffffffffffffffffffffffffafafafaf0f0f0f0f");

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

#endif
