#!/usr/bin/env bash
set -e

BUILD_TYPE="Debug"

if [[ $# > 0 ]]; then
  if [[ $1 == "-d" ]]; then
    BUILD_TYPE="Debug"
  elif [[ $1 == "-r" ]]; then
    BUILD_TYPE="Release"
  else
    printf "usage: ./build.sh [-d|-r]\n";
    exit 1
  fi
fi

# source environment variables
source ./env.sh

printf "\nBuilding ${APP} in ${BUILD_TYPE} mode\n"

if [[ ${BUILD_TYPE} == "Debug" ]]; then
  printf "\nCompiling ${APP}\n"
  mkdir -p build/debug
  cd build/debug
  cmake ../../ -DCMAKE_BUILD_TYPE=${BUILD_TYPE}
  time make

elif [[ ${BUILD_TYPE} == "Release" ]]; then
  printf "\nCompiling ${APP}\n"
  mkdir -p build/release
  cd build/release
  cmake ../../ -DCMAKE_BUILD_TYPE=${BUILD_TYPE}
  time make
fi
