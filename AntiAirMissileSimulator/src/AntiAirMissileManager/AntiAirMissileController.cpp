#include "AntiAirMissileController.h"
#include <math.h>
#include <iostream>
#include <nFramework/nom/NOMMain.h>
#include <nFramework/nTimer/NTimer.h>


AntiAirMissileController::AntiAirMissileController()
{
}

AntiAirMissileController::~AntiAirMissileController()
{
}

/* -------------------------------------------------------------------------------------
*  #0. 초기 세팅
-------------------------------------------------------------------------------------*/
void AntiAirMissileController::setScenarioInfo(ScenarioInfo scenarioInfo)
{
	this->scenarioInfo = scenarioInfo;
	scenarioInfo.airThreatSpeed = scenarioInfo.airThreatSpeed / 3600;
}

void AntiAirMissileController::setAirThreatInfo(AirThreatInfo airThreatInfo)
{
	this->airThreatInfo = airThreatInfo;
}

void AntiAirMissileController::setSimulationStatus(SimulationStatus status)
{
	this->status = status;
}
void AntiAirMissileController::getStartAntiAirMissile()
{
	antiAirMissileInfo.currentLatitude = scenarioInfo.antiAirMissileLatitude;
	antiAirMissileInfo.currentLongitude = scenarioInfo.antiAirMissileLongitude;
	antiAirMissileInfo.currentSpeed = scenarioInfo.antiAirMissileSpeed;
	antiAirMissileInfo.currentAngle = 0;
	antiAirMissileInfo.currentTime = scenarioInfo.startTime;
}

/* -------------------------------------------------------------------------------------
*  #1. 시작 중단
-------------------------------------------------------------------------------------*/
void AntiAirMissileController::start() 
{
	antiAirMissileInfo.chasing = false;
	getStartAntiAirMissile();
	nframework::NTimer& nTimer = nframework::NTimer::getInstance();
	std:function <void(void*)> periodicFunc;
	periodicFunc = std::bind(&AntiAirMissileController::threatSimulationThread, this);
	timerHandle = nTimer.addPeriodicTask(AMSConfiguration::getInstance().getUpdateDuration(), periodicFunc);
}

void AntiAirMissileController::stop()
{
	nframework::NTimer& nTimer = nframework::NTimer::getInstance();
	nTimer.removeTask(timerHandle);
}


/* -------------------------------------------------------------------------------------
*  #2. detection
-------------------------------------------------------------------------------------*/
void AntiAirMissileController::detectAntiAirMissile()
{
	double radX = pow(scenarioInfo.antiAirMissileLatitude - airThreatInfo.currentLatitude, 2);
	double radY = pow(scenarioInfo.antiAirMissileLongitude - airThreatInfo.currentLongitude, 2);
	
	int radius = AMSConfiguration::getInstance().getDetectionRadius();
	if (pow(radius,2) >= (radX + radY)&& antiAirMissileInfo.chasing == true)
	{
		setSimulationStatus(CHASING);
		sendSimulationStatusInfo(CHASING);
		updateAntiAirMissileInfo();
		antiAirMissileInfo.chasing = true;
	}
	else if (antiAirMissileInfo.chasing = true)
	{
		updateAntiAirMissileInfo();
	}
	/*else
	{
		setSimulationStatus(DETECTING);
		sendSimulationStatusInfo(DETECTING);
	}*/
}



/* -------------------------------------------------------------------------------------
*  #3. Simulation & thread
-------------------------------------------------------------------------------------*/
bool isTermination(ScenarioInfo& scenarioInfo, AntiAirMissileInfo& antiAirMissileInfo, AirThreatInfo& airThreatInfo )
{
	double xrange = pow((std::abs(antiAirMissileInfo.currentLatitude - airThreatInfo.currentLatitude)), 2);
	double yrange = pow((std::abs(antiAirMissileInfo.currentLongitude - airThreatInfo.currentLongitude)), 2);
	double range = std::sqrt(xrange + yrange);
	if (range < 5)
		return true;
	else
		return false;
	
}

void AntiAirMissileController::threatSimulationThread()
{
	tcout << _T("모의 시스템 상태") << status << std::endl;
	if (isTermination(scenarioInfo, antiAirMissileInfo, airThreatInfo))
	{
		antiAirMissileInfo.chasing = false;
		setSimulationStatus(SUCCESS);
		sendSimulationStatusInfo(SUCCESS);
		tcout << _T("모의 시스템 상태") << status << std::endl;
		stop();
	}
	if (status == FAIL)
	{
		antiAirMissileInfo.chasing = false;
		stop();
	}
	if (status == IDLE) { //if (status == 4 || status == 1)
		antiAirMissileInfo.chasing = false;
		stop();
	}
	detectAntiAirMissile();
	antiAirMissileInfo.currentTime = antiAirMissileInfo.currentTime + 1;
	sendAntiAirMissileInfo(antiAirMissileInfo);
}

/* -------------------------------------------------------------------------------------
*  #4. 계산 - 추적 알고리즘
-------------------------------------------------------------------------------------*/
// 4-1.방향 계산
void AntiAirMissileController::getCurrentAntiAirMissile()
{
	double deltaX = antiAirMissileInfo.currentLatitude - airThreatInfo.currentLatitude;
	double deltaY = antiAirMissileInfo.currentLongitude - airThreatInfo.currentLongitude;
	if (deltaX == 0.0)
	{
		airThreatInfo.currentAngle = 3.14 / 2;
	}
	else if (deltaY == 0.0)
	{
		airThreatInfo.currentAngle = 0;
	}
	else
		antiAirMissileInfo.currentAngle = std::abs((std::atan(deltaY / deltaX)));
}

// 4-2.방향 계산
void AntiAirMissileController::updateAntiAirMissileInfo()
{
	getCurrentAntiAirMissile();
	if (antiAirMissileInfo.currentLatitude < airThreatInfo.currentLatitude)
		antiAirMissileInfo.currentLatitude = antiAirMissileInfo.currentLatitude + antiAirMissileInfo.currentSpeed * std::cos(antiAirMissileInfo.currentAngle);
	else
		antiAirMissileInfo.currentLatitude = antiAirMissileInfo.currentLatitude - antiAirMissileInfo.currentSpeed * std::cos(antiAirMissileInfo.currentAngle);

	if (antiAirMissileInfo.currentLongitude < airThreatInfo.currentLongitude)
		antiAirMissileInfo.currentLongitude = antiAirMissileInfo.currentLongitude + antiAirMissileInfo.currentSpeed * std::sin(antiAirMissileInfo.currentAngle);
	else
		antiAirMissileInfo.currentLongitude = antiAirMissileInfo.currentLongitude - antiAirMissileInfo.currentSpeed * std::sin(antiAirMissileInfo.currentAngle);

	//tcout << _T("Threat position : (") << antiAirMissileInfo.currentLatitude << _T(", ") << antiAirMissileInfo.currentLongitude << _T(")") << std::endl;
}



/* -------------------------------------------------------------------------------------
*  #5. send
-------------------------------------------------------------------------------------*/
void AntiAirMissileController::sendAntiAirMissileInfoMethod(std::function<void(AntiAirMissileInfo&)> sendAntiAirMissileInfo)
{
	this->sendAntiAirMissileInfo = sendAntiAirMissileInfo;
	
}

void AntiAirMissileController::setSendSimulationStatusInfoMethod(std::function<void(int)> sendSimulationStatusInfo)
{
	this->sendSimulationStatusInfo = sendSimulationStatusInfo;
}


