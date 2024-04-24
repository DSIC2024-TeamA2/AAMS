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

void
AirThreatManager::updateMsg(shared_ptr<NOM> nomMsg)
{
	tcout << _T("AirThreatManager updateMsg called") << endl;
	mec->updateMsg(nomMsg);
}

void
AirThreatManager::reflectMsg(shared_ptr<NOM> nomMsg)
{
	// if need be, write your code
	if (nomMsg->getName() == _T("ScenarioInfo"))
	{
		//map<tstring, function<void(shared_ptr<NOM>)>
		tcout << _T("AirThreatManager ScenarioInfo OK") << endl;
		int startTime = nomMsg->getValue(_T("startTime"))->toInt();
		double airThreatStartLatitude = nomMsg->getValue(_T("airThreatStartLatitude"))->toDouble();
		double airThreatStartLongitude = nomMsg->getValue(_T("airThreatStartLongitude"))->toDouble();
		double airThreatEndLatitude = nomMsg->getValue(_T("airThreatEndLatitude"))->toDouble();
		double airThreatEndLongitude = nomMsg->getValue(_T("airThreatEndLongitude"))->toDouble();
		double airThreatSpeed = nomMsg->getValue(_T("airThreatSpeed"))->toFloat();
		double antiAirMissileLatitude = nomMsg->getValue(_T("antiAirMissileLatitude"))->toDouble();
		double antiAirMissileLongitude = nomMsg->getValue(_T("antiAirMissileLongitude"))->toDouble();
		double antiAirMissileSpeed = nomMsg->getValue(_T("antiAirMissileSpeed"))->toFloat();
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
		airThreatController.setScenarioInfo(info);
		airThreatController.start();
	}
	else if (nomMsg->getName() == _T("SimulationStatusInfo"))
	{
		tcout << _T("AirThreatManager SimulationStatusInfo OK") << endl;
		tcout << nomMsg->getValue(_T("status"))->toInt() << endl;
		SimulationStatus status = IDLE;
		int statusint = nomMsg->getValue(_T("status"))->toInt();
		if (statusint == 2)
			status = DETECTING;
		if (statusint == 3)
			status = CHASING;
		if (statusint == 4)
			status = SUCCESS;
		if (statusint == 5)
			status = FAIL;
		airThreatController.setSimulationStatus(status);
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

