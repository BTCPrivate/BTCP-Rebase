// Copyright (c) 2018 The Bitcoin Private Developers
// Distributed under the MIT software license, see the accompanying
// file COPYING or http://www.opensource.org/licenses/mit-license.php.

#ifndef BTCP_CONSENSUS_REORG_H
#define BTCP_CONSENSUS_REORG_H

/** Maximum reorg length we will accept before we shut down and alert the user. */
/** Note: Not a true consensus parameter, just a network-wide default. */
static const unsigned int MAX_REORG_LENGTH = COINBASE_MATURITY - 1;

#endif