// Copyright (c) 2016 Steinwurf ApS
// All Rights Reserved
//
// Distributed under the "BSD License". See the accompanying LICENSE.rst file.

#pragma once

#include <cstdint>
#include <cstring>
#include <iostream>
#include <system_error>
#include <vector>

#include <asm/types.h>
#include <linux/if.h>
#include <linux/netlink.h>
#include <linux/rtnetlink.h>
#include <netinet/in.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/ioctl.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <unistd.h>

#include "../../interface.hpp"
#include "../../log_level.hpp"
#include "../action.hpp"
#include "../log.hpp"
#include "../scoped_file_descriptor.hpp"
#include "error.hpp"
#include "if_nametoindex.hpp"

namespace tunnel
{
namespace detail
{
namespace platform_linux
{
inline int32_t netlink_port()
{
    // https://stackoverflow.com/a/29671337/1717320

    static int32_t offset = 1;

    int32_t local_port = ::getpid() | (offset << 22);

    ++offset;

    return local_port;
}

inline uint32_t sequence_number()
{
    static uint32_t number = 0;
    return number++;
}

/// rtnetlink - Linux IPv4 routing socket
/// http://man7.org/linux/man-pages/man7/rtnetlink.7.html
///
/// Good introduction:
/// https://www.infradead.org/~tgr/libnl/doc/core.html
///
/// Solution for default interface:
/// https://stackoverflow.com/a/15670190/1717320
template <class Super>
class layer_netlink_v4 : public Super
{
public:
    void create(const tunnel::interface::config& config, std::error_code& error)
    {
        assert(!error);

        Super::create(config, error);

        if (error)
        {
            return;
        }

        m_dev_fd = Super::socket(AF_NETLINK, SOCK_DGRAM, NETLINK_ROUTE, error);

        if (error)
        {
            return;
        }

        struct sockaddr_nl sa
        {
        };

        sa.nl_family = AF_NETLINK;
        sa.nl_pid = m_netlink_port;

        Super::bind(m_dev_fd, (struct sockaddr*)&sa, sizeof(sa), error);
    }

    auto is_default_route(std::error_code& error) -> bool
    {
        struct rtmsg payload
        {
        };

        // http://man7.org/linux/man-pages/man7/rtnetlink.7.html
        payload.rtm_family = AF_INET;
        payload.rtm_table = RT_TABLE_MAIN;

        send_netlink(RTM_GETROUTE, NLM_F_DUMP | NLM_F_REQUEST, &payload,
                     sizeof(payload), error);

        if (error)
        {
            return false;
        }

        std::string default_interface = recv_netlink(error);

        if (error)
        {
            return false;
        }

        std::string interface_name = Super::interface_name(error);

        if (error)
        {
            return false;
        }

        Super::do_log(log_level::debug, action::is_default_route,
                      log::boolean{"is_default_route",
                                   default_interface == interface_name});

        return default_interface == interface_name;
    }

private:
    void send_netlink(int type, int flags, const void* data, uint32_t size,
                      std::error_code& error)
    {
        assert(!error);

        uint32_t message_size = NLMSG_SPACE(size);

        // We need a buffer to store the message
        std::vector<uint8_t> message(message_size, 0);

        // Pointer to the header
        struct nlmsghdr* header = (struct nlmsghdr*)message.data();

        header->nlmsg_len = NLMSG_LENGTH(size);
        header->nlmsg_type = type;
        header->nlmsg_flags = flags;
        header->nlmsg_seq = sequence_number();
        header->nlmsg_pid = 111;

        void* dst_data = NLMSG_DATA(header);
        std::memcpy(dst_data, data, size);

        Super::send(m_dev_fd, message.data(), message.size(), 0, error);

        Super::do_log(log_level::debug, action::send_netlink,
                      log::uinteger{"nlmsg_pid", header->nlmsg_pid},
                      log::uinteger{"nlmsg_seq", header->nlmsg_seq},
                      log::str{"error", error.message().c_str()});
    }

    auto recv_netlink(std::error_code& error) -> std::string
    {
        assert(!error);

        std::vector<std::vector<uint8_t>> messages =
            recv_netlink_messages(error);

        if (error)
        {
            return {};
        }

        for (auto& message : messages)
        {
            struct nlmsghdr* header = (struct nlmsghdr*)message.data();
            struct rtmsg* payload = (struct rtmsg*)NLMSG_DATA(header);

            if (payload->rtm_family != AF_INET)
            {
                continue;
            }

            if (payload->rtm_table != RT_TABLE_MAIN)
            {
                continue;
            }

            struct rtattr* attr = (struct rtattr*)RTM_RTA(payload);
            uint32_t attr_size = RTM_PAYLOAD(header);

            uint32_t dest = 0;
            std::string name;

            for (; RTA_OK(attr, attr_size); attr = RTA_NEXT(attr, attr_size))
            {
                switch (attr->rta_type)
                {
                case RTA_OIF:
                {
                    uint32_t index = *(uint32_t*)RTA_DATA(attr);
                    name = if_indextoname(index);
                    break;
                }
                case RTA_DST:
                {
                    dest = *(uint32_t*)RTA_DATA(attr);
                    break;
                }
                default:
                {
                    break;
                }
                }
            }

            if (dest == 0)
            {
                return name;
            }
        }

        // There was no default interface
        return {};
    }

    auto recv_netlink_message(std::error_code& error) -> std::vector<uint8_t>
    {
        std::vector<uint8_t> message;

        struct nlmsghdr* header;

        do
        {

            uint32_t size = 0;
            Super::recv(m_dev_fd, &size, sizeof(size), MSG_PEEK, error);

            if (error)
            {
                return {};
            }

            Super::do_log(log_level::debug, action::recv_netlink_message,
                          log::uinteger{"size", size});

            message.resize(size, 0);

            Super::recv(m_dev_fd, message.data(), message.size(), 0, error);

            header = (struct nlmsghdr*)message.data();

            if (NLMSG_OK(header, size) == false)
            {
                error = make_error_code(linux_error::nlmsg_ok);
                return {};
            }

            if (header->nlmsg_type == NLMSG_ERROR)
            {
                error = make_error_code(linux_error::nlmsg_error);
                return {};
            }

            Super::do_log(log_level::debug, action::recv_netlink_message,
                          log::uinteger{"nlmsg_pid", header->nlmsg_pid},
                          log::uinteger{"nlmsg_seq", header->nlmsg_seq},
                          log::integer{"m_netlink_port", m_netlink_port},
                          log::str{"error", error.message().c_str()});

            // Filter out messages not for us
        } while ((pid_t)header->nlmsg_pid != m_netlink_port);

        return message;
    }

    auto recv_netlink_messages(std::error_code& error)
        -> std::vector<std::vector<uint8_t>>
    {
        std::vector<std::vector<uint8_t>> messages;

        std::vector<uint8_t> message = recv_netlink_message(error);

        if (error)
        {
            return {};
        }

        messages.push_back(message);

        struct nlmsghdr* header = (struct nlmsghdr*)message.data();

        // Check if multipart
        bool multipart = header->nlmsg_flags & NLM_F_MULTI;

        if (!multipart)
        {
            return messages;
        }

        while (header->nlmsg_type != NLMSG_DONE)
        {

            message = recv_netlink_message(error);

            if (error)
            {
                return {};
            }

            messages.push_back(message);
            header = (struct nlmsghdr*)message.data();
        };

        return messages;
    }

private:
    scoped_file_descriptor m_dev_fd;
    const pid_t m_netlink_port = netlink_port();
};
}
}
}
