#include "AntiAirMissileManager.h"

/************************************************************************
	Constructor / Destructor
************************************************************************/
AntiAirMissileManager::AntiAirMissileManager(void)
{
	init();
}

AntiAirMissileManager::~AntiAirMissileManager(void)
{
	release();
}

void
AntiAirMissileManager::init()
{
	setUserName(_T("AntiAirMissileManager"));

	// by contract
	mec = new MECComponent;
	mec->setUser(this);
}

void
AntiAirMissileManager::release()
{
	meb = nullptr;
	delete mec;
	mec = nullptr;
}

/************************************************************************
	Inherit Function
************************************************************************/
shared_ptr<NOM>
AntiAirMissileManager::registerMsg(tstring msgName)
{
	shared_ptr<NOM> nomMsg = mec->registerMsg(msgName);
	registeredMsg.insert(pair<unsigned int, shared_ptr<NOM>>(nomMsg->getInstanceID(), nomMsg));

	return nomMsg;
}

void
AntiAirMissileManager::discoverMsg(shared_ptr<NOM> nomMsg)
{
	discoveredMsg.insert(pair<unsigned int, shared_ptr<NOM>>(nomMsg->getInstanceID(), nomMsg));
}

void AntiAirMissileManager::sendAntiAirMissileInfoMsg(int currentTime, int currentLatitude, int currentLongitude, float currentSpeed, float currentAngle)
{
	tcout << _T("AirThreatManager sendAntiAirMissileInfoMsg OK") << endl;
	antiAirMissileInfoMsg->setValue(_T("currentTime"), &NUInteger(currentTime));
	antiAirMissileInfoMsg->setValue(_T("currentLatitude"), &NUInteger(currentLatitude));
	antiAirMissileInfoMsg->setValue(_T("currentLongitude"), &NUInteger(currentLongitude));
	antiAirMissileInfoMsg->setValue(_T("currentSpeed"), &NFloat(currentSpeed));
	antiAirMissileInfoMsg->setValue(_T("currentAngle"), &NFloat(currentAngle));
	updateMsg(antiAirMissileInfoMsg);
}

void
AntiAirMissileManager::updateMsg(shared_ptr<NOM> nomMsg)
{
	mec->updateMsg(nomMsg);
}

void
AntiAirMissileManager::reflectMsg(shared_ptr<NOM> nomMsg)
{
	// if need be, write your code
	if (nomMsg->getName() == _T("ScenarioInfo"))
	{
		tcout << _T("AntiAirMissileManager ScenarioInfo OK") << endl;
		int startTime = nomMsg->getValue(_T("startTime"))->toInt();
		int airThreatStartLatitude = nomMsg->getValue(_T("airThreatStartLatitude"))->toInt();
		int airThreatStartLongitude = nomMsg->getValue(_T("airThreatStartLongitude"))->toInt();
		int airThreatEndLatitude = nomMsg->getValue(_T("airThreatEndLatitude"))->toInt();
		int airThreatEndLongitude = nomMsg->getValue(_T("airThreatEndLongitude"))->toInt();
		float airThreatSpeed = nomMsg->getValue(_T("airThreatSpeed"))->toFloat();
		int antiAirMissileLatitude = nomMsg->getValue(_T("antiAirMissileLatitude"))->toInt();
		int antiAirMissileLongitude = nomMsg->getValue(_T("antiAirMissileLongitude"))->toInt();
		float antiAirMissileSpeed = nomMsg->getValue(_T("antiAirMissileSpeed"))->toFloat();
	}
	else if (nomMsg->getName() == _T("AirThreatInfo"))
	{
		tcout << _T("AntiAirMissileManager AirThreatInfo OK") << endl;
		int currentTime = nomMsg->getValue(_T("currentTime"))->toInt();
		int currentLatitude = nomMsg->getValue(_T("currentLatitude"))->toInt();
		int currentLongitude = nomMsg->getValue(_T("currentLongitude"))->toInt();
		float currentSpeed = nomMsg->getValue(_T("currentSpeed"))->toFloat();
		float currentAngle = nomMsg->getValue(_T("currentAngle"))->toFloat();
	}
}

void
AntiAirMissileManager::deleteMsg(shared_ptr<NOM> nomMsg)
{
	mec->deleteMsg(nomMsg);
	registeredMsg.erase(nomMsg->getInstanceID());
}

void
AntiAirMissileManager::removeMsg(shared_ptr<NOM> nomMsg)
{
	discoveredMsg.erase(nomMsg->getInstanceID());
}

void AntiAirMissileManager::sendDetectingMsg()
{
	tcout << _T("AntiAirMissileManager sendDetectingMsg OK") << endl;
	sendMsg(detectingMsg);
}

void AntiAirMissileManager::sendChasingMsg()
{
	tcout << _T("AntiAirMissileManager sendChasingMsg OK") << endl;
	sendMsg(chasingMsg);
}

void AntiAirMissileManager::sendSuccessMsg()
{
	tcout << _T("AntiAirMissileManager sendSuccessMsg OK") << endl;
	sendMsg(successMsg);
}

void
AntiAirMissileManager::sendMsg(shared_ptr<NOM> nomMsg)
{
	mec->sendMsg(nomMsg);
}

void
AntiAirMissileManager::recvMsg(shared_ptr<NOM> nomMsg)
{
	// you can change the code below, if necessary
	if (nomMsg->getName() == _T("GUI_Start"))
	{
		tcout << _T("OK") << endl;
		ICD_TestNOM->setValue(_T("Message1"), &NUInteger(7));
		this->updateMsg(ICD_TestNOM);
	}
	else if (nomMsg->getName() == _T("IDLE"))
	{
		tcout << _T("AirThreatManager IDLE OK") << endl;
	}
	else if (nomMsg->getName() == _T("FAIL"))
	{
		tcout << _T("AirThreatManager FAIL OK") << endl;
	}

	// if need be, write your code
}

void
AntiAirMissileManager::setUserName(tstring userName)
{
	name = userName;
}

tstring
AntiAirMissileManager::getUserName()
{
	return name;
}

void
AntiAirMissileManager::setData(void* data)
{
	// if need be, write your code
}

bool
AntiAirMissileManager::start()
{
	// you can change the code below, if necessary
	ICD_TestNOM = this->registerMsg(_T("ICD_Test1"));
	detectingMsg = this->registerMsg(_T("DETECTING"));
	chasingMsg = this->registerMsg(_T("CHASING"));
	successMsg = this->registerMsg(_T("SUCCESS"));
	antiAirMissileInfoMsg = this->registerMsg(_T("AntiAirMissileInfo"));
	return true;
}

bool
AntiAirMissileManager::stop()
{
	this->deleteMsg(ICD_TestNOM);
	this->deleteMsg(detectingMsg);
	this->deleteMsg(chasingMsg);
	this->deleteMsg(successMsg);
	this->deleteMsg(antiAirMissileInfoMsg);

	return true;
}

void
AntiAirMissileManager::setMEBComponent(IMEBComponent* realMEB)
{
	meb = realMEB;
	mec->setMEB(meb);
}

/************************************************************************
	Export Function
************************************************************************/
extern "C" BASEMGRDLL_API
BaseManager * createObject()
{
	return new AntiAirMissileManager;
}

extern "C" BASEMGRDLL_API
void deleteObject(BaseManager * userManager)
{
	delete userManager;
}

