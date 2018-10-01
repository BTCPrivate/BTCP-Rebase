// Copyright (c) 2012-2018 The Bitcoin Core developers
// Distributed under the MIT software license, see the accompanying
// file COPYING or http://www.opensource.org/licenses/mit-license.php.

#ifndef BITCOIN_VERSION_H
#define BITCOIN_VERSION_H

/**
 * network protocol versioning
 */

static const int PROTOCOL_VERSION = 180104;

//! initial proto version, to be increased after version/verack negotiation
static const int INIT_PROTO_VERSION = 209;
static const int FORK_PROTO_VERSION = 180003;
static const int REFACTORED_PROTO_VERSION = 180104;

//! In this version, 'getheaders' was introduced.
static const int GETHEADERS_VERSION = FORK_PROTO_VERSION;

//! disconnect from peers older than this proto version
static const int MIN_PEER_PROTO_VERSION = FORK_PROTO_VERSION;

//! nTime field added to CAddress, starting with this version;
//! if possible, avoid requesting addresses nodes older than this
static const int CADDR_TIME_VERSION = FORK_PROTO_VERSION;

//! BIP 0031, pong message, is enabled for all versions AFTER this one
static const int BIP0031_VERSION = FORK_PROTO_VERSION - 1;

//! "filter*" commands are disabled without NODE_BLOOM after and including this version
static const int NO_BLOOM_VERSION = REFACTORED_PROTO_VERSION;

//! "sendheaders" command and announcing blocks with headers starts with this version
static const int SENDHEADERS_VERSION = REFACTORED_PROTO_VERSION;

//! "feefilter" tells peers to filter invs to you by fee starts with this version
static const int FEEFILTER_VERSION = REFACTORED_PROTO_VERSION;

//! short-id-based block download starts with this version
static const int SHORT_IDS_BLOCKS_VERSION = REFACTORED_PROTO_VERSION;

//! not banning for invalid compact blocks starts with this version
static const int INVALID_CB_NO_BAN_VERSION = REFACTORED_PROTO_VERSION;

#endif // BITCOIN_VERSION_H
