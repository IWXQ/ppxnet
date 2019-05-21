/*******************************************************************************
* Copyright (C) 2018 - 2020, winsoft666, <winsoft666@outlook.com>.
*
* THIS CODE AND INFORMATION IS PROVIDED "AS IS" WITHOUT WARRANTY OF ANY KIND,
* EITHER EXPRESSED OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE IMPLIED
* WARRANTIES OF MERCHANTABILITY AND/OR FITNESS FOR A PARTICULAR PURPOSE.
*
* Expect bugs
*
* Please use and enjoy. Please let me know of any bugs/improvements
* that you have found/implemented and I will fix/incorporate them into this
* file.
*******************************************************************************/

#ifndef PPXNET_EXPORT_H_
#define PPXNET_EXPORT_H_
#pragma once


#ifdef PPXNET_STATIC
#define PPXNET_API 
#else
#if defined(PPXNET_EXPORTS)
#	if defined(_MSC_VER)
#		define PPXNET_API __declspec(dllexport)
#	else
#		define PPXNET_API 
#	endif
#else
#	if defined(_MSC_VER)
#		define PPXNET_API __declspec(dllimport)
#	else
#		define PPXNET_API 
#	endif
#endif
#endif

#endif // !PPX_EXPORT_H_