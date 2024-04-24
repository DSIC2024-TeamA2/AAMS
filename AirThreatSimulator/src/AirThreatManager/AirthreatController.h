#pragma once
#include <thread>
#include <chrono>
#include <functional>
#include "Data/ams_configuration.h"

struct AirThreatInfo
{
	int currentTime;
	double currentLatitude;
	double currentLongitude;
	double currentSpeed;
	double currentAngle;
};

struct ScenarioInfo
{
	int startTime;
	double airThreatStartLatitude;
	double airThreatStartLongitude;
	double airThreatEndLatitude;
	double airThreatEndLongitude;
	double airThreatSpeed;
	double antiAirMissileLatitude;
	double antiAirMissileLongitude;
	double antiAirMissileSpeed;
};

enum SimulationStatus
{
	IDLE = 1, DETECTING = 2, CHASING = 3, SUCCESS = 4, FAIL = 5
};

class AirthreatController
{
public:
	AirthreatController();
	~AirthreatController();

	void setScenarioInfo(ScenarioInfo scenarioInfo);
	void setSimulationStatus(SimulationStatus status);

	void start();
	void stop();
	void setSendAirThreatInfoMethod(std::function<void(AirThreatInfo&)> sendAirThreatInfo);
	void setSendSimulationStatusInfoMethod(std::function<void(int)> sendSimulationStatusInfo);
	void GetCurrenAngle();
	void GetCurrentAirThreat();

private:
	std::thread* simThread = nullptr;

	AirThreatInfo airThreatInfo;
	ScenarioInfo scenarioInfo;
	SimulationStatus status;

	std::function<void(AirThreatInfo&)> sendAirThreatInfo;
	std::function<void(int)> sendSimulationStatusInfo;

	bool isThreadRunning = false;

private:
	void threatSimulationThread();
	void updateAirThreatInfo();
};

