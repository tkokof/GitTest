/*!
    \file ConsoleDataManager.h
	\brief console data manager implementation
*/

#ifndef __HL_CONSOLE_DATA_MANAGER_H__
#define __HL_CONSOLE_DATA_MANAGER_H__

#include <string>
#include <vector>
#include <map>

#include "ConsoleVariable.h"
#include "ConsoleCommand.h"

namespace HugoLib
{

class ConsoleDataManager
{
public:
	//! simple singleton implementation
	static ConsoleDataManager* GetSingleton();
public:
	~ConsoleDataManager();
	//! init method
	bool Init();
	//! release method
	//! call this before quit
	void Release();
	//! add console variable
	ConsoleVariable* AddCVar(const std::string& name, int value, ConsoleVarFunc varFunc = NULL, const std::string& help = "");
	ConsoleVariable* AddCVar(const std::string& name, float value, ConsoleVarFunc varFunc = NULL, const std::string& help = "");
	ConsoleVariable* AddCVar(const std::string& name, const std::string& value, ConsoleVarFunc varFunc = NULL, const std::string& help = "");
	//! get console variable
	ConsoleVariable* GetCVar(const std::string& name);
	//! remove console variable
	void RemoveCVar(const std::string& name);
	//! add console command
	ConsoleCommand* AddCCmd(const std::string& name, ConsoleCmdFunc cmdFunc, const std::string& help = "");
	//! get console command
	ConsoleCommand* GetCCmd(const std::string& name);
	//! remove console command
	void RemoveCCmd(const std::string& name);
    //////////////////////////////////////////////////////////////////////////
	//! cvar similar info struct
	struct CVarSimilarInfo
	{
		float similarity;
		ConsoleVariable* cvar;
		CVarSimilarInfo(float sim = 0, ConsoleVariable* var = NULL):similarity(sim),cvar(var) {};
		CVarSimilarInfo(const CVarSimilarInfo& si):similarity(si.similarity),cvar(si.cvar) {};
		CVarSimilarInfo& operator = (const CVarSimilarInfo& si)
		{
			if (this != &si)
			{
				similarity = si.similarity;
				cvar = si.cvar;
			}
			return *this;
		}
		// for compare
		bool operator < (const CVarSimilarInfo& si) const { return similarity < si.similarity; }
	};
	//! get similar cvars, return values are arranged by similarity
	//! NOTE: now return value is just pass-by-value vector
	//! TODO: using c++11 moving constructor
	std::vector<CVarSimilarInfo> GetSimilarCVars(const std::string& name);
	//! ccmd similar info struct
	struct CCmdSimilarInfo
	{
		float similarity;
		ConsoleCommand* ccmd;
		CCmdSimilarInfo(float sim = 0, ConsoleCommand* cmd = NULL):similarity(sim),ccmd(cmd) {};
		CCmdSimilarInfo(const CCmdSimilarInfo& si):similarity(si.similarity),ccmd(si.ccmd) {};
		CCmdSimilarInfo& operator = (const CCmdSimilarInfo& si)
		{
			if (this != &si)
			{
				similarity = si.similarity;
				ccmd = si.ccmd;
			}
			return *this;
		}
		// for compare
		bool operator < (const CCmdSimilarInfo& si) const { return similarity < si.similarity; }
	};
	//! get similar ccmds, return values are arranged by similarity
	//! NOTE: now return value is just pass-by-value vector
	//! TODO: using c++11 moving constructor
	std::vector<CCmdSimilarInfo> GetSimilarCCmds(const std::string& name);
	//! dump console variable
	void DumpCVar(IConsoleVariableDumper* dumper);
	//! dump console command
	void DumpCCmd(IConsoleCommandDumper* dumper);
private:
	//! prevent raw creation
	ConsoleDataManager() {};
	// add cvar internal
	size_t addCVar(ConsoleVariable* cvar);
	// remove cvar internal
	void removeCVar(size_t index);
	// add ccmd internal
	size_t addCCmd(ConsoleCommand* ccmd);
	// remove ccmd internal
	void removeCCmd(size_t index);
	// check name if is valid
	bool isNameValid(const std::string& name);
	// make name valid
	bool makeNameValid(std::string& name);
private:
	
private:
	// cvar & ccmd database
	typedef std::map<std::string, size_t> TConsoleMap; // key : name ; value : index
	TConsoleMap m_cvarMap; //< console variable map
	typedef std::vector<ConsoleVariable*> TCVarContainer;
	TCVarContainer m_cvars; //< console variables

	TConsoleMap m_ccmdMap; //< console command map
	typedef std::vector<ConsoleCommand*> TCCmdContainer;
	TCCmdContainer m_ccmds; //< console commands
};

} // namespace HugoLib

#endif // __HL_CONSOLE_DATA_MANAGER_H__