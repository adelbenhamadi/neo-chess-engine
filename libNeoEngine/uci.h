#pragma once

#include <thread>

#include "board.h"
#include "move.h"
#include "engine.h"


struct UciOption {

	enum EOptionType { STRING, SPIN, COMBO, CHECK, BUTTON };

	// Constructors
	UciOption() : _defValue(""), _value(""), _optionType(EOptionType::STRING), _min(0), _max(0)
	{
	};

	UciOption(std::string defaultValue, EOptionType type) :
		_defValue(defaultValue),
		_value(defaultValue),
		_optionType(type),
		_min(0), _max(0)
	{
	};

	UciOption(std::string defaultValue, EOptionType type, int min, int max) :
		_defValue(defaultValue),
		_value(defaultValue),
		_optionType(type),
		_min(min),
		_max(max)
	{
	};

	// Overloading
	UciOption& operator=(const std::string& v)
	{
		//Blindly update for now
		_value = v;

		return *this;
	}

	UciOption(const std::string v) : _optionType(EOptionType::STRING), _min(0), _max(0)
	{
		_defValue = _value = v;
	}

	const std::string& getCurrentValue() const
	{
		return _value;
	}

	void setCurrentValue(const std::string& currentValue)
	{
		_value = currentValue;
	}

	const std::string& getDefaultValue() const
	{
		return _defValue;
	}

	void setDefaultValue(const std::string& defaultValue)
	{
		_defValue = defaultValue;
	}

	const EOptionType getType() const
	{
		return _optionType;
	}
	const std::string getTypeString() const
	{
		std::string res = "";
		switch (_optionType)
		{
		case EOptionType::STRING: res = "string"; break;
		case EOptionType::SPIN: res = "spin"; break;
		case EOptionType::CHECK: res = "check"; break;
		case EOptionType::BUTTON: res = "button"; break;
		case EOptionType::COMBO: res = "combo"; break;
		default:
			break;
		}
		return res;
	}
	void setType(const EOptionType type)
	{
		_optionType = type;
	}

	int getMax() const
	{
		return _max;
	}

	void setMax(int max)
	{
		_max = max;
	}

	int getMin() const
	{
		return _min;
	}

	void setMin(int min)
	{
		_min = min;
	}

private:
	std::string _defValue, _value;
	int _min, _max;
	EOptionType _optionType;

	//size_t idx;
};

typedef std::map<std::string, UciOption> OptionsMap;

enum EUciCommand {
	UCI = 0, ISREADY, SETOPTION, COLOR, UCINEWGAME, POSITION, GO , STOP , DEBUG, PRINT, PRINTOPTIONS, QUIT = 99, UNKNOWN = 100
};
typedef std::map<std::string, EUciCommand > UciCommandMap;

enum EGoSubCommand { SEARCHMOVES, PONDER, WTIME, BTIME, WINC, BINC, MOVESTOGO, DEPTH, NODES, MATE, MOVETIME, INFINITE };
typedef std::map<std::string, EGoSubCommand > GoSubCommandMap;

class Uci
{
public:

	Uci() : _board(std::shared_ptr<Board>(new Board())),
		_engine(std::shared_ptr<engine::Engine>(new engine::Engine(_board))),
		_optionsMap(),
		_uciCommands(),
		_goSubCommands()
	{

		_optionsMap["debug"] = UciOption("on", UciOption::EOptionType::STRING);
		_optionsMap["hash"] = UciOption("256", UciOption::EOptionType::SPIN, 1, 2048);

		//uci commands		
		_uciCommands["uci"] = UCI;
		_uciCommands["isready"] = ISREADY;
		_uciCommands["setoption"] = SETOPTION;
		_uciCommands["ucinewgame"] = UCINEWGAME;
		_uciCommands["position"] = POSITION;
		_uciCommands["go"] = GO;
		_uciCommands["debug"] = DEBUG;
		_uciCommands["print"] = PRINT;
		_uciCommands["printoptions"] = PRINTOPTIONS;
		_uciCommands["quit"] = QUIT;

		//uci go sub commands
		_goSubCommands["searchmoves"] = EGoSubCommand::SEARCHMOVES;
		_goSubCommands["ponder"] = EGoSubCommand::PONDER;
		_goSubCommands["wtime"] = EGoSubCommand::WTIME;
		_goSubCommands["btime"] = EGoSubCommand::BTIME;
		_goSubCommands["winc"] = EGoSubCommand::WINC;
		_goSubCommands["binc"] = EGoSubCommand::BINC;
		_goSubCommands["movestogo"] = EGoSubCommand::MOVESTOGO;
		_goSubCommands["depth"] = EGoSubCommand::DEPTH;
		_goSubCommands["nodes"] = EGoSubCommand::NODES;
		_goSubCommands["mate"] = EGoSubCommand::MATE;
		_goSubCommands["movetime"] = EGoSubCommand::MOVETIME;
		_goSubCommands["infinite"] = EGoSubCommand::INFINITE;

	}

	void listen();
	void init();
	void printOptions() const;
	void newGame();

	std::string getOption(const std::string str) const;
	void setOption(std::istringstream& is);

	EUciCommand getUciCommand(std::string command) {
		auto it = _uciCommands.find(command);
		if (it != _uciCommands.end()) {
			return it->second;
		}
		return EUciCommand::UNKNOWN;
	}
	EGoSubCommand getGoSubCommand(std::string command) {
		auto it = _goSubCommands.find(command);
		if (it != _goSubCommands.end()) {
			return it->second;
		}
		return EGoSubCommand::DEPTH;
	}

private:

	std::shared_ptr <engine::EngineOption> _engineOptions;
	std::shared_ptr<Board> _board;
	std::shared_ptr<engine::Engine> _engine;
	


	void setupPosition(std::istringstream& is);

	void go(std::istringstream& is);

	OptionsMap _optionsMap;
	UciCommandMap _uciCommands;
	GoSubCommandMap _goSubCommands;
};

