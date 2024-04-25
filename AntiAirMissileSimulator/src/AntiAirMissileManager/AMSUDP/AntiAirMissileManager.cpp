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

SimulationStatus intToSimulationStatus(int value)
{
	SimulationStatus status = IDLE;
	int statusint = value;
	if (statusint == 2)
		status = DETECTING;
	if (statusint == 3)
		status = CHASING;
	if (statusint == 4)
		status = SUCCESS;
	if (statusint == 5)
		status = FAIL;
	return status;
}

void
AntiAirMissileManager::init()
{
	setUserName(_T("AntiAirMissileManager"));

	mec = new MECComponent;
	mec->setUser(this);

	auto reflectScenarioInfo = [](AntiAirMissileController &antiAirMissileController, shared_ptr<NOM> nomMsg)
	{
		ScenarioInfo info;
		info.startTime = nomMsg->getValue(_T("startTime"))->toInt();
		info.airThreatStartLatitude = nomMsg->getValue(_T("airThreatStartLatitude"))->toDouble();
		info.airThreatStartLongitude = nomMsg->getValue(_T("airThreatStartLongitude"))->toDouble();
		info.airThreatEndLatitude = nomMsg->getValue(_T("airThreatEndLatitude"))->toDouble();
		info.airThreatEndLongitude = nomMsg->getValue(_T("airThreatEndLongitude"))->toDouble();
		info.airThreatSpeed = nomMsg->getValue(_T("airThreatSpeed"))->toFloat();
		info.antiAirMissileLatitude = nomMsg->getValue(_T("antiAirMissileLatitude"))->toDouble();
		info.antiAirMissileLongitude = nomMsg->getValue(_T("antiAirMissileLongitude"))->toDouble();
		info.antiAirMissileSpeed = nomMsg->getValue(_T("antiAirMissileSpeed"))->toFloat();
		tcout << _T("======= Received: ScenarioInfo =======") << endl;
		tcout << _T("startTime: ") << info.startTime << endl;
		tcout << _T("airThreatStartLatitude: ") << info.airThreatStartLatitude << endl;
		tcout << _T("airThreatStartLongitude: ") << info.startTime << endl;
		tcout << _T("airThreatEndLatitude: ") << info.airThreatEndLatitude << endl;
		tcout << _T("airThreatEndLongitude: ") << info.airThreatEndLongitude << endl;
		tcout << _T("airThreatSpeed: ") << info.airThreatSpeed << endl;
		tcout << _T("antiAirMissileLatitude: ") << info.antiAirMissileLatitude << endl;
		tcout << _T("antiAirMissileLongitude: ") << info.antiAirMissileLongitude << endl;
		tcout << _T("antiAirMissileSpeed: ") << info.antiAirMissileSpeed << endl;
		tcout << _T("======================================") << endl;
		antiAirMissileController.setScenarioInfo(info);
		antiAirMissileController.start(); 
	};
	msgFuncMap.insert(make_pair(_T("ScenarioInfo"), reflectScenarioInfo));

	auto reflectAirThreatInfo = [](AntiAirMissileController& antiAirMissileController, shared_ptr<NOM> nomMsg)
	{
		AirThreatInfo info;
		info.currentTime = nomMsg->getValue(_T("currentTime"))->toInt();
		info.currentLatitude = nomMsg->getValue(_T("currentLatitude"))->toDouble();
		info.currentLongitude = nomMsg->getValue(_T("currentLongitude"))->toDouble();
		info.currentSpeed = nomMsg->getValue(_T("currentSpeed"))->toFloat();
		info.currentAngle = nomMsg->getValue(_T("currentAngle"))->toFloat();
		tcout << _T("======= Received: AirThreatInfo ======") << endl;
		tcout << _T("currentTime: ") << info.currentTime << endl;
		tcout << _T("currentLatitude: ") << info.currentLatitude << endl;
		tcout << _T("currentLongitude: ") << info.currentLongitude << endl;
		tcout << _T("currentSpeed: ") << info.currentSpeed << endl;
		tcout << _T("currentAngle: ") << info.currentAngle << endl;
		tcout << _T("======================================") << endl;
		antiAirMissileController.setAirThreatInfo(info);
	};
	msgFuncMap.insert(make_pair(_T("AirThreatInfo"), reflectAirThreatInfo));

	auto reflectSimulationStatusInfo = [](AntiAirMissileController& antiAirMissileController, shared_ptr<NOM> nomMsg)
	{
		SimulationStatus status = intToSimulationStatus(nomMsg->getValue(_T("status"))->toInt());
		tcout << _T("=== Received: SimulationStatusInfo ===") << endl;
		tcout << _T("status: ") << status << endl;
		tcout << _T("======================================") << endl;
		antiAirMissileController.setSimulationStatus(status);
	};
	msgFuncMap.insert(make_pair(_T("SimulationStatusInfo"), reflectSimulationStatusInfo));
}

void
AntiAirMissileManager::release()
{
	meb = nullptr;
	delete mec;
	mec = nullptr;
	msgFuncMap.clear();
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
	msgFuncMap[nomMsg->getName()](antiAirMissileController, nomMsg);
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

}

bool
AntiAirMissileManager::start()
{
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
	NEnum enumType;
	enumType.setValue(&NInteger(status));
	simulationStatusInfoMsg->setValue(_T("status"), &enumType);
	tcout << _T("===== Send: SimulationStatusInfo =====") << endl;
	tcout << _T("status: ") << status << endl;
	tcout << _T("======================================") << endl;
	updateMsg(simulationStatusInfoMsg);
}

void AntiAirMissileManager::sendAntiAirMissileInfoMsg(AntiAirMissileInfo& antiAirMissileInfo)
{
	antiAirMissileInfoMsg->setValue(_T("currentTime"), &NInteger(antiAirMissileInfo.currentTime));
	antiAirMissileInfoMsg->setValue(_T("currentLatitude"), &NDouble(antiAirMissileInfo.currentLatitude));
	antiAirMissileInfoMsg->setValue(_T("currentLongitude"), &NDouble(antiAirMissileInfo.currentLongitude));
	antiAirMissileInfoMsg->setValue(_T("currentSpeed"), &NFloat(antiAirMissileInfo.currentSpeed));
	antiAirMissileInfoMsg->setValue(_T("currentAngle"), &NFloat(antiAirMissileInfo.currentAngle));
	tcout << _T("======= Send: AntiAirMissileInfo =====") << endl;
	tcout << _T("currentTime: ") << antiAirMissileInfo.currentTime << endl;
	tcout << _T("currentLatitude: ") << antiAirMissileInfo.currentLatitude << endl;
	tcout << _T("currentLongitude: ") << antiAirMissileInfo.currentLongitude << endl;
	tcout << _T("currentSpeed: ") << antiAirMissileInfo.currentSpeed << endl;
	tcout << _T("currentAngle: ") << antiAirMissileInfo.currentAngle << endl;
	tcout << _T("======================================") << endl;
	updateMsg(antiAirMissileInfoMsg);
}

bool
AntiAirMissileManager::stop()
{
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

