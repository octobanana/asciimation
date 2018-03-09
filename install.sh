#!/usr/bin/env bash
set -e

BUILD_TYPE="Debug"

if [[ $# > 0 ]]; then
  if [[ $1 == "-d" ]]; then
    BUILD_TYPE="Debug"
  elif [[ $1 == "-r" ]]; then
    BUILD_TYPE="Release"
  else
    printf "usage: ./install.sh [-d|-r]\n";
    exit 1
  fi
fi

# source environment variables
source ./env.sh

if [[ ${BUILD_TYPE} == "Debug" ]]; then
  ./build.sh -d
  cd build/debug

  printf "\nInstalling ${APP}\n"
  sudo make install

elif [[ ${BUILD_TYPE} == "Release" ]]; then
  ./build.sh -r
  cd build/release

  printf "\nInstalling ${APP}\n"
  sudo make install
fi
