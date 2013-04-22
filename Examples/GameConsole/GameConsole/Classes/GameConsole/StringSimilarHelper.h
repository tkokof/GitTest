/*!
    \file StringSimilarHelper.h
	\brief class for calculating string similarity
	\note now we calculate similarity using simple LCS(LongestCommonSubsequence) algorithm, for more info, please google :)
*/

#ifndef __HL_STRING_SIMILAR_HELPER_H__
#define __HL_STRING_SIMILAR_HELPER_H__

#include <string>

#include "ConsoleConfig.h"

namespace HugoLib
{

class StringSimilarHelper
{
public:
	//! simple singleton implementation
	static StringSimilarHelper* GetSingleton();
public:
	~StringSimilarHelper();
	//! init method
	bool Init();
	//! release method
	//! NOTE: call this before quit
	void Release();
	//! get similarity
	//! return value 0~1 (1 means same, 0 means totally different)
	float GetSimilarity(const std::string& base, const std::string& cmp) const;
private:
	// prevent raw creation
	StringSimilarHelper() {};
	// get LCS count
	size_t getLCSCount(const std::string& str1, const std::string& str2) const;
	// get LCS count memory less
	// NOTE: this is just a simple test
	size_t getLCSCountMemLess(const std::string& str1, const std::string& str2) const;
	// get LCS string
	std::string getLCSStr(const std::string& str1, const std::string& str2) const;
	// lower string
	std::string lowerString(const std::string& str) const;
	// upper string
	std::string upperString(const std::string& str) const;
private:
	mutable size_t m_LCSBuffer[MAX_CONSOLE_VALUE_LENGTH + 1][MAX_CONSOLE_VALUE_LENGTH + 1]; //< LCS buffer, internal using
};

} // HugoLib

#endif // __HL_STRING_SIMILAR_HELPER_H__