// Copyright (c) 2018 The Bitcoin Private developers
// Distributed under the MIT software license, see the accompanying
// file COPYING or http://www.opensource.org/licenses/mit-license.php.

#ifndef BITCOIN_CONSENSUS_JOINSPLIT_H
#define BITCOIN_CONSENSUS_JOINSPLIT_H

#include <primitives/transaction.h>
#include <consensus/validation.h>

bool CheckTransactionJoinSplits(const CTransaction& tx, CValidationState& state, unsigned int flags);

#endif // BITCOIN_CONSENSUS_JOINSPLIT_H
