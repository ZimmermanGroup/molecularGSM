#!/usr/bin/env bash

# Exit immediately if any command fails
set -e

# Get the directory where this script is located
REPO_DIR=$( cd -- "$( dirname -- "${BASH_SOURCE[0]}" )" &> /dev/null && pwd )

# Activate the pixi environment using the manifest in the repository
eval "$(pixi shell-hook --manifest-path ${REPO_DIR})"

# Create a subshell to isolate the build process
(
    # Create BUILD directory if it doesn't exist
    mkdir -p BUILD
    # Change to the BUILD directory
    cd BUILD

    # Run CMake to configure the project
    # Uses the repository directory as source
    # Sets installation path to the active conda environment
    cmake ${REPO_DIR} -DCMAKE_INSTALL_PREFIX=${CONDA_PREFIX}

    # Compile the project
    make -j8
    # Install the compiled binaries and libraries
    make install
)
