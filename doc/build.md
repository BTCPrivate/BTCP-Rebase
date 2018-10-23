# Build instructions

You'll need the ZK-SNARK params from the [Zcash ceremony](https://z.cash/blog/the-design-of-the-ceremony/) before running, so let's do that now:

```
./scripts/fetch-zcash-params.sh
```

If your device is has low RAM (<1.5GB), the build may fail. `gcc` can be tuned to conserve memory with additional CXXFLAGS:

```
./configure CXXFLAGS="--param ggc-min-expand=1 --param ggc-min-heapsize=32768"
```

## Local build (using `depends/`) - Linux

For this approach, you will need to specify `--prefix` when running `./configure`:

Requirements:

```
sudo apt-get install build-essential libtool autotools-dev automake pkg-config libssl-dev libevent-dev bsdmainutils python3 libboost-system-dev libboost-filesystem-dev libboost-chrono-dev libboost-test-dev libboost-thread-dev
```

Replace `HOST` with any OS in `host-platform-triplets` ([see here](doc/https://github.com/BTCPrivate/BTCP-Rebase/tree/master/depends#usage)).

```
./autogen.sh
cd depends
make HOST=x86_64-unknown-linux-gnu
cd ..
./configure --prefix=$PWD/depends/x86_64-unknown-linux-gnu
make
```

Done!


## Global build - Linux

**Tested on Ubuntu 18**

To build + install global dependencies, we'll need a working dir: `mkdir ~/mylibs`.

You should be able to execute the following commands in order to get set up.

First,

```
sudo apt-get install build-essential libtool autotools-dev automake pkg-config libssl-dev libevent-dev bsdmainutils python3 libboost-system-dev libboost-filesystem-dev libboost-chrono-dev libboost-test-dev libboost-thread-dev wget
```

And optionally,

```
sudo apt-get install libminiupnpc-dev
sudo apt-get install libzmq3-dev
```

Now we begin the build, stepping through each of the would-be errors. The final steps will be:

```
./autogen
./configure
make
```

### Build steps + possible errors

**Step 1:** Run `./autogen`

**Step 2:** Install dependencies before we run `./configure`

Install bdb 6.2.23 globally (http://www.linuxfromscratch.org/blfs/view/8.0/server/db.html):
```
wget http://download.oracle.com/berkeley-db/db-6.2.23.tar.gz
cd build_unix                        &&
../dist/configure --prefix=/usr      \
                  --enable-compat185 \
                  --enable-dbm       \
                  --disable-static   \
                  --enable-cxx       &&
make
```

```
sudo make docdir=/usr/share/doc/db-6.2.23 install

sudo chown -v -R root:root                   \
      /usr/bin/db_*                          \
      /usr/include/db{,_185,_cxx}.h          \
      /usr/lib/libdb*.{so,la}                \
      /usr/share/doc/db-6.2.23
```
Fixes error: `configure: error: libdb_cxx headers missing`

Install libgmp globally (https://gmplib.org/download/gmp/gmp-6.1.1.tar.xz):
```
cd ~/mylibs

wget https://gmplib.org/download/gmp/gmp-6.1.1.tar.xz
tar -xvf gmp-6.1.1.tar.xz
cd gmp-6.1.1

./configure
make
make check # (Verify OK)
sudo make install
```
Fixes error: `configure: error: libgmp headers missing`,

`sudo apt install libgmp-dev`
Fixes error: `configure: error: libgmpxx headers missing`

**Step 3:** Run `make`

```sudo apt install libsodium-dev```

Fixes error:
```
./crypto/common.h:15:10: fatal error: sodium.h:
No such file or directory #include <sodium.h>
                                   ^~~~~~~~~~
compilation terminated.
```

```sudo apt install libgtest-dev```

Fixes error:
```
libsnark/algebra/curves/tests/test_bilinearity.cpp:13:10: fatal error: gtest/gtest.h: No such file or directory
#include <gtest/gtest.h>
         ^~~~~~~~~~~~~~~
compilation terminated.
```

Install cargo and build librustzcash:
```
sudo apt-install cargo

cd ~/mylibs

wget https://github.com/zcash/librustzcash/archive/91348647a86201a9482ad4ad68398152dc3d635e.tar.gz
mkdir librustzcash-9134864
tar -xvf 91348647a86201a9482ad4ad68398152dc3d635e.tar.gz -C librustzcash-9134864
cd librustzcash-9134864
cargo build --release

sudo cp target/release/librustzcash.a /usr/lib
sudo cp include/librustzcash.h /usr/include
```
Fixes error: `/usr/bin/ld: cannot find -lrustzcash`

**All set** - Dependencies globally installed. Now let's run the build:

```
./autogen
./configure
make
make install # optional, installs cli/daemon globally
```

**Done!**


# MacOS

Instead of the above steps, run:

```
cd scripts
./build-mac.sh
```

**Done!**

