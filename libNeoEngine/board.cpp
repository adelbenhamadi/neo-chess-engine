#include "board.h"
#include "helpers.h"



//construct board from fen string
Board::Board(std::string fen) :_stateStack()
{
	/* Fen notation syntax
	* <FEN> ::= <Piece Placement>
		   ' ' <Side to move>
		   ' ' <Castling ability>
		   ' ' <En passant target square>
		   ' ' <Halfmove clock>
		   ' ' <Fullmove counter>
	*/

	/*initialize board*/
	BoardState* newSt = new BoardState;
	_state = _stateStack.push(*newSt);
	init();
	std::stringstream ss(fen);
	std::vector<std::string> segments = Helpers::splitStringstream(ss, ' ');
	ss = std::stringstream(segments[0]);
	std::vector<std::string> piecePlacements = Helpers::splitStringstream(ss, '/');

	//setup board reading piece placements
	setup(piecePlacements);

	//setup current side:
	assert(segments[1][0] == 'w' || segments[1][0] == 'b');
	_currentSide = segments[1][0] == 'w' ? Side::WHITE : Side::BLACK;
	//setup castling rights
	_state->castlingRight = CastlingRight(segments[2]);

	/*update castle paths,masks etc..*/
	for (Side side : { WHITE, BLACK }) {
		Square kfrom = getPieceSquare(side, KING);
		_castlingRightsMask[kfrom] |= _state->castlingRight.get(side);
		for (CastlingRight::ECastlingRight cr : {CastlingRight::KING_SIDE, CastlingRight::QUEEN_SIDE}) {
			if (_state->castlingRight.get(side) & cr) {
				Square rfrom = square::relative_square(side, cr == CastlingRight::QUEEN_SIDE ? SQ_A1 : SQ_H1);
				if (pieceAt(rfrom) == NO_PIECE) continue;
				Square kto = square::relative_square(side, cr == CastlingRight::KING_SIDE ? SQ_G1 : SQ_C1);
				Square rto = square::relative_square(side, cr == CastlingRight::QUEEN_SIDE ? SQ_D1 : SQ_F1);
				_castlingRightsMask[rfrom] |= cr;
				_castlingRookSquare[side][cr] = rfrom;

				_castlingPath[side][cr] = (between_bb(rfrom, rto) | between_bb(kfrom, kto) | rto | kto)
					& ~(kfrom | rfrom);

			}
		}
	}

	//setup history for en passant target square: segments[3][0] is the position "behind" the moved pawn
	if ((segments[3][0] != '-') && (_historyMoves.size() == 0))
	{
		update_ep_history(_currentSide, squareIndexFromString(segments[3]));
	}
	else
	{
		_enPassantTargetSquares.push_back(Square::SQ_OUT);
	}
	update_pinnedBB();
	/*
	halfmove clock counter for fifty-moves rule, starts at 0*/
	_halfmoveClockCount = segments.size() >= 5 ? std::stoi(segments[4]) : 0;
	/*full move counter: if not present always starts at 1*/
	_plyCount = segments.size() >= 6 ? std::stoi(segments[5]) : 1;
}

const std::string Board::fen() const
{
	return std::string();
}


/*force update all aux BB from 12 to 20*/
void Board::update_auxiliary_BB() {
	// reset per piece occupancies BB : from 12 to 17
	//memset(&_boards[14U], 0ULL, 6 * sizeof(Bitboard));

	// update both sides per piece occupancies
	for (unsigned int pc = W_PAWNS; pc <= W_KINGS; pc++)
	{
		_boards[pc + 12u] = _boards[pc] | _boards[pc + 6u]; //for white side
	}
	// update both sides per piece type occupancies
	_boards[ALL_W_PIECES] =
		_boards[W_PAWNS] |
		_boards[W_KNIGHTS] |
		_boards[W_BISHOPS] |
		_boards[W_ROOKS] |
		_boards[W_QUEENS] |
		_boards[W_KINGS]
		;
	_boards[ALL_B_PIECES] =
		_boards[B_PAWNS] |
		_boards[B_KNIGHTS] |
		_boards[B_BISHOPS] |
		_boards[B_ROOKS] |
		_boards[B_QUEENS] |
		_boards[B_KINGS]
		;
	//update all pieces occupancy
	_boards[ALL_PIECES] = _boards[ALL_W_PIECES] | _boards[ALL_B_PIECES];

}

void Board::update_castle_rights( Square from, Square to)
{
/*
	for (auto side : { WHITE,BLACK }) {
		auto cr = _state->castlingRight.get(side);
		if (cr && (_castlingRightsMask[from] | _castlingRightsMask[to]))
		{
			_zkey ^= ZHash::castleKeys[side][cr];
			cr &= ~(_castlingRightsMask[from] | _castlingRightsMask[to]);
			_state->castlingRight.set(side, cr);
			_zkey ^= ZHash::castleKeys[side][cr];
		}
	}
	return;
	*/
	for (auto side : { WHITE,BLACK }) {
		auto cr = _state->castlingRight.get(side);
		//if any castle is still possible, update rights
		if (cr ^ CastlingRight::NO_CASTLE) {


			if ((from == SQ_E1 && side == WHITE) || (from == SQ_E8 && side == BLACK)) {
				_state->castlingRight.set(side, CastlingRight::NO_CASTLE);
			}
			else if ((from == SQ_A1 && side == WHITE) || (from == SQ_A8 && side == BLACK)) {
				_state->castlingRight.remove(side, CastlingRight::QUEEN_SIDE);
			}
			else if ((from == SQ_H1 && side == WHITE) || (from == SQ_H8 && side == BLACK)) {
				_state->castlingRight.remove(side, CastlingRight::KING_SIDE);
			}

			if ((to == SQ_A1 && side == WHITE) || (to == SQ_A8 && side == BLACK)) {
				_state->castlingRight.remove(side, CastlingRight::QUEEN_SIDE);
			}
			else if ((to == SQ_H1 && side == WHITE) || (to == SQ_H8 && side == BLACK)) {
				_state->castlingRight.remove(side, CastlingRight::KING_SIDE);
			}

			/*
					switch (movedPiece)
					{
					case KING:
						_castlingRight.set(side, CastlingRight::NO_CASTLE);
						break;
					case ROOK:
						if ((from == SQ_A1 && side == WHITE) || (from == SQ_A8 && side == BLACK)) {
							_castlingRight.remove(side, CastlingRight::QUEEN_SIDE);
						}
						else if ((from == SQ_H1 && side == WHITE) || (from == SQ_H8 && side == BLACK)) {
							_castlingRight.remove(side, CastlingRight::KING_SIDE);
						}
						break;
					default: return;
						break;
					}
					*/

			//unset old castle keys
			_zkey ^= ZHash::castleKeys[side][cr];
			//set castle keys
			_zkey ^= ZHash::castleKeys[side][_state->castlingRight.get(side)];
		}
	
		
	}
	
	
}

bool Board::doMove(Move move, BoardState& newSt)
{
	pushState(newSt);

	Square from = move.from_sq();
	Square to = move.to_sq();
	auto movingPiece = pieceAt(from);// move.piece_type()
	auto promType = move.promoted_type();
	bool isPromotion = promType != EPieceType::EMPTY;
	auto moveType = move.type();
	Side oppositeSide = (Side)(1 ^ _currentSide);
	auto captured =(EPiece) move.captured();
	//auto captured =  moveType == EP_CAPTURE ? piece::make_piece(PAWN, oppositeSide) : pieceAt(to);

	/*need some checks before moving*/
	assert(captured == (moveType == EP_CAPTURE ? piece::make_piece(PAWN, oppositeSide) : pieceAt(to)) );
	assert(captured == NO_PIECE || piece::side_of(captured) == oppositeSide );
	assert(captured == NO_PIECE || piece::type_of(captured) != KING);
	assert(movingPiece != NO_PIECE && piece::side_of(movingPiece) == _currentSide);
	assert( (isPromotion || moveType == CAPTURE || pieceAt(to) == NO_PIECE) && "Bad move target!");
	assert((captured == NO_PIECE || moveType == EP_CAPTURE || pieceAt(to) != NO_PIECE) && "Bad capture: target is empty!");

	//handle all different types of move: CASTLING,CAPTURE,EP_CAPTURE and DPP	
	{
		//handle castling
		if (moveType == CASTLING) {
			//assert(from == SQ_E1);
			ASSERT_IF(do_castle(_currentSide, from, to, false));
		}
		//handle promotion
		else if (isPromotion) {
			ASSERT_IF(do_promote(_currentSide, from, to, (EPieceType)promType, false));
			
		}
		else {

			if (captured != NO_PIECE) {
				if (moveType == EP_CAPTURE) {
					//sq: position of opponent dpp which need to be removed by ep
					auto sq = (Square)(to - (_currentSide == WHITE ? 8 : -8));
					assert(movingPiece == piece::make_piece(PAWN, _currentSide));
					assert(to == _state->epSquare);
					assert(square::relative_rank(_currentSide, to) == RANK_6);
					assert(pieceAt(to) == NO_PIECE);
					assert(pieceAt(sq) == piece::make_piece(PAWN, oppositeSide));
					//update opposite side BB: remove captured pawn 
					ASSERT_IF(removePiece((Side)oppositeSide, PAWN, sq));
					_state->epSquare = sq;

				}
				else {
					ASSERT_IF(removePiece((Side)oppositeSide, to));
				}

			}
			
			if (moveType == DOUBLE_PAWN_PUSH) {
				Square ep = (Square)(to - square::pawn_push_dir(_currentSide));
				//if it can be captured by ep rule
				//if ((tables::SquareBB[to - 1] | tables::SquareBB[to + 1]) & getBB(oppositeSide, PAWN)) {
				if (pawn_attacks_bb(_currentSide, ep) & getBB(oppositeSide, PAWN)) {
					//set epSquare
					_state->epSquare = ep;
					//update ep hash keys
					_zkey ^= ZHash::epKeys[square::file_of(_state->epSquare)];
				}
				else {
					// Reset ep square
					resetEp();
				}

			}

			movePiece(_currentSide, from, to);
		}
		if (moveType != DOUBLE_PAWN_PUSH)
			resetEp();
		//update castle rights for both sides!
		update_castle_rights( from, to);

		_state->captured = captured;

	}
	

	//inc _plyCount only when BLACK is playing  
	_plyCount += _currentSide;
	_halfmoveClockCount++;
	// update fifty move rule counter
	if (piece::type_of(movingPiece) == PAWN || moveType == CAPTURE) {
		_fiftyCount = 0;
	}
	else {
		_fiftyCount++;
	}


	/**/
	update_check_info();
	// hash side keys
	_zkey ^= ZHash::sideKey;
	//revert current side for next move
	_currentSide = (Side)(oppositeSide);

	//update history
	_historyMoves.push_back(move);

	return true;
}

bool Board::undoMove(Move move)
{
	Square from = move.from_sq();
	Square to = move.to_sq();
	EPiece piece = pieceAt(to);
	auto pt = piece::type_of(piece);
	auto promType = move.promoted_type();
	auto moveType = move.type();
	//auto captured = (EPiece)move.captured();// pieceAt(to);
	assert(pieceAt(from) == NO_PIECE || moveType == CASTLING);
	assert(piece::type_of(_state->captured) != KING);
	//flip sides
	Side oppositeSide = _currentSide;
	_currentSide = (Side)(1 ^ _currentSide);
	/*undo all types of move*/
	{
		//undo castling
		if (moveType == CASTLING) {
			do_castle(_currentSide, from, to, true);
		}
		//undo promotion
		else if (promType != EPieceType::EMPTY) {
			do_promote(_currentSide, from, to, (EPieceType)promType, true);
		}
		else {
			if (move.isDoublePawnPush()) {
				//f: file just behind the double pushed pawn
				unsigned f = to + (_currentSide == WHITE ? 8 : -8);
				//undo epKeys
				_zkey ^= ZHash::epKeys[f];
			}
			movePiece(_currentSide, to, from);
			if (_state->captured != NO_PIECE)
			{
				Square capsq = to;

				if (moveType == EMoveType::EP_CAPTURE)
				{
					capsq -= (Direction)square::pawn_push_dir(_currentSide);

					assert(piece::type_of(piece) == PAWN);
					assert(to == _state->previous->epSquare);
					assert(square::relative_rank(_currentSide, to) == RANK_6);
					assert(pieceAt(capsq) == NO_PIECE);
					assert(_state->captured == piece::make_piece(PAWN, oppositeSide));
				}
				// Restore the captured piece
				addPiece(_state->captured, capsq);
			}

		}
		//restore previous castle rights only if KING or ROOK has moved
		/*if (pt == KING || pt == ROOK) {
			if (!move.isCastling() && _state->cr.get(_currentSide) != _state->castlingRight.get(_currentSide)) {
				_state->castlingRight = _state->cr;
			}
		}*/
	}
	/*restore Board previous state*/
	/*pop state*/
	popState();

	_historyMoves.pop_back();

	return true;
}

