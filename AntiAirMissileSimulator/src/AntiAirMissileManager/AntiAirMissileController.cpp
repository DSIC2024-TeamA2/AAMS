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
	antiAirMissileInfo.chasing = 0;
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
	antiAirMissileInfo.chasing = 0;
	getStartAntiAirMissile();
	nframework::NTimer& nTimer = nframework::NTimer::getInstance();
	std:function <void(void*)> periodicFunc;
	periodicFunc = std::bind(&AntiAirMissileController::threatSimulationThread, this);
	timerHandle = nTimer.addPeriodicTask(AMSConfiguration::getInstance().getUpdateDuration(), periodicFunc);
}

void AntiAirMissileController::stop()
{
	antiAirMissileInfo.chasing = 0;
	nframework::NTimer& nTimer = nframework::NTimer::getInstance();
	nTimer.removeTask(timerHandle);
}


/* -------------------------------------------------------------------------------------
*  #2. detection
-------------------------------------------------------------------------------------*/
//void AntiAirMissileController::detectAntiAirMissile()
//{
//	double radX = pow(scenarioInfo.antiAirMissileLatitude - airThreatInfo.currentLatitude, 2);
//	double radY = pow(scenarioInfo.antiAirMissileLongitude - airThreatInfo.currentLongitude, 2);
//	
//	int radius = 300;
//	if (pow(radius,2) >= (radX + radY)|| antiAirMissileInfo.chasing == true)
//	{
//		setSimulationStatus(CHASING);
//		sendSimulationStatusInfo(CHASING);
//		updateAntiAirMissileInfo();
//		antiAirMissileInfo.chasing = true;
//	}
//	/*else
//	{
//		setSimulationStatus(DETECTING);
//		sendSimulationStatusInfo(DETECTING);
//	}*/
//}

void AntiAirMissileController::detectAntiAirMissile()
{
	int radius = AMSConfiguration::getInstance().getDetectionRadius();
	double radiusDouble = pow(radius, 2);
	double deltaX = scenarioInfo.antiAirMissileLatitude - airThreatInfo.currentLatitude;
	double deltaY = scenarioInfo.antiAirMissileLongitude - airThreatInfo.currentLongitude;
	double radX = pow(deltaX, 2);
	double radY = pow(deltaY, 2);
	if (radiusDouble >= (radX + radY) && antiAirMissileInfo.chasing == 0)
	{
		setSimulationStatus(CHASING);
		sendSimulationStatusInfo(CHASING);
		antiAirMissileInfo.chasing = 1;
		updateAntiAirMissileInfo();
	}
	else if (radiusDouble >= (radX + radY) && antiAirMissileInfo.chasing == 1)
	{
		updateAntiAirMissileInfo();
	}
}

/* -------------------------------------------------------------------------------------
*  #3. Simulation & thread
-------------------------------------------------------------------------------------*/
bool isTermination(ScenarioInfo& scenarioInfo, AntiAirMissileInfo& antiAirMissileInfo, AirThreatInfo& airThreatInfo)
{
	double xrange = pow((std::abs(antiAirMissileInfo.currentLatitude - airThreatInfo.currentLatitude)), 2);
	double yrange = pow((std::abs(antiAirMissileInfo.currentLongitude - airThreatInfo.currentLongitude)), 2);
	double range = std::sqrt(xrange + yrange);
	if (range < antiAirMissileInfo.currentSpeed / 3600)
	{
		return true;
	}
	else
		return false;
	
}

void AntiAirMissileController::threatSimulationThread()
{
	tcout << _T("모의 시스템 상태") << status << std::endl;
	if (isTermination(scenarioInfo, antiAirMissileInfo, airThreatInfo))
	{
		setSimulationStatus(SUCCESS);
		sendSimulationStatusInfo(SUCCESS);

		antiAirMissileInfo.chasing = 0;

		tcout << _T("모의 시스템 상태") << status << std::endl;
		stop();
	}
	if (status == FAIL)
	{
		antiAirMissileInfo.chasing = 0;
		stop();
	}
	if (status == IDLE) { //if (status == 4 || status == 1)
		antiAirMissileInfo.chasing = 0;
		scenarioInfo.antiAirMissileLatitude = 0;
		scenarioInfo.antiAirMissileLongitude = 0;
		antiAirMissileInfo.currentLatitude = 0;
		antiAirMissileInfo.currentLongitude = 0;
		stop();
	}
	antiAirMissileInfo.currentTime = antiAirMissileInfo.currentTime + 1;
	sendAntiAirMissileInfo(antiAirMissileInfo);

	detectAntiAirMissile();
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
		antiAirMissileInfo.currentAngle = 3.14 / 2;
	}
	else if (deltaY == 0.0)
	{
		antiAirMissileInfo.currentAngle = 0;
	}
	else
		antiAirMissileInfo.currentAngle = std::abs((std::atan(deltaY / deltaX)));
}

// 4-2.방향 계산
void AntiAirMissileController::updateAntiAirMissileInfo()
{
	getCurrentAntiAirMissile();
	if (antiAirMissileInfo.currentLatitude < airThreatInfo.currentLatitude)
		antiAirMissileInfo.currentLatitude = antiAirMissileInfo.currentLatitude + antiAirMissileInfo.currentSpeed / 3600 * std::cos(antiAirMissileInfo.currentAngle);
	else
		antiAirMissileInfo.currentLatitude = antiAirMissileInfo.currentLatitude - antiAirMissileInfo.currentSpeed / 3600 * std::cos(antiAirMissileInfo.currentAngle);

	if (antiAirMissileInfo.currentLongitude < airThreatInfo.currentLongitude)
		antiAirMissileInfo.currentLongitude = antiAirMissileInfo.currentLongitude + antiAirMissileInfo.currentSpeed / 3600 * std::sin(antiAirMissileInfo.currentAngle);
	else
		antiAirMissileInfo.currentLongitude = antiAirMissileInfo.currentLongitude - antiAirMissileInfo.currentSpeed / 3600 * std::sin(antiAirMissileInfo.currentAngle);

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


