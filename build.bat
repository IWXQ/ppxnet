@echo off

goto Start

:ECHORED
%Windir%\System32\WindowsPowerShell\v1.0\Powershell.exe write-host -foregroundcolor Red %1
goto :eof



:Exit
endlocal
exit /B 1



:ShowError
echo.
call :ECHORED %1
goto :eof



:ShowSyntax
rem Display the help
echo.
echo Usage: build ^<build_type^> [-ppxbase_DIR] ["ppxbase_DIR"]
echo.
echo build_type:
echo.
echo static        - Use Build Static Library
echo shared        - Use Build Shared Library
echo.
echo -ppxbase_DIR  - Specify the custom ppxbase directory if ppxbase is installed at other location 
echo                 then "C:/ppxbase"
echo                 For e.g. -ppxbase_DIR "C:\ppxbase"
echo.
goto Exit



:ParseArgs

if "%~1" == "static" (
	set VCPKG_TARGET_TRIPLET=x86-windows-static
	set BUILD_SHARED_LIBS=OFF
) else if "%~1" == "shared" (
	set VCPKG_TARGET_TRIPLET=x86-windows
	set BUILD_SHARED_LIBS=ON
) else if "%~1" == "-ppxbase_DIR" (
	if "%~2" == "" (
		call :ShowError "Please provide ppbase install directory."
		goto ShowSyntax
	) else (		
		set PPXBASE_DIR="%~2"
		shift
	)
)

shift

if NOT "%~1" == "" (
	goto ParseArgs
)
goto :eof



:Start
setlocal
set VCPKG_TARGET_TRIPLET=
set BUILD_SHARED_LIBS=
set PPXBASE_DIR=

call :ParseArgs %*

if "" == "%VCPKG_TARGET_TRIPLET%" (
	goto ShowSyntax
)

if "" == "%BUILD_SHARED_LIBS%" (
	goto ShowSyntax
)

vcpkg install curl[openssl]:%VCPKG_TARGET_TRIPLET%
vcpkg install gtest:%VCPKG_TARGET_TRIPLET%

cmake.exe -G "Visual Studio 14" -DCMAKE_TOOLCHAIN_FILE=D:\sourcecode\vcpkg\scripts\buildsystems\vcpkg.cmake -DVCPKG_TARGET_TRIPLET=%VCPKG_TARGET_TRIPLET% -DBUILD_SHARED_LIBS=%BUILD_SHARED_LIBS% -Dppxbase_DIR=%PPXBASE_DIR% -DCMAKE_INSTALL_PREFIX=D:\ppxnet -DBUILD_TESTS=OFF -S %~dp0 -B %~dp0build

cmake.exe --build %~dp0build --config Debug --target INSTALL
cmake.exe --build %~dp0build --config Release --target INSTALL


endlocal
goto :eof

