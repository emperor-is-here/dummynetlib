#include <stdio.h>
#include "netlib.h"

int main()
{
    TCPListener ls(Endpoint(27015));
    byte buf[255];
    ls.listen();
    while (true)
    {
        TCPClient cl = ls.accept();
        int recived = cl.read(buf, 255);
        printf("recived(%d): %s\n", recived, buf);
        cl.write(buf, recived);
        cl.close();
    }
}

