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

void AirthreatController::setScenarioInfo(ScenarioInfo scenarioInfo)
{
	this->scenarioInfo = scenarioInfo;
	scenarioInfo.airThreatSpeed = scenarioInfo.airThreatSpeed / 3600;
	// std::cout << std::endl;
	// std::cout << "Threat position : (" << scenarioInfo.airThreatEndLatitude << ", " << scenarioInfo.airThreatEndLongitude << ")" << std::endl;
	//tcout << _T("Threat position : (") << scenarioInfo.airThreatEndLatitude << _T(", ") << scenarioInfo.airThreatEndLongitude << _T(")") << endl;
}

// 설정한 공중위협 정보 초기
void AirthreatController::GetCurrentAirThreat()
{
	airThreatInfo.currentTime = scenarioInfo.startTime;
	airThreatInfo.currentLatitude = scenarioInfo.airThreatStartLatitude;
	airThreatInfo.currentLongitude = scenarioInfo.airThreatStartLongitude;
	airThreatInfo.currentSpeed = scenarioInfo.airThreatSpeed;// scenarioInfo.airThreatSpeed / 3600;
}

void AirthreatController::setSimulationStatus(SimulationStatus status)
{
	this->status = status;
}

void AirthreatController::start()
{
	GetCurrenAngle();
	GetCurrentAirThreat();
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

bool isTermination(ScenarioInfo& scenarioInfo, AirThreatInfo& airThreatInfo)
{
	if ((scenarioInfo.airThreatStartLatitude < scenarioInfo.airThreatEndLatitude && airThreatInfo.currentLatitude >= scenarioInfo.airThreatEndLatitude) || (scenarioInfo.airThreatStartLatitude > scenarioInfo.airThreatEndLatitude && airThreatInfo.currentLatitude <= scenarioInfo.airThreatEndLatitude)) // 목적지 도착. fail
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
		if (isTermination(scenarioInfo, airThreatInfo))
		{
			tcout << _T("isTermination(scenarioInfo, airThreatInfo) == true") << std::endl;
			setSimulationStatus(FAIL);
			sendSimulationStatusInfo(FAIL);
			isThreadRunning = false;
			tcout << _T("모의 시스템 상태") << status << std::endl;
			break;
		}
		if (status == 4) {
			isThreadRunning = false;
			break;
		}
		airThreatInfo.currentTime = airThreatInfo.currentTime + 1;
		updateAirThreatInfo();
		sendAirThreatInfo(airThreatInfo);

		tcout << _T("sleep_for wait") << std::endl;
		std::this_thread::sleep_for(std::chrono::milliseconds(1000));
		tcout << _T("sleep_for end") << std::endl;
	}
}

// 방향 계산
void AirthreatController::GetCurrenAngle()
{
	double deltaX = scenarioInfo.airThreatEndLatitude - scenarioInfo.airThreatStartLatitude;
	double deltaY = scenarioInfo.airThreatEndLongitude - scenarioInfo.airThreatStartLongitude;

	if (deltaX == 0.0 || deltaY == 0.0)
	{
		std::cerr << "Error: deltaX and deltaY cannot be both zero." << std::endl;
		return;
	}

	airThreatInfo.currentAngle = std::abs(std::atan(deltaY / deltaX));
}

// 좌표 계산
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

	tcout << _T("Threat position : (") << airThreatInfo.currentLatitude << _T(", ") << airThreatInfo.currentLongitude << _T(")") << std::endl;
}

void AirthreatController::setSendAirThreatInfoMethod(std::function<void(AirThreatInfo&)> sendAirThreatInfo)
{
	this->sendAirThreatInfo = sendAirThreatInfo;
}

void AirthreatController::setSendSimulationStatusInfoMethod(std::function<void(int)> sendSimulationStatusInfo)
{
	this->sendSimulationStatusInfo = sendSimulationStatusInfo;
}
