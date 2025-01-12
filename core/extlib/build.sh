#!/bin/bash
CUR_DIR=$1
BUILD_TYPE=Release
CROSS_C_COMPILER=$2
CROSS_CXX_COMPILER=$3
CMAKE_COMPILE_FLAGS="-DCMAKE_C_COMPILER=$CROSS_C_COMPILER -DCMAKE_CXX_COMPILER=$CROSS_CXX_COMPILER -DCMAKE_INSTALL_PREFIX=$CUR_DIR/output -DCMAKE_BUILD_TYPE=$BUILD_TYPE"

pushd $CUR_DIR
mkdir -p output
mkdir -p output/include
mkdir -p output/lib

# lua
pushd lua
make a CC=$CROSS_C_COMPILER CFLAGS="-Wall -O2 -std=c99 -fno-stack-protector -fno-common" -j
cp *.h ../output/include
cp liblua.a ../output/lib
popd

# lvgl
pushd lvgl
cp ../patch/lv_conf.h .
mkdir build
cd build
cmake .. $CMAKE_COMPILE_FLAGS
make -j
make install
popd

# luavgl
cp CMakeLists.extlib.tostatic luavgl/CMakeLists.txt
pushd luavgl
# v0.1.0编译时少包含了头文件；暂时不打算尝试luavgl上最新的提交了，就这么对付一下
git apply ../patch/luavgl-v0.1.0.patch
mkdir -p build
cd build
cmake .. -DSRC_PATH=$CUR_DIR/luavgl/src -DEXTERN_INCLUDE_PATH="$CUR_DIR/output/include/lvgl;$CUR_DIR/luavgl/src;$CUR_DIR/lua" $CMAKE_COMPILE_FLAGS -DLIBRARY_NAME="luavgl"
make -j
cp libluavgl.a $CUR_DIR/output/lib
cp ../src/*.h $CUR_DIR/output/include
popd

# WiringPi
pushd WiringPi/wiringPi
make CC=$CROSS_C_COMPILER -j
make install DESTDIR=$CUR_DIR/output LDCONFIG=
popd

# rpi-ws281x
pushd rpi-ws281x
mkdir build
cd build
cmake .. $CMAKE_COMPILE_FLAGS
make -j
make install
popd

# lua-cjson
cp CMakeLists.extlib.tostatic lua-cjson/CMakeLists.txt
pushd lua-cjson
# 用一个比较傻的办法让它使用fpconv.c...
mv dtoa.c dtoa.c.disable
mkdir build
cd build
cmake .. -DSRC_PATH=$CUR_DIR/lua-cjson -DEXTERN_INCLUDE_PATH="$CUR_DIR/lua" $CMAKE_COMPILE_FLAGS -DLIBRARY_NAME="lua-cjson"
make -j
cp liblua-cjson.a $CUR_DIR/output/lib
cp ../*.h $CUR_DIR/output/include
cd ..
mv dtoa.c.disable dtoa.c
popd

popd
