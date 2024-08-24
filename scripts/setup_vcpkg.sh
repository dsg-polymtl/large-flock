#!/bin/bash

# Step 1: Check if the vcpkg repository is already cloned
if [ -d "vcpkg" ]; then
    echo "vcpkg repository already cloned. Skipping the cloning phase..."
else
    echo "Cloning the vcpkg repository..."
    git clone https://github.com/Microsoft/vcpkg.git

    # Check if cloning was successful
    if [ $? -ne 0 ]; then
        echo "Error: Failed to clone the vcpkg repository."
        exit 1
    fi
    echo "vcpkg repository cloned successfully."
fi

# Step 2: Bootstrap vcpkg
echo "Bootstrapping vcpkg..."
./vcpkg/bootstrap-vcpkg.sh

# Check if bootstrapping was successful
if [ $? -ne 0 ]; then
    echo "Error: Failed to bootstrap vcpkg."
    exit 1
fi
echo "vcpkg bootstrapped successfully."

# Step 3: Export the VCPKG toolchain path environment variable
export VCPKG_TOOLCHAIN_PATH=`pwd`/vcpkg/scripts/buildsystems/vcpkg.cmake
echo "VCPKG_TOOLCHAIN_PATH is set to $VCPKG_TOOLCHAIN_PATH"

echo "vcpkg setup complete."