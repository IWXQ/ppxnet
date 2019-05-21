# 1. ppxnet是什么
[ppxnet](https://github.com/winsoft666/ppxnet) 是一个C++的基础网络库。

# 如何编译
1. ppxnet 依赖如下的第三方库:
* ppxbase
* curl

使用[vcpkg](https://github.com/microsoft/vcpkg)对依赖库进行安装，如：
```
vcpkg install curl:x86-windows
vcpkg install curl:x86-windows-static
```

2. 修改`run_cmake.bat`文件
CMAKE_TOOLCHAIN_FILE: `***\scripts\buildsystems\vcpkg.cmake`文件路径.

VCPKG_TARGET_TRIPLET: 静态库`x86-windows-static`，动态库`x86-windows`.

BUILD_SHARED_LIBS: 是否编译动态库，`ON`或`OFF`.

PPXBASE_INSTALL_DIR: ppxbase的安装目录.

# 2. ppxnet包含哪些功能

## 文件下载
`net\file_download.h`
支持多线程下载、断点续传

## HTTP请求
`net\http_request.h`

## Ping功能
`net\ping.h`

## IP、Socket封装
`net\ipaddress.h`
`net\socket.h`
`net\socketaddress.h`

## Host解析
`net\host_resolve.h`

## IOCP及IOCP Server
`net\iocp.h`
`net\iocpserver.h`

Windows网络模型可以参考：[Windows网络模型](https://blog.csdn.net/china_jeffery/column/info/19222)


---------------------------------------------------------------

**感谢您的使用，欢迎提交BUG！**