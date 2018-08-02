// Copyright (c) 2009-2010 Satoshi Nakamoto
// Copyright (c) 2009-2014 The Bitcoin Core developers
// Distributed under the MIT software license, see the accompanying
// file COPYING or http://www.opensource.org/licenses/mit-license.php.

#ifndef BITCOIN_WALLET_JOINSPLIT_H
#define BITCOIN_WALLET_JOINSPLIT_H

#include <uint256.h>
#include <zcash/Address.hpp>

/** A note outpoint */
class JSOutPoint
{
public:
    // Transaction hash
    uint256 hash;
    // Index into CTransaction.vjoinsplit
    uint64_t js;
    // Index into JSDescription fields of length ZC_NUM_JS_OUTPUTS
    uint8_t n;

    JSOutPoint() { SetNull(); }
JSOutPoint(uint256 h, uint64_t js, uint8_t n) : hash {h}, js {js}, n {n} { }

    ADD_SERIALIZE_METHODS;

    template <typename Stream, typename Operation>
        inline void SerializationOp(Stream& s, Operation ser_action, int nType, int nVersion) {
        READWRITE(hash);
        READWRITE(js);
        READWRITE(n);
    }

    void SetNull() { hash.SetNull(); }
    bool IsNull() const { return hash.IsNull(); }

    friend bool operator<(const JSOutPoint& a, const JSOutPoint& b) {
        return (a.hash < b.hash ||
                (a.hash == b.hash && a.js < b.js) ||
                (a.hash == b.hash && a.js == b.js && a.n < b.n));
    }

    friend bool operator==(const JSOutPoint& a, const JSOutPoint& b) {
        return (a.hash == b.hash && a.js == b.js && a.n == b.n);
    }

    friend bool operator!=(const JSOutPoint& a, const JSOutPoint& b) {
        return !(a == b);
    }

    std::string ToString() const;
};

class CNoteData
{
public:
    libzcash::PaymentAddress address;

    /**
     * Cached note nullifier. May not be set if the wallet was not unlocked when
     * this was CNoteData was created. If not set, we always assume that the
     * note has not been spent.
     *
     * It's okay to cache the nullifier in the wallet, because we are storing
     * the spending key there too, which could be used to derive this.
     * If the wallet is encrypted, this means that someone with access to the
     * locked wallet cannot spend notes, but can connect received notes to the
     * transactions they are spent in. This is the same security semantics as
     * for transparent addresses.
     */
    boost::optional<uint256> nullifier;

    /**
     * Cached incremental witnesses for spendable Notes.
     * Beginning of the list is the most recent witness.
     */
    std::list<ZCIncrementalWitness> witnesses;

    /**
     * Block height corresponding to the most current witness.
     *
     * When we first create a CNoteData in CWallet::FindMyNotes, this is set to
     * -1 as a placeholder. The next time CWallet::ChainTip is called, we can
     * determine what height the witness cache for this note is valid for (even
     * if no witnesses were cached), and so can set the correct value in
     * CWallet::IncrementNoteWitnesses and CWallet::DecrementNoteWitnesses.
     */
    int witnessHeight;

CNoteData() : address(), nullifier(), witnessHeight {-1} { }
CNoteData(libzcash::PaymentAddress a) :
    address {a}, nullifier(), witnessHeight {-1} { }
CNoteData(libzcash::PaymentAddress a, uint256 n) :
    address {a}, nullifier {n}, witnessHeight {-1} { }

    ADD_SERIALIZE_METHODS;

    template <typename Stream, typename Operation>
        inline void SerializationOp(Stream& s, Operation ser_action, int nType, int nVersion) {
        READWRITE(address);
        READWRITE(nullifier);
        READWRITE(witnesses);
        READWRITE(witnessHeight);
    }

    friend bool operator<(const CNoteData& a, const CNoteData& b) {
        return (a.address < b.address ||
                (a.address == b.address && a.nullifier < b.nullifier));
    }

    friend bool operator==(const CNoteData& a, const CNoteData& b) {
        return (a.address == b.address && a.nullifier == b.nullifier);
    }

    friend bool operator!=(const CNoteData& a, const CNoteData& b) {
        return !(a == b);
    }
};

typedef std::map<JSOutPoint, CNoteData> mapNoteData_t;

/** Decrypted note and its location in a transaction. */
struct CNotePlaintextEntry
{
    JSOutPoint jsop;
    libzcash::PaymentAddress address;
    libzcash::NotePlaintext plaintext;
};

/** Decrypted note, location in a transaction, and confirmation height. */
struct CUnspentNotePlaintextEntry {
    JSOutPoint jsop;
    libzcash::PaymentAddress address;
    libzcash::NotePlaintext plaintext;
    int nHeight;
};

#endif // BITCOIN_WALLET_JOINSPLIT_H
