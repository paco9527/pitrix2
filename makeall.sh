# 供各层makefile使用的包含目录列表
rm inc_list.config
DIR=$(pwd)
(find $DIR/core \(   -path "*demos" \
                -o -path "*examples" \
                -o -path "*docs" \
                -o -path "*.github" \
                -o -path "*WiringPi" \
                -o -path "*build" \) -prune -o \
                -type d | awk '{printf "INC += %s\n",$1}' ) >> inc_list.config

CROSS_COMPILE=$1
BUILDLIB=$2

if [ ! -n "$CROSS_COMPILE" ]; then
    CROSS_COMPILE="arm-linux-gnueabihf-"
fi
echo "Toolchain:"$CROSS_COMPILE

# 需要单独编译的库
if [ "$BUILDLIB" = "buildlib" ]; then
    echo "Buildlib:"$BUILDLIB

    # WiringPi：只编译wiringPi库
    make -C $DIR/core/extlib/WiringPi/wiringPi \
            CC="$CROSS_COMPILE"gcc
    mv $DIR/core/extlib/WiringPi/wiringPi/libwiringPi.so.2.70 \
        $DIR/output/libwiringPi.so

    # rpi_ws281x
    if [ ! -d $DIR/core/extlib/rpi_ws281x-1.0.0/build ]; then
        mkdir $DIR/core/extlib/rpi_ws281x-1.0.0/build
        cmake   -D BUILD_SHARED=OFF \
                -D BUILD_TEST=ON \
                -B $DIR/core/extlib/rpi_ws281x-1.0.0/build \
                $DIR/core/extlib/rpi_ws281x-1.0.0
    fi
    cmake --build $DIR/core/extlib/rpi_ws281x-1.0.0/build
    mv  $DIR/core/extlib/rpi_ws281x-1.0.0/build/libws2811.a \
        $DIR/output
fi

# 编译其余代码
make CROSS_COMPILE=$CROSS_COMPILE
