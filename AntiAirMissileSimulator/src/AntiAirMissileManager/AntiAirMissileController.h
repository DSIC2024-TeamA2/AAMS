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

struct AntiAirMissileInfo
{
	int currentTime;
	double currentLatitude;
	double currentLongitude;
	double currentSpeed;
	double currentAngle;
	bool chasing;
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
	void getStartAntiAirMissile();
	void detectAntiAirMissile();
	void getCurrentAntiAirMissile();

private:
	std::thread* simThread = nullptr;

	AirThreatInfo airThreatInfo;
	AntiAirMissileInfo antiAirMissileInfo;
	ScenarioInfo scenarioInfo;
	SimulationStatus status;

	std::function<void(AntiAirMissileInfo&)> sendAntiAirMissileInfo;
	std::function<void(int)> sendSimulationStatusInfo;

	bool isThreadRunning = false;

private:
	void threatSimulationThread();
	void updateAntiAirMissileInfo();
};

