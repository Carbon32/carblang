#!/bin/bash

if [ -d "vsc-extension" ]; then
    cd vsc-extension
    vsce package --allow-missing-repository
    echo "Extension built successfully"
    cd ..
else
    echo "The extension folder does not exist"
fi

