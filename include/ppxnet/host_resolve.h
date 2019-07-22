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

#ifndef PPX_NET_HOST_RESOLVE_H_
#define PPX_NET_HOST_RESOLVE_H_
#pragma once

#include <string>
#include <functional>
#include <thread>
#include <vector>
#include "ppxnet/ipaddress.h"
#include "ppxnet_export.h"

namespace ppx {
    namespace net {
        class PPXNET_API HostResolve {
        public:
            HostResolve();
            virtual~HostResolve();

            bool Resolve(const std::string &host, std::vector<IPAddress>& ip_list);

        private:
            void DoResolve(const std::string &host, std::function<void(const std::vector<IPAddress>&)> callback);

        private:
            HANDLE exit_;
            std::vector<std::thread>* threads_;
        };
    }
}

#endif // !PPX_NET_HOST_RESOLVE_H_