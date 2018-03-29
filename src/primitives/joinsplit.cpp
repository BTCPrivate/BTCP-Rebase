// Copyright (c) 2009-2010 Satoshi Nakamoto
// Copyright (c) 2009-2014 The Bitcoin Core developers
// Distributed under the MIT software license, see the accompanying
// file COPYING or http://www.opensource.org/licenses/mit-license.php.

#include "primitives/transaction.h"

#include "hash.h"
#include "tinyformat.h"
#include "utilstrencodings.h"

JSDescription::JSDescription(ZCJoinSplit& params,
                             const uint256& pubKeyHash,
                             const uint256& anchor,
                             const boost::array<libzcash::JSInput, ZC_NUM_JS_INPUTS>& inputs,
                             const boost::array<libzcash::JSOutput, ZC_NUM_JS_OUTPUTS>& outputs,
                             CAmount vpub_old,
                             CAmount vpub_new,
                             bool computeProof) : vpub_old(vpub_old), vpub_new(vpub_new), anchor(anchor)
{
    boost::array<libzcash::Note, ZC_NUM_JS_OUTPUTS> notes;

    if (computeProof) {
        params.loadProvingKey();
    }
    proof = params.prove(
        inputs,
        outputs,
        notes,
        ciphertexts,
        ephemeralKey,
        pubKeyHash,
        randomSeed,
        macs,
        nullifiers,
        commitments,
        vpub_old,
        vpub_new,
        anchor,
        computeProof
        );
}

JSDescription JSDescription::Randomized(
    ZCJoinSplit& params,
    const uint256& pubKeyHash,
    const uint256& anchor,
    boost::array<libzcash::JSInput, ZC_NUM_JS_INPUTS>& inputs,
    boost::array<libzcash::JSOutput, ZC_NUM_JS_OUTPUTS>& outputs,
#ifdef __LP64__ // required to build on MacOS due to size_t ambiguity errors
    boost::array<uint64_t, ZC_NUM_JS_INPUTS>& inputMap,
    boost::array<uint64_t, ZC_NUM_JS_OUTPUTS>& outputMap,
#else
    boost::array<size_t, ZC_NUM_JS_INPUTS>& inputMap,
    boost::array<size_t, ZC_NUM_JS_OUTPUTS>& outputMap,
#endif
    CAmount vpub_old,
    CAmount vpub_new,
    bool computeProof,
    std::function<int(int)> gen)
{
    // Randomize the order of the inputs and outputs
    inputMap = {0, 1};
    outputMap = {0, 1};

    assert(gen);

    MappedShuffle(inputs.begin(), inputMap.begin(), ZC_NUM_JS_INPUTS, gen);
    MappedShuffle(outputs.begin(), outputMap.begin(), ZC_NUM_JS_OUTPUTS, gen);

    return JSDescription(
        params, pubKeyHash, anchor, inputs, outputs,
        vpub_old, vpub_new, computeProof);
}

bool JSDescription::Verify(
    ZCJoinSplit& params,
    libzcash::ProofVerifier& verifier,
    const uint256& pubKeyHash
    ) const {
    return params.verify(
        proof,
        verifier,
        pubKeyHash,
        randomSeed,
        macs,
        nullifiers,
        commitments,
        vpub_old,
        vpub_new,
        anchor
        );
}

uint256 JSDescription::h_sig(ZCJoinSplit& params, const uint256& pubKeyHash) const
{
    return params.h_sig(randomSeed, nullifiers, pubKeyHash);
}
