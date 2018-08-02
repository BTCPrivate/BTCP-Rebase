// Copyright (c) 2009-2010 Satoshi Nakamoto
// Copyright (c) 2009-2014 The Bitcoin Core developers
// Copyright (c) 2016-2017 The Zcash Developers
// Distributed under the MIT software license, see the accompanying
// file COPYING or http://www.opensource.org/licenses/mit-license.php.

#ifndef BITCOIN_ZKADDR_H
#define BITCOIN_ZKADDR_H

#include <base58.h>
#include <chainparams.h>
#include <zcash/Address.hpp>

template<class DATA_TYPE, CChainParams::Base58Type PREFIX, size_t SER_SIZE>
    class CZCEncoding : public CBase58Data {
protected:
    virtual std::string PrependName(const std::string& s) const = 0;

public:
    bool Set(const DATA_TYPE& addr);

    DATA_TYPE Get() const;
};

class CZCPaymentAddress : public CZCEncoding<libzcash::PaymentAddress, CChainParams::ZCPAYMENT_ADDRRESS, libzcash::SerializedPaymentAddressSize> {
protected:
    std::string PrependName(const std::string& s) const { return "payment address" + s; }

public:
    CZCPaymentAddress() {}

    CZCPaymentAddress(const std::string& strAddress) { SetString(strAddress.c_str(), 2); }
    CZCPaymentAddress(const libzcash::PaymentAddress& addr) { Set(addr); }
};

class CZCViewingKey : public CZCEncoding<libzcash::ViewingKey, CChainParams::ZCVIEWING_KEY, libzcash::SerializedViewingKeySize> {
protected:
    std::string PrependName(const std::string& s) const { return "viewing key" + s; }

public:
    CZCViewingKey() {}

    CZCViewingKey(const std::string& strViewingKey) { SetString(strViewingKey.c_str(), 3); }
    CZCViewingKey(const libzcash::ViewingKey& vk) { Set(vk); }
};

class CZCSpendingKey : public CZCEncoding<libzcash::SpendingKey, CChainParams::ZCSPENDING_KEY, libzcash::SerializedSpendingKeySize> {
protected:
    std::string PrependName(const std::string& s) const { return "spending key" + s; }

public:
    CZCSpendingKey() {}

    CZCSpendingKey(const std::string& strAddress) { SetString(strAddress.c_str(), 2); }
    CZCSpendingKey(const libzcash::SpendingKey& addr) { Set(addr); }
};

#endif // BITCOIN_ZKADDR_H
