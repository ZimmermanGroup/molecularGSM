#!/usr/bin/env bash

set -e
REPO_DIR=$( cd -- "$( dirname -- "${BASH_SOURCE[0]}" )" &> /dev/null && pwd )

eval "$(pixi shell-hook --manifest-path ${REPO_DIR})"

(
    mkdir -p BUILD
    cd BUILD

    cmake ${REPO_DIR} -DCMAKE_INSTALL_PREFIX=${CONDA_PREFIX}

    make
    make install
)
