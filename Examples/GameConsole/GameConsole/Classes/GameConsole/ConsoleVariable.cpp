
#include <cfloat>

#include "ConsoleVariable.h"

namespace HugoLib
{

// ConsoleValue
//////////////////////////////////////////////////////////////////////////
//! get console value
int ConsoleValue::GetInt() const
{
	int ret = INT_MIN;
	const char* numberStr = m_value.c_str();
	sscanf(numberStr, "%d", &ret);
	return ret;
}

float ConsoleValue::GetFloat() const
{
	float ret = FLT_MIN;
	const char* numberStr = m_value.c_str();
	sscanf(numberStr, "%f", &ret);
	return ret;
}

//! set console value
void ConsoleValue::Set(int val)
{
	char strBuffer[64];
	sprintf(strBuffer, "%d", val);
	m_value = strBuffer;
}

void ConsoleValue::Set(float val)
{
	char strBuffer[64];
	sprintf(strBuffer, "%f", val);
	m_value = strBuffer;
}
//////////////////////////////////////////////////////////////////////////

} // HugoLib