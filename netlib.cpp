#include "netlib.h"


TCPClient::TCPClient():
valid_(false)
{
}

TCPClient::TCPClient(int socket, const Endpoint& endpoint):
socket_(socket),
valid_(true),
endpoint_(endpoint)
{
}

int TCPClient::send(byte *buf, size_t size, size_t* sent = nullptr)
{
    if (!valid_)
        return false;
    int bsent = 0;
    int bleft = size;
    int n;
    while (bsent < size) // send pice by pice till end or error
    {
        n = send(socket_, buf + bsent, bleft);
        if (n == -1) // en error
            break;
        bsent += n;
        bleft -= n;
    }
    if (sent != nullptr)
        *sent = bsent;
    return (n == -1) ? false : true;
}

bool TCPClient::recive(byte *buf, size_t size)
{
    if (!valid_)
        return false;
    return recv(socket_, buf, size, 0);
}

int TCPClient::bytesAvialable()
{
    int count;
    ioctl(fd, FIONREAD, &count);
    return count;
}

void TCPClient::close()
{
    ::close(socket_);
    valid_ = false;
}

bool TCPClient::isValid() const
{
    return valid_;
}


/* TCPListener */

TCPListener::TCPListener(const Endpoint &endpoint):
endpoint_(endpoint),
errorExist_(false)
{
    if (endpoint.isErrorExist())
    {
        errorExist_ = true;
        return;
    }
    if ((socket_ = socket(endpoint.getAFtype(), SOCK_STREAM, 0)) == -1)
    {
        errorExist_ = true;
    }
    if ((bind(socket_, endpoint.getRawStruct(), sizeof (sockaddr))) == -1)
    {
        errorExist_ = true;
    }
}

bool TCPListener::listen(int queueSize)
{
    if (errorExist_)
        return false;
    if (::listen(socket_, queueSize) == -1)
    {
        errorExist_ = true;
        return false;
    }
    return true;
}


TCPClient TCPListener::accept()
{
    sockaddr clientAddr;
    int clientSocket;
    socklen_t structSize = sizeof (sockaddr);
    if ((clientSocket = ::accept(socket_, &clientAddr, &structSize)) == -1)
    {
        return TCPClient();
    }
    return TCPClient(clientSocket, Endpoint(endpoint_.getType(), &clientAddr));
}


/* Endpoint */

Endpoint::Type Endpoint::getType() const
{
    return addrType_;
}

const sockaddr* Endpoint::getRawStruct() const
{
    return reinterpret_cast<const sockaddr*>(&addrData_);
}

int Endpoint::getAFtype() const
{
    /* AF_INET for IPv4 and AF_INET6 for IPv6 */
    return addrType_ == IPv4 ? AF_INET : AF_INET6;
}

bool Endpoint::isErrorExist() const
{
    return errorExist_;
}

const char* Endpoint::getAddressStr()
{
    if (addrStr_[0] == '\0')
    {
        if (addrType_ == Endpoint::IPv4)
        {
           if (!inet_ntop(AF_INET, &addrData_.addrV4.sin_addr, addrStr_, INET6_ADDRSTRLEN))
           {
               errorExist_ = true;
               return nullptr;
           }
        } else { /* IPv6 */
           if(!inet_ntop(AF_INET6, &addrData_.addrV6.sin6_addr, addrStr_, INET6_ADDRSTRLEN))
           {
               errorExist_ = true;
               return nullptr;
           }
        }
    }
    return addrStr_;
}

in_port_t Endpoint::getPort() const
{
    return port_;
}

/* empty address for listening */
Endpoint::Endpoint(in_port_t port, Endpoint::Type addrType):
addrType_(addrType),
port_(port),
errorExist_(false)
{
    if (addrType == Endpoint::IPv4)
    {
        addrData_.addrV4.sin_addr.s_addr = INADDR_ANY;
        addrData_.addrV4.sin_family = AF_INET;
        addrData_.addrV4.sin_port = htons(port);
        memset(&addrData_.addrV4.sin_zero, 0, sizeof addrData_.addrV4.sin_zero);
    } else { /* IPv6 */
        addrData_.addrV6.sin6_addr = in6addr_any;
        addrData_.addrV6.sin6_family = AF_INET6;
        addrData_.addrV6.sin6_port = htons(port);
        addrData_.addrV6.sin6_flowinfo = 0;
    }
}

/* convert raw sockaddr to Endpoint */
Endpoint::Endpoint(Endpoint::Type addrType, const sockaddr *addr):
addrType_(addrType),
errorExist_(false)
{
    memcpy(&addrData_, addr, sizeof(sockaddr));
    if (addrType == IPv4)
    {
        sockaddr_in* addrV4 = reinterpret_cast<sockaddr_in*>(&addr);
        port_ = ntohs(addrV4->sin_port);
    } else { /* IPv6 */
        sockaddr_in6* addrV6 = reinterpret_cast<sockaddr_in6*>(&addr);
        port_ = ntohs(addrV6->sin6_port);
    }
}

/* address for connect */
Endpoint::Endpoint(const char* hostName, in_port_t port):
port_(port),
errorExist_(false)
{
    hostent *hostInfo;
    hostInfo = gethostbyname(hostName);
    if (hostInfo == NULL)
    {
        errorExist_ = true;
        return;
    }
    if (hostInfo->h_addrtype == AF_INET) /* IPv4 */
    {
        addrType_ = IPv4;
        memcpy(&addrData_.addrV4.sin_addr.s_addr, hostInfo->h_addr, sizeof (in_addr_t));
        addrData_.addrV4.sin_family = AF_INET;
        addrData_.addrV4.sin_port = htons(port);
        memset(&addrData_.addrV4.sin_zero, 0, sizeof addrData_.addrV4.sin_zero);
    } else {
        addrType_ = IPv6;
        memcpy(&addrData_.addrV6.sin6_addr, hostInfo->h_addr, sizeof (in6_addr));
        addrData_.addrV6.sin6_family = AF_INET6;
        addrData_.addrV6.sin6_port = htons(port);
        addrData_.addrV6.sin6_flowinfo = 0;
    }
}
