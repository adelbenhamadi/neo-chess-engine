#include "piece.h"
#include "zebrinsky_hash.h"

namespace piece {

    const  std::map<char, EPiece> piecesCodesMap = {
        //white pieces
        {'P', EPiece::W_PAWN } ,
        {'B', EPiece::W_BISHOP } ,
        {'R', EPiece::W_ROOK } ,
        {'N', EPiece::W_KNIGHT } ,
        {'Q', EPiece::W_QUEEN } ,
        {'K', EPiece::W_KING } ,
        //black pieces
        {'p', EPiece::B_PAWN},
        {'b', EPiece::B_BISHOP } ,
        {'r', EPiece::B_ROOK } ,
        {'n', EPiece::B_KNIGHT } ,
        {'q', EPiece::B_QUEEN } ,
        {'k', EPiece::B_KING } ,

        {'-', EPiece::NO_PIECE}

    };
    ZKey hashKeys[ PIECE_TYPE_NB][ SQUARE_NB];
    ZKey epHashKeys[SQUARE_NB];
   
};


