#include "AntiAirMissileController.h"
#include <math.h>
#include <iostream>
#include <nFramework/nom/NOMMain.h>


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
	isThreadRunning = true;
	getStartAntiAirMissile();
	simThread = new std::thread(std::bind(&AntiAirMissileController::threatSimulationThread, this));
	
}

void AntiAirMissileController::stop()
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
*  #2. detection
-------------------------------------------------------------------------------------*/
void AntiAirMissileController::detectAntiAirMissile()
{
	double radX = pow(scenarioInfo.antiAirMissileLatitude - airThreatInfo.currentLatitude, 2);
	double radY = pow(scenarioInfo.antiAirMissileLongitude - airThreatInfo.currentLongitude, 2);
	
	int radius = 200;
	if (pow(radius,2) >= (radX + radY))
	{
		setSimulationStatus(CHASING);
		sendSimulationStatusInfo(CHASING);
		updateAntiAirMissileInfo();
	}
	else
	{
		setSimulationStatus(DETECTING);
		sendSimulationStatusInfo(DETECTING);
	}

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
	while (isThreadRunning)
	{
		tcout << _T("모의 시스템 상태") << status << std::endl;
		if (isTermination(scenarioInfo, antiAirMissileInfo, airThreatInfo))
		{
			setSimulationStatus(SUCCESS);
			sendSimulationStatusInfo(SUCCESS);
			isThreadRunning = false;
			tcout << _T("모의 시스템 상태") << status << std::endl;
			break;
		}
		if (status == 5) //(status == 5 || status == 1)
		{
			isThreadRunning = false;
			break;
		}
		detectAntiAirMissile();
		antiAirMissileInfo.currentTime = antiAirMissileInfo.currentTime + 1;
		sendAntiAirMissileInfo(antiAirMissileInfo);

		std::this_thread::sleep_for(std::chrono::milliseconds(1000));
	}
}



/* -------------------------------------------------------------------------------------
*  #4. 계산 - 추적 알고리즘
-------------------------------------------------------------------------------------*/
// 4-1.방향 계산
void AntiAirMissileController::getCurrentAntiAirMissile()
{
	double deltaX = antiAirMissileInfo.currentLatitude - airThreatInfo.currentLatitude;
	double deltaY = antiAirMissileInfo.currentLongitude - airThreatInfo.currentLongitude;
	if (deltaX == 0.0 || deltaY == 0.0)
	{
		std::cerr << "Error: deltaX and deltaY cannot be both zero." << std::endl;
		return;
	}

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

	tcout << _T("Threat position : (") << antiAirMissileInfo.currentLatitude << _T(", ") << antiAirMissileInfo.currentLongitude << _T(")") << std::endl;
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


