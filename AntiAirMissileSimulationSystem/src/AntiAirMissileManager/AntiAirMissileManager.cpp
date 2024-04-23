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
		tcout << _T("startTime: ") << startTime << endl;
		tcout << _T("airThreatStartLatitude: ") << airThreatStartLatitude << endl;
		tcout << _T("airThreatStartLongitude: ") << airThreatStartLongitude << endl;
		tcout << _T("airThreatEndLatitude: ") << airThreatEndLatitude << endl;
		tcout << _T("airThreatEndLongitude: ") << airThreatEndLongitude << endl;
		tcout << _T("airThreatSpeed: ") << airThreatSpeed << endl;
		tcout << _T("antiAirMissileLatitude: ") << antiAirMissileLatitude << endl;
		tcout << _T("antiAirMissileLongitude: ") << antiAirMissileLongitude << endl;
		tcout << _T("antiAirMissileSpeed: ") << antiAirMissileSpeed << endl;
		ScenarioInfo info;
		info.startTime = startTime;
		info.airThreatStartLatitude = airThreatStartLatitude;
		info.airThreatStartLongitude = airThreatStartLongitude;
		info.airThreatEndLatitude = airThreatEndLatitude;
		info.airThreatEndLongitude = airThreatEndLongitude;
		info.airThreatSpeed = airThreatSpeed;
		info.antiAirMissileLatitude = antiAirMissileLatitude;
		info.antiAirMissileLongitude = antiAirMissileLongitude;
		info.antiAirMissileSpeed = antiAirMissileSpeed;
		antiAirMissileController.setScenarioInfo(info);
	}
	else if (nomMsg->getName() == _T("AirThreatInfo"))
	{
		tcout << _T("AntiAirMissileManager AirThreatInfo OK") << endl;
		int currentTime = nomMsg->getValue(_T("currentTime"))->toInt();
		int currentLatitude = nomMsg->getValue(_T("currentLatitude"))->toInt();
		int currentLongitude = nomMsg->getValue(_T("currentLongitude"))->toInt();
		float currentSpeed = nomMsg->getValue(_T("currentSpeed"))->toFloat();
		float currentAngle = nomMsg->getValue(_T("currentAngle"))->toFloat();
		AirThreatInfo info;
		info.currentTime = currentTime;
		info.currentLatitude = currentLatitude;
		info.currentLongitude = currentLongitude;
		info.currentSpeed = currentSpeed;
		info.currentAngle = currentAngle;
		antiAirMissileController.setAirThreatInfo(info);
	}
	else if (nomMsg->getName() == _T("SimulationStatusInfo"))
	{
		tcout << _T("AntiAirMissileManager SimulationStatusInfo OK") << endl;
		tcout << nomMsg->getValue(_T("status"))->toInt() << endl;
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
	antiAirMissileInfoMsg = this->registerMsg(_T("AntiAirMissileInfo"));
	simulationStatusInfoMsg = this->registerMsg(_T("SimulationStatusInfo"));

	std::function<void(AntiAirMissileInfo&)> sendAntiAirMissileInfo;
	sendAntiAirMissileInfo = bind(&AntiAirMissileManager::sendAntiAirMissileInfoMsg, this, placeholders::_1);
	antiAirMissileController.sendAntiAirMissileInfoMethod(sendAntiAirMissileInfo);
	std::function<void(int)> sendSimulationStatusInfo;
	sendSimulationStatusInfo = bind(&AntiAirMissileManager::sendSimulationStatusInfoMsg, this, placeholders::_1);
	antiAirMissileController.setSendSimulationStatusInfoMethod(sendSimulationStatusInfo);

	return true;
}

void AntiAirMissileManager::sendSimulationStatusInfoMsg(int status)
{
	tcout << _T("AntiAirMissileManager sendSimulationStatusInfoMsg OK") << endl;
	simulationStatusInfoMsg->setValue(_T("status"), &NUInteger(status));
	updateMsg(simulationStatusInfoMsg);
}

void AntiAirMissileManager::sendAntiAirMissileInfoMsg(AntiAirMissileInfo& antiAirMissileInfo)
{
	tcout << _T("AirThreatManager sendAntiAirMissileInfoMsg OK") << endl;
	antiAirMissileInfoMsg->setValue(_T("currentTime"), &NUInteger(antiAirMissileInfo.currentTime));
	antiAirMissileInfoMsg->setValue(_T("currentLatitude"), &NUInteger(antiAirMissileInfo.currentLatitude));
	antiAirMissileInfoMsg->setValue(_T("currentLongitude"), &NUInteger(antiAirMissileInfo.currentLongitude));
	antiAirMissileInfoMsg->setValue(_T("currentSpeed"), &NFloat(antiAirMissileInfo.currentSpeed));
	antiAirMissileInfoMsg->setValue(_T("currentAngle"), &NFloat(antiAirMissileInfo.currentAngle));
	updateMsg(antiAirMissileInfoMsg);
}


bool
AntiAirMissileManager::stop()
{
	this->deleteMsg(ICD_TestNOM);
	this->deleteMsg(antiAirMissileInfoMsg);
	this->deleteMsg(simulationStatusInfoMsg);

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

