#include <stdio.h>
#include "netlib.h"

int main()
{
    TCPListener ls(Endpoint(27015));
    byte buf[255];
    ls.listen();
    while (true)
    {
        printf("%d\n", ls.endpoint_.getPort());
        TCPClient cl = ls.accept();
        if (cl.isValid())
        {
            int recived = cl.read(buf, 255);
            printf("recived(%d): %s\n", recived, buf);
            cl.write(buf, recived);
            printf("recived(%d): %s\n", recived, buf);
            cl.write(buf, recived);
            cl.close();
        }
    }
}

