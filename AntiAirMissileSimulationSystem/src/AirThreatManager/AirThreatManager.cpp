#include "AirThreatManager.h"

/************************************************************************
	Constructor / Destructor
************************************************************************/
AirThreatManager::AirThreatManager(void)
{
	init();
}

AirThreatManager::~AirThreatManager(void)
{
	release();
}

void
AirThreatManager::init()
{
	setUserName(_T("AirThreatManager"));

	// by contract
	mec = new MECComponent;
	mec->setUser(this);
}

void
AirThreatManager::release()
{
	meb = nullptr;
	delete mec;
	mec = nullptr;
}

/************************************************************************
	Inherit Function
************************************************************************/
shared_ptr<NOM>
AirThreatManager::registerMsg(tstring msgName)
{
	shared_ptr<NOM> nomMsg = mec->registerMsg(msgName);
	registeredMsg.insert(pair<unsigned int, shared_ptr<NOM>>(nomMsg->getInstanceID(), nomMsg));

	return nomMsg;
}

void
AirThreatManager::discoverMsg(shared_ptr<NOM> nomMsg)
{
	discoveredMsg.insert(pair<unsigned int, shared_ptr<NOM>>(nomMsg->getInstanceID(), nomMsg));
}

void AirThreatManager::sendAirThreatInfoMsg(int currentTime, int currentLatitude, int currentLongitude, float currentSpeed, float currentAngle)
{
	tcout << _T("AirThreatManager sendAirThreatInfoMsg OK") << endl;
	airThreatInfoMsg->setValue(_T("currentTime"), &NUInteger(currentTime));
	airThreatInfoMsg->setValue(_T("currentLatitude"), &NUInteger(currentLatitude));
	airThreatInfoMsg->setValue(_T("currentLongitude"), &NUInteger(currentLongitude));
	airThreatInfoMsg->setValue(_T("currentSpeed"), &NFloat(currentSpeed));
	airThreatInfoMsg->setValue(_T("currentAngle"), &NFloat(currentAngle));
	updateMsg(airThreatInfoMsg);
}

void
AirThreatManager::updateMsg(shared_ptr<NOM> nomMsg)
{
	// AirThreatInfo 메시지 send 해야함
	// airThreatInfoMsg를 매개변수로 전달하면 됨
	mec->updateMsg(nomMsg);
}

void
AirThreatManager::reflectMsg(shared_ptr<NOM> nomMsg)
{
	// if need be, write your code
	if (nomMsg->getName() == _T("ScenarioInfo"))
	{
		tcout << _T("AirThreatManager ScenarioInfo OK") << endl;
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
}

void
AirThreatManager::deleteMsg(shared_ptr<NOM> nomMsg)
{
	mec->deleteMsg(nomMsg);
	registeredMsg.erase(nomMsg->getInstanceID());
}

void
AirThreatManager::removeMsg(shared_ptr<NOM> nomMsg)
{
	discoveredMsg.erase(nomMsg->getInstanceID());
}

void AirThreatManager::sendFailMsg()
{
	tcout << _T("AirThreatManager sendFailMsg OK") << endl;
	sendMsg(failMsg);
}

void
AirThreatManager::sendMsg(shared_ptr<NOM> nomMsg)
{
	// FAIL 메시지 send 해야함
	// FAIL_MSG를 매개변수로 전달하면 됨
	mec->sendMsg(nomMsg);
}

void
AirThreatManager::recvMsg(shared_ptr<NOM> nomMsg)
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
	else if (nomMsg->getName() == _T("DETECTING"))
	{
		tcout << _T("AirThreatManager DETECTING OK") << endl;
	}
	else if (nomMsg->getName() == _T("CHASING"))
	{
		tcout << _T("AirThreatManager CHASING OK") << endl;
	}
	else if (nomMsg->getName() == _T("SUCCESS"))
	{
		tcout << _T("AirThreatManager SUCCESS OK") << endl;
	}
	// if need be, write your code
}

void
AirThreatManager::setUserName(tstring userName)
{
	name = userName;
}

tstring
AirThreatManager::getUserName()
{
	return name;
}

void
AirThreatManager::setData(void* data)
{
	// if need be, write your code
}

bool
AirThreatManager::start()
{
	// you can change the code below, if necessary
	ICD_TestNOM = this->registerMsg(_T("ICD_Test1"));
	failMsg = this->registerMsg(_T("FAIL"));
	scenarioInfoMsg = this->registerMsg(_T("ScenarioInfo"));
	airThreatInfoMsg = this->registerMsg(_T("AirThreatInfo"));
	return true;
}

bool
AirThreatManager::stop()
{
	this->deleteMsg(ICD_TestNOM);
	this->deleteMsg(failMsg);
	this->deleteMsg(scenarioInfoMsg);
	this->deleteMsg(airThreatInfoMsg);

	return true;
}

void
AirThreatManager::setMEBComponent(IMEBComponent* realMEB)
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
	return new AirThreatManager;
}

extern "C" BASEMGRDLL_API
void deleteObject(BaseManager * userManager)
{
	delete userManager;
}

