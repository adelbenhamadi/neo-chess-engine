#include "piece.h"


 const  std::map<char, EPieceCode> Piece::_piecesCodesMap = {
    //white pieces
    {'K', EPieceCode::W_KING } ,
    {'Q', EPieceCode::W_KING } ,
    {'R', EPieceCode::W_KING } ,
    {'N', EPieceCode::W_KING } ,
    {'B', EPieceCode::W_KING } ,
    {'P', EPieceCode::W_KING } ,
    //black pieces
    {'k', EPieceCode::W_KING } ,
    {'q', EPieceCode::W_KING } ,
    {'r', EPieceCode::W_KING } ,
    {'n', EPieceCode::W_KING } ,
    {'b', EPieceCode::W_KING } ,
    {'p', EPieceCode::W_KING }

};