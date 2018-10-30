#include <cassert>
#include <cstdint>
#include <net/if.h>
#include <string>

namespace tunnel
{
namespace linux
{
namespace detail
{
// Bug in the Linux headers.
// https://bugzilla.redhat.com/show_bug.cgi?id=1300256
std::string if_indextoname(uint32_t index)
{

    char ifname[IF_NAMESIZE];

    ::if_indextoname(index, ifname);

    return ifname;
}
}
}
}
