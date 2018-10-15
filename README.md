Bitcoin Private Rebase
=====================================

IN DEVELOPMENT - FOR TESTING ONLY - DO NOT USE IN PRODUCTION
===========
This is currently in development software, not all consensus rules have been implemented. **You should NOT rely on this in production yet**, for production use cases please use https://github.com/BTCPrivate/BitcoinPrivate/

Development Status
------
[![Build Status](https://travis-ci.com/BTCPrivate/BTCP-Rebase.svg?branch=master)](https://travis-ci.com/BTCPrivate/BTCP-Rebase)

This should build on linux and sync the full BTCP mainnet chain. Wallet code and test suite is not fully working yet.

Build Instructions
-------

#### Linux (>= Ubuntu 16.04):

```
sudo apt-get install build-essential libtool autotools-dev automake pkg-config libssl-dev libevent-dev bsdmainutils python3 libboost-system-dev libboost-filesystem-dev libboost-chrono-dev libboost-test-dev libboost-thread-dev
```

```
cd scripts
./build.sh
```

Or alternatively:
```
./autogen.sh
cd depends
make HOST=x86_64-unknown-linux-gnu
cd ..
./configure --prefix=$PWD/depends/x86_64-unknown-linux-gnu
make
```

This will build all the dependencies and bootstrap a linux build. There are several options you can use:
- `--disable-wallet`
- `--without-gui`
- `--help`


#### MacOS (High Sierra):
You must first install libomp (Apple's version of Clang does not have support for OpenMP included); you can do that via the following Terminal commands:

Install XCode:
```
xcode-select --install
```
Install Homebrew:
```
/usr/bin/ruby -e "$(curl -fsSL https://raw.githubusercontent.com/Homebrew/install/master/install)"
```
Install libomp:
```
brew install libomp
```

Then, run `scripts/build-mac.sh` to compile.


What is the Bitcoin Private Rebase?
----------------

Bitcoin Private is currently based primarily on the Zcash codebase, which itself is a snapshot of Bitcoin dating to mid-2015. Most z protocol coins all rely on this same code, which is now missing roughly 3 years of blockchain upgrades that have been integrated into Bitcoin. Based on the structure of Zcash, and because there’s now a 3 year code divergence, integrating blockchain upgrades developed in bitcoin core into any z protocol coin has become increasingly cumbersome.

We endeavor a complete refactoring of Bitcoin Private to allow for more facile incorporation of Bitcoin upstream changes. This release will coincide with activation of full SegWit support, giving the BTCP more scale both on chain as well as enabling off-chain protocols such as the lightning network and cross-chain swaps. Additionally, it will bring another 3 years of upgrades to the core node including stability and speed improvements as well as a number of usability enhancements to the reference wallet including support for hierarchical deterministic wallets (for shielded and transparent addresses) and wallet encryption.

Learn more about the rebase at https://btcprivate.org/roadmap.

License
-------

Bitcoin Private is released under the terms of the MIT license. See [COPYING](COPYING) for more
information or see https://opensource.org/licenses/MIT.
