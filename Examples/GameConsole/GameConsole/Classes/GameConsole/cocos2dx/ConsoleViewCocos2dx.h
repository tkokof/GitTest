/*!
    \file ConsoleViewCocos2dx.h
	\brief simple console view implementation of cocos2d-x
*/

#ifndef __HL_CONSOLE_VIEW_COCOS2DX_H__
#define __HL_CONSOLE_VIEW_COCOS2DX_H__

#include "cocos2d.h"

USING_NS_CC;

#include "ConsoleView.h"

namespace HugoLib
{

class ConsoleViewCocos2dx: public CCLayer, public CCIMEDelegate, public ConsoleView
{
public:
	//! simple create interface
	static ConsoleViewCocos2dx* create();
public:
	virtual ~ConsoleViewCocos2dx();
	// console view
	//////////////////////////////////////////////////////////////////////////
	//! set console action delegate
	virtual void SetActionDelegate(ConsoleActionDelegate* delegate) { m_consoleActionDelegate = delegate; }
	//! get console action delegate
	virtual ConsoleActionDelegate* GetActionDelegate() { return m_consoleActionDelegate; }

	//! set line max char count
	virtual void SetLineMaxCharCount(size_t maxCount) { m_lineMaxCharCount = maxCount; }
	//! get line max char count
	virtual size_t GetLineMaxCharCount() const { return m_lineMaxCharCount; }

	//! output an new line
	virtual void OutputLine(const std::string& line) { m_outputText->setString(line.c_str()); }
	//! output input string
	virtual void OutputInput(const std::string& input) { m_inputText->setString(input.c_str()); }

	//! set prompt position
	virtual void SetPromptPos(size_t pos);
	//! get prompt position
	virtual size_t GetPromptPos() const;
	//////////////////////////////////////////////////////////////////////////
	// CCIMEDelegate interface
	//////////////////////////////////////////////////////////////////////////
    virtual bool attachWithIME();
    virtual bool detachWithIME();
	virtual bool canAttachWithIME() { return true; }
	virtual bool canDetachWithIME() { return true; }
	virtual void insertText(const char * text, int len);
	virtual void deleteBackward();
	virtual const char * getContentText() { return m_inputText->getString(); }
	//////////////////////////////////////////////////////////////////////////
	// CCLayer interface
	//////////////////////////////////////////////////////////////////////////
	virtual bool init();
	virtual void onEnter();
	virtual void onExit();
	// console touch event
	virtual bool ccTouchBegan(CCTouch* pTouch, CCEvent* pEvent);
	virtual void ccTouchMoved(CCTouch* pTouch, CCEvent* pEvent);
	virtual void ccTouchEnded(CCTouch* pTouch, CCEvent* pEvent);
	virtual void ccTouchCancelled(CCTouch* pTouch, CCEvent* pEvent);
	//////////////////////////////////////////////////////////////////////////
protected:
	// handle touch move
	virtual bool handleTouchMoved(const CCPoint& movePoint);
	// get advance of pos
	virtual float getAdvanceOfPos(size_t pos);
protected:
	// console action delegate
	ConsoleActionDelegate* m_consoleActionDelegate;
	size_t m_lineMaxCharCount;

	// input display
	CCLabelBMFont* m_inputText;
	CCLabelBMFont* m_outputText;
	CCBMFontConfiguration* m_fontConfig;
	CCSprite* m_prompt;
	size_t m_promptPos;

	// touch manage
	CCTouch* m_curTouch;
	CCPoint m_lastPhaseTouchPoint;
	bool m_touchMoved;
};

} // HugoLib

#endif // __HL_CONSOLE_VIEW_COCOS2DX_H__

