﻿/*******************************************************************************
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

#include "ppxnet/ping.h"
#include <assert.h>
#include "ppxbase/timeutils.h"
#include "ppxbase/safe_release_macro.h"

namespace ppx {
    namespace net {
        Ping::Ping(int packet_size /*= 32*/, int send_timeout_ms /*= 3000*/, int recv_timeout_ms /*= 3000*/, int ttl /*= 128*/) :
            packet_size_(packet_size),
            send_timeout_ms_(send_timeout_ms),
            recv_timeout_ms_(recv_timeout_ms),
            ttl_(ttl),
            exit_(CreateEvent(NULL, TRUE, FALSE, NULL))
        {
            WSADATA wsaData;
            WORD wVersionRequested = MAKEWORD(2, 2);
            WSAStartup(wVersionRequested, &wsaData);
            threads_ = new std::vector<std::thread>();
        }

        Ping::~Ping() {
            if (exit_)
                SetEvent(exit_);
            for (size_t i = 0; i < threads_->size(); i++) {
                if ((*threads_)[i].joinable())
                    (*threads_)[i].join();
            }
            (*threads_).clear();
            SAFE_CLOSE(exit_);
            WSACleanup();

            SAFE_DELETE(threads_);
        }

        void Ping::FillPingPacket(__u8* icmp_packet, __u16 seq, __u16 icmp_packet_size) {
            assert(icmp_packet);
            ping_hdr* p_ping_hdr = reinterpret_cast<ping_hdr*>(icmp_packet);
            if (!p_ping_hdr)
                return;
            p_ping_hdr->common_hdr.type = 8;
            p_ping_hdr->common_hdr.code = 0;
            p_ping_hdr->id = (__u16)GetCurrentProcessId();
            p_ping_hdr->seq = seq;
            __u32 now = (__u32)(base::GetTimeStamp() / 1000);

            memcpy((icmp_packet + sizeof(ping_hdr)), &now, sizeof(__u32));

            // fill some junk in the buffer.
            int junk_data_size = packet_size_ - sizeof(__u32); // timestamp
            int junk_offset = icmp_packet_size - junk_data_size;

            if (junk_data_size > 0)
                memset((icmp_packet + junk_offset), 'E', junk_data_size);

            p_ping_hdr->common_hdr.check = 0;
            p_ping_hdr->common_hdr.check = net::GetCheckSum(reinterpret_cast<__u16*>(icmp_packet), icmp_packet_size);
        }

        bool Ping::DecodeIPPacket(__u8* ip_packet, __u16 packet_size, PingRsp &rsp) {
            iphdr* ip_hdr = reinterpret_cast<iphdr*>(ip_packet);
            if (!ip_hdr)
                return false;
            __u32 now = (__u32)(base::GetTimeStamp() / 1000);

            __u16 ip_hdr_len = ip_hdr->ihl * 4; // bytes

            ping_hdr *p_ping_hdr = reinterpret_cast<ping_hdr*>(ip_packet + ip_hdr_len);
            if (p_ping_hdr->common_hdr.type != 0 || p_ping_hdr->common_hdr.code != 0) {
                printf("non-echo response, type=%d, code=%d\n", p_ping_hdr->common_hdr.type, p_ping_hdr->common_hdr.code);
                return false;
            }

            if (p_ping_hdr->id != (__u16)GetCurrentProcessId()) {
                printf("other process ping response packet, pid=%d\n", GetCurrentProcessId());
                return false;
            }

            __u32 timestamp = 0;
            memcpy(&timestamp, reinterpret_cast<__u32*>((__u8*)p_ping_hdr + sizeof(ping_hdr)), sizeof(__u32));

            in_addr from;
            from.s_addr = ip_hdr->saddr;

            rsp.icmp_seq = p_ping_hdr->seq;
            rsp.from_ip = IPAddress(from);
            rsp.recved_bytes = packet_size - ip_hdr_len - sizeof(ping_hdr);
            rsp.used_time_ms = now - timestamp;
            rsp.ttl = ip_hdr->ttl;

            return true;
        }

        bool Ping::AsyncPing(const IPAddress &ip, unsigned short times, std::function<void(const std::vector<PingRsp>&)> callback) {
            if (!ip.IsValid())
                return false;
            if (times <= 0 || times > 0xFFFF)
                return false;

            (*threads_).push_back(std::thread(&Ping::DoPing, this, ip, times, callback));

            return true;
        }

        void Ping::DoPing(const IPAddress &ip, unsigned short times, std::function<void(const std::vector<PingRsp>&)> callback) {
            // socket函数需要管理员权限
            // 需要绕开管理员权限，可以使用IcmpSendEcho系列函数
            //
            SOCKET s = socket(AF_INET, SOCK_RAW, IPPROTO_ICMP);
            if (s == INVALID_SOCKET) {
                return;
            }

            int err = setsockopt(s, SOL_SOCKET, SO_SNDTIMEO, reinterpret_cast<const char*>(&send_timeout_ms_), sizeof(send_timeout_ms_));
            if (err == SOCKET_ERROR) {
                closesocket(s);
                return;
            }

            err = setsockopt(s, SOL_SOCKET, SO_RCVTIMEO, reinterpret_cast<const char*>(&recv_timeout_ms_), sizeof(recv_timeout_ms_));
            if (err == SOCKET_ERROR) {
                closesocket(s);
                return;
            }

            err = setsockopt(s, IPPROTO_IP, IP_TTL, reinterpret_cast<const char*>(&ttl_), sizeof(ttl_));
            if (err == SOCKET_ERROR) {
                closesocket(s);
                return;
            }

            // ping request
            int icmp_packet_size = sizeof(ping_hdr) + packet_size_;

            __u8 *icmp_packet = new __u8[icmp_packet_size];

            // ping response
            __u16 ip_packet_size = icmp_packet_size + 20; // 20 bytes ip header, no option.
            __u8 *ip_packet = new __u8[ip_packet_size];


            std::vector<PingRsp> rsps;

            unsigned short i = 0;
            while (i++ < times && (exit_ && WaitForSingleObject(exit_, 0) != WAIT_OBJECT_0)) {
                PingRsp rsp;
                rsp.recved_bytes = SOCKET_ERROR;
                rsp.sent_bytes = SOCKET_ERROR;

                FillPingPacket(icmp_packet, i, icmp_packet_size);

                sockaddr_in addr;
                addr.sin_family = ip.GetFamily();
                addr.sin_addr = ip.GetIPv4Address();

                int sent = sendto(s,
                    reinterpret_cast<const char*>(icmp_packet),
                    icmp_packet_size,
                    0,
                    reinterpret_cast<const sockaddr*>(&addr),
                    sizeof(sockaddr));

                if (sent == SOCKET_ERROR) {
                    int gle = WSAGetLastError();
                    continue;
                }
                
                rsp.sent_bytes = packet_size_;


                sockaddr_in from;
                int from_len = sizeof(sockaddr_in);
                int bread = recvfrom(s,
                    reinterpret_cast<char*>(ip_packet),
                    ip_packet_size,
                    0,
                    reinterpret_cast<sockaddr*>(&from),
                    &from_len);

                if (bread == SOCKET_ERROR) {
                    int gle = WSAGetLastError();
                    continue;
                }

                if (bread < ip_packet_size) {
                    continue;
                }

                DecodeIPPacket(reinterpret_cast<__u8*>(ip_packet), ip_packet_size, rsp);

                rsps.push_back(rsp);

                if( exit_ && WaitForSingleObject(exit_, 1000) == WAIT_OBJECT_0)
                    break;
            }

            delete[] icmp_packet;
            delete[] ip_packet;

            if (s != INVALID_SOCKET) {
                closesocket(s);
            }

            if (callback) {
                callback(rsps);
            }
        }

    }
}