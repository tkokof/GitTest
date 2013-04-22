/*!
    \file ConsoleCocos2dxLayer.h
	\brief simple implementation of cocos2dx console layer
*/

#ifndef __HL_CONSOLE_COCOS2DX_LAYER_H__
#define __HL_CONSOLE_COCOS2DX_LAYER_H__

#include "cocos2d.h"

USING_NS_CC;

namespace HugoLib
{

class ConsoleViewCocos2dx;
class ConsoleControllerBase;

class ConsoleCocos2dxLayer: public CCLayer
{
public:
	static ConsoleCocos2dxLayer* create();
public:
	virtual ~ConsoleCocos2dxLayer();
	virtual bool init();
private:
	ConsoleCocos2dxLayer():m_consoleView(NULL),m_consoleController(NULL) {};
	bool initCVarAndCCmd();
	void onConsoleButtonClick(CCObject* pSender);
private:
	ConsoleViewCocos2dx* m_consoleView; //< console view
	ConsoleControllerBase* m_consoleController; //< console controller
};

} // HugoLib

#endif // __HL_CONSOLE_COCOS2DX_LAYER_H__