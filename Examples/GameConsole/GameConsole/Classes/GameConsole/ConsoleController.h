/*!
    \file ConsoleController.h
	\brief base console controller implementation
*/

#ifndef __HL_CONSOLE_CONTROLLER_H__
#define __HL_CONSOLE_CONTROLLER_H__

#include <string>
#include <vector>

#include "ConsoleView.h"

namespace HugoLib
{

class ConsoleVariable;
class ConsoleCommand;


class ConsoleController: public ConsoleActionDelegate
{
public:
	virtual ~ConsoleController() {};
	//! init method
	virtual bool Init() = 0;
	//! release method
	virtual void Release() = 0;
	//! ConsoleActionDelegate
	//////////////////////////////////////////////////////////////////////////
	//virtual bool OnEvent(const ConsoleActionData& data) = 0;
	//////////////////////////////////////////////////////////////////////////
	// NOTE: now we just support one view for simple and clear
	//! set view
	virtual void SetView(ConsoleView* view) = 0;
	//! get view
	virtual ConsoleView* GetView() = 0;
};

class ConsoleControllerBase: public ConsoleController
{
public:
	ConsoleControllerBase():m_consoleView(NULL) {};
	virtual ~ConsoleControllerBase() { Release(); }
	//! init method
	virtual bool Init();
	//! release method
	virtual void Release();
	//! ConsoleActionDelegate
	//////////////////////////////////////////////////////////////////////////
	virtual bool OnEvent(const ConsoleActionData& data);
	//////////////////////////////////////////////////////////////////////////
	//! set view
	virtual void SetView(ConsoleView* view);
	//! get view
	virtual ConsoleView* GetView();
protected:
	// console action handler
	virtual bool onEventChar(const ConsoleActionData& data);
	virtual bool onEventDelete(const ConsoleActionData& data);
	virtual bool onEventConfirm(const ConsoleActionData& data);
	virtual bool onEventPrePrompt(const ConsoleActionData& data);
	virtual bool onEventPostPrompt(const ConsoleActionData& data);
	virtual bool onEventPreInput(const ConsoleActionData& data);
	virtual bool onEventPostInput(const ConsoleActionData& data);
	virtual bool onEventAutoComplete(const ConsoleActionData& data);
	// output console input
	virtual void outputConsoleInput();
	// output helper
	virtual void outputCVarInfo(ConsoleVariable* cvar);
	virtual void outputCCmdInfo(ConsoleCommand* ccmd);
	virtual void outputInfo(const std::string& name);
	// update console input
	virtual void updateConsoleInput(const std::string& input);
	// clear console input
	virtual void clearConsoleInput();
	// update input history
	virtual void updateInputHistory(const std::string& newInput);
	// clear input history
	virtual void clearInputHistory();
	// update auto-complete
	virtual void updateAutoComplete();
	// clear auto-complete
	virtual void clearAutoComplete();
protected:
	ConsoleView* m_consoleView; //< console view
    
	// console action event data
	std::string m_inputData; //< input data buffer
	size_t m_promptPos; //< input prompt position

	// console input history
	typedef std::vector<std::string> TStrContainer;
	TStrContainer m_inputHistory; //< input history
	size_t m_inputHistoryPos; //< input history position

	// auto-complete handler
	bool m_autoCompleting; //< doing auto-complete flag
	size_t m_autoCompleteCurIndex; //< current auto-complete index
	std::vector<std::string> m_autoCompleteBuffer; //< auto complete buffer
};

} // HugoLib

#endif // __HL_CONSOLE_CONTROLLER_H__