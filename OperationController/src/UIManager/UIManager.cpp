#include "UIManager.h"
#include "UIMessageEnum.h"
#include <cassert>

/************************************************************************
	constructor / destructor
************************************************************************/
UIManager::UIManager(void)
{
	initialize();
}

UIManager::~UIManager(void)
{
	release();
}

/************************************************************************
	initialize / release
************************************************************************/
void
UIManager::initialize(void)
{
	setUserName(_T("UIManager"));

	// design by contract
	mec = new MECComponent;
	mec->setUser(this);
}

void
UIManager::release()
{
	meb = NULL;
	delete mec;
	mec = NULL;
}

/************************************************************************
	inherited function
************************************************************************/
shared_ptr<NOM>
UIManager::registerMsg(tstring msgName)
{
	shared_ptr<NOM> nomMsg = mec->registerMsg(msgName);
	registeredMsg.insert(pair<unsigned int, shared_ptr<NOM>>(nomMsg->getInstanceID(), nomMsg));

	return nomMsg;
}

void
UIManager::discoverMsg(shared_ptr<NOM> nomMsg)
{
	discoveredMsg.insert(pair<unsigned int, shared_ptr<NOM>>(nomMsg->getInstanceID(), nomMsg));
	::SendMessage(winHandle, UM_DiscoveredNOM, (WPARAM)nomMsg.get(), 0);
}

void
UIManager::updateMsg(shared_ptr<NOM> nomMsg)
{
	if (nomMsg->getName() == _T("ScenarioInfo"))
	{
		tcout << _T("========= Send: ScenarioInfo =========") << endl;
		tcout << _T("startTime: ") << nomMsg->getValue(_T("startTime"))->toInt() << endl;
		tcout << _T("airThreatStartLatitude: ") << nomMsg->getValue(_T("airThreatStartLatitude"))->toDouble() << endl;
		tcout << _T("airThreatStartLongitude: ") << nomMsg->getValue(_T("airThreatStartLongitude"))->toDouble() << endl;
		tcout << _T("airThreatEndLatitude: ") << nomMsg->getValue(_T("airThreatEndLatitude"))->toDouble() << endl;
		tcout << _T("airThreatEndLongitude: ") << nomMsg->getValue(_T("airThreatEndLongitude"))->toDouble() << endl;
		tcout << _T("airThreatSpeed: ") << nomMsg->getValue(_T("airThreatSpeed"))->toFloat() << endl;
		tcout << _T("antiAirMissileLatitude: ") << nomMsg->getValue(_T("antiAirMissileLatitude"))->toDouble() << endl;
		tcout << _T("antiAirMissileLongitude: ") << nomMsg->getValue(_T("antiAirMissileLongitude"))->toDouble() << endl;
		tcout << _T("antiAirMissileSpeed: ") << nomMsg->getValue(_T("antiAirMissileSpeed"))->toFloat() << endl;
		tcout << _T("======================================") << endl;
		nomMsg->copyTo(scenarioInfoMsg);
		mec->updateMsg(scenarioInfoMsg);
	}
	else if (nomMsg->getName() == _T("SimulationStatusInfo"))
	{
		tcout << _T("===== Send: SimulationStatusInfo =====") << endl;
		tcout << _T("status: ") << nomMsg->getValue(_T("status"))->toInt() << endl;
		tcout << _T("======================================") << endl;
		nomMsg->copyTo(simulationStatusInfoMsg);
		mec->updateMsg(simulationStatusInfoMsg);
	}
}

void
UIManager::reflectMsg(shared_ptr<NOM> nomMsg)
{
	if (nomMsg->getName() == _T("AirThreatInfo")
		|| nomMsg->getName() == _T("AntiAirMissileInfo")
		|| nomMsg->getName() == _T("SimulationStatusInfo"))
	{
		tcout << _T("===== Received: ");
		tcout << nomMsg->getName();
		tcout << " =====" << endl;

		auto length = 0;
		unsigned char* nomBytes = nomMsg->serialize(length);

		NOMInfo nomInfo;
		_tcscpy(nomInfo.MsgName, nomMsg->getName().c_str());
		nomInfo.MsgID = nomMsg->getMessageID();
		nomInfo.MsgLen = length;

		::SendMessage(winHandle, UM_ReflectedNOM, (WPARAM)&nomInfo, (LPARAM)nomBytes);
		delete[] nomBytes;
	}
}

void
UIManager::deleteMsg(shared_ptr<NOM> nomMsg)
{
	registeredMsg.erase(nomMsg->getInstanceID());
	mec->deleteMsg(nomMsg);
}

void
UIManager::removeMsg(shared_ptr<NOM> nomMsg)
{
	discoveredMsg.erase(nomMsg->getInstanceID());
	::SendMessage(winHandle, UM_RemovedNOM, (WPARAM)nomMsg.get(), 0);
}

void
UIManager::sendMsg(shared_ptr<NOM> nomMsg)
{
	// if need be, write your code
	mec->sendMsg(nomMsg);
}

void
UIManager::recvMsg(shared_ptr<NOM> nomMsg)
{
	// you can use the code below, if necessary
	// shared_ptr<NOM> guiNOM = uiMessageHadler->getGUIMsgNOM(nomMsg);
	::SendMessage(winHandle, UM_ReceivedNOM, (WPARAM)nomMsg.get(), 0);
}

void
UIManager::setUserName(tstring userName)
{
	name = userName;
}

tstring
UIManager::getUserName()
{
	return name;
}

void
UIManager::setData(void* data)
{
	this->winHandle = (HWND)data;
	assert("HWND == 0" && winHandle);
}

bool
UIManager::start()
{
	// if need be, write your code
	scenarioInfoMsg = this->registerMsg(_T("ScenarioInfo"));
	simulationStatusInfoMsg = this->registerMsg(_T("SimulationStatusInfo"));

	return true;
}

bool
UIManager::stop()
{
	// if need be, write your code
	this->deleteMsg(scenarioInfoMsg);
	this->deleteMsg(simulationStatusInfoMsg);

	return true;
}

void
UIManager::setMEBComponent(IMEBComponent* realMEB)
{
	meb = realMEB;
	mec->setMEB(meb);
}

/************************************************************************
	exported function
************************************************************************/
extern "C" BASEMGRDLL_API
BaseManager * createObject()
{
	return new UIManager;
}

extern "C" BASEMGRDLL_API
void deleteObject(BaseManager * userManager)
{
	delete userManager;
}