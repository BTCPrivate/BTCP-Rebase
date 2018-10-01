// Copyright (c) 2009-2010 Satoshi Nakamoto
// Copyright (c) 2009-2014 The Bitcoin Core developers
// Distributed under the MIT software license, see the accompanying
// file COPYING or http://www.opensource.org/licenses/mit-license.php.

#include <ios>
#include <sstream>
#include <utility>

#include <boost/tuple/tuple.hpp>
#include <boost/range/combine.hpp>

#include <chain.h>
#include <chainparams.h>
#include <checkpoints.h>
#include <script/script.h>
#include <fork.h>
#include <util.h>
#include <validation.h>

std::string GetUTXOFileName(int nHeight, const CChainParams& chainparams)
{
    boost::filesystem::path utxo_path(GetDataDir() / "utxo_snapshot");
    if (utxo_path.empty() || !utxo_path.has_filename())
    {
        LogPrintf("GetUTXOFileName(): UTXO path is not specified, add utxo-path=<path-to-utxo-files> to your bitcoin.conf and restart\n");
        return "";
    }

    std::stringstream ss;
    ss << strprintf("utxo-%05d.bin", nHeight - chainparams.ForkStartHeight());
    boost::filesystem::path utxo_file = utxo_path;
    utxo_file /= ss.str();

    return utxo_file.generic_string();
}

inline uint64_t bytes2uint64(char* array)
{
    uint64_t x =
        static_cast<uint64_t>(array[0])       & 0x00000000000000ff |
        static_cast<uint64_t>(array[1]) << 8  & 0x000000000000ff00 |
        static_cast<uint64_t>(array[2]) << 16 & 0x0000000000ff0000 |
        static_cast<uint64_t>(array[3]) << 24 & 0x00000000ff000000 |
        static_cast<uint64_t>(array[4]) << 32 & 0x000000ff00000000 |
        static_cast<uint64_t>(array[5]) << 40 & 0x0000ff0000000000 |
        static_cast<uint64_t>(array[6]) << 48 & 0x00ff000000000000 |
        static_cast<uint64_t>(array[7]) << 56 & 0xff00000000000000;
    return x;
}

bool ContextualCheckBlockFork(const CBlock& block, CValidationState& state,
                              const CChainParams& chainparams, const CBlockIndex* pindexprev)
{
    bool fExpensiveChecks = true;
    if (fCheckpointsEnabled) {
        CBlockIndex* pindexLastCheckpoint = Checkpoints::GetLastCheckpoint(chainparams.Checkpoints());
        if (pindexLastCheckpoint && pindexLastCheckpoint->GetAncestor(pindexprev->nHeight) == pindexprev) {
            // This block is an ancestor of a checkpoint: disable script checks
            fExpensiveChecks = false;
        }
    }

    int nHeight = pindexprev->nHeight + 1;
    if (fExpensiveChecks && isForkBlock(nHeight, chainparams.ForkStartHeight(), chainparams.ForkHeightRange())) {
        // If block is in forking region, validate it against file records

        std::string utxo_file_path = GetUTXOFileName(nHeight, chainparams);
        std::ifstream if_utxo(utxo_file_path, std::ios::binary | std::ios::in);
        if (if_utxo.is_open()) {
            LogPrintf("AcceptBlock(): FORK Block - Validating block - %u / %s  with UTXO file - %s\n",
                      nHeight, block.GetHash().ToString(), utxo_file_path);

            std::vector<std::pair<uint64_t, CScript>> txFromFile;
            txFromFile.reserve(FORK_CB_PER_BLOCK);
            int recs = 0;

            while (if_utxo && recs < FORK_CB_PER_BLOCK) {
                char term = 0;
                char coin[8] = {};
                if (!if_utxo.read(coin, 8)) {
                    LogPrintf("AcceptBlock(): FORK Block - No more data in the file \n");
                    break;
                }
                uint64_t amount = bytes2uint64(coin);

                char pubkeysize[8] = {};
                if (!if_utxo.read(pubkeysize, 8)) {
                    LogPrintf("AcceptBlock(): FORK Block - UTXO file corrupted? - Not more data (PubKeyScript size)\n");
                    break;
                }
                int pbsize = bytes2uint64(pubkeysize);
                if (pbsize == 0) {
                    LogPrintf("AcceptBlock(): FORK Block - UTXO file corrupted? - Warning! PubKeyScript size = 0\n");
                    //but proceed
                }
                std::unique_ptr<char[]> pubKeyScript(new char[pbsize]);
                if (!if_utxo.read(&pubKeyScript[0], pbsize)) {
                    LogPrintf("AcceptBlock(): FORK Block - UTXO file corrupted? - Not more data (PubKeyScript)\n");
                    break;
                }
                unsigned char* pks = (unsigned char*)pubKeyScript.get();
                CScript script = CScript(pks, pks+pbsize);

                txFromFile.push_back(std::make_pair(amount, script));

                if (!if_utxo.read(&term, 1)) {
                    LogPrintf("AcceptBlock(): FORK Block - UTXO file corrupted? - No more data (record separator)\n");
                    break;
                }
                if (term != '\n') {
                    //This maybe not an error, but warning none the less
                    LogPrintf("AcceptBlock(): FORK Block - UTXO file corrupted? - Warning! No record separator ('0xA') was found\n");
                    if_utxo.seekg(-1, std::ios_base::cur); //move one char back - if it is not a separator, maybe there is not separators at all
                }
                recs++;
            }
            LogPrintf("AcceptBlock(): FORK Block - %d records read from UTXO file\n", recs);

            if (txFromFile.size() != block.vtx.size() || recs != block.vtx.size()) {
                return state.DoS(100, error("AcceptBlock(): Number of file records - %d doesn't match number of transcations in block - %d\n", recs, block.vtx.size()), REJECT_INVALID, "bad-fork-block");
            }

            int txid = 0;
            typedef boost::tuple<std::pair<uint64_t, CScript>&, const CTransaction&> fork_cmp_tuple;
            for (int i = 0; i < recs; i++) {
                std::pair<uint64_t, CScript>& rec = txFromFile[i];
                const CTransaction& tx = *block.vtx[i];

                if (rec.first != tx.vout[0].nValue ||
                    rec.second != tx.vout[0].scriptPubKey)
                {
                    LogPrintf("AcceptBlock(): FORK Block - Error: Transaction (%d) mismatch\n", txid);
                    return state.DoS(100, error("AcceptBlock(): FORK Block - Transaction (%d) doesn't match record in the UTXO file", txid), REJECT_INVALID, "bad-fork-block");
                }
            }
            txid++;
        }
    }

    return true;
}
