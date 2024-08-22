
#include <iostream>
#include <cstring>
#include <fcntl.h>
#include <unistd.h>
#include <sys/ioctl.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <sys/sysctl.h>
#include <net/if.h>
#include <net/if_utun.h>
#include <iostream>
#include <cstring>
#include <fcntl.h>
#include <unistd.h>
#include <sys/ioctl.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <sys/sysctl.h>
#include <net/if.h>
#include <net/if_utun.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <net/route.h>
#include <sstream>
#include "../../interface_config.hpp"
// #include "../../log_level.hpp"
#include "../log_kind.hpp"
#include "../monitor.hpp"
#include <stdio.h>
#include <sys/socket.h>
#include <sys/sys_domain.h>
#include <unistd.h>

#include <string.h> // strlcpy

#include <sys/ioctl.h> // ioctl
#include <sys/kern_control.h> // struct socketaddr_ctl
#include <net/if_utun.h> // UTUN_CONTROL_NAME

#include <fcntl.h>
#include "../to_json_property.hpp"


namespace tunnel
{
namespace detail
{
namespace platform_macos
{

class tun_interface {
public:
    tun_interface() : fd(-1), unit(-1), m_monitor("tunnel::detail::platform_macos::utun_interface", {}) {}
    
    ~tun_interface() {
        close_tun_device();
    }

    // create the utun device
    void create(const config& config, std::error_code& error){
        assert(fd == -1 && "Cannot create an already created device.");

        struct ctl_info ctlInfo;
        strlcpy(ctlInfo.ctl_name, UTUN_CONTROL_NAME, sizeof(ctlInfo.ctl_name));

        fd = socket(PF_SYSTEM, SOCK_DGRAM, SYSPROTO_CONTROL);
        if (fd < 0) {
            m_monitor.m_monitor.log(poke::log_level::error, log_kind::open, poke::log::str{"error", strerror(errno)});
            error = std::make_error_code(std::errc::io_error);
            return;
        }

        struct sockaddr_ctl sc;
        memset(&sc, 0, sizeof(sc));
        if (ioctl(fd, CTLIOCGINFO, &ctlInfo) == -1) {
            m_monitor.m_monitor.log(poke::log_level::error, log_kind::open, poke::log::str{"error", strerror(errno)});
            error = std::make_error_code(std::errc::io_error);
            close_tun_device();
            return;
        }

        // Find a free unit number
        auto find_free_unit = [&]() -> int {
            int sock = socket(PF_SYSTEM, SOCK_DGRAM, SYSPROTO_CONTROL);
            if (sock < 0) {
                m_monitor.m_monitor.log(poke::log_level::error, log_kind::open, poke::log::str{"error", strerror(errno)});
                error = std::make_error_code(std::errc::io_error);
                return -1;
            }
            
            for (int i = 7; i < 256; ++i) {
                struct sockaddr_ctl sc;
                memset(&sc, 0, sizeof(sc));
                sc.sc_len = sizeof(sc);
                sc.sc_family = AF_SYSTEM;
                sc.ss_sysaddr = AF_SYS_CONTROL;
                sc.sc_id = -1;
                sc.sc_unit = i;

                if (connect(sock, (struct sockaddr *)&sc, sizeof(sc)) < 0) {
                    close(sock);
                    return i;
                }
                close(sock);
            }
            m_monitor.m_monitor.log(poke::log_level::error, log_kind::open, poke::log::str{"error", "No free unit found"});
            error = std::make_error_code(std::errc::io_error);
            return -1;
        };

        // Scan for a free unit number
        unit = find_free_unit();
        if (unit == -1) {
            close_tun_device();
            return;
        }

        sc.sc_id = ctlInfo.ctl_id;
        sc.sc_len = sizeof(sc);
        sc.sc_family = AF_SYSTEM;
        sc.ss_sysaddr = AF_SYS_CONTROL;
        sc.sc_unit = unit;

        if (connect(fd, (struct sockaddr *)&sc, sizeof(sc)) < 0) {
            m_monitor.m_monitor.log(poke::log_level::error, log_kind::open, poke::log::str{"error", strerror(errno)});
            error = std::make_error_code(std::errc::io_error);
            close_tun_device();
            return;
        }

        // Set non-blocking mode
        if (fcntl(fd, F_SETFL, O_NONBLOCK) < 0) {
            m_monitor.m_monitor.log(poke::log_level::error, log_kind::open, poke::log::str{"error", strerror(errno)});
            error = std::make_error_code(std::errc::io_error);
            close_tun_device();
            return;
        }

        // Construct the interface name
        name = "utun" + std::to_string(unit-1);

    }

    // Close the utun device
    void close_tun_device() {
        if (fd != -1) {
            ::close(fd);
            fd = -1;
            name.clear();
        }
    }

    // Check if the device is open
    bool isOpen() const {
        return fd != -1;
    }

    // Get the file descriptor
    int native_handle() const {
        return fd;
    }

    // Get the device name
    std::string getName() const {
        assert(!name.empty() && "Device name is empty.");
        return name;
    }

    // Set IP address and netmask
    void set_ipv4(const std::string& ipAddress, std::error_code& error) {
        assert(isOpen() && "Device is not open.");

        int sock = socket(AF_INET, SOCK_DGRAM, 0);
        if (sock < 0) {
            m_monitor.m_monitor.log(poke::log_level::error, log_kind::set_ipv4, poke::log::str{"error", strerror(errno)});
            error = std::make_error_code(std::errc::io_error);
            return;
        }

        struct ifreq ifr;
        memset(&ifr, 0, sizeof(ifr));
        strncpy(ifr.ifr_name, name.c_str(), IFNAMSIZ);

        // Set IP address
        struct sockaddr_in* addr = (struct sockaddr_in*)&ifr.ifr_addr;
        addr->sin_family = AF_INET;
        if (inet_pton(AF_INET, ipAddress.c_str(), &addr->sin_addr) <= 0) {
            m_monitor.m_monitor.log(poke::log_level::error, log_kind::set_ipv4, poke::log::str{"error", strerror(errno)});
            error = std::make_error_code(std::errc::io_error);
            close(sock);
            return;
        }

        if (ioctl(sock, SIOCSIFADDR, &ifr) < 0) {
            m_monitor.m_monitor.log(poke::log_level::error, log_kind::set_ipv4, poke::log::str{"error", strerror(errno)});
            error = std::make_error_code(std::errc::io_error);
            close(sock);
            return;
        }

        // if everything is successful, save the IP address
        m_ipAddress = ipAddress;
    }

    void set_ipv4_netmask(const std::string& mask, std::error_code& error){
        assert(isOpen() && "Device is not open.");

        int sock = socket(AF_INET, SOCK_DGRAM, 0);
        if (sock < 0) {
            m_monitor.m_monitor.log(poke::log_level::error, log_kind::set_ipv4_netmask, poke::log::str{"error", strerror(errno)});
            error = std::make_error_code(std::errc::io_error);
            return;
        }

        struct ifreq ifr;
        memset(&ifr, 0, sizeof(ifr));
        strncpy(ifr.ifr_name, name.c_str(), IFNAMSIZ);

        // Set IP address
        struct sockaddr_in* addr = (struct sockaddr_in*)&ifr.ifr_addr;
        addr->sin_family = AF_INET;
        if (inet_pton(AF_INET, mask.c_str(), &addr->sin_addr) <= 0) {
            m_monitor.m_monitor.log(poke::log_level::error, log_kind::set_ipv4_netmask, poke::log::str{"error", strerror(errno)});
            error = std::make_error_code(std::errc::io_error);
            close(sock);
            return;
        }

        ifr.ifr_addr.sa_family = AF_INET;
        if (ioctl(sock, SIOCSIFNETMASK, &ifr) < 0) {
            m_monitor.m_monitor.log(poke::log_level::error, log_kind::set_ipv4_netmask, poke::log::str{"error", strerror(errno)});
            error = std::make_error_code(std::errc::io_error);
            close(sock);
            return;
        }
        m_netmask = mask;
    }

    std::string ipv4(std::error_code& error) const {
        assert(m_ipAddress.empty() && "IP address is empty.");
        return m_ipAddress;
    }

    std::string ipv4_netmask(std::error_code& error) const {
        assert(m_netmask.empty() && "Netmask is empty.");
        return m_netmask;
    }

    // Set MTU
    void set_mtu(uint32_t mtu, std::error_code& error) {
        assert(isOpen() && "Device is not open.");

        int sock = socket(AF_INET, SOCK_DGRAM, 0);
        if (sock < 0) {
            m_monitor.m_monitor.log(poke::log_level::error, log_kind::set_mtu, poke::log::str{"error", strerror(errno)});
            error = std::make_error_code(std::errc::io_error);
            return;
        }

        struct ifreq ifr;
        memset(&ifr, 0, sizeof(ifr));
        strncpy(ifr.ifr_name, name.c_str(), IFNAMSIZ);

        // Set MTU
        ifr.ifr_mtu = mtu;
        if (ioctl(sock, SIOCSIFMTU, &ifr) < 0) {
            m_monitor.m_monitor.log(poke::log_level::error, log_kind::set_mtu, poke::log::str{"error", strerror(errno)});
            error = std::make_error_code(std::errc::io_error);
            close(sock);
            return;
        }

        close(sock);
        m_mtu = mtu;
    }

    // Bring up the interface
    void up(std::error_code& error) const {
        assert(isOpen() && "Device is not open.");

        int sock = socket(AF_INET, SOCK_DGRAM, 0);
        if (sock < 0) {
            m_monitor.m_monitor.log(poke::log_level::error, log_kind::interface_up, poke::log::str{"error", strerror(errno)});
            error = std::make_error_code(std::errc::io_error);
            return;
        }

        struct ifreq ifr;
        memset(&ifr, 0, sizeof(ifr));
        strncpy(ifr.ifr_name, name.c_str(), IFNAMSIZ);

        // Get current flags
        if (ioctl(sock, SIOCGIFFLAGS, &ifr) < 0) {
            m_monitor.m_monitor.log(poke::log_level::error, log_kind::interface_up, poke::log::str{"error", strerror(errno)});
            error = std::make_error_code(std::errc::io_error);
            close(sock);
            return;
        }

        // Set the interface up
        ifr.ifr_flags |= IFF_UP;
        if (ioctl(sock, SIOCSIFFLAGS, &ifr) < 0) {
            m_monitor.m_monitor.log(poke::log_level::error, log_kind::interface_up, poke::log::str{"error", strerror(errno)});
            error = std::make_error_code(std::errc::io_error);
            close(sock);
            return;
        }

        close(sock);
    }

    // Bring down the interface
    void down(std::error_code& error) const {
        assert(isOpen() && "Device is not open.");

        int sock = socket(AF_INET, SOCK_DGRAM, 0);
        if (sock < 0) {
            m_monitor.m_monitor.log(poke::log_level::error, log_kind::interface_down, poke::log::str{"error", strerror(errno)});
            error = std::make_error_code(std::errc::io_error);
            return;
        }

        struct ifreq ifr;
        memset(&ifr, 0, sizeof(ifr));
        strncpy(ifr.ifr_name, name.c_str(), IFNAMSIZ);

        // Get current flags
        if (ioctl(sock, SIOCGIFFLAGS, &ifr) < 0) {
            m_monitor.m_monitor.log(poke::log_level::error, log_kind::interface_down, poke::log::str{"error", strerror(errno)});
            error = std::make_error_code(std::errc::io_error);
            close(sock);
            return;
        }

        // Set the interface down
        ifr.ifr_flags &= ~IFF_UP;
        if (ioctl(sock, SIOCSIFFLAGS, &ifr) < 0) {
            m_monitor.m_monitor.log(poke::log_level::error, log_kind::interface_down, poke::log::str{"error", strerror(errno)});
            error = std::make_error_code(std::errc::io_error);
            close(sock);
            return;
        }

        close(sock);
    }

    // Set the default route
    void enable_default_route(std::error_code& error){
        std::stringstream ss;

        ss << "route add default " << m_ipAddress << " -ifscope " << name;
        std::string command = ss.str();

        if (system(command.c_str()) == -1) {
            m_monitor.m_monitor.log(poke::log_level::error, log_kind::enable_default_route, poke::log::str{"error", "Failed to run command: " + command});
            error = std::make_error_code(std::errc::io_error);
            return;
        }
        m_is_default_route = true;
    }

    // remove the default route
    void disable_default_route(std::error_code& error){
        std::stringstream ss;

        ss << "route delete default " << m_ipAddress << " -ifscope " << name;
        std::string command = ss.str();

        if (system(command.c_str()) == -1) {
            m_monitor.m_monitor.log(poke::log_level::error, log_kind::enable_default_route, poke::log::str{"error", "Failed to run command: " + command});
            error = std::make_error_code(std::errc::io_error);
        }
        m_is_default_route = false;
    }

    static auto is_platform_supported() -> bool
    {
        return true;
    }

    static auto type() -> std::string
    {
        return "tunnel::detail::platform_macos::utun_interface";
    }

    std::string interface_name(std::error_code& error) const {
        assert(!name.empty() && "Interface name is empty.");
        return name;
    }
    
    uint32_t mtu(std::error_code& error) const {
        assert(m_mtu == -1 && "MTU is not set.");
        return m_mtu;
    }

    bool is_default_route(std::error_code& error) const {
        return m_is_default_route;
    }
    
    auto monitor() const -> const tunnel::monitor&{
        return m_monitor;
    }

    auto monitor() -> tunnel::monitor& {
        return m_monitor;
    }

    void set_log_callback(const log_callback& callback) {
        m_monitor.set_log_callback(callback);
    }
    // all the function below are not implemented
    // they are just here to make the code compile

    void rename(const std::string& newName, std::error_code& error) const {
        m_monitor.m_monitor.log(poke::log_level::error, log_kind::unsupported_platform);
        error = std::make_error_code(std::errc::not_supported);
    }

    std::string owner(std::error_code& error) const {
        m_monitor.m_monitor.log(poke::log_level::error, log_kind::unsupported_platform);
        error = std::make_error_code(std::errc::not_supported);
        return "";
    }

    std::string group(std::error_code& error) const {
        m_monitor.m_monitor.log(poke::log_level::error, log_kind::unsupported_platform);
        error = std::make_error_code(std::errc::not_supported);
        return "";
    }

    void set_owner(const std::string& owner, std::error_code& error) const {
        m_monitor.m_monitor.log(poke::log_level::error, log_kind::unsupported_platform);
        error = std::make_error_code(std::errc::not_supported);
    }

    void set_group(const std::string& group, std::error_code& error) const {
        m_monitor.m_monitor.log(poke::log_level::error, log_kind::unsupported_platform);
        error = std::make_error_code(std::errc::not_supported);
    }

    bool is_persistent(std::error_code& error) const {
        m_monitor.m_monitor.log(poke::log_level::error, log_kind::unsupported_platform);
        error = std::make_error_code(std::errc::not_supported);
        return false;
    }

    bool is_up(std::error_code& error) const {
        m_monitor.m_monitor.log(poke::log_level::error, log_kind::unsupported_platform);
        error = std::make_error_code(std::errc::not_supported);
        return false;
    }

    bool is_down( std::error_code& error) const {
        m_monitor.m_monitor.log(poke::log_level::error, log_kind::unsupported_platform);
        error = std::make_error_code(std::errc::not_supported);
        return false;
    }

    void set_persistent(std::error_code& error) const {
        m_monitor.m_monitor.log(poke::log_level::error, log_kind::unsupported_platform);
        error = std::make_error_code(std::errc::not_supported);
    }

    void set_non_persistent(std::error_code& error) const {
        m_monitor.m_monitor.log(poke::log_level::error, log_kind::unsupported_platform);
        error = std::make_error_code(std::errc::not_supported);
    }



private:
    int fd;
    int unit;
    std::string name;
    std::string m_ipAddress;
    std::string m_netmask;
    uint32_t m_mtu = -1;
    bool m_is_default_route;

    tunnel::detail::monitor m_monitor;


};

}
}
}