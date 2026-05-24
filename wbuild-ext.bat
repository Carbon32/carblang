@echo off

IF EXIST vsc-extension (
    cd vsc-extension
    vsce package --allow-missing-repository
    echo Extension built successfully
    cd ..

) ELSE (
    echo The extension folder does not exist
)

@echo off

IF EXIST vsc-extension (
    cd vsc-extension
    vsce package --allow-missing-repository
    echo Extension built successfully
    cd ..

) ELSE (
    echo The extension folder does not exist
)