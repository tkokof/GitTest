
#include "ConsoleController.h"

#include "ConsoleVariable.h"
#include "ConsoleCommand.h"

#include "ConsoleDataManager.h"
#include "StringSimilarHelper.h"

namespace HugoLib
{

//! init method
bool ConsoleControllerBase::Init()
{
	// clear input data
    clearConsoleInput();

	// clear input history
	clearInputHistory();

	return true;
}

//! release method
void ConsoleControllerBase::Release()
{
	// release view if need
	if (m_consoleView)
	{
		m_consoleView->SetActionDelegate(NULL);
		m_consoleView = NULL;
	}
}
	
//! ConsoleActionDelegate
bool ConsoleControllerBase::OnEvent(const ConsoleActionData& data)
{
    switch (data.action)
	{
	case ECA_Char:
		return onEventChar(data);
	case ECA_Delete:
		return onEventDelete(data);
	case ECA_Confirm:
		return onEventConfirm(data);
	case ECA_PrePrompt:
		return onEventPrePrompt(data);
	case ECA_PostPrompt:
		return onEventPostPrompt(data);
	case ECA_PreInput:
        return onEventPreInput(data);
	case ECA_PostInput:
		return onEventPostInput(data);
	case ECA_AutoComplete:
		return onEventAutoComplete(data);
	}

	// if we do not handle event, just return false
	return false;
}
	
//! set view
void ConsoleControllerBase::SetView(ConsoleView* view)
{
	// first we release original view
	if (m_consoleView)
	{
		m_consoleView->SetActionDelegate(NULL);
		m_consoleView = NULL;
	}

	// then set view if need
	if (m_consoleView != view)
	{
		m_consoleView = view;
		m_consoleView->SetActionDelegate(this);
	}
}
	
//! get view
ConsoleView* ConsoleControllerBase::GetView()
{
	return m_consoleView;
}

bool ConsoleControllerBase::onEventChar(const ConsoleActionData& data)
{
	char chr = data.chr;

    // check if prompt is at last
	if (m_promptPos >= m_inputData.size())
	{
		// seems in the end
		m_inputData.push_back(chr);
		m_promptPos = m_inputData.size();
	}
	else
	{
	    // seems in the middle or something
		m_inputData.insert(m_promptPos, 1, chr);
		++m_promptPos;
	}

	// show result
	outputConsoleInput();

	// clear auto-complete
	clearAutoComplete();

	return true;
}

bool ConsoleControllerBase::onEventDelete(const ConsoleActionData& data)
{
	// first we check if the input data is null
	if (m_inputData.empty()) return false;

	// check if prompt is at last
	if (m_promptPos >= m_inputData.size())
	{
		// seems in the end
		m_inputData.erase(m_inputData.size() - 1, 1);
		m_promptPos = m_inputData.size();
	}
	else if (m_promptPos > 0)
	{
		// seems in the middle or something
		m_inputData.erase(m_promptPos - 1, 1);
		// update prompt position
		--m_promptPos;
	}

	// show result
	outputConsoleInput();

	// clear auto-complete
	clearAutoComplete();

	return true;
}

bool ConsoleControllerBase::onEventPrePrompt(const ConsoleActionData& data)
{
	if (m_promptPos == 0) return false;

	--m_promptPos;

	// show result
	outputConsoleInput();

	return true;
}

bool ConsoleControllerBase::onEventPostPrompt(const ConsoleActionData& data)
{
	if (m_promptPos == m_inputData.size()) return false;

	++m_promptPos;

	// show result
	outputConsoleInput();

	return true;
}

void ConsoleControllerBase::outputConsoleInput()
{
	if (m_consoleView)
	{
		m_consoleView->OutputInput(m_inputData);
		m_consoleView->SetPromptPos(m_promptPos);
	}
}

bool ConsoleControllerBase::onEventConfirm(const ConsoleActionData& data)
{
	// NOTE: whatever we update input history
	if (!m_inputData.empty())
	{
		updateInputHistory(m_inputData);
	}

	// and clear auto-complete
	clearAutoComplete();

	// first we do input data split
	std::string varOrCmd;
	std::vector<ConsoleValue> valOrArgs;
	std::string splitBuffer;

	// we add a ' ' to the end of input to unify parsing
	m_inputData.push_back(' ');

	// NOTE: now we just support simple command format(just strings divided by ' ')
	for (size_t i = 0; i < m_inputData.length(); ++i)
	{
		if (m_inputData[i] == ' ')
		{
		    if (splitBuffer.empty()) // seems just multi-blanks
			{
				continue;
			}
			else // seems get a new blank
			{
			    if (varOrCmd.empty()) // seems first find, we should set to varOrCmd
				{
					varOrCmd = splitBuffer;
					splitBuffer.clear(); // clear buffer
				}
				else
				{
					valOrArgs.push_back(ConsoleValue(splitBuffer));
					splitBuffer.clear(); // clear buffer
				}
			}
		}
		else
		{
			splitBuffer.push_back(m_inputData[i]);
		}
	}

	// now we split input, check CVar or CCmd
	ConsoleVariable* cvar = ConsoleDataManager::GetSingleton()->GetCVar(varOrCmd);
	// this is a cvar
	if (cvar)
	{
		if (valOrArgs.size() > 0) // set cvar operation
		{
			cvar->SetValue(valOrArgs[0]);
		}

		// output cvar info
		outputCVarInfo(cvar);

		// clear input
		clearConsoleInput();

		// update view
		outputConsoleInput();

		return true;
	}
	else
	{
		ConsoleCommand* ccmd = ConsoleDataManager::GetSingleton()->GetCCmd(varOrCmd);
		ConsoleCmdArgs args(valOrArgs);
		
		if (ccmd)
		{
			ccmd->Execute(&args);

			// output ccmd info
			outputCCmdInfo(ccmd);

			// clear input
			clearConsoleInput();

			// update view
			outputConsoleInput();

			return true;
		}
	}

	// otherwise, we just clear input here

	// clear input
	clearConsoleInput();

	// update view
	outputConsoleInput();

	return false;
}

// output helper
void ConsoleControllerBase::outputCVarInfo(ConsoleVariable* cvar)
{
	if (cvar)
	{
		std::string name = cvar->GetName();
		std::string value = cvar->GetValue().GetString();
		std::string help = cvar->GetHelp();
		std::string info = name + " = " + value + " ( " + help + " ) ";
		if (m_consoleView)
		{
			m_consoleView->OutputLine(info);
		}
	}
}

void ConsoleControllerBase::outputCCmdInfo(ConsoleCommand* ccmd)
{
	if (ccmd)
	{
		std::string name = ccmd->GetName();
		std::string help = ccmd->GetHelp();
		std::string info = name + " ( " + help + " ) ";
		if (m_consoleView)
		{
			m_consoleView->OutputLine(info);
		}
	}
}

void ConsoleControllerBase::outputInfo(const std::string& name)
{
	ConsoleVariable* cvar = ConsoleDataManager::GetSingleton()->GetCVar(name);
	if (cvar) outputCVarInfo(cvar);
	
	ConsoleCommand* ccmd = ConsoleDataManager::GetSingleton()->GetCCmd(name);
	if (ccmd) outputCCmdInfo(ccmd);
}

// update console input
void ConsoleControllerBase::updateConsoleInput(const std::string& input)
{
	m_inputData = input;
	m_promptPos = m_inputData.size();
}

void ConsoleControllerBase::clearConsoleInput()
{
	m_inputData.clear();
	m_promptPos = 0;
}

void ConsoleControllerBase::updateInputHistory(const std::string& newInput)
{
	m_inputHistory.push_back(newInput);
	m_inputHistoryPos = m_inputHistory.size();
}

void ConsoleControllerBase::clearInputHistory()
{
	m_inputHistory.clear();
	m_inputHistoryPos = 0;
}

// update auto-complete
void ConsoleControllerBase::updateAutoComplete()
{
	m_autoCompleting = true;
	m_autoCompleteCurIndex = 0;
}

// clear auto-complete
void ConsoleControllerBase::clearAutoComplete()
{
	m_autoCompleting = false;
	m_autoCompleteCurIndex = 0;
}

bool ConsoleControllerBase::onEventPreInput(const ConsoleActionData& data)
{
	if (m_inputHistoryPos == 0) return false;

	--m_inputHistoryPos;

	updateConsoleInput(m_inputHistory[m_inputHistoryPos]);

	// show result
	outputConsoleInput();

	// clear auto-complete
	clearAutoComplete();

	return true;
}

bool ConsoleControllerBase::onEventPostInput(const ConsoleActionData& data)
{
	// NOTE: pay attention to this
	if (m_inputHistoryPos + 1 >= m_inputHistory.size()) return false;

	++m_inputHistoryPos;

	updateConsoleInput(m_inputHistory[m_inputHistoryPos]);

	// show result
	outputConsoleInput();

	// clear auto-complete
	clearAutoComplete();

	return true;
}

// TODO: optimize
bool ConsoleControllerBase::onEventAutoComplete(const ConsoleActionData& data)
{
	// first we check if we are doing auto-completing
	if (m_autoCompleting)
	{
		if (!m_autoCompleteBuffer.empty())
		{
			m_autoCompleteCurIndex = (m_autoCompleteCurIndex + 1) % m_autoCompleteBuffer.size();
			
			// output help info
			outputInfo(m_autoCompleteBuffer[m_autoCompleteCurIndex]);

			// update console input
			updateConsoleInput(m_autoCompleteBuffer[m_autoCompleteCurIndex]);

			// update console view
			outputConsoleInput();

			return true;
		}
		
		return false;
	}
	else // seems new auto-complete operation
	{
		std::vector<ConsoleDataManager::CVarSimilarInfo> simCVars = ConsoleDataManager::GetSingleton()->GetSimilarCVars(m_inputData);
		std::vector<ConsoleDataManager::CCmdSimilarInfo> simCCmds = ConsoleDataManager::GetSingleton()->GetSimilarCCmds(m_inputData);		

		// merge similar cvar and ccmd names
		if (simCVars.empty() && simCCmds.empty()) 
		{
			// clear auto-complete buffer
			m_autoCompleteBuffer.clear();

			// update auto-complete data
			updateAutoComplete();

			return false;
		}
		else if (!simCVars.empty() && simCCmds.empty()) 
		{
			// update auto-complete buffer
			m_autoCompleteBuffer.clear();
			for (int i = int(simCVars.size()) - 1; i >=0; --i)
			{
				m_autoCompleteBuffer.push_back(simCVars[i].cvar->GetName());
			}
		}
		else if (simCVars.empty() && !simCCmds.empty()) 
		{
			// update auto-complete buffer
			m_autoCompleteBuffer.clear();
			for (int i = int(simCCmds.size()) - 1; i >=0; --i)
			{
				m_autoCompleteBuffer.push_back(simCCmds[i].ccmd->GetName());
			}
		}
		else if (!simCVars.empty() && !simCCmds.empty())
		{
			// merge simCVars and simCCmds to auto-complete buffer
			m_autoCompleteBuffer.clear();

			// simple merge sort algorithm
			int cvarIndex = int(simCVars.size()) - 1;
			int ccmdIndex = int(simCCmds.size()) - 1;

			while (cvarIndex >= 0 || ccmdIndex >= 0)
			{
				if (cvarIndex >= 0 && ccmdIndex < 0)
				{
					m_autoCompleteBuffer.push_back(simCVars[cvarIndex].cvar->GetName());
					--cvarIndex;
				}
				else if (ccmdIndex >=0 && cvarIndex < 0)
				{
					m_autoCompleteBuffer.push_back(simCCmds[ccmdIndex].ccmd->GetName());
					--ccmdIndex;
				}
				else
				{
					if (simCVars[cvarIndex].similarity > simCCmds[ccmdIndex].similarity)
					{
						m_autoCompleteBuffer.push_back(simCVars[cvarIndex].cvar->GetName());
						--cvarIndex;
					}
					else
					{
						m_autoCompleteBuffer.push_back(simCCmds[ccmdIndex].ccmd->GetName());
						--ccmdIndex;
					}
				}
			}
		}

		// output help info
		outputInfo(m_autoCompleteBuffer.front());

		// update console input
		updateConsoleInput(m_autoCompleteBuffer.front());

		// update console view
		outputConsoleInput();

		// update auto-complete data
		updateAutoComplete();

		return true;
	}
}

} // HugoLib