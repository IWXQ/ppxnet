@echo off
cmake.exe -G "Visual Studio 14" -DCMAKE_TOOLCHAIN_FILE=D:\sourcecode\vcpkg\scripts\buildsystems\vcpkg.cmake -DVCPKG_TARGET_TRIPLET=x86-windows -DBUILD_SHARED_LIBS=ON -DCMAKE_INSTALL_PREFIX=D:\ppxnet -Dppxbase_DIR=D:\ppxbase\share\ppxbase -DBUILD_TESTS=OFF -S %~dp0 -B %~dp0build
pause