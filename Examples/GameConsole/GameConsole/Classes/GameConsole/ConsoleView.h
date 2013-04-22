/*!
    \file ConsoleView.h
	\brief base console view class
*/

#ifndef __HL_CONSOLE_VIEW_H__
#define __HL_CONSOLE_VIEW_H__

#include <string>

namespace HugoLib
{

//! console action enum
enum EConsoleAction
{
	ECA_Char = 0, //< user input a char
	ECA_Delete, //< user delete a char
	ECA_Confirm, //< user confirm input

	ECA_PrePrompt, //< move prompt to pre-position
	ECA_PostPrompt, //< move prompt to post-position

	ECA_PreInput, //< show user pre-input text
	ECA_PostInput, //< show user post-input text

	ECA_AutoComplete, //< execute auto complete


	ECA_Unknown //< unknown console action
};

//! console action data
struct ConsoleActionData
{
	EConsoleAction action; //< console action
	char chr; //< input char
	void* userData; //< user data

	ConsoleActionData(EConsoleAction action_, char chr_ = '\0', void* userData_ = NULL):action(action_),chr(chr_),userData(userData_) {};
	ConsoleActionData(const ConsoleActionData& d):action(d.action),chr(d.chr),userData(d.userData) {};
	ConsoleActionData& operator = (const ConsoleActionData& d)
	{
		if (this != &d)
		{
			action = d.action;
			chr = d.chr;
			userData = d.userData;
		}

		return *this;
	}

};

//! console action delegate
class ConsoleActionDelegate
{
public:
	virtual ~ConsoleActionDelegate() {};
	//! console action event handler
	virtual bool OnEvent(const ConsoleActionData& data) = 0;
};

//! console view interface
class ConsoleView
{
public:
	virtual ~ConsoleView() {};
	// init method
	//virtual bool Init() = 0;
	//! release method
	//virtual void Release() = 0;

	//! set console action delegate
	virtual void SetActionDelegate(ConsoleActionDelegate* delegate) = 0;
	//! get console action delegate
	virtual ConsoleActionDelegate* GetActionDelegate() = 0;

	//! set line max char count
	virtual void SetLineMaxCharCount(size_t maxCount) = 0;
	//! get line max char count
	virtual size_t GetLineMaxCharCount() const = 0;

	//! output an new line
	virtual void OutputLine(const std::string& line) = 0;
	//! output input string
	virtual void OutputInput(const std::string& input) = 0;
	
	//! set prompt position
	virtual void SetPromptPos(size_t pos) = 0;
	//! get prompt position
	virtual size_t GetPromptPos() const = 0;
};

} // HugoLib

#endif // __HL_CONSOLE_VIEW_H__