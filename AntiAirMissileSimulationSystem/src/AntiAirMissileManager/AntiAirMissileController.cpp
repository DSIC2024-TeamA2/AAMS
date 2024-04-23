#include "AntiAirMissileController.h"
#include <math.h>
#include <iostream>

AntiAirMissileController::AntiAirMissileController()
{
}

AntiAirMissileController::~AntiAirMissileController()
{
}

void AntiAirMissileController::setScenarioInfo(ScenarioInfo scenarioInfo)
{
	this->scenarioInfo = scenarioInfo;
}

void AntiAirMissileController::setAirThreatInfo(AirThreatInfo airThreatInfo)
{
	this->airThreatInfo = airThreatInfo;
}

void AntiAirMissileController::setSimulationStatus(SimulationStatus status)
{
	this->status = status;
}

void AntiAirMissileController::start()
{
	isThreadRunning = true;
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

bool isTermination(ScenarioInfo& scenarioInfo, AntiAirMissileInfo& antiAirMissileInfo, AirThreatInfo& airThreatInfo)
{
	// 구현 필요 //
	return true;
}

void AntiAirMissileController::threatSimulationThread()
{
	while (isThreadRunning)
	{
		updateAntiAirMissileInfo();
		sendAntiAirMissileInfo(antiAirMissileInfo);

		if (isTermination(scenarioInfo, antiAirMissileInfo, airThreatInfo))
		{
			sendSimulationStatusInfo(5);
			isThreadRunning = false;
			break;
		}

		std::this_thread::sleep_for(std::chrono::milliseconds(1000));
	}
}

void AntiAirMissileController::updateAntiAirMissileInfo()
{

}

void AntiAirMissileController::sendAntiAirMissileInfoMethod(std::function<void(AntiAirMissileInfo&)> sendAntiAirMissileInfo)
{
	this->sendAntiAirMissileInfo = sendAntiAirMissileInfo;
}

void AntiAirMissileController::setSendSimulationStatusInfoMethod(std::function<void(int)> sendSimulationStatusInfo)
{
	this->sendSimulationStatusInfo = sendSimulationStatusInfo;
}
