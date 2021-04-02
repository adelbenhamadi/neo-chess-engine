#pragma once
#include <vector>
#include <string>
#include <map>

#include "types.h"


namespace piece {
    extern const  std::map<char, EPiece> piecesCodesMap;
   
    constexpr EPiece charToCode(char pc) {
        return piecesCodesMap.at(pc);
    }
    constexpr char codeToChar(EPiece c) {
        for (auto cod : piecesCodesMap)
            if (cod.second == c) {
                return cod.first;
            }

        return '?';
    }
    constexpr EPieceType type_of(EPiece p) { return (EPieceType)(p % 6); }
    constexpr EPiece make_piece(EPieceType pt, Side side) {
        return EPiece(pt + 6 * side);
    }
}

