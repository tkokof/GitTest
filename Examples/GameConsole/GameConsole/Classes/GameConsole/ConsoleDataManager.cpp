
#include <cfloat>
#include <climits> // for "INT_MAX" etc.
#include <cstdio>
#include <algorithm>

#include "ConsoleDataManager.h"
#include "StringSimilarHelper.h"

namespace HugoLib
{

//! static instance pointer
static ConsoleDataManager* s_consoleDataManager = NULL;

//! simple singleton implementation
ConsoleDataManager* ConsoleDataManager::GetSingleton()
{
    if (!s_consoleDataManager)
	{
		s_consoleDataManager = new ConsoleDataManager();
		s_consoleDataManager->Init();
	}
	return s_consoleDataManager;
}

ConsoleDataManager::~ConsoleDataManager()
{
	// clear console variables
	m_cvarMap.clear();
	for (size_t i = 0; i < m_cvars.size(); ++i)
	{
		delete m_cvars[i];
	}
	m_cvars.clear();

	// clear console commands
	m_ccmdMap.clear();
	for (size_t i = 0; i < m_ccmds.size(); ++i)
	{
		delete m_ccmds[i];
	}
	m_ccmds.clear();

	// set back instance pointer
	s_consoleDataManager = NULL;
}

//! init method
bool ConsoleDataManager::Init()
{
    // TODO: implement
	return true;
}

//! release method
void ConsoleDataManager::Release()
{
	// NOTE: not so sure about this
	delete this;
}

//! add console variable
ConsoleVariable* ConsoleDataManager::AddCVar(const std::string& name, int value, ConsoleVarFunc varFunc, const std::string& help)
{
	// check name first
	if (!isNameValid(name)) return NULL;

	// create new console variable
	ConsoleVariable* cvar = new ConsoleVariable(name, ConsoleValue(value), varFunc, help);
	
	// register console variable
	size_t index = addCVar(cvar);
	m_cvarMap[name] = index;

	return cvar;
}

ConsoleVariable* ConsoleDataManager::AddCVar(const std::string& name, float value, ConsoleVarFunc varFunc, const std::string& help)
{
	// check name first
	if (!isNameValid(name)) return NULL;

	// create new console variable
	ConsoleVariable* cvar = new ConsoleVariable(name, ConsoleValue(value), varFunc, help);

	// register console variable
	size_t index = addCVar(cvar);
	m_cvarMap[name] = index;

	return cvar;
}

ConsoleVariable* ConsoleDataManager::AddCVar(const std::string& name, const std::string& value, ConsoleVarFunc varFunc, const std::string& help)
{
	// check name first
	if (!isNameValid(name)) return NULL;

	// create new console variable
	ConsoleVariable* cvar = new ConsoleVariable(name, ConsoleValue(value), varFunc, help);

	// register console variable
	size_t index = addCVar(cvar);
	m_cvarMap[name] = index;

	return cvar;
}

//! get console variable
ConsoleVariable* ConsoleDataManager::GetCVar(const std::string& name)
{
	TConsoleMap::iterator iter = m_cvarMap.find(name);
	if (iter != m_cvarMap.end())
	{
		return m_cvars[iter->second];
	}
	return NULL;
}

//! remove console variable
void ConsoleDataManager::RemoveCVar(const std::string& name)
{
	TConsoleMap::iterator iter = m_cvarMap.find(name);
	if (iter != m_cvarMap.end())
	{
		size_t index = iter->second;
		removeCVar(index);
		m_cvarMap.erase(iter);
	}
}

//! add console command
ConsoleCommand* ConsoleDataManager::AddCCmd(const std::string& name, ConsoleCmdFunc cmdFunc, const std::string& help)
{
	// check name first
	if (!isNameValid(name)) return NULL;

	// create new console command
	ConsoleCommand* ccmd = new ConsoleCommand(name, cmdFunc, help);

	// register console command
	size_t index = addCCmd(ccmd);
	m_ccmdMap[name] = index;

	return ccmd;
}

//! get console command
ConsoleCommand* ConsoleDataManager::GetCCmd(const std::string& name)
{
	TConsoleMap::iterator iter = m_ccmdMap.find(name);
	if (iter != m_ccmdMap.end())
	{
		return m_ccmds[iter->second];
	}
	return NULL;
}

//! remove console command
void ConsoleDataManager::RemoveCCmd(const std::string& name)
{
	TConsoleMap::iterator iter = m_ccmdMap.find(name);
	if (iter != m_ccmdMap.end())
	{
		size_t index = iter->second;
		removeCCmd(index);
		m_ccmdMap.erase(iter);
	}
}

//! get similar cvars, return values are arranged by similarity
//! TODO: optimize
std::vector<ConsoleDataManager::CVarSimilarInfo> 
ConsoleDataManager::GetSimilarCVars(const std::string& name_)
{
	std::vector<CVarSimilarInfo> similarCVarInfo;

	// make name valid
	// NOTE: not so sure about this
	std::string name = name_;
	if (!makeNameValid(name)) return similarCVarInfo;

	// linear search to find similar console var
	for (size_t i = 0; i < m_cvars.size(); ++i)
	{
		float similarValue = StringSimilarHelper::GetSingleton()->GetSimilarity(name, m_cvars[i]->GetName());
		if (similarValue >= CONSOLE_AUTO_COMPLETE_FACTOR)
		{
			similarCVarInfo.push_back(CVarSimilarInfo(similarValue, m_cvars[i]));
		}
	}

	// then sort by similar
	std::sort(similarCVarInfo.begin(), similarCVarInfo.end());

	return similarCVarInfo;
}

//! get similar ccmds, return values are arranged by similarity
//! TODO: optimize
std::vector<ConsoleDataManager::CCmdSimilarInfo> 
ConsoleDataManager::GetSimilarCCmds(const std::string& name_)
{
	std::vector<CCmdSimilarInfo> similarCCmdInfo;

	// make name valid
	// NOTE: not so sure about this
	std::string name = name_;
	if (!makeNameValid(name)) return similarCCmdInfo;

	// linear search to find similar console cmd
	for (size_t i = 0; i < m_ccmds.size(); ++i)
	{
		float similarValue = StringSimilarHelper::GetSingleton()->GetSimilarity(name, m_ccmds[i]->GetName());
		if (similarValue >= CONSOLE_AUTO_COMPLETE_FACTOR)
		{
			similarCCmdInfo.push_back(CCmdSimilarInfo(similarValue, m_ccmds[i]));
		}
	}

	// then sort by similar
	std::sort(similarCCmdInfo.begin(), similarCCmdInfo.end());

	return similarCCmdInfo;
}

//! dump console variable
void ConsoleDataManager::DumpCVar(IConsoleVariableDumper* dumper)
{
	// linear cvar dump
	for (size_t i = 0; i < m_cvars.size(); ++i)
	{
		if (m_cvars[i])
		{
			dumper->OnCVarFound(m_cvars[i]);
		}
	}
}

//! dump console command
void ConsoleDataManager::DumpCCmd(IConsoleCommandDumper* dumper)
{
	// linear ccmd dump
	for (size_t i = 0; i < m_ccmds.size(); ++i)
	{
		if (m_ccmds[i])
		{
			dumper->OnCCmdFound(m_ccmds[i]);
		}
	}
}

// add cvar internal
size_t ConsoleDataManager::addCVar(ConsoleVariable* cvar)
{
	// first we search if there is empty place
	for (size_t i = 0; i < m_cvars.size(); ++i)
	{
		if (m_cvars[i] == NULL) // empty place
		{
			m_cvars[i] = cvar;
			return i;
		}
	}
	// then seems there is no empty place
	m_cvars.push_back(cvar);
	return m_cvars.size() - 1;
}

// remove cvar internal
void ConsoleDataManager::removeCVar(size_t index)
{
	if (m_cvars[index])
	{
		delete m_cvars[index];
		m_cvars[index] = NULL;
	}
}

// add ccmd internal
size_t ConsoleDataManager::addCCmd(ConsoleCommand* ccmd)
{
	// first we search if there is empty place
	for (size_t i = 0; i < m_ccmds.size(); ++i)
	{
		if (m_ccmds[i] == NULL) // empty place
		{
			m_ccmds[i] = ccmd;
			return i;
		}
	}
	// then seems there is no empty place
	m_ccmds.push_back(ccmd);
	return m_ccmds.size() - 1;
}

// remove ccmd internal
void ConsoleDataManager::removeCCmd(size_t index)
{
	if (m_ccmds[index])
	{
		delete m_ccmds[index];
		m_ccmds[index] = NULL;
	}
}

bool ConsoleDataManager::isNameValid(const std::string& name)
{
	return name.length() > 0 && name.length() <= MAX_CONSOLE_VALUE_LENGTH;
}

bool ConsoleDataManager::makeNameValid(std::string& name)
{
	if (name.length() <= 0) 
	{
		return false;
	}
	else if (name.length() > MAX_CONSOLE_VALUE_LENGTH)
	{
		name = name.substr(0, MAX_CONSOLE_VALUE_LENGTH);
		return true;
	}

	return true;
}

} // HugoLib