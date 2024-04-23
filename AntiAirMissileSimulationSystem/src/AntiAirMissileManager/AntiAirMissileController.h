#pragma once
#include <thread>
#include <chrono>
#include <functional>

struct AirThreatInfo
{
	int currentTime;
	int currentLatitude;
	int currentLongitude;
	float currentSpeed;
	float currentAngle;
};

struct AntiAirMissileInfo
{
	int currentTime;
	int currentLatitude;
	int currentLongitude;
	float currentSpeed;
	float currentAngle;
};

struct ScenarioInfo
{
	int startTime;
	int airThreatStartLatitude;
	int airThreatStartLongitude;
	int airThreatEndLatitude;
	int airThreatEndLongitude;
	float airThreatSpeed;
	int antiAirMissileLatitude;
	int antiAirMissileLongitude;
	float antiAirMissileSpeed;
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

