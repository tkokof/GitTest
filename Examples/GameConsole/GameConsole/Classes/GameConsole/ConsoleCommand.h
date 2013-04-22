/*!
    \file ConsoleCommand.h
	\brief console command implementation
*/

#ifndef __HL_CONSOLE_COMMAND_H__
#define __HL_CONSOLE_COMMAND_H__

#include <cassert>
#include <vector>

#include "ConsoleVariable.h"

namespace HugoLib
{

//! console command arguments
class ConsoleCmdArgs
{
public:
	ConsoleCmdArgs(const std::vector<ConsoleValue>& cmdArgs):m_cmdArgs(cmdArgs) {};
	ConsoleCmdArgs(const ConsoleCmdArgs& consoleCmdArgs):m_cmdArgs(consoleCmdArgs.m_cmdArgs) {};
	ConsoleCmdArgs& operator = (const ConsoleCmdArgs& consoleCmdArgs)
	{
		if (this != &consoleCmdArgs)
		{
		    m_cmdArgs = consoleCmdArgs.m_cmdArgs;
		}
		return *this;
	}
	//! get command arg count
	size_t GetArgsCount() const { return m_cmdArgs.size(); }
	//! get command arg
	const ConsoleValue& GetArgs(size_t index) const
	{
        assert(index < m_cmdArgs.size());
		return m_cmdArgs[index];
	}
private:
	typedef std::vector<ConsoleValue> TCmdArgs;
	TCmdArgs m_cmdArgs;
};

//! console command callback
typedef void (*ConsoleCmdFunc)(const ConsoleCmdArgs*);

//! console commands
class ConsoleCommand
{
public:
	ConsoleCommand(const std::string& name, ConsoleCmdFunc cmdFunc, const std::string& help = ""):m_name(name),m_cmdFunc(cmdFunc),m_help(help) {};
	//! set command name
	//! NOTE: not so sure about this
	void SetName(const std::string& name) { m_name = name; }
	//! get command name
	const std::string& GetName() const { return m_name; }
	//! set command function
	void SetFunc(ConsoleCmdFunc cmdFunc) { m_cmdFunc = cmdFunc; }
	//! get command function
	ConsoleCmdFunc GetFunc() const { return m_cmdFunc; }
	//! set command help
	void SetHelp(const std::string& help) { m_help = help; }
	//! get command help
	const std::string& GetHelp() const { return m_help; }
	//! execute command
	//! NOTE: not so sure about this
	void Execute(const ConsoleCmdArgs* cmdArgs)
	{
		if (m_cmdFunc)
		{
			m_cmdFunc(cmdArgs);
		}
	}
private:
	std::string m_name; //< console command name
	ConsoleCmdFunc m_cmdFunc; //< console command function
	std::string m_help; //< console command help
};

//! console variable dumper interface
class IConsoleCommandDumper
{
public:
	virtual ~IConsoleCommandDumper() {};
	virtual void OnCCmdFound(ConsoleCommand* ccmd) = 0;
};

} // HugoLib

#endif // __HL_CONSOLE_COMMAND_H__