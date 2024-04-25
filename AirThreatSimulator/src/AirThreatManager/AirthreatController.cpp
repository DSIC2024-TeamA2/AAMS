#include "AirthreatController.h"
#include <math.h>
#include <iostream>
#include <nFramework/nom/NOMMain.h>
#include <nFramework/nTimer/NTimer.h>

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

double calculateAngle(const int startX, const int startY, const int endX, const int endY) {
	double deltaX = endX - startX;
	double deltaY = endY - startY;

	double angleInRadians = std::atan2(deltaY, deltaX);

	double angleInDegrees = angleInRadians * (180.0 / M_PI);
	if (angleInDegrees < 0)
		angleInDegrees += 360;
	return angleInDegrees;
}

double degreesToRadians(double degrees) {
	return degrees * (M_PI / 180.0);
}

// #0-2. 설정한 공중 위협 정보 초기 세팅  
void AirthreatController::InitCurrentAirThreat()
{
	airThreatInfo.currentTime = scenarioInfo.startTime;
	airThreatInfo.currentLatitude = scenarioInfo.airThreatStartLatitude;
	airThreatInfo.currentLongitude = scenarioInfo.airThreatStartLongitude;
	airThreatInfo.currentSpeed = scenarioInfo.airThreatSpeed;// scenarioInfo.airThreatSpeed / 3600;
	airThreatInfo.currentAngle = calculateAngle(scenarioInfo.airThreatStartLatitude, scenarioInfo.airThreatStartLongitude,
		scenarioInfo.airThreatEndLatitude, scenarioInfo.airThreatEndLongitude);
}

// #0-3. 설정한 공중 위협 정보 초기 세팅  
void AirthreatController::setSimulationStatus(SimulationStatus status)
{
	if (status == SUCCESS)
		stop();
	this->status = status;
}

/* -------------------------------------------------------------------------------------
*  #1. 시작 중단
-------------------------------------------------------------------------------------*/
void AirthreatController::start()
{
	InitCurrentAirThreat();

	nframework::NTimer& nTimer = nframework::NTimer::getInstance();
	std:function <void(void*)> periodicFunc;
	periodicFunc = std::bind(&AirthreatController::threatSimulationThread, this);
	timerHandle = nTimer.addPeriodicTask(AMSConfiguration::getInstance().getUpdateDuration(), periodicFunc);

}

void AirthreatController::stop()
{
	nframework::NTimer& nTimer = nframework::NTimer::getInstance();
	nTimer.removeTask(timerHandle);
}

/* -------------------------------------------------------------------------------------
*  #2. Simulation & thread
-------------------------------------------------------------------------------------*/
bool isTermination(ScenarioInfo& scenarioInfo, AirThreatInfo& airThreatInfo)
{
	bool boolX = (scenarioInfo.airThreatStartLatitude < scenarioInfo.airThreatEndLatitude && airThreatInfo.currentLatitude > scenarioInfo.airThreatEndLatitude) || (scenarioInfo.airThreatStartLatitude > scenarioInfo.airThreatEndLatitude && airThreatInfo.currentLatitude < scenarioInfo.airThreatEndLatitude);
	bool boolY = (scenarioInfo.airThreatStartLongitude < scenarioInfo.airThreatEndLongitude && airThreatInfo.currentLongitude > scenarioInfo.airThreatEndLongitude) || (scenarioInfo.airThreatStartLatitude > scenarioInfo.airThreatEndLatitude && airThreatInfo.currentLatitude < scenarioInfo.airThreatEndLatitude);

	if (boolX || boolY) // 목적지 도착. fail
	{
		airThreatInfo.currentLatitude = scenarioInfo.airThreatEndLatitude;
		airThreatInfo.currentLongitude = scenarioInfo.airThreatEndLongitude;
		return true;
	}
	else
		return false;
}


void AirthreatController::threatSimulationThread()
{
	if (status == SUCCESS) { //if (status == 4 || status == 1)
		stop();
		return;
	}
	if (status == IDLE) { //if (status == 4 || status == 1)
		airThreatInfo.currentLatitude = 0;
		airThreatInfo.currentLongitude = 0;
		stop();
		return;
	}

	updateAirThreatInfo();

	airThreatInfo.currentTime = airThreatInfo.currentTime + 1;
	sendAirThreatInfo(airThreatInfo);

	if (isTermination(scenarioInfo, airThreatInfo))
	{
		setSimulationStatus(FAIL);
		sendSimulationStatusInfo(FAIL);
		stop();
		return;
	}
}

/* -------------------------------------------------------------------------------------
*  #3. 계산
-------------------------------------------------------------------------------------*/
// 3-1.방향 계산

// 3-2.좌표 계산
void AirthreatController::updateAirThreatInfo()
{
	double radianAngle = degreesToRadians(airThreatInfo.currentAngle);
	airThreatInfo.currentLatitude = airThreatInfo.currentLatitude + scenarioInfo.airThreatSpeed * std::cos(radianAngle);
	airThreatInfo.currentLongitude = airThreatInfo.currentLongitude + scenarioInfo.airThreatSpeed * std::sin(radianAngle);
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
