#include "netlib.h"


const char* Address::getAddress() const
{
    return addr_str_;
}

const in_port_t Address::getPort() const
{
    return port_;
}

/* empty address for listening */
Address::Address(Address::Type addrType, in_port_t port):
addr_type_(addrType),
port_(port),
error_exist(false)
{
    if (addrType == Address::IPv4)
    {
        addr_data_.addrV4.sin_addr.s_addr = INADDR_ANY;
        addr_data_.addrV4.sin_family = AF_INET;
        addr_data_.addrV4.sin_port = htons(port);
        memset(&addr_data_.addrV4.sin_zero, 0, sizeof addr_data_.addrV4.sin_zero);
    } else { /* IPv6 */
        addr_data_.addrV6.sin6_addr = in6addr_any;
        addr_data_.addrV6.sin6_family = AF_INET6;
        addr_data_.addrV6.sin6_port = htons(port);
        addr_data_.addrV6.sin6_flowinfo = 0;
    }
}

/* address for connect */
Address::Address(const char* hostName, in_port_t port):
port_(port),
error_exist(false)
{
    hostent *hostInfo;
    hostInfo = gethostbyname(hostName);
    if (hostInfo == NULL)
    {
        error_exist = true;
        return;
    }
    inet_ntop(hostInfo->h_addrtype, hostInfo->h_addr, addr_str_, INET6_ADDRSTRLEN);
    if (hostInfo->h_addrtype == AF_INET) /* IPv4 */
    {
        addr_type_ = IPv4;
        memcpy(&addr_data_.addrV4.sin_addr.s_addr, hostInfo->h_addr, sizeof (in_addr_t));
        addr_data_.addrV4.sin_family = AF_INET;
        addr_data_.addrV4.sin_port = htons(port);
        memset(&addr_data_.addrV4.sin_zero, 0, sizeof addr_data_.addrV4.sin_zero);
    } else {
        addr_type_ = IPv6;
        memcpy(&addr_data_.addrV6.sin6_addr, hostInfo->h_addr, sizeof (in6_addr));
        addr_data_.addrV6.sin6_family = AF_INET6;
        addr_data_.addrV6.sin6_port = htons(port);
        addr_data_.addrV6.sin6_flowinfo = 0;
    }
}
