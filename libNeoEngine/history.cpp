#include "history.h"

void History::clear(const bool bAll)
{
    _counterMoves.fill(0);
    _mainHistory.fill(0);
    _captureHistory.fill(0);

}
