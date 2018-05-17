// Copyright (c) 2018      The Bitcoin Private developers
// Distributed under the MIT software license, see the accompanying
// file COPYING or http://www.opensource.org/licenses/mit-license.php.

#ifndef BTCP_CONSENSUS_JOINSPLIT_H
#define BTCP_CONSENSUS_JOINSPLIT_H

bool CheckTransactionJoinsplits(const CTransaction& tx, CValidationState &state);

#endif
