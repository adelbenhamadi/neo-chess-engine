#include <iostream>

#include "../libNeoEngine/uci.h"

int main()
{
    bitboards::setup();
    Uci uci;
    uci.init();
    uci.printOptions();
    uci.listen();
    uci.clean();
    return 0;
}

