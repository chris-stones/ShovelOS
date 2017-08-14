TARGET=i386-elf

JOBS="6"

BINUTILS_URL="http://ftp.gnu.org/gnu/binutils/"
BINUTILS_VER="2.29"

GCC_URL="ftp://ftp.mirrorservice.org/sites/sourceware.org/pub/gcc/releases/"
GCC_VER="7.1.0"

wget "$BINUTILS_URL/binutils-$BINUTILS_VER.tar.gz"
tar -xf "binutils-$BINUTILS_VER.tar.gz"

mkdir "build-binutils-$TARGET"
cd "build-binutils-$TARGET"

../binutils-$BINUTILS_VER/configure --target="$TARGET" --with-sysroot --disable-nls --disable-werror

make -j "$JOBS"

sudo make install

cd ..

wget "$GCC_URL/gcc-$GCC_VER/gcc-$GCC_VER.tar.gz"
tar -xf "gcc-$GCC_VER.tar.gz"

mkdir "build-gcc-$GCC_VER"
cd "build-gcc-$GCC_VER"

../gcc-$GCC_VER/configure --target="$TARGET" --without-headers --disable-nls --disable-werror --enable-languages=c

make -j "$JOBS" all-gcc
make -j "$JOBS" all-target-libgcc
sudo make install-gcc
sudo make install-target-libgcc
