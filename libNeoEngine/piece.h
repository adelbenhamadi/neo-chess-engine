#pragma once
#include <vector>
#include <string>
#include <map>

#include "types.h"


namespace piece {
    extern const  std::map<char, EPiece> piecesCodesMap;
   
    const EPiece charToCode(char pc);
    const char codeToChar(EPiece c);
    constexpr EPieceType type_of(EPiece p) { return (EPieceType)(p % 6); }
    constexpr EPiece make_piece(EPieceType pt, Side side) {
        return EPiece(pt + 6 * side);
    }
}

