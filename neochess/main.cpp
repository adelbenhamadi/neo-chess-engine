#include <iostream>

#include "../libNeoEngine/uci.h"

int main()
{
    Uci uci;
    uci.init();
    uci.printOptions();
    uci.listen();
}

