// Copyright (c) 2009-2010 Satoshi Nakamoto
// Copyright (c) 2009-2017 The Bitcoin Core developers
// Distributed under the MIT software license, see the accompanying
// file COPYING or http://www.opensource.org/licenses/mit-license.php.

#ifndef BITCOIN_KEYSTORE_H
#define BITCOIN_KEYSTORE_H

#include <key.h>
#include <pubkey.h>
#include <script/script.h>
#include <script/sign.h>
#include <script/standard.h>
#include <sync.h>

#include <boost/signals2/signal.hpp>

/** A virtual base class for key stores */
class CKeyStore : public SigningProvider
{
public:
    //! Add a key to the store.
    virtual bool AddKeyPubKey(const CKey &key, const CPubKey &pubkey) =0;

    //! Check whether a key corresponding to a given address is present in the store.
    virtual bool HaveKey(const CKeyID &address) const =0;
    virtual std::set<CKeyID> GetKeys() const =0;

    //! Support for BIP 0013 : see https://github.com/bitcoin/bips/blob/master/bip-0013.mediawiki
    virtual bool AddCScript(const CScript& redeemScript) =0;
    virtual bool HaveCScript(const CScriptID &hash) const =0;
    virtual std::set<CScriptID> GetCScripts() const =0;

    //! Support for Watch-only addresses
    virtual bool AddWatchOnly(const CScript &dest) =0;
    virtual bool RemoveWatchOnly(const CScript &dest) =0;
    virtual bool HaveWatchOnly(const CScript &dest) const =0;
    virtual bool HaveWatchOnly() const =0;

    //! Add a spending key to the store.
    virtual bool AddSpendingKey(const libzcash::SpendingKey &sk) =0;

    //! Check whether a spending key corresponding to a given payment address is present in the store.
    virtual bool HaveSpendingKey(const libzcash::PaymentAddress &address) const =0;
    virtual bool GetSpendingKey(const libzcash::PaymentAddress &address, libzcash::SpendingKey& skOut) const =0;
    virtual void GetPaymentAddresses(std::set<libzcash::PaymentAddress> &setAddress) const =0;
};

typedef std::map<CKeyID, CKey> KeyMap;
typedef std::map<CKeyID, CPubKey> WatchKeyMap;
typedef std::map<CScriptID, CScript > ScriptMap;
typedef std::set<CScript> WatchOnlySet;

typedef std::map<libzcash::PaymentAddress, libzcash::SpendingKey> SpendingKeyMap;
typedef std::map<libzcash::PaymentAddress, libzcash::ViewingKey> ViewingKeyMap;
typedef std::map<libzcash::PaymentAddress, ZCNoteDecryption> NoteDecryptorMap;

/** Basic key store, that keeps keys in an address->secret map */
class CBasicKeyStore : public CKeyStore
{
protected:
    mutable CCriticalSection cs_KeyStore;
    mutable CCriticalSection cs_SpendingKeyStore;

    KeyMap mapKeys;
    WatchKeyMap mapWatchKeys;
    ScriptMap mapScripts;
    WatchOnlySet setWatchOnly;
    SpendingKeyMap mapSpendingKeys;
    ViewingKeyMap mapViewingKeys;
    NoteDecryptorMap mapNoteDecryptors;

    void ImplicitlyLearnRelatedKeyScripts(const CPubKey& pubkey) EXCLUSIVE_LOCKS_REQUIRED(cs_KeyStore);

public:
    bool AddKeyPubKey(const CKey& key, const CPubKey &pubkey) override;
    bool AddKey(const CKey &key) { return AddKeyPubKey(key, key.GetPubKey()); }
    bool GetPubKey(const CKeyID &address, CPubKey& vchPubKeyOut) const override;
    bool HaveKey(const CKeyID &address) const override;
    std::set<CKeyID> GetKeys() const override;
    bool GetKey(const CKeyID &address, CKey &keyOut) const override;
    bool AddCScript(const CScript& redeemScript) override;
    bool HaveCScript(const CScriptID &hash) const override;
    std::set<CScriptID> GetCScripts() const override;
    bool GetCScript(const CScriptID &hash, CScript& redeemScriptOut) const override;

    bool AddWatchOnly(const CScript &dest) override;
    bool RemoveWatchOnly(const CScript &dest) override;
    bool HaveWatchOnly(const CScript &dest) const override;
    bool HaveWatchOnly() const override;

    bool AddSpendingKey(const libzcash::SpendingKey &sk);
    bool HaveSpendingKey(const libzcash::PaymentAddress &address) const
    {
        bool result;
        {
            LOCK(cs_SpendingKeyStore);
            result = (mapSpendingKeys.count(address) > 0);
        }
        return result;
    }
    bool GetSpendingKey(const libzcash::PaymentAddress &address, libzcash::SpendingKey &skOut) const
    {
        {
            LOCK(cs_SpendingKeyStore);
            SpendingKeyMap::const_iterator mi = mapSpendingKeys.find(address);
            if (mi != mapSpendingKeys.end())
            {
                skOut = mi->second;
                return true;
            }
        }
        return false;
    }
    bool GetNoteDecryptor(const libzcash::PaymentAddress &address, ZCNoteDecryption &decOut) const
    {
        {
            LOCK(cs_SpendingKeyStore);
            NoteDecryptorMap::const_iterator mi = mapNoteDecryptors.find(address);
            if (mi != mapNoteDecryptors.end())
            {
                decOut = mi->second;
                return true;
            }
        }
        return false;
    }
    void GetPaymentAddresses(std::set<libzcash::PaymentAddress> &setAddress) const
    {
        setAddress.clear();
        {
            LOCK(cs_SpendingKeyStore);
            SpendingKeyMap::const_iterator mi = mapSpendingKeys.begin();
            while (mi != mapSpendingKeys.end())
            {
                setAddress.insert((*mi).first);
                mi++;
            }
            ViewingKeyMap::const_iterator mvi = mapViewingKeys.begin();
            while (mvi != mapViewingKeys.end())
            {
                setAddress.insert((*mvi).first);
                mvi++;
            }
        }
    }

    virtual bool AddViewingKey(const libzcash::ViewingKey &vk);
    virtual bool RemoveViewingKey(const libzcash::ViewingKey &vk);
    virtual bool HaveViewingKey(const libzcash::PaymentAddress &address) const;
    virtual bool GetViewingKey(const libzcash::PaymentAddress &address, libzcash::ViewingKey& vkOut) const;
};

typedef std::vector<unsigned char, secure_allocator<unsigned char> > CKeyingMaterial;
typedef std::map<CKeyID, std::pair<CPubKey, std::vector<unsigned char> > > CryptedKeyMap;
typedef std::map<libzcash::PaymentAddress, std::vector<unsigned char> > CryptedSpendingKeyMap;

/** Return the CKeyID of the key involved in a script (if there is a unique one). */
CKeyID GetKeyForDestination(const CKeyStore& store, const CTxDestination& dest);

/** Checks if a CKey is in the given CKeyStore compressed or otherwise*/
bool HaveKey(const CKeyStore& store, const CKey& key);

#endif // BITCOIN_KEYSTORE_H
