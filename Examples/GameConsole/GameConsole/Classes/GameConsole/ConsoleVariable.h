/*!
    \file ConsoleVariable.h
	\brief console variable implementation
*/

#ifndef __HL_CONSOLE_VARIABLE_H__
#define __HL_CONSOLE_VARIABLE_H__

#include <string>

namespace HugoLib
{

//! console value struct
//! NOTE: now we just support three data type : int,float,string
class ConsoleValue
{
public:
	ConsoleValue() {};
	ConsoleValue(int value) { Set(value); }
	ConsoleValue(float value) { Set(value); }
	ConsoleValue(const std::string& value):m_value(value) {};
	ConsoleValue(const ConsoleValue& cv):m_value(cv.m_value) {};
	ConsoleValue& operator = (const ConsoleValue& cv)
	{ 
		if (this != &cv)
		{
			m_value = cv.m_value;
		}
		return *this;
	}
	//! get console value
	int GetInt() const;
	float GetFloat() const;
	const std::string& GetString() const { return m_value; }
	//! set console value
	void Set(int val);
	void Set(float val);
	void Set(const std::string val) { m_value = val; }
private:
	std::string m_value; //< base value
};

//! console variable change callback
class ConsoleVariable;
typedef void (*ConsoleVarFunc)(const ConsoleVariable*);

//! console variable
class ConsoleVariable
{
public:
    ConsoleVariable(const std::string& name, const ConsoleValue& value, ConsoleVarFunc varFunc = NULL, const std::string& help = ""):
		m_name(name),m_value(value),m_help(help),m_varFunc(varFunc) {};
    //! set variable name
	//! NOTE: not so sure about this
	void SetName(const std::string& name) { m_name = name; }
	//! get variable name
	const std::string& GetName() const { return m_name; }
	//! set variable value
	void SetValue(const ConsoleValue& value)
	{ 
		m_value = value;
		// NOTE: not so sure about this
		if (m_varFunc)
		{
			m_varFunc(this);
		}
	}
	//! get variable value
	const ConsoleValue& GetValue() const { return m_value; }
	//! set variable help
	void SetHelp(const std::string& help) { m_help = help; }
	//! get variable help
	const std::string& GetHelp() const { return m_help; }
	//! set variable func
	void SetFunc(ConsoleVarFunc varFunc) { m_varFunc = varFunc; }
	//! get variable func
	ConsoleVarFunc GetFunc() const { return m_varFunc; }
private:
	std::string m_name; //< console variable name
	ConsoleValue m_value; //< console variable value
	std::string m_help; //< console variable help
	ConsoleVarFunc m_varFunc; //< console variable function
};

//! console variable dumper interface
class IConsoleVariableDumper
{
public:
	virtual ~IConsoleVariableDumper() {};
	virtual void OnCVarFound(ConsoleVariable* cvar) = 0;
};

} // HugoLib

#endif // __HL_CONSOLE_VARIABLE_H__