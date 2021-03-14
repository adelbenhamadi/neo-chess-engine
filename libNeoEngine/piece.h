#pragma once
#include <vector>
#include <string>
#include <map>

#include "square.h"

enum EPieceType: unsigned int
{
    PAWN = 0,
    KNIGHT = 1,
    BISHOP = 2,
    ROOK = 3,
    QUEEN = 4,
    KING = 5,
    EMPTY = 6,
 
};
enum EPieceWeight: unsigned int { KING_W = 999, QUEEN_W = 99, ROOK_W = 50, KNIGHT_W = 32, BISHOP_W = 33, PAWN_W = 10,};
enum EPieceCode : unsigned int
{
    W_PAWN = 0,
    W_KNIGHT = 1,
    W_BISHOP = 2,
    W_ROOK = 3,
    W_QUEEN = 4,
    W_KING = 5,
    B_PAWN = 6,
    B_KNIGHT = 7,
    B_BISHOP = 8,
    B_ROOK = 9,
    B_QUEEN = 10,
    B_KING = 11,
    NO_PIECE = 14

};


class Piece {
    EPieceCode _code;
	EPieceType _type;
	Square _square;
    unsigned int _weight;
	Piece(const EPieceType t);

    static const  std::map<char, EPieceCode> _piecesCodesMap;
public:
	EPieceType getType() { return _type; }
	EPieceCode getCode() { return _code; }
	Square getSquare() { return _square; }
    unsigned int getValue() { return _weight; }
    static EPieceCode charToCode(char pc) {
        return _piecesCodesMap.at(pc);
    }
};