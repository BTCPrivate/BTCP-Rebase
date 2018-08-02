// Copyright (c) 2009-2010 Satoshi Nakamoto
// Copyright (c) 2009-2014 The Bitcoin Core developers
// Copyright (c) 2018      The Bitcoin Private developers
// Distributed under the MIT software license, see the accompanying
// file COPYING or http://www.opensource.org/licenses/mit-license.php.

#ifndef BITCOIN_PRIMITIVES_JOINSPLIT_H
#define BITCOIN_PRIMITIVES_JOINSPLIT_H

#include <amount.h>
#include <random.h>
#include <script/script.h>
#include <serialize.h>
#include <uint256.h>
#include <consensus/consensus.h>

#include <zcash/NoteEncryption.hpp>
#include <zcash/Zcash.h>
#include <zcash/JoinSplit.hpp>
#include <zcash/Proof.hpp>

#include <array>

class JSDescription
{
public:
    // These values 'enter from' and 'exit to' the value
    // pool, respectively.
    CAmount vpub_old;
    CAmount vpub_new;

    // JoinSplits are always anchored to a root in the note
    // commitment tree at some point in the blockchain
    // history or in the history of the current
    // transaction.
    uint256 anchor;

    // Nullifiers are used to prevent double-spends. They
    // are derived from the secrets placed in the note
    // and the secret spend-authority key known by the
    // spender.
    std::array<uint256, ZC_NUM_JS_INPUTS> nullifiers;

    // Note commitments are introduced into the commitment
    // tree, blinding the public about the values and
    // destinations involved in the JoinSplit. The presence of
    // a commitment in the note commitment tree is required
    // to spend it.
    std::array<uint256, ZC_NUM_JS_OUTPUTS> commitments;

    // Ephemeral key
    uint256 ephemeralKey;

    // Ciphertexts
    // These contain trapdoors, values and other information
    // that the recipient needs, including a memo field. It
    // is encrypted using the scheme implemented in crypto/NoteEncryption.cpp
    std::array<ZCNoteEncryption::Ciphertext, ZC_NUM_JS_OUTPUTS> ciphertexts = {{ {{0}} }};

    // Random seed
    uint256 randomSeed;

    // MACs
    // The verification of the JoinSplit requires these MACs
    // to be provided as an input.
    std::array<uint256, ZC_NUM_JS_INPUTS> macs;

    // JoinSplit proof
    // This is a zk-SNARK which ensures that this JoinSplit is valid.
    libzcash::ZCProof proof;

    JSDescription(): vpub_old(0), vpub_new(0) { }

    JSDescription(ZCJoinSplit& params,
                  const uint256& pubKeyHash,
                  const uint256& rt,
                  const std::array<libzcash::JSInput, ZC_NUM_JS_INPUTS>& inputs,
                  const std::array<libzcash::JSOutput, ZC_NUM_JS_OUTPUTS>& outputs,
                  CAmount vpub_old,
                  CAmount vpub_new,
                  bool computeProof = true, // Set to false in some tests
                  uint256 *esk = nullptr // payment disclosure
        );

    static JSDescription Randomized(
        ZCJoinSplit& params,
        const uint256& pubKeyHash,
        const uint256& rt,
        std::array<libzcash::JSInput, ZC_NUM_JS_INPUTS>& inputs,
        std::array<libzcash::JSOutput, ZC_NUM_JS_OUTPUTS>& outputs,
        std::array<uint64_t, ZC_NUM_JS_INPUTS>& inputMap,
        std::array<uint64_t, ZC_NUM_JS_OUTPUTS>& outputMap,
        CAmount vpub_old,
        CAmount vpub_new,
        bool computeProof = true, // Set to false in some tests
        uint256 *esk = nullptr, // payment disclosure
        std::function<int(int)> gen = GetRandInt
        );

    // Verifies that the JoinSplit proof is correct.
    bool Verify(
        ZCJoinSplit& params,
        libzcash::ProofVerifier& verifier,
        const uint256& pubKeyHash
        ) const;

    // Returns the calculated h_sig
    uint256 h_sig(ZCJoinSplit& params, const uint256& pubKeyHash) const;

    ADD_SERIALIZE_METHODS;

    template <typename Stream, typename Operation>
        inline void SerializationOp(Stream& s, Operation ser_action) {
        READWRITE(vpub_old);
        READWRITE(vpub_new);
        READWRITE(anchor);
        READWRITE(nullifiers);
        READWRITE(commitments);
        READWRITE(ephemeralKey);
        READWRITE(randomSeed);
        READWRITE(macs);
        READWRITE(proof);
        READWRITE(ciphertexts);
    }

    friend bool operator==(const JSDescription& a, const JSDescription& b)
    {
        return (
            a.vpub_old == b.vpub_old &&
            a.vpub_new == b.vpub_new &&
            a.anchor == b.anchor &&
            a.nullifiers == b.nullifiers &&
            a.commitments == b.commitments &&
            a.ephemeralKey == b.ephemeralKey &&
            a.ciphertexts == b.ciphertexts &&
            a.randomSeed == b.randomSeed &&
            a.macs == b.macs &&
            a.proof == b.proof
            );
    }

    friend bool operator!=(const JSDescription& a, const JSDescription& b)
    {
        return !(a == b);
    }
};

#endif // BITCOIN_PRIMITIVES_JOINSPLIT_H
