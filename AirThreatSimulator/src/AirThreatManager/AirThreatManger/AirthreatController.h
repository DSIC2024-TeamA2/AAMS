#pragma once
#include <thread>
#include <chrono>
#include <functional>
#include "../Data/ams_configuration.h"

#ifndef M_PI
#define M_PI 3.14159265358979323846
#endif

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
	void InitCurrentAirThreat();

private:
	AirThreatInfo airThreatInfo;
	ScenarioInfo scenarioInfo;
	SimulationStatus status;

	std::function<void(AirThreatInfo&)> sendAirThreatInfo;
	std::function<void(int)> sendSimulationStatusInfo;

	int timerHandle;

private:
	void threatSimulationThread();
	void updateAirThreatInfo();
};

