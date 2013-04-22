
#include <cmath>

#include "ConsoleViewCocos2dx.h"

namespace HugoLib
{

//! simple create interface
ConsoleViewCocos2dx* ConsoleViewCocos2dx::create()
{
    ConsoleViewCocos2dx* consoleView = new ConsoleViewCocos2dx();
	if (consoleView->init())
	{
		consoleView->autorelease();
		return consoleView;
	}

	CC_SAFE_RELEASE(consoleView);
	return NULL;
}

ConsoleViewCocos2dx::~ConsoleViewCocos2dx()
{
	CC_SAFE_RELEASE(m_fontConfig);
}

//! set prompt position
void ConsoleViewCocos2dx::SetPromptPos(size_t pos)
{
	// buffer pos data
	m_promptPos = pos;

	// set prompt pos
	CCPoint basePos = m_inputText->getPosition();

	const std::string inputText = m_inputText->getString();
	float xOffset = 0;
	// first we calculate front "pos - 1" char advance
	for (size_t i = 0; i < pos; ++i)
	{
	    xOffset += getAdvanceOfPos(i);
	}
    
	// last pos should be half advance
	xOffset += getAdvanceOfPos(pos) * 0.5f;

	// that's it :)
	m_prompt->setPosition(ccpAdd(basePos, ccp(xOffset, 0)));
}

size_t ConsoleViewCocos2dx::GetPromptPos() const
{
	return m_promptPos;
}

bool ConsoleViewCocos2dx::attachWithIME()
{
    bool bRet = CCIMEDelegate::attachWithIME();
    if (bRet)
    {
        // open keyboard
        CCEGLView * pGlView = CCDirector::sharedDirector()->getOpenGLView();
        if (pGlView)
        {
            pGlView->setIMEKeyboardState(true);
        }
    }
    return bRet;
}
    
bool ConsoleViewCocos2dx::detachWithIME()
{
    bool bRet = CCIMEDelegate::detachWithIME();
    if (bRet)
    {
        // close keyboard
        CCEGLView * pGlView = CCDirector::sharedDirector()->getOpenGLView();
        if (pGlView)
        {
            pGlView->setIMEKeyboardState(false);
        }
    }
    return bRet;
}
    
void ConsoleViewCocos2dx::insertText(const char * text, int len)
{
	if (m_consoleActionDelegate)
	{
		ConsoleActionData action(ECA_Char);
		for (int i = 0; i < len; ++i)
		{
			if (text[i] == '\n') // confirm character
			{
				action.action = ECA_Confirm;
#if (CC_TARGET_PLATFORM == CC_PLATFORM_IOS)
                // NOTE: not so good, but at least it works ...
                // TODO: make it better
                detachWithIME();
#endif
			}
			else // normal character
			{
				action.chr = text[i];
			}
            m_consoleActionDelegate->OnEvent(action);
		}
	}
}

void ConsoleViewCocos2dx::deleteBackward()
{
	if (m_consoleActionDelegate)
	{
		ConsoleActionData action(ECA_Delete);
		m_consoleActionDelegate->OnEvent(action);
	}
}

bool ConsoleViewCocos2dx::init()
{
	if (CCLayer::init())
	{
		CCSize visualSize = CCDirector::sharedDirector()->getVisibleSize();

		// init background
		CCSprite* bg = CCSprite::create("Console/ConsoleBackground.png");
		if (!bg) return false;
		bg->setPosition(ccp(visualSize.width * 0.5f, visualSize.height * 0.88f));
		bg->setOpacity(128);
		addChild(bg);

		// init input text
		m_inputText = CCLabelBMFont::create("", "Console/Console.fnt");
		if (!m_inputText) return false;
		m_inputText->setAnchorPoint(ccp(0, 0.5));
		
		m_inputText->setPosition(ccp(visualSize.width * 0.12f, visualSize.height * 0.83f));
		addChild(m_inputText);

		// init output text
		m_outputText = CCLabelBMFont::create("", "Console/Console.fnt");
		if (!m_outputText) return false;
		m_outputText->setAnchorPoint(ccp(0, 0.5));
		m_outputText->setColor(ccGREEN);
		m_outputText->setPosition(ccp(visualSize.width * 0.12f, visualSize.height * 0.93f));
		addChild(m_outputText);

		// get font config
		m_fontConfig = FNTConfigLoadFile(m_inputText->getFntFile());
		m_fontConfig->retain();

		// simple set font size
		// TODO: make this better
		//////////////////////////////////////////////////////////////////////////
		tCCFontDefHashElement* element = NULL;
		size_t key = 'a';

		HASH_FIND_INT(m_fontConfig->m_pFontDefDictionary, &key, element);
		if (element)
		{
			ccBMFontDef fontDef = element->fontDef;
			CCRect fntSize = fontDef.rect;
			m_inputText->setScale(10.0f / fntSize.size.width);
			m_outputText->setScale(10.0f / fntSize.size.width);
		}
		//////////////////////////////////////////////////////////////////////////

		// init prompt
		m_prompt = CCSprite::create("Console/Prompt.png");
		if (!m_prompt) return false;
		addChild(m_prompt);
		SetPromptPos(0);

		// reset data
		m_promptPos = 0;
		m_consoleActionDelegate = NULL;
		m_curTouch = NULL;
		m_touchMoved = false;

		// set touch mode so we can receive target touch event
		setTouchMode(kCCTouchesOneByOne);
		// set touch priority
		setTouchPriority(kCCMenuHandlerPriority - 1); // NOTE: pay attention to this
		// enable touch
		setTouchEnabled(true);

		return true;
	}
	
	return false;
}

void ConsoleViewCocos2dx::onEnter()
{
	CCLayer::onEnter();

	attachWithIME();

	// do prompt blink action
	CCActionInterval* fade = CCFadeOut::create(0.6f);
	CCActionInterval* fade_in = fade->reverse();
	m_prompt->runAction(CCRepeatForever::create(CCSequence::createWithTwoActions(fade, fade_in)));
}

void ConsoleViewCocos2dx::onExit()
{
	// stop blink action
	m_prompt->stopAllActions();

	detachWithIME();

	CCLayer::onExit();
}

bool ConsoleViewCocos2dx::ccTouchBegan(CCTouch* pTouch, CCEvent* pEvent)
{
	// now we just support single touch
	if (m_curTouch) return true; // NOTE: not so sure about "true" return to swallow touch

	// update touch data
	m_curTouch = pTouch;
	m_lastPhaseTouchPoint = m_curTouch->getLocation();
	m_touchMoved = false;

	return true;
}

void ConsoleViewCocos2dx::ccTouchMoved(CCTouch* pTouch, CCEvent* pEvent)
{
	if (m_curTouch == pTouch)
	{
		CCPoint curPoint = pTouch->getLocation();
		if (handleTouchMoved(curPoint))
		{
			m_lastPhaseTouchPoint = curPoint;
			m_touchMoved = true;
		}
	}
}

void ConsoleViewCocos2dx::ccTouchEnded(CCTouch* pTouch, CCEvent* pEvent)
{
	if (m_curTouch == pTouch)
	{
		if (!m_touchMoved) // seems a simple tap, do auto complete
		{
			ConsoleActionData action(ECA_AutoComplete);
			if (m_consoleActionDelegate)
			{
				m_consoleActionDelegate->OnEvent(action);
			}
		}

		// reset touch data
		m_curTouch = NULL;
	}
}

void ConsoleViewCocos2dx::ccTouchCancelled(CCTouch* pTouch, CCEvent* pEvent)
{
	if (m_curTouch == pTouch)
	{
		// reset touch data
		m_curTouch = NULL;
	}
}

bool ConsoleViewCocos2dx::handleTouchMoved(const CCPoint& movePoint)
{
	float moveThreshold = 16; // just test number

	float moveDist = ccpDistance(m_lastPhaseTouchPoint, movePoint);
	
	if (moveDist < moveThreshold) return false;

	// estimate direction
	float deltaX = movePoint.x - m_lastPhaseTouchPoint.x;
	float deltaY = movePoint.y - m_lastPhaseTouchPoint.y;
	float slope = fabs(deltaY / deltaX);

	ConsoleActionData action(ECA_Unknown);
	if (slope >= 1) // vertical move
	{
		if (deltaY > 0) // up move
		{
			action.action = ECA_PreInput;
		}
		else // down move
		{
			action.action = ECA_PostInput;
		}
	}
	else
	{
		if (deltaX > 0) // right move
		{
			action.action = ECA_PostPrompt;
		}
		else // left move
		{
			action.action = ECA_PrePrompt;
		}
	}

	if (m_consoleActionDelegate)
	{
		m_consoleActionDelegate->OnEvent(action);
	}

	return true;
}

// get advance of pos
float ConsoleViewCocos2dx::getAdvanceOfPos(size_t pos)
{
	const std::string inputText = m_inputText->getString();
	if (pos >= inputText.size()) // not in the text
	{
		// use prompt width
		CCSize promptSize = m_prompt->getContentSize();
		return promptSize.width * m_prompt->getScaleX();
	}
	else
	{
		tCCFontDefHashElement* element = NULL;
		size_t key = inputText[pos];

		HASH_FIND_INT(m_fontConfig->m_pFontDefDictionary, &key, element);
		if (element)
		{
			ccBMFontDef fontDef = element->fontDef;
			return fontDef.xAdvance * m_inputText->getScaleX();
		}
	}

	return 0; // default we just return 0
}

} // HugoLib