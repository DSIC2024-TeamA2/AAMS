#include "AirthreatController.h"
#include <math.h>
#include <iostream>
#include <nFramework/nom/NOMMain.h>

AirthreatController::AirthreatController()
{
}

AirthreatController::~AirthreatController()
{
}

/* -------------------------------------------------------------------------------------
*  #0. 초기 세팅
* - 시나리오
* - 공중위협
* - 시스템 상태
-------------------------------------------------------------------------------------*/

// #0-1. 시나리오 정보 세팅 
void AirthreatController::setScenarioInfo(ScenarioInfo scenarioInfo)
{
	this->scenarioInfo = scenarioInfo;
	scenarioInfo.airThreatSpeed = scenarioInfo.airThreatSpeed / 3600;
}

// #0-2. 설정한 공중 위협 정보 초기 세팅  
void AirthreatController::GetCurrentAirThreat()
{
	airThreatInfo.currentTime = scenarioInfo.startTime;
	airThreatInfo.currentLatitude = scenarioInfo.airThreatStartLatitude;
	airThreatInfo.currentLongitude = scenarioInfo.airThreatStartLongitude;
	airThreatInfo.currentSpeed = scenarioInfo.airThreatSpeed;// scenarioInfo.airThreatSpeed / 3600;
}

// #0-3. 설정한 공중 위협 정보 초기 세팅  
void AirthreatController::setSimulationStatus(SimulationStatus status)
{
	this->status = status;
}



/* -------------------------------------------------------------------------------------
*  #1. 시작 중단
-------------------------------------------------------------------------------------*/
void AirthreatController::start()
{
	GetCurrentAirThreat();
	GetCurrenAngle();
	
	isThreadRunning = true;
	simThread = new std::thread(std::bind(&AirthreatController::threatSimulationThread, this));
}

void AirthreatController::stop()
{
	isThreadRunning = false;

	if (simThread)
	{
		simThread->join();
		delete simThread;
		simThread = nullptr;
	}
}


/* -------------------------------------------------------------------------------------
*  #2. Simulation & thread
-------------------------------------------------------------------------------------*/
bool isTermination(ScenarioInfo& scenarioInfo, AirThreatInfo& airThreatInfo)
{
	bool boolX = (scenarioInfo.airThreatStartLatitude < scenarioInfo.airThreatEndLatitude && airThreatInfo.currentLatitude > scenarioInfo.airThreatEndLatitude) || (scenarioInfo.airThreatStartLatitude > scenarioInfo.airThreatEndLatitude && airThreatInfo.currentLatitude < scenarioInfo.airThreatEndLatitude);
	bool boolY = (scenarioInfo.airThreatStartLongitude < scenarioInfo.airThreatEndLongitude && airThreatInfo.currentLongitude > scenarioInfo.airThreatEndLongitude) || (scenarioInfo.airThreatStartLatitude > scenarioInfo.airThreatEndLatitude && airThreatInfo.currentLatitude < scenarioInfo.airThreatEndLatitude);

	if (boolX||boolY) // 목적지 도착. fail
	{
		return true;
	}
	else
		return false;
}


void AirthreatController::threatSimulationThread()
{
	while (isThreadRunning)
	{
		tcout << _T("threatSimulationThread() called") << std::endl;
		tcout << _T("모의 시스템 상태") << status << std::endl;

		// # 1. 중단 조건 확인
		if (isTermination(scenarioInfo, airThreatInfo))
		{
			tcout << _T("isTermination(scenarioInfo, airThreatInfo) == true") << std::endl;
			setSimulationStatus(FAIL);
			sendSimulationStatusInfo(FAIL);
			isThreadRunning = false;
			tcout << _T("모의 시스템 상태") << status << std::endl;
			break;
		}
		if (status == SUCCESS) { //if (status == 4 || status == 1)
			isThreadRunning = false;
			break;
		}
		if (status == IDLE) { //if (status == 4 || status == 1)
			isThreadRunning = false;
			break;
		}

		// # 2. 수행
		airThreatInfo.currentTime = airThreatInfo.currentTime + 1;
		updateAirThreatInfo();
		sendAirThreatInfo(airThreatInfo);

		tcout << _T("sleep_for wait") << std::endl;
		std::this_thread::sleep_for(std::chrono::milliseconds(AMSConfiguration::getInstance().getUpdateDuration()));
		tcout << _T("sleep_for end") << std::endl;
	}
}


/* -------------------------------------------------------------------------------------
*  #3. 계산
-------------------------------------------------------------------------------------*/
// 3-1.방향 계산
void AirthreatController::GetCurrenAngle()
{
	double deltaX = scenarioInfo.airThreatEndLatitude - scenarioInfo.airThreatStartLatitude;
	double deltaY = scenarioInfo.airThreatEndLongitude - scenarioInfo.airThreatStartLongitude;

	if (deltaX == 0.0)
	{
		airThreatInfo.currentAngle = 3.14 / 2;
	}
	else if(deltaY == 0.0)
	{
		airThreatInfo.currentAngle = 0;
	}
	else
	{
		airThreatInfo.currentAngle = std::abs(std::atan(deltaY / deltaX));
	}
	
}

// 3-2.좌표 계산
void AirthreatController::updateAirThreatInfo()
{
	if (scenarioInfo.airThreatStartLatitude < scenarioInfo.airThreatEndLatitude)
		airThreatInfo.currentLatitude = airThreatInfo.currentLatitude + scenarioInfo.airThreatSpeed * std::cos(airThreatInfo.currentAngle);
	else
		airThreatInfo.currentLatitude = airThreatInfo.currentLatitude - scenarioInfo.airThreatSpeed * std::cos(airThreatInfo.currentAngle);

	if (scenarioInfo.airThreatStartLongitude < scenarioInfo.airThreatEndLongitude)
		airThreatInfo.currentLongitude = airThreatInfo.currentLongitude + scenarioInfo.airThreatSpeed * std::sin(airThreatInfo.currentAngle);
	else
		airThreatInfo.currentLongitude = airThreatInfo.currentLongitude - scenarioInfo.airThreatSpeed * std::sin(airThreatInfo.currentAngle);

	//tcout << _T("Threat position : (") << airThreatInfo.currentLatitude << _T(", ") << airThreatInfo.currentLongitude << _T(")") << std::endl;
}


/* -------------------------------------------------------------------------------------
*  #4. send
-------------------------------------------------------------------------------------*/
void AirthreatController::setSendAirThreatInfoMethod(std::function<void(AirThreatInfo&)> sendAirThreatInfo)
{
	this->sendAirThreatInfo = sendAirThreatInfo;
}

void AirthreatController::setSendSimulationStatusInfoMethod(std::function<void(int)> sendSimulationStatusInfo)
{
	this->sendSimulationStatusInfo = sendSimulationStatusInfo;
}
