#!/bin/bash -x

if [ "$(basename $PWD)" -ne "matterialize" ]; then
  echo "Please run this script from the project root directory."
  exit 1
fi

PROJECT_ROOT=$PWD
BUILD_DIR=build_dir
CMAKE_TOOLCHAIN_FILE=$(realpath vcpkg/scripts/buildsystems/vcpkg.cmake)
V4L2_LOOPBACK_CONFIG=v4l2loopback.conf
EXE=matterialize-server
MATTERALIZE_USER_PATH=$HOME/.matterialize
BENCHMARK_PATH=$MATTERALIZE_USER_PATH/benchmark/


# Install system deps
if [ -f /etc/arch-release ]; then
  sudo pacman -Sy --noconfirm v4l2loopback-dkms opencv
elif [ -f /etc/debian_version ]; then
  sudo apt update -y
  sudo apt install -y v4l2loopback-dkms opencv
else
  echo "Failed: Distro must be arch or debian based"
  exit 1
fi

# Update configuration and reloads module
sudo install -D -m644 "install/$V4L2_LOOPBACK_CONFIG" "/etc/modprobe.d/$V4L2_LOOPBACK_CONFIG"
sudo modprobe -r v4l2loopback
sudo modprobe v4l2loopback

# Copy benchmarking images
cp --archive images "$BENCHMARK_PATH"

git submodule update --init --recursive

# install C++ deps
if [ ! -f vcpkg/vcpkg ]; then
  ./vcpkg/bootstrap-vcpkg.sh -disableMetrics
fi

./vcpkg/vcpkg install pistache
./vcpkg/vcpkg install rapidjson
./vcpkg/vcpkg install nlohmann-json
./vcpkg/vcpkg install gtest
# ./vcpkg/vcpkg install opencv

# build executable
mkdir -p $BUILD_DIR
cmake -DCMAKE_BUILD_TYPE=Release -DCMAKE_TOOLCHAIN_FILE=$CMAKE_TOOLCHAIN_FILE -S $PROJECT_ROOT -B $BUILD_DIR
cmake --build $BUILD_DIR --target $EXE -- -j 6
chmod +x $BUILD_DIR/$EXE

sudo install -D -m755 "$BUILD_DIR/$EXE" "/usr/local/bin/$EXE"
