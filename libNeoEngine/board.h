#pragma once
#include <iostream>
#include <sstream>
#include <vector>
#include <string>

#include <algorithm>

#include "types.h"
#include "piece.h"
#include "bitboard.h"
#include "move.h"
#include "helpers.h"
#include "zebrinsky_hash.h"

struct BoardState {	
	//CastlingRight cr;
	unsigned int  halfmoveClockCount = 0;
	unsigned int plyCount = 0;
	unsigned int fiftyCount = 0;
	unsigned int repetionCount = 0;
	//Side side;
	ZKey zkey;
	Square epSquare = SQ_OUT;
	EPiece captured = NO_PIECE;
	/*holds castling abilities for both sides*/
	CastlingRight castlingRight;
	Bitboard pinnedPieces[2/*sides*/];
	//BB for per square attackers
	Bitboard attakersTo[SQUARE_NB];

	Bitboard checkSquares[PIECE_TYPE_NB];
	Bitboard pinners[2/*sides*/];
	BoardState* previous;

};
class StateStack {
	std::vector<BoardState*> _stack;

public:

	BoardState* push(BoardState& st) {
		BoardState* prevSt =_stack.size()? _stack.back(): nullptr;
		_stack.push_back(&st);
		BoardState* newSt = _stack.back();
		if(prevSt)
			memcpy(newSt, prevSt,sizeof(BoardState));
		newSt->previous = prevSt;
		return newSt;
	}
	BoardState* pop() {
		BoardState* prev = _stack.back()->previous;
		_stack.pop_back();
		return prev;
	}
	BoardState* current() {
		return _stack.back();
	}
	BoardState* previous() {
		if (_stack.size()>1) {
			return _stack.at( _stack.size()-2);
		}
		return nullptr;
	}
};
class Board {
	std::array<Bitboard, BITBORD_NB> _boards;

	//all board pieces per squere map
	EPiece _piecesBoard[SQUARE_NB];

	//Bitboard _kingAttackers[2/*sides*/];
	unsigned _piecesCounter[2/*sides*/][PIECE_TYPE_NB];

	
	/*number of halfmoves since the last capture or pawn advance: used in the 50-move rule*/
	unsigned int  _halfmoveClockCount;
	/*ply count: number of full moves (ply) : starts at 1, incremented only after BLACK move*/
	unsigned int _plyCount;
	/*vector<Move> for full history moves*/
	std::vector<Move> _historyMoves;
	/*fifty move rule counter*/
	unsigned int _fiftyCount;
	/*vector<Square> for en passant target squares*/
	std::vector<Square> _enPassantTargetSquares;


	int _castlingRightsMask[SQUARE_NB];
	Square _castlingRookSquare[2/*sides*/][CastlingRight::CASTLING_NB] = { {SQ_OUT} };
	Bitboard _castlingPath[2/*sides*/][CastlingRight::CASTLING_NB] = { { 0 } };

	/*current player: initialized from fen*/
	Side _currentSide;
	/*current hash key for this board*/
	ZKey _zkey;
	/*bord state*/
	BoardState* _state;
	StateStack _stateStack ;
public:
	//construct Board from fen
	Board(std::string fen);
	//new board constructor
	Board() : Board("rnbqkbnr/pppppppp/8/8/8/8/PPPPPPPP/RNBQKBNR w KQkq - 0 1") {}

	inline void init() {
		for (int sq = SQ_A1; sq <= SQ_H8; sq++) {
			_piecesBoard[sq] = NO_PIECE;
			//TODO initialize hash keys
			_zkey = 0;
		}
		for (int p = PAWN; p <= KING; p++) {
			_piecesCounter[WHITE][p] = 0;
			_piecesCounter[BLACK][p] = 0;

		}
		for (int bb = 0; bb < BITBORD_NB;bb++) {
			_boards[bb] = 0;
		}
	}
	inline void updatePiecesCounter() {
		for (int p = PAWN; p <= KING; p++) {
			_piecesCounter[WHITE][p] = population::populationCount(getBB(WHITE, (EPieceType)p));
			_piecesCounter[BLACK][p] = population::populationCount(getBB(BLACK, (EPieceType)p));
		}
	}
	/*return fen notation of current board position*/
	const std::string fen() const;
	constexpr Side side() const { return _currentSide; }
	inline std::vector<Move> historyMoves() const { 
		return _historyMoves;
	};

	/*setup board*/
	void setup(std::vector<std::string> placementSegments) {
		assert(placementSegments.size() == 8);
		unsigned int rank = 7;
		for (auto segment : placementSegments) {
			setSegment(segment, rank);
			rank--;
		}
		
	}
	inline void pushState(BoardState& st ) {
		/*set new state*/
		//_state = _stateStack.push(st);
		memcpy(&st, _state, sizeof(BoardState));
		st.previous = _state;
		_state = &st;
	}
	inline void popState() {
		//_state = _stateStack.pop();
		_state = _state->previous;
	}

	/* BitBoard getters, there is 22 bitboards indexed from 0 to 21 */
	inline Bitboard getAllBB(const EPieceType pt) const { 
		return _boards[pt + 12U];
		//return _boards[pt] | _boards[pt+6]; 
	};

	inline Bitboard getAllBB(const EPieceType pt1, const EPieceType pt2) const { 
		return _boards[pt1 + 12U] | _boards[pt2 + 12U];
		//return _boards[pt1] | _boards[pt2] | _boards[pt1 +6] | _boards[pt2+6]; 
	};

	inline Bitboard getAllBB(const Side side) const { return _boards[ALL_W_PIECES + side]; }

	inline Bitboard getAllBB() const { return _boards[ALL_PIECES]; }

	inline Bitboard getUnoccupiedBB()const { return ~_boards[ALL_PIECES]; }

	inline Bitboard getUnoccupiedBB(const Side side)const { return ~_boards[ALL_W_PIECES + side]; }

	inline Bitboard getBB(const Side side, const EPieceType pt) const { return _boards[pt + 6U * side]; };

	inline Bitboard getBB(const EPiece pc) const { return _boards[pc]; };

	inline Bitboard getBB(const Side side, const EPieceType pt1, const EPieceType pt2) const { return _boards[pt1 + 6U * side] | _boards[pt2 + 6U * side]; };

	inline Bitboard getBB(const EBitboardType btype) const { return _boards[btype]; }

	inline Bitboard getBB(const EBitboardType btype1, const EBitboardType btype2) const { return _boards[btype1] | _boards[btype2]; }

	inline Bitboard getExcludeKingPawnBB(const Side side) const {
		return _boards[KNIGHT + 6U * side] | _boards[BISHOP + 6U * side] | _boards[ROOK + 6U * side] | _boards[QUEEN + 6U * side];
	};

	inline bool empty(Square s) const {
		return pieceAt(s) == NO_PIECE;
	}


	inline bool capture_or_promotion(Move m) const {
		assert(m.isOk());
		auto typ = m.type();
		return typ == CAPTURE || typ == EP_CAPTURE || typ == PROMOTION;
		//return  typ!= QUIET ? typ == EP_CAPTURE : !empty(m.to_sq());
	}

	inline bool capture(Move m) const {
		assert(m.isOk());
		return ((!empty(m.to_sq())) || m.type() == EP_CAPTURE);
	}

	/*return captured piece for a given move*/
	inline EPiece captured( Move m) {
		auto pc = pieceAt(m.from_sq());
		assert(m.isOk() && pc != NO_PIECE);
		Side side = piece::side_of(pc);
		return (m.type() == EP_CAPTURE ? piece::make_piece(PAWN, (Side)(1 ^ side)) : pieceAt(m.to_sq()));
	}

	inline EPiece captured_piece() const {
		return _state->captured;
	}

	inline Square ep_square() const {
		return _state->epSquare;
	}
	inline BoardState* state() const {
		return _state;
	}
	inline void update_pinnedBB() {
		_state->pinnedPieces[WHITE] = slider_blockers(getAllBB(BLACK), getPieceSquare(WHITE, KING), _state->pinners[BLACK]);
		_state->pinnedPieces[BLACK] = slider_blockers(getAllBB(WHITE), getPieceSquare(BLACK, KING), _state->pinners[WHITE]);
	}
	inline void update_check_info() {
		
		update_pinnedBB();
		/*update checksquares*/
		Square ksq = getPieceSquare((Side)(1 ^ _currentSide), KING);

		_state->checkSquares[PAWN] = pawn_attacks_bb((Side)(1 ^ _currentSide), ksq);
		_state->checkSquares[KNIGHT] = attacks_bb<KNIGHT>(ksq);
		_state->checkSquares[BISHOP] = attacks_bb(BISHOP, ksq, getAllBB());
		_state->checkSquares[ROOK] = attacks_bb(ROOK, ksq, getAllBB());
		_state->checkSquares[QUEEN] = _state->checkSquares[BISHOP] | _state->checkSquares[ROOK];
		_state->checkSquares[KING] = 0;
	}
	/*BB of side pinned pieces*/
	inline Bitboard pinnedPiecesBB(const Side side) const {
		return _state->pinnedPieces[side];
	}

	/*check if the piece on a given square is pinned:
	ie exposes side king to check if moved*/
	inline bool isPinnedSquare(const Side side, Square sq) const {
		return _state->pinnedPieces[side] & sq;
	}
	inline Bitboard attackers_to(Square s) const{
		return attackers_to(s, getAllBB());
	}
	inline Bitboard attackers_to(Square s, Bitboard occ) const {

		Bitboard b = (tables::PawnAttacks[WHITE][s] & getBB(BLACK, PAWN));
		b |=( tables::PawnAttacks[BLACK][s] & getBB(WHITE, PAWN)); 
		b |= (tables::PseudoAttacks[KNIGHT][s] & getAllBB(KNIGHT));//knight attacks
		b |= (RookMagics[s].attacks[RookMagics[s].index(occ)] & getAllBB(ROOK, QUEEN));//rook attacks
		b |= (BishopMagics[s].attacks[BishopMagics[s].index(occ)] & getAllBB(BISHOP, QUEEN));//bishop attacks
		b |= (tables::PseudoAttacks[KING][s] & getAllBB(KING));//king attacks
		return b;
	}

	inline Bitboard attackers_to2(Square s, Bitboard occupied) const {

		return  (pawn_attacks_bb(BLACK, s) & getBB(WHITE, PAWN))
			| (pawn_attacks_bb(WHITE, s) & getBB(BLACK, PAWN))
			| (attacks_bb<KNIGHT>(s) & getAllBB(KNIGHT))
			| (attacks_bb(ROOK, s, occupied) & getAllBB(ROOK, QUEEN))
			| (attacks_bb(BISHOP, s, occupied) & getAllBB(BISHOP, QUEEN))
			| (attacks_bb<KING>(s) & getAllBB(KING));
	}

	inline Bitboard squareAttackersBB(const Square sq) const {
		return attackers_to(sq, getAllBB());
		//return _attakersTo[sq] | _boards[ALL_PIECES]; 
	};
	inline Bitboard squareAttackersBB(const Side side, const Square sq) const {
		return attackers_to(sq, getAllBB((Side)(1 ^ side)));
		//return _attakersTo[sq] | _boards[ALL_B_PIECES - side]; 
	};
	/*is square attacked by opposite side*/
	inline bool isSquareAttacked(const Side side, Square sq) const {
		return squareAttackersBB(sq) & getAllBB((Side)(1 ^ side));
		//return _attakersTo[sq] & _boards[ALL_B_PIECES - side]; 

	}

	/*attackers to side king*/
	inline Bitboard kingAttackersBB(const Side side) const {
		Square ksq = getPieceSquare(side, KING);
		return attackers_to(ksq) & getAllBB((Side)(1 ^ side));

	}
	/*checkersBB : attackers to current king*/
	inline Bitboard checkersBB() const {
		//Square ksq = getPieceSquare(_currentSide, KING);
		//return attackers_to(ksq,getAllBB()) & getAllBB((Side)(1^ _currentSide));
		return kingAttackersBB(_currentSide);
	}
	
	inline Bitboard check_squares( EPieceType pt) const {
		return _state->checkSquares[pt];
	}

	inline Bitboard slider_blockers(Bitboard sliders, Square sq, Bitboard& pinners) const {

		Bitboard blockers = 0;
		pinners = 0;

		// Snipers are sliders that attack 's' when a piece and other snipers are removed
		Bitboard snipers = ((attacks_bb<  ROOK>(sq) & getAllBB(QUEEN, ROOK))
			| (attacks_bb<BISHOP>(sq) & getAllBB(QUEEN, BISHOP))) & sliders;
		Bitboard occupancy = getAllBB() ^ snipers;

		while (snipers)
		{
			Square sniperSq = pop_lsb(&snipers);
			Bitboard b = between_bb(sq, sniperSq) & occupancy;

			if (b && !more_than_one(b))
			{
				blockers |= b;
				if (b & getAllBB(piece::side_of(pieceAt(sq))))
					pinners |= sniperSq;
			}
		}
		return blockers;
	}

	inline unsigned piecesCount(const Side side, EPieceType pt) const{ 
		return _piecesCounter[side][pt];
	}
	/*tests a given pseudo move:
	return true if it is legal*/
	inline bool isLegalMove(Move move) const {
		//check move 
		assert(move.isOk());

		Square from = move.from_sq();
		Square to = move.to_sq();
		EMoveType mtype = move.type();
		const bool isKingMoving = piece::type_of(pieceAt(from)) == KING;
		const Square ksq = getPieceSquare(_currentSide, KING);
		assert(piece::side_of(pieceAt(from)) == _currentSide);
		assert(pieceAt(ksq) == piece::make_piece(KING, _currentSide));

		/*check castling is legal*/
		if (mtype == EMoveType::CASTLING) {
			//check intial king pos: must be E1 or E8
			if (from != square::relative_square(_currentSide, SQ_E1)) return false;
			//set the correct target square
			to = square::relative_square(_currentSide, to > from ? SQ_G1 : SQ_C1);
			int step = to > from ? -1 : 1;
			//loop over (to->from) to check there is no attcked sqiare
			for (unsigned sq = to; sq != from; sq += step)
				if (isSquareAttacked(_currentSide, (Square)sq))
					return false;
		}

		//king is moving: check this does not expose him to attack
		if (isKingMoving && isSquareAttacked(_currentSide, (Square)to)) {
			return false;
		}
		//if moving piece is not king, check it is not pinned
		//TODO
		return !isPinnedSquare(_currentSide, from) || aligned(from, to, ksq);
	}


	void update_king_attackers(Side side);

	void update_ep_history(Side side, unsigned int enPassantInd) {
		if (square::is_ok((Square)enPassantInd)) {
			Square from = static_cast<Square>(enPassantInd + 8 - 16 * side);
			Square to = static_cast<Square>(enPassantInd - 8 + 16 * side);
			_historyMoves.emplace_back(Move(from, to, EMoveType::DOUBLE_PAWN_PUSH, piece::make_piece(EPieceType::PAWN,side)));
		}
	}

	void update_castle_rights( Square from,Square to);

	inline bool can_castle(Side side, CastlingRight::ECastlingRight right) const {
		return _state->castlingRight.get(side) & right;
	}

	inline CastlingRight::ECastlingRight castling_rights(Side side) const {
		return CastlingRight::ECastlingRight(_state->castlingRight.get(side));
	}

	inline bool castling_impeded(Side side, CastlingRight::ECastlingRight right) const {

		assert(right == CastlingRight::KING_SIDE || right == CastlingRight::QUEEN_SIDE);
		return getAllBB() & _castlingPath[side][right];
	}


	inline bool castling_impeded(Side side, CastlingRight cr) const {
		return castling_impeded(side, CastlingRight::ECastlingRight(cr.get(side)));
	}
	/*
	inline Square castling_rook_square(Side side, CastlingRight::ECastlingRight right) const {

		assert(right == CastlingRight::KING_SIDE || right == CastlingRight::QUEEN_SIDE);
		return castlingRookSquare[side][right];
	}
	inline Square castling_rook_square(Side side, CastlingRight cr) const {

		return castling_rook_square(side, CastlingRight::ECastlingRight(cr.get(side)));
	}
	*/
	inline void update_check_squares();

	/*update auxiliary bitboards from 12 to 20*/
	void update_auxiliary_BB();

	bool doMove(Move move, BoardState& newSt);

	bool undoMove(Move move);

	inline EPiece pieceAt(const File file, const Rank rank) const
	{
		Square sq = square::make_square(file, rank);
		return _piecesBoard[sq];
		/*
		for (unsigned int pt = EPieceType::PAWN; pt < EPieceType::EMPTY; pt++) {
			if (bitboard::isBitSet(getBB(WHITE, (EPieceType)pt), file, rank)) {
				return (EPiece)pt;
			}
			if (bitboard::isBitSet(getBB(BLACK, (EPieceType)pt), file, rank)) {
				return (EPiece)(pt + 6);
			}
		}
		return EPiece::NO_PIECE;
		*/
	}

	inline EPiece pieceAt(const Square sq) const
	{
		return _piecesBoard[sq];
	}

	inline bool do_castle(Side side, Square from, Square to, bool undo) {
		bool kingSide = to > from;
		//set the correct target square!!
		to = square::relative_square(side, kingSide ? SQ_G1 : SQ_C1);

		Square rookFrom = kingSide ? square::relative_square(side, SQ_H1) : square::relative_square(side, SQ_A1);
		Square rookTo = kingSide ? square::relative_square(side, SQ_F1) : square::relative_square(side, SQ_D1);
		if (undo) {
			movePiece(side, to, from);
			movePiece(side, rookTo, rookFrom);
			//update castling rights
			_state->castlingRight.add(side, kingSide ? CastlingRight::KING_SIDE : CastlingRight::QUEEN_SIDE);
		}
		else {
			movePiece(side, from, to);
			movePiece(side, rookFrom, rookTo);
			//update castling rights
			//_state->castlingRight.remove(side, kingSide ? CastlingRight::KING_SIDE : CastlingRight::QUEEN_SIDE);
		}


		/*
			switch (to)
			{
				//white KS castle
			case SQ_G1:
				movePiece(SQ_H1, SQ_F1, ROOK, side);
				_state->castlingRight.remove(side, CastlingRight::KING_SIDE);
				break;
				//white QS castle
			case SQ_C1:
				movePiece(SQ_A1, SQ_D1, ROOK, side);
				_state->castlingRight.remove(side, CastlingRight::QUEEN_SIDE);
				break;
				//black KS castle
			case SQ_G8:
				movePiece(SQ_H8, SQ_F8, ROOK, side);
				_state->castlingRight.remove(side, CastlingRight::KING_SIDE);
				break;
				//black QS castle
			case SQ_C8:
				movePiece(SQ_A8, SQ_D8, ROOK, side);
				_state->castlingRight.remove(side, CastlingRight::QUEEN_SIDE);
				break;

			default:
				break;
			}
			*/

		return true;
	}

	inline bool do_promote(Side side, Square from, Square to, EPieceType prt, bool undo) {

		EPiece promoted = piece::make_piece(prt, side);
		if (!undo) {
			
			return
				/*if it's a promotion with capture:
				remove opponent piece on "to" */
				removePiece((Side)(1 ^ side), to) &&
				//remove the promoted pawn
				removePiece(side, PAWN, from) &&

				//add promoted piece
				addPiece(promoted, to);
		}
		else {
			
			return
				//remove promoted piece from "to" square
				removePiece(promoted, to) &&
				//restore opponent captured piece if any
				((_state->captured == NO_PIECE) || addPiece(_state->captured, to)) &&
				//restore promoted pawn on "from" square
				addPiece(piece::make_piece(PAWN, side), from);

				;
		}
		return false;
	}

	inline void resetEp() {
		if (_state->epSquare != SQ_OUT)
		{
			_zkey ^= ZHash::epKeys[square::file_of(_state->epSquare)];
			_state->epSquare = SQ_OUT;
		}
	}
public:
	/*move a piece from "from" to "to"
	does not make capture if there's an opponent piece on target square*/
	inline void movePiece(Side side, Square from, Square to) {
		EPiece p = pieceAt(from);
		EPieceType pt = piece::type_of(p);
		assert(p != NO_PIECE && piece::side_of(p) == side && _piecesBoard[from] == p);
		_piecesBoard[from] = NO_PIECE;
		_piecesBoard[to] = p;
		/*tricky and clever from Stockfish*/
		Bitboard mask = (1ULL << from) | (1ULL << to);
		_boards[p] ^= mask;
		_boards[pt + 12U] ^= mask;
		_boards[ALL_PIECES] ^= mask;
		_boards[ALL_W_PIECES + side] ^= mask;

		//update zkey for both source and target square
		_zkey ^= ZHash::piecesKeys[side][pt][from];
		_zkey ^= ZHash::piecesKeys[side][pt][to];

	}
	
	inline bool addPiece(EPiece added, Square sq) {
		//check there's no piece at square sq:
		assert(_piecesBoard[sq] == NO_PIECE);
		if (_piecesBoard[sq] == NO_PIECE) {
			const EPieceType pt = piece::type_of(added);
			const Side side = piece::side_of(added);
			_piecesBoard[sq] = added;
			Bitboard mask = 1ULL << sq;
			_boards[added] |= mask;
			_boards[pt + 12U] |= mask;
			_boards[ALL_PIECES] |= mask;
			_boards[ALL_W_PIECES + side] |= mask;
		
			_zkey ^= ZHash::piecesKeys[side][pt][sq];
			_piecesCounter[side][pt]++;
			return true;
		}
		return false;

	}
	
	/*capture/delete a piece on a given square:
	we have to test if "captured" is on square "sq"*/
	inline bool removePiece(Side side, EPieceType capturedType, Square sq) {
		auto pc = piece::make_piece(capturedType, side);
		return removePiece(pc,sq);
	}
	
	/*capture/delete the piece on a given square:
	we have to test if "captured" is on square "sq"*/
	inline bool removePiece(EPiece captured, Square sq) {
		assert(captured != EPiece::NO_PIECE && _piecesBoard[sq] == captured);
		if (captured != EPiece::NO_PIECE && _piecesBoard[sq] == captured) {
			
			auto side = piece::side_of(captured);
			auto pt = piece::type_of(captured);
			_piecesBoard[sq] = NO_PIECE;
			_piecesCounter[side][pt]--;
			Bitboard mask = 1ULL << sq;
			_boards[captured] ^= mask;
			_boards[pt + 12U] ^= mask;
			_boards[ALL_PIECES] ^= mask;
			_boards[ALL_W_PIECES + side] ^= mask;
		
			//this piece is gone so unset it from hash key
			_zkey ^= ZHash::piecesKeys[side][pt][sq];

			return true;
		}
		return false;
	}
	inline bool removePiece(Square sq) {
		EPiece cap = pieceAt(sq);
		return removePiece(cap, sq);
	}
	/*remove piece on square sq only if it belongs to side*/
	inline bool removePiece( Side side,Square sq) {
		EPiece cap = pieceAt(sq);
		if (cap != NO_PIECE && piece::side_of(cap) == side)	return removePiece(cap, sq);
		return true;
	}

	inline Square getPieceSquare(Side side, EPieceType pt)const {
		const Bitboard& b = getBB(side, pt);
		return msb(b);
	}

	inline Square getPieceSquare(EPiece p)const {
		//TODO slow loop, change it to sth more faster
		for (int sq = SQ_A1; sq <= SQ_H8; sq++)
			if (_piecesBoard[sq] == p) return (Square)sq;
		return Square::SQ_OUT;
	}
private:
	void setSegment(std::string fenSegment, const unsigned int rank)
	{
		unsigned int f = -1;
		for (const char& pchar : fenSegment) {
			if (isdigit(pchar))
			{

				f += Helpers::charToi(pchar);
			}
			else
			{
				f++;
				EPiece code = piece::charToCode(pchar);
				addPiece(code, square::make_square((File)f, (Rank)rank));

			}
		}

	}
	inline const unsigned int squareIndexFromString(const std::string& c) {
		assert(c.size() == 2);
		return (c[0] - 'a' + 8 * (c[1] - '0' - 1));
	}

};

inline std::ostream& operator<<(std::ostream& os, Board& board) {

	for (int rank = 7; rank >= 0; rank--)
	{
		os << rank + 1 << "|  ";

		for (int file = 0; file < 8; file++)
		{
			os << piece::codeToChar(board.pieceAt((File)file, (Rank)rank)) << " ";
		}

		os << std::endl;
	}

	os << "   ________________" << std::endl;
	os << "    a b c d e f g h" << std::endl;

	return os;
}