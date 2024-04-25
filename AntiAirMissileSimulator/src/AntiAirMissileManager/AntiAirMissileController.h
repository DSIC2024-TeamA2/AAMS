#pragma once
#include <chrono>
#include <functional>
#include <cmath>
#include "Data/ams_configuration.h"

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

struct AntiAirMissileInfo
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

class AntiAirMissileController
{
public:
	AntiAirMissileController();
	~AntiAirMissileController();

	void setScenarioInfo(ScenarioInfo scenarioInfo);
	void setAirThreatInfo(AirThreatInfo airThreatInfo);
	void setSimulationStatus(SimulationStatus status);

	void start();
	void stop();
	void sendAntiAirMissileInfoMethod(std::function<void(AntiAirMissileInfo&)> sendAntiAirMissileInfo);
	void setSendSimulationStatusInfoMethod(std::function<void(int)> sendSimulationStatusInfo);
	void initStartAntiAirMissile();
	void detectAntiAirMissile();

private:
	AirThreatInfo airThreatInfo;
	AntiAirMissileInfo antiAirMissileInfo;
	ScenarioInfo scenarioInfo;
	SimulationStatus status;

	std::function<void(AntiAirMissileInfo&)> sendAntiAirMissileInfo;
	std::function<void(int)> sendSimulationStatusInfo;

	int timerHandle;

private:
	void threatSimulationThread();
	void updateAntiAirMissileInfo();
};

