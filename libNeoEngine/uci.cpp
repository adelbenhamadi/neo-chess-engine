
#include <cstring>
#include "uci.h"

#define GET_TOKEN() token.clear();\
					 is >> token;
void Uci::listen()
{
	std::string command;
	std::string token;
	bool bStop = false;
	std::cout.setf(std::ios::unitbuf);
	while (!bStop && std::getline(std::cin, command)) {

		//if (Engine::DEBUG_MODE) std::cout << "info debug -> loop() command =  " << command << std::endl;
		std::istringstream is(command, std::ios::in);

		GET_TOKEN();

		EUciCommand uciCommand = getUciCommand(token);

		switch (uciCommand)
		{
		case EUciCommand::UCI:
			std::cout << "id name " << engine::IDENTITY << std::endl;
			std::cout << "id author " << engine::AUTHOR << std::endl;
			printOptions();
			std::cout << "uciok" << std::endl;
			break;
		case EUciCommand::ISREADY:
			std::cout << "readyok" << std::endl;
			break;
		case EUciCommand::SETOPTION:
			setOption(is);
			break;
		case EUciCommand::UCINEWGAME:
			newGame();
			break;
		case EUciCommand::POSITION:
			setupPosition(is);
			break;
		case EUciCommand::PRINT:
			_engine->printBoard();
			break;
		case EUciCommand::PRINTOPTIONS:
			printOptions();
			break;
		case EUciCommand::GO:
			go(is);
			break;
		case EUciCommand::QUIT:
			_engine->stop();
			bStop = true;

			break;
		case EUciCommand::UNKNOWN:
			std::cout << "Unknown command!" << std::endl;
			break;
		}
	}
	std::cout << "Terminating.." << std::endl;
}

void Uci::init()
{
	_engine->reset();
}

void Uci::clean()
{
	
}

void Uci::setOption(std::istringstream& is)
{
}

void Uci::printOptions() const
{
	std::cout << "Current options" << std::endl;


	for (auto elem : _optionsMap)
	{
		UciOption uciOption = elem.second;

		std::cout << "option name " << elem.first
			<< " type " << uciOption.getTypeString()
			<< " default " << uciOption.getDefaultValue();
		if (uciOption.getType() == UciOption::EOptionType::SPIN) {
			std::cout << " min " << uciOption.getMin() << " max " << uciOption.getMax();
		}
		else if (uciOption.getType() == UciOption::EOptionType::CHECK) {

		}
		else if (uciOption.getType() == UciOption::EOptionType::COMBO) {

		}
		else if (uciOption.getType() == UciOption::EOptionType::BUTTON) {

		}
		std::cout << std::endl;

	}
}


void Uci::newGame()
{
	_engine->reset();
	
}



std::string Uci::getOption(const std::string str) const
{
	return std::string();
}

void Uci::setupPosition(std::istringstream& is)
{
	Move m;
	std::string token, fen;

	GET_TOKEN();
	//creta new board
	if (token == "startpos")
	{
		_board = std::shared_ptr<Board>(new Board());
	}
	else if (token == "fen")
	{
		while (is >> token && token != "moves")
			fen += token + " ";
		_board = std::shared_ptr<Board>(new Board(fen));
	}
	else
	{
		return;
	}

	while (is >> token)
	{
		if (token != "moves")
		{
			//execute Move!
			_engine->doMove(m);
		}
	}
	//setup engine
	//TODO
}

void Uci::go(std::istringstream& is)
{

	std::string token;
	_engineOptions->reset();
	//continue listning for needed Go sub commands
	while (is >> token)
	{
		const EGoSubCommand goSubCommand = getGoSubCommand(token);
		switch (goSubCommand)
		{
		case EGoSubCommand::SEARCHMOVES:
			_engineOptions->searchmoves.clear();
			while (!is.eof()) {
				is >> token;
				_engineOptions->searchmoves.push_back(token);
			}
			break;
		case EGoSubCommand::PONDER: _engineOptions->ponder = true; break;
			//white has x msec left on the clock
		case EGoSubCommand::WTIME: is >> _engineOptions->wtime; break;

			//black has x msec left on the clock
		case EGoSubCommand::BTIME: is >> _engineOptions->btime; break;

			//white increment per move in mseconds if x > 0
		case EGoSubCommand::WINC: is >> _engineOptions->winc; break;

			//black increment per move in mseconds if x > 0
		case EGoSubCommand::BINC: is >> _engineOptions->binc; break;

			/*	there are x moves to the next time control,
				this will only be sent if x > 0,
				if you don't get this and get the wtime and btime it's sudden death*/
		case EGoSubCommand::MOVESTOGO: is >> _engineOptions->movestogo; break;
			//search x plies only.
		case EGoSubCommand::DEPTH: 	is >> _engineOptions->depth;	break;
			//search x nodes only,
		case EGoSubCommand::NODES: is >> _engineOptions->nodes; break;
			//search for a mate in x moves
		case EGoSubCommand::MATE: is >> _engineOptions->mate; break;
			//search exactly x mseconds
		case EGoSubCommand::MOVETIME: is >> _engineOptions->movetime; break;

			//search until the "stop" command. Do not exit the search without being told so in this mode!
		case EGoSubCommand::INFINITE:  _engineOptions->infinite = true; break;

		default:
			//this a "dry" Go command
			//TODO set default engine options
			break;
		}

	}
	_engine->start();

	//launch search then send out best move to gui
	std::cout << "bestmove " << "?" << std::endl;
}
