#ifndef NETLIB_H
#define NETLIB_H

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <arpa/inet.h>
#include <netinet/in.h>
#include <netdb.h>
#include <unistd.h>

class Address{
public:
    enum Type {
        IPv4,
        IPv6
    };
Address(Address::Type addrType = Address::IPv4, in_port_t port = 0);
Address(const char*, in_port_t port);
const char* getAddress() const;
const in_port_t getPort() const;
private:
char addr_str_[INET6_ADDRSTRLEN] = {0};
union{
    sockaddr_in addrV4;
    sockaddr_in6 addrV6;
}addr_data_;
Type addr_type_;
in_port_t port_;
bool error_exist;
};


#endif // NETLIB_H
