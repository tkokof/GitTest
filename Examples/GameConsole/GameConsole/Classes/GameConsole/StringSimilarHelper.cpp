
#include <algorithm>
#include <vector>

#include "StringSimilarHelper.h"

namespace HugoLib
{

static StringSimilarHelper* s_stringSimilarHelper = NULL;

//! simple singleton implementation
StringSimilarHelper* StringSimilarHelper::GetSingleton()
{
	if (!s_stringSimilarHelper)
	{
		s_stringSimilarHelper = new StringSimilarHelper();
		s_stringSimilarHelper->Init();
	}

	return s_stringSimilarHelper;
}

StringSimilarHelper::~StringSimilarHelper() 
{
	// reset static instance pointer
	s_stringSimilarHelper = NULL;
}
	
//! init method
bool StringSimilarHelper::Init()
{
	// TODO: implement
	return true;
}
	
//! release method
//! NOTE: call this before quit
void StringSimilarHelper::Release()
{
	// NOTE: not so sure about this
	delete this;
}
	
//! get similarity
//! return value 0~1 (1 means same, 0 means totally different)
float StringSimilarHelper::GetSimilarity(const std::string& base_, const std::string& cmp_) const
{
	// compare in-case
	std::string base = lowerString(base_);
	std::string cmp = lowerString(cmp_);

	// NOTE: now we just use a simple equation
	// string similar = LCS(base,cmp) / base * factorBase + LCS(base,cmp) / cmp * factorCmp
	size_t lenBase = base.length();
	size_t lenCmp = cmp.length();
	size_t lenLCS = getLCSCount(base, cmp);
	// below values are just test values
	float factorBase = 0.75f;
	float factorCmp = 0.25f;
	return lenLCS / (float)lenBase * factorBase + lenLCS / (float)lenCmp * factorCmp;
}

// get LCS count
size_t StringSimilarHelper::getLCSCount(const std::string& str1, const std::string& str2) const
{
	size_t bufRow = str1.length() + 1; // pay attention to "+1"
    size_t bufCol = str2.length() + 1; // pay attention to "+1"

    // init begin value
    for (size_t i = 0; i < bufRow; ++i)
    {
        m_LCSBuffer[i][0] = 0;
    }

    for (size_t i = 0; i < bufCol; ++i)
    {
        m_LCSBuffer[0][i] = 0;
    }

    // dynamic programming
    for (size_t i = 1; i < bufRow; ++i)
    {
        for (size_t j = 1; j < bufCol; ++j)
        {
            if (str1[i - 1] == str2[j - 1])
            {
                m_LCSBuffer[i][j] = m_LCSBuffer[i - 1][j - 1] + 1;
            }
            else
            {
                size_t val1 = m_LCSBuffer[i - 1][j];
                size_t val2 = m_LCSBuffer[i][j - 1];
                m_LCSBuffer[i][j] = std::max(val1, val2);
            }
        }
    }

    return m_LCSBuffer[bufRow - 1][bufCol - 1];
}

// get LCS count memory less
// NOTE: this is just a simple test
size_t StringSimilarHelper::getLCSCountMemLess(const std::string& str1, const std::string& str2) const
{
	size_t bufRow = str1.length() + 1; // pay attention to "+1"
    size_t bufCol = str2.length() + 1; // pay attention to "+1"

    std::vector<size_t> LCSBuffer(bufCol, 0);
    std::vector<size_t> LCSBufferAnother(bufCol, 0);

    // last col value
    size_t val_last_col = 0;
    size_t LCSCount = 0;
   
	// dynamic programming
    for (size_t i = 1; i < bufRow; ++i)
    {
        for (size_t j = 1; j < bufCol; ++j)
        {
            if (str1[i - 1] == str2[j - 1])
            {
                LCSCount = LCSBuffer[j - 1] + 1;
                val_last_col = LCSCount;
                LCSBufferAnother[j] = LCSCount;
            }
            else
            {
                size_t val1 = LCSBuffer[j];
                size_t val2 = val_last_col;
                LCSCount = std::max(val1, val2);
                LCSBufferAnother[j] = LCSCount;
            }
        }

        // swap buffer
        LCSBuffer.swap(LCSBufferAnother);
        val_last_col = 0; // pay attention reset last col value
    }

    return LCSCount;
}
	
// get LCS string
std::string StringSimilarHelper::getLCSStr(const std::string& str1, const std::string& str2) const
{
    size_t bufRow = str1.length() + 1; // pay attention to "+1"
    size_t bufCol = str2.length() + 1; // pay attention to "+1"

    // alloc buffer
    std::vector<size_t> LCSBuffer(bufRow * bufCol, 0);
    std::vector<char> LCSBacktrackBuffer(bufRow * bufCol, 0);

    // dynamic programming
    for (size_t i = 1; i < bufRow; ++i)
    {
        for (size_t j = 1; j < bufCol; ++j)
        {
            if (str1[i - 1] == str2[j - 1])
            {
                LCSBuffer[i * bufCol + j] = LCSBuffer[(i - 1) * bufCol + j - 1] + 1;
                LCSBacktrackBuffer[i * bufCol + j] = 0; // "0" means char equal
            }
            else
            {
                size_t val1 = LCSBuffer[(i - 1) * bufCol + j];
                size_t val2 = LCSBuffer[i * bufCol + j - 1];
                if (val1 > val2)
                {
                    LCSBuffer[i * bufCol + j] = val1;
                    LCSBacktrackBuffer[i * bufCol + j] = 1; // "1" means use last row
                }
                else
                {
                    LCSBuffer[i * bufCol + j] = val2;
                    LCSBacktrackBuffer[i * bufCol + j] = -1; // "-1" means use last col
                }
            }
        }
    }

    // back trace LCS string
    std::string LCSStr;
    size_t rowIndex = bufRow - 1;
    size_t colIndex = bufCol - 1;

    while (rowIndex > 0 && colIndex > 0)
    {
        if (LCSBacktrackBuffer[rowIndex * bufCol + colIndex] == 0)
        {
            LCSStr.insert(0, 1, str1[rowIndex - 1]);

            --rowIndex;
            --colIndex;
        }
        else if (LCSBacktrackBuffer[rowIndex * bufCol + colIndex] > 0)
        {
            --rowIndex;
        }
        else
        {
            --colIndex;
        }
    }

    return LCSStr;
}

// lower string
std::string StringSimilarHelper::lowerString(const std::string& str) const
{
	std::string tmpStr = str;
	for (size_t i = 0; i < tmpStr.length(); ++i)
	{
		if (tmpStr[i] >= 'A' && tmpStr[i] <= 'Z')
		{
			tmpStr[i] = tmpStr[i] - 'A' + 'a';
		}
	}

	return tmpStr;
}

// upper string
std::string StringSimilarHelper::upperString(const std::string& str) const
{
	std::string tmpStr = str;
	for (size_t i = 0; i < tmpStr.length(); ++i)
	{
		if (tmpStr[i] >= 'a' && tmpStr[i] <= 'z')
		{
			tmpStr[i] = tmpStr[i] - 'a' + 'A';
		}
	}

	return tmpStr;
}

} // HugoLib