#include "AirthreatController.h"
#include <math.h>
#include <iostream>

AirthreatController::AirthreatController()
{
}

AirthreatController::~AirthreatController()
{
}

void AirthreatController::setScenarioInfo(ScenarioInfo scenarioInfo)
{
	this->scenarioInfo = scenarioInfo;
}

void AirthreatController::setSimulationStatus(SimulationStatus status)
{
	this->status = status;
}

void AirthreatController::start()
{
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

bool isTermination(ScenarioInfo &scenarioInfo, AirThreatInfo &airThreatInfo)
{
	// 구현 필요 //
	return true;
}

void AirthreatController::threatSimulationThread()
{
	while (isThreadRunning)
	{
		updateAirThreatInfo();
		sendAirThreatInfo(airThreatInfo);

		if (isTermination(scenarioInfo, airThreatInfo))
		{
			sendSimulationStatusInfo(FAIL);
			isThreadRunning = false;
			break;
		}

		std::this_thread::sleep_for(std::chrono::milliseconds(1000));
	}
}

void AirthreatController::updateAirThreatInfo()
{

}

void AirthreatController::setSendAirThreatInfoMethod(std::function<void(AirThreatInfo&)> sendAirThreatInfo)
{
	this->sendAirThreatInfo = sendAirThreatInfo;
}

void AirthreatController::setSendSimulationStatusInfoMethod(std::function<void(int)> sendSimulationStatusInfo)
{
	this->sendSimulationStatusInfo = sendSimulationStatusInfo;
}
