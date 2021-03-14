
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
		if (uciCommand == EUciCommand::GO) {
			resetGoOptions();

			while (is >> token)
			{
				const EGoSubCommand goSubCommand = getGoSubCommand(token);
				switch (goSubCommand)
				{

					//white has x msec left on the clock
				case EGoSubCommand::WTIME: is >> _wtime; break;

					//black has x msec left on the clock
				case EGoSubCommand::BTIME: is >> _btime; break;

					//white increment per move in mseconds if x > 0
				case EGoSubCommand::WINC: is >> _winc; break;

					//black increment per move in mseconds if x > 0
				case EGoSubCommand::BINC: is >> _binc; break;

					/*	there are x moves to the next time control,
						this will only be sent if x > 0,
						if you don't get this and get the wtime and btime it's sudden death*/
				case EGoSubCommand::MOVESTOGO: is >> _movestogo; break;

				case EGoSubCommand::DEPTH: is >> _depth; break;	//search x plies only.
				case EGoSubCommand::NODES: is >> _nodes; break;	//search x nodes only,
				case EGoSubCommand::MATE: is >> _mate; break;	//search for a mate in x moves
				case EGoSubCommand::MOVETIME: is >> _movetime; break; //search exactly x mseconds

				//search until the "stop" command. Do not exit the search without being told so in this mode!
				case EGoSubCommand::INFINITE:  _infinite = true; break;

				default:
					break;
				}
			}
			//create new thread for searching
			std::thread thr(&Uci::go, this);
			std::swap(thr, myThread);
			myThread.join();
		}
		else
			switch (uciCommand)
			{
			case EUciCommand::UCI:
				std::cout << "id name " << Engine::IDENTITY << std::endl;
				std::cout << "id author " << Engine::AUTHOR << std::endl;
				printOptions();
				std::cout << "uciok" << std::endl;
				break;
			case EUciCommand::ISREADY:
				std::cout << "readyok" << std::endl;
				break;
			case EUciCommand::SETOPTION:
				setoption(is);
				break;
			case EUciCommand::UCINEWGAME:
				newGame();
				break;
			case EUciCommand::POSITION:
				setupPosition(is);
				break;
			case EUciCommand::PRINT:
				std::cout << *_pBoard << std::endl;
				break;
			case EUciCommand::PRINTOPTIONS:
				printOptions();
				break;
			case EUciCommand::QUIT:
				std::cout << "Terminating.." << std::endl;
				_searchManager.stop();
				bStop = true;
				
				break;
			case EUciCommand::UNKNOWN:
				std::cout << "Unknown command!" << std::endl;
				break;
			}
	}

}

void Uci::init()
{
}

void Uci::setoption(std::istringstream& is)
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
		if (uciOption.getType() == EOptionType::SPIN) {
			std::cout << " min " << uciOption.getMin() << " max " << uciOption.getMax();
		}
		else if (uciOption.getType() == EOptionType::CHECK) {

		}
		else if (uciOption.getType() == EOptionType::COMBO) {

		}
		else if (uciOption.getType() == EOptionType::BUTTON) {

		}
		std::cout << std::endl;

	}
}


void Uci::newGame()
{
}

void Uci::resetGoOptions()
{
	//reset go options
	_infinite = false;
	_wtime = 0;
	_btime = 0;
	_winc = 0;
	_binc = 0;
	_movestogo = 0;
	_mate = 0;
	_depth = 1;
	_nodes = 0;
	_movetime = 0;

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

	if (token == "startpos")
	{
		_pBoard = std::shared_ptr<Board>(new Board());
	}
	else if (token == "fen")
	{
		while (is >> token && token != "moves")
			fen += token + " ";
		_pBoard = std::shared_ptr<Board>(new Board(fen));
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
			_pBoard->doMove(m);
		}
	}
	//setup searchManager
	//TODO
}

void Uci::go()
{
	//TODO
	//launch search then send out best move to uci gui
	std::cout << "bestmove " << "?" << std::endl;
}
