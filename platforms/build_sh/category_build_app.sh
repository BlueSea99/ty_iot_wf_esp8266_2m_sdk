#!/bin/sh

# 传入的参数说明：
#
# $1 - APP产物名字，如： apps/product
# $2 - APP产物名字，如： product
# $3 - APP产物版本，如： x.x.x 
# $4 - APP类型，如： debug  release 默认 release
# $5 - APP产物包路径，如： output/dist/product1_1.0.0
#
# 例：
# ./build_app.sh apps/product product x.x.x  debug output/dist/product_x.x.x

set -e

cd `dirname $0`

# 通过环境变量传递编译工具链
export TARGET_PLATFORM=xtensa-lx106-elf
export PATH=$PATH:$(pwd)/platforms/$TARGET_PLATFORM/bin

if [ -z "$1" ];then
        echo "please input app path!!!"
        exit 1
fi
if [ -z "$2" ];then
        echo "please input the app name!!!"
        exit 1
fi
if [ -z "$3" ];then
        echo "please input the app sversion!!!"
        exit 1
fi

if [ -z "$4" ];then
        DEBUG=0
else
        if [ $4 = "debug" ];then
                DEBUG=1
        else
                if [ $4 = "release" ];then
		                DEBUG=0
		        else
		                DEBUG=0
		        fi
        fi
fi
if [ -z "$5" ];then
        # 通过环境变量传递生成的产物包全路径名称
		export CI_PACKAGE_PATH="$(pwd)/output/dist/$2_$3/"
		echo "$CI_PACKAGE_PATH!!!"	
else
        # 通过环境变量传递生成的产物包全路径名称
		export CI_PACKAGE_PATH="$(pwd)/$5"
		echo "$CI_PACKAGE_PATH!!!"		
fi

export SDK_TARGET_APP_PATH="$1"
echo "SDK_TARGET_APP_PATH=$SDK_TARGET_APP_PATH"
echo -e "FLASH_SIZE=$FLASH_SIZE DEBUG=$DEBUG"

echo -e "******************Build APP Begin********************"
cd ./platforms/build_sh

if [ $DEBUG = 0 ];then
	./build_app_release_2m.sh $2 $3
else
	./build_app_2m.sh $2 $3
fi                


echo -e "******************Build Finish***********************"

