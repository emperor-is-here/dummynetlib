#include <stdio.h>
#include "netlib.h"

using namespace std;

int main()
{
    Address g("example.com", 80);
    printf("%s\n", g.getAddress());
}

