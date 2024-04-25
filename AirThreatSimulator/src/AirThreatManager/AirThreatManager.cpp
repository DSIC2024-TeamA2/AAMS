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
AirThreatManager::init()
{
	setUserName(_T("AirThreatManager"));

	// by contract
	mec = new MECComponent;
	mec->setUser(this);

	auto reflectScenarioInfo = [](AirthreatController& airThreatController, shared_ptr<NOM> nomMsg)
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
		airThreatController.setScenarioInfo(info);
		airThreatController.start();
	};
	msgFuncMap.insert(make_pair(_T("ScenarioInfo"), reflectScenarioInfo));

	auto reflectSimulationStatusInfo = [](AirthreatController& airThreatController, shared_ptr<NOM> nomMsg)
	{
		SimulationStatus status = intToSimulationStatus(nomMsg->getValue(_T("status"))->toInt());
		tcout << _T("=== Received: SimulationStatusInfo ===") << endl;
		tcout << _T("status: ") << status << endl;
		tcout << _T("======================================") << endl;
		airThreatController.setSimulationStatus(status);
	};
	msgFuncMap.insert(make_pair(_T("SimulationStatusInfo"), reflectSimulationStatusInfo));

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

void
AirThreatManager::updateMsg(shared_ptr<NOM> nomMsg)
{
	mec->updateMsg(nomMsg);
}

void
AirThreatManager::reflectMsg(shared_ptr<NOM> nomMsg)
{
	msgFuncMap[nomMsg->getName()](airThreatController, nomMsg);
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

void
AirThreatManager::sendMsg(shared_ptr<NOM> nomMsg)
{
	mec->sendMsg(nomMsg);
}

void
AirThreatManager::recvMsg(shared_ptr<NOM> nomMsg)
{

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

}

bool
AirThreatManager::start()
{
	simulationStatusInfoMsg = this->registerMsg(_T("SimulationStatusInfo"));
	airThreatInfoMsg = this->registerMsg(_T("AirThreatInfo"));

	function<void(AirThreatInfo &)> sendAirThreatInfo;
	sendAirThreatInfo = bind(&AirThreatManager::sendAirThreatInfoMsg, this, placeholders::_1);
	airThreatController.setSendAirThreatInfoMethod(sendAirThreatInfo);
	function<void(int)> sendSimulationStatusInfo;
	sendSimulationStatusInfo = bind(&AirThreatManager::sendSimulationStatusInfoMsg, this, placeholders::_1);
	airThreatController.setSendSimulationStatusInfoMethod(sendSimulationStatusInfo);

	return true;
}

void
AirThreatManager::sendSimulationStatusInfoMsg(int status)
{
	NEnum enumType;
	enumType.setValue(&NInteger(status));
	simulationStatusInfoMsg->setValue(_T("status"), &enumType);
	tcout << _T("===== Send: SimulationStatusInfo =====") << endl;
	tcout << _T("status: ") << status << endl;
	tcout << _T("======================================") << endl;
	updateMsg(simulationStatusInfoMsg);
}

void
AirThreatManager::sendAirThreatInfoMsg(AirThreatInfo &airThreatInfo)
{
	airThreatInfoMsg->setValue(_T("currentTime"), &NInteger(airThreatInfo.currentTime));
	airThreatInfoMsg->setValue(_T("currentLatitude"), &NDouble(airThreatInfo.currentLatitude));
	airThreatInfoMsg->setValue(_T("currentLongitude"), &NDouble(airThreatInfo.currentLongitude));
	airThreatInfoMsg->setValue(_T("currentSpeed"), &NFloat(airThreatInfo.currentSpeed));
	airThreatInfoMsg->setValue(_T("currentAngle"), &NFloat(airThreatInfo.currentAngle));
	tcout << _T("========= Send: AirThreatInfo ========") << endl;
	tcout << _T("currentTime: ") << airThreatInfo.currentTime << endl;
	tcout << _T("currentLatitude: ") << airThreatInfo.currentLatitude << endl;
	tcout << _T("currentLongitude: ") << airThreatInfo.currentLongitude << endl;
	tcout << _T("currentSpeed: ") << airThreatInfo.currentSpeed << endl;
	tcout << _T("currentAngle: ") << airThreatInfo.currentAngle << endl;
	tcout << _T("======================================") << endl;
	updateMsg(airThreatInfoMsg);
}

bool
AirThreatManager::stop()
{
	this->deleteMsg(simulationStatusInfoMsg);
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

