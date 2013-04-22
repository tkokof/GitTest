
#include "ConsoleCocos2dxLayer.h"

#include "ConsoleDataManager.h"

#include "StringSimilarHelper.h"

#include "ConsoleController.h"

#include "ConsoleViewCocos2dx.h"

namespace HugoLib
{

ConsoleCocos2dxLayer* ConsoleCocos2dxLayer::create()
{
	ConsoleCocos2dxLayer* consoleLayer = new ConsoleCocos2dxLayer();
	if (consoleLayer->init())
	{
		consoleLayer->autorelease();
		return consoleLayer;
	}

	CC_SAFE_RELEASE(consoleLayer);
	return NULL;
}

ConsoleCocos2dxLayer::~ConsoleCocos2dxLayer()
{
	// release controller
	CC_SAFE_DELETE(m_consoleController);

	// release view
	CC_SAFE_RELEASE(m_consoleView);

	// NOTE: pay attention to this, now we just clear console data here
    //////////////////////////////////////////////////////////////////////////
	// release console data manager
	ConsoleDataManager::GetSingleton()->Release();
	// release string similar helper
	StringSimilarHelper::GetSingleton()->Release();
	//////////////////////////////////////////////////////////////////////////
}

bool ConsoleCocos2dxLayer::init()
{
	if (CCLayer::init())
	{
		// first we create console view
		m_consoleView = ConsoleViewCocos2dx::create();
		if (!m_consoleView) return false;
		m_consoleView->retain();

		// then create console controller
		m_consoleController = new ConsoleControllerBase();
		if (!m_consoleController->Init())
		{
			CC_SAFE_DELETE(m_consoleController);
			return false;
		}

		// bind view to controller
		m_consoleController->SetView(m_consoleView);

		// then we create console menu
		CCSize visualSize = CCDirector::sharedDirector()->getVisibleSize();

		CCMenuItemImage* consoleButton = CCMenuItemImage::create("Console/Terminal.png", "Console/Terminal.png", this, menu_selector(ConsoleCocos2dxLayer::onConsoleButtonClick));
		if (!consoleButton) return false;
		consoleButton->setScale(0.45f);
		consoleButton->setOpacity(128);
		consoleButton->setPosition(ccp(visualSize.width * 0.05f, visualSize.height * 0.88f));

		CCMenu* consoleMenu = CCMenu::create(consoleButton, NULL);
		consoleMenu->setPosition(ccp(0, 0));
		consoleMenu->setTouchPriority(kCCMenuHandlerPriority - 2); // NOTE: pay attention to this
		addChild(consoleMenu, 1); // NOTE: should at top

		// at last, we init cvars and ccmds
		// and that's it :)
		return initCVarAndCCmd();
	}
	
	return false;
}

void ConsoleCocos2dxLayer::onConsoleButtonClick(CCObject* pSender)
{
	CCMenuItemImage* consoleButton = static_cast<CCMenuItemImage*>(pSender);
	if (consoleButton->getOpacity() > 128) // seems in console mode
	{
		consoleButton->setOpacity(128);
		// remove console view
		removeChild(m_consoleView);
	}
	else // seems not in console mode
	{
		consoleButton->setOpacity(255);
		// add console view
		addChild(m_consoleView);
	}
}

//////////////////////////////////////////////////////////////////////////
//CVar callback : void (*ConsoleVarFunc)(const ConsoleVariable*);
//CCmd callback : void (*ConsoleCmdFunc)(const ConsoleCmdArgs*);

// render info variable
static void CVariable_RenderInfo(const ConsoleVariable* cvar)
{
	if (cvar)
	{
		int renderInfo = cvar->GetValue().GetInt();
		if (renderInfo > 0) // show render info
		{
			CCDirector::sharedDirector()->setDisplayStats(true);
		}
		else // hide render info
		{
			CCDirector::sharedDirector()->setDisplayStats(false);
		}
	}
}

// game fps variable
static void CVariable_GameFPS(const ConsoleVariable* cvar)
{
	if (cvar)
	{
		float fpsValue = cvar->GetValue().GetFloat();
		CCDirector::sharedDirector()->setAnimationInterval(1.0f / fpsValue);
	}
}

// exit command(just for test)
static void CCommand_Exit(const ConsoleCmdArgs* args)
{
	CCDirector::sharedDirector()->end();

#if (CC_TARGET_PLATFORM == CC_PLATFORM_IOS)
	exit(0);
#endif
}

// dump cvars and ccmds
static void CCommand_Dump(const ConsoleCmdArgs* args)
{
	class SimplestDumper: public IConsoleVariableDumper, public IConsoleCommandDumper
	{
	public:
		virtual void OnCVarFound(ConsoleVariable* cvar) 
		{ 
			CCLog("Found CVar : %s", cvar->GetName().c_str());
		}
		virtual void OnCCmdFound(ConsoleCommand* ccmd) 
		{ 
			CCLog("Found CCmd : %s", ccmd->GetName().c_str());
		}
	} simplestDumper;

	ConsoleDataManager::GetSingleton()->DumpCVar(&simplestDumper);
	ConsoleDataManager::GetSingleton()->DumpCCmd(&simplestDumper);
}

bool ConsoleCocos2dxLayer::initCVarAndCCmd()
{
	// add cvars here
	ConsoleDataManager::GetSingleton()->AddCVar("renderInfo", 1, CVariable_RenderInfo, "Variable of render info");
	ConsoleDataManager::GetSingleton()->AddCVar("gameFPS", 60.0f, CVariable_GameFPS, "Variable of game FPS");

	// add ccmds here
	ConsoleDataManager::GetSingleton()->AddCCmd("Dump", CCommand_Dump, "Command to dump cvars and cmds simplest");
	ConsoleDataManager::GetSingleton()->AddCCmd("EXIT", CCommand_Exit, "Command to exit game");

	return true;
}

//////////////////////////////////////////////////////////////////////////

} // HugoLib