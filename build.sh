#!/bin/bash

./build/tool/makeconfig

# $1: the string to echo
echo_suc()
{
	echo -e "\033[34;49;1m $1 \033[39;49;0m"
}

# $1: the string to echo
echo_err()
{
	echo -e "\033[31;49;1m $1 \033[39;49;0m"
}

#check should uncompress toolchain package
if [ ! -d ./tools/toolchain ]; then
	if [ -d ./tools ]; then
		echo uncompress toolchain package...
		toolchain_package=`find ./tools -name "toolchain*.tar.bz2"`
		tar  -jxf $toolchain_package -C ./tools/
		echo uncompress toolchain finish
	fi
fi

if [ ! -d ./app/pmu_config/out ]; then
	if [ -d ./app/pmu_config ];then
		mkdir ./app/pmu_config/out
	fi
fi

if [ ! -d ./build/out ]; then
	if [ -d ./build ];then
		mkdir ./build/out
	fi
fi

source ./build/envsetup.sh
# If "color_compile" command exists in $PATH, use color_compile.
# NOTE: It is possible to use "CC_FLAGS -fdiagnostics-color" instead of color_compile in the future.
if hash color_compile 2>/dev/null; then
	color_compile make -C ./build sysconfig
else
	make -C ./build sysconfig
fi

	echo_suc "#############################################################" >&2
	echo_suc "#              Building! Wait few minutes!                  #" >&2
	echo_suc "#############################################################" >&2

if [[ "$#" -eq "0" ]]; then
	if hash color_compile 2>/dev/null; then
		color_compile make -C ./build app
	else
		make -C ./build app
	fi
else
	if hash color_compile 2>/dev/null; then
		color_compile make -C ./build $*
	else
		make -C ./build $*
	fi
fi

if [ $? -ne 0 ]; then
	echo_err "#############################################################" >&2
	echo_err "#                        Build fail!                        #" >&2
	echo_err "#############################################################" >&2
	exit 1
else
	echo_suc "#############################################################" >&2
	echo_suc "#                       Build finish!                       #" >&2
	echo_suc "#############################################################" >&2
	exit 0
fi
