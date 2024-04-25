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
	if (status == IDLE) {
		memset(&scenarioInfo, 0, sizeof(ScenarioInfo));
		memset(&antiAirMissileInfo, 0, sizeof(AntiAirMissileInfo));
		memset(&airThreatInfo, 0, sizeof(AirThreatInfo));
	}
}
void AntiAirMissileController::initStartAntiAirMissile()
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
	initStartAntiAirMissile();
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
	if (status == DETECTING)
	{
		int radius = AMSConfiguration::getInstance().getDetectionRadius();
		double radiusDouble = pow(radius, 2);
		double deltaX = scenarioInfo.antiAirMissileLatitude - airThreatInfo.currentLatitude;
		double deltaY = scenarioInfo.antiAirMissileLongitude - airThreatInfo.currentLongitude;
		double radX = pow(deltaX, 2);
		double radY = pow(deltaY, 2);
		if (radiusDouble >= (radX + radY))
		{
			setSimulationStatus(CHASING);
			sendSimulationStatusInfo(CHASING);
		}
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
	if (range < antiAirMissileInfo.currentSpeed)
	{
		return true;
	}
	else
		return false;
}

void AntiAirMissileController::threatSimulationThread()
{
	tcout << _T("모의 시스템 상태") << status << std::endl;
	if (status == FAIL)
	{
		stop();
		return;
	}
	if (status == IDLE) { //if (status == 4 || status == 1)
		stop();
		return;
	}
	if (isTermination(scenarioInfo, antiAirMissileInfo, airThreatInfo))
	{
		antiAirMissileInfo.currentTime = antiAirMissileInfo.currentTime + 1;
		antiAirMissileInfo.currentLatitude = airThreatInfo.currentLatitude;
		antiAirMissileInfo.currentLongitude = airThreatInfo.currentLongitude;

		sendAntiAirMissileInfo(antiAirMissileInfo);
		setSimulationStatus(SUCCESS);
		sendSimulationStatusInfo(SUCCESS);

		tcout << _T("모의 시스템 상태") << status << std::endl;
		stop();
		return;
	}

	detectAntiAirMissile();

	if (status == CHASING)
	{
		tcout << _T("pre Threat position : (") << antiAirMissileInfo.currentLatitude << _T(", ") << antiAirMissileInfo.currentLongitude << _T(")") << std::endl;
		updateAntiAirMissileInfo();
		tcout << _T("post Threat position : (") << antiAirMissileInfo.currentLatitude << _T(", ") << antiAirMissileInfo.currentLongitude << _T(")") << std::endl;
		antiAirMissileInfo.currentTime = antiAirMissileInfo.currentTime + 1;
		sendAntiAirMissileInfo(antiAirMissileInfo);
	}
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

/* -------------------------------------------------------------------------------------
*  #4. 계산 - 추적 알고리즘
-------------------------------------------------------------------------------------*/
// 4-1.방향 계산
void AntiAirMissileController::updateAntiAirMissileInfo()
{
	antiAirMissileInfo.currentAngle = calculateAngle(antiAirMissileInfo.currentLatitude, antiAirMissileInfo.currentLongitude,
		airThreatInfo.currentLatitude, airThreatInfo.currentLongitude);
	double radianAngle = degreesToRadians(antiAirMissileInfo.currentAngle);
	antiAirMissileInfo.currentLatitude = antiAirMissileInfo.currentLatitude + antiAirMissileInfo.currentSpeed * std::cos(radianAngle);
	antiAirMissileInfo.currentLongitude = antiAirMissileInfo.currentLongitude + antiAirMissileInfo.currentSpeed * std::sin(radianAngle);
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


