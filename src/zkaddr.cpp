// Copyright (c) 2009-2010 Satoshi Nakamoto
// Copyright (c) 2009-2014 The Bitcoin Core developers
// Copyright (c) 2016-2017 The Zcash Developers
// Distributed under the MIT software license, see the accompanying
// file COPYING or http://www.opensource.org/licenses/mit-license.php.

#include <streams.h>
#include <zkaddr.h>

template<class DATA_TYPE, CChainParams::Base58Type PREFIX, size_t SER_SIZE>
bool CZCEncoding<DATA_TYPE, PREFIX, SER_SIZE>::Set(const DATA_TYPE& addr)
{
    CDataStream ss(SER_NETWORK, PROTOCOL_VERSION);
    ss << addr;
    std::vector<unsigned char> addrSerialized(ss.begin(), ss.end());
    assert(addrSerialized.size() == SER_SIZE);
    SetData(Params().Base58Prefix(PREFIX), &addrSerialized[0], SER_SIZE);
    return true;
}

template<class DATA_TYPE, CChainParams::Base58Type PREFIX, size_t SER_SIZE>
DATA_TYPE CZCEncoding<DATA_TYPE, PREFIX, SER_SIZE>::Get() const
{
    if (vchData.size() != SER_SIZE) {
        throw std::runtime_error(
            PrependName(" is invalid")
            );
    }

    if (vchVersion != Params().Base58Prefix(PREFIX)) {
        throw std::runtime_error(
            PrependName(" is for wrong network type")
            );
    }

    std::vector<unsigned char> serialized(vchData.begin(), vchData.end());

    CDataStream ss(serialized, SER_NETWORK, PROTOCOL_VERSION);
    DATA_TYPE ret;
    ss >> ret;
    return ret;
}

// Explicit instantiations for libzcash::PaymentAddress
template bool CZCEncoding<libzcash::PaymentAddress,
                          CChainParams::ZCPAYMENT_ADDRRESS,
                          libzcash::SerializedPaymentAddressSize>::Set(const libzcash::PaymentAddress& addr);
template libzcash::PaymentAddress CZCEncoding<libzcash::PaymentAddress,
                                              CChainParams::ZCPAYMENT_ADDRRESS,
                                              libzcash::SerializedPaymentAddressSize>::Get() const;

// Explicit instantiations for libzcash::ViewingKey
template bool CZCEncoding<libzcash::ViewingKey,
                          CChainParams::ZCVIEWING_KEY,
                          libzcash::SerializedViewingKeySize>::Set(const libzcash::ViewingKey& vk);
template libzcash::ViewingKey CZCEncoding<libzcash::ViewingKey,
                                          CChainParams::ZCVIEWING_KEY,
                                          libzcash::SerializedViewingKeySize>::Get() const;

// Explicit instantiations for libzcash::SpendingKey
template bool CZCEncoding<libzcash::SpendingKey,
                          CChainParams::ZCSPENDING_KEY,
                          libzcash::SerializedSpendingKeySize>::Set(const libzcash::SpendingKey& sk);
template libzcash::SpendingKey CZCEncoding<libzcash::SpendingKey,
                                           CChainParams::ZCSPENDING_KEY,
                                           libzcash::SerializedSpendingKeySize>::Get() const;
