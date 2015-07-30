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

typedef u_char byte;

class Endpoint{
public:
enum Type {
    IPv4,
    IPv6
};

Endpoint(in_port_t port = 0, Endpoint::Type addrType = Endpoint::IPv4 );
Endpoint(Endpoint::Type addrType, const sockaddr* endpoint);
Endpoint(const char*, in_port_t port);

const char* getAddressStr();
in_port_t getPort() const;
bool isErrorExist() const;
int getAFtype() const;
Endpoint::Type getType() const;
const sockaddr* getRawStruct() const;

private:
char addrStr_[INET6_ADDRSTRLEN] = {0};
union{
    sockaddr_in addrV4;
    sockaddr_in6 addrV6;
} addrData_;
Type addrType_;
in_port_t port_;
bool errorExist_;
};

class TCPClient{
public:
    TCPClient();
    TCPClient(int socket, const Endpoint& endpoint);
    size_t write(byte *buf, size_t size);
    size_t read(byte *buf, size_t size);
    void close();

private:
    int socket_;
    bool enable_;
    Endpoint endpoint_;
};

class TCPListener{
public:
    const static int defQueueSize = 10;
    TCPListener(const Endpoint& endpoint = Endpoint());
    bool listen(int queueSize = defQueueSize);
    TCPClient accept();

 private:
    int socket_;
    Endpoint endpoint_;
    bool errorExist_;
};


#endif // NETLIB_H
