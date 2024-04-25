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
		airThreatController.setScenarioInfo(info);
		airThreatController.start();
	};
	msgFuncMap.insert(make_pair(_T("ScenarioInfo"), reflectScenarioInfo));

	auto reflectSimulationStatusInfo = [](AirthreatController& airThreatController, shared_ptr<NOM> nomMsg)
	{
		SimulationStatus status = intToSimulationStatus(nomMsg->getValue(_T("status"))->toInt());
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
	tcout << _T("AirThreatManager updateMsg called") << endl;
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

void AirThreatManager::sendSimulationStatusInfoMsg(int status)
{
	tcout << _T("AirThreatManager sendSimulationStatusInfoMsg OK") << endl;
	NEnum enumType;
	enumType.setValue(&NInteger(status));
	simulationStatusInfoMsg->setValue(_T("status"), &enumType);
	updateMsg(simulationStatusInfoMsg);
}

void AirThreatManager::sendAirThreatInfoMsg(AirThreatInfo &airThreatInfo)
{
	tcout << _T("AirThreatManager sendAirThreatInfoMsg OK") << endl;
	airThreatInfoMsg->setValue(_T("currentTime"), &NInteger(airThreatInfo.currentTime));
	airThreatInfoMsg->setValue(_T("currentLatitude"), &NDouble(airThreatInfo.currentLatitude));
	airThreatInfoMsg->setValue(_T("currentLongitude"), &NDouble(airThreatInfo.currentLongitude));
	airThreatInfoMsg->setValue(_T("currentSpeed"), &NFloat(airThreatInfo.currentSpeed));
	airThreatInfoMsg->setValue(_T("currentAngle"), &NFloat(airThreatInfo.currentAngle));
	updateMsg(airThreatInfoMsg);
}

bool
AirThreatManager::stop()
{
	this->deleteMsg(ICD_TestNOM);
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

