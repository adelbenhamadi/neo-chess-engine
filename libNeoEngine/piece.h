#pragma once
#include <vector>
#include <string>
#include <map>

#include "types.h"


namespace piece {
    extern const  std::map<EPieceType, unsigned> piecesValues;
    extern const  std::map<char, EPiece> piecesCodesMap;
   
    const EPiece charToCode(char pc);
    const char codeToChar(EPiece c);
    constexpr EPieceType type_of(EPiece p) { return (EPieceType)(p % 6); }
    constexpr EPiece make_piece(EPieceType pt, Side side) {
        return EPiece(pt + 6 * side);
    }
    constexpr Side side_of(EPiece p){
        assert(p >= W_PAWN && p <= B_KING);
        return Side(p / 6);
    }
    const unsigned typeToValue(EPieceType pt);;
}

