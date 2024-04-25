#pragma once
#include <chrono>
#include <functional>
#include "../Data/DataStruct.h"
#include "../AirThreatDetect/AirThreatDetector.h";

#ifndef M_PI
#define M_PI 3.14159265358979323846
#endif

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

	std::unique_ptr<IAirThreatDetector> airThreatDetector;
private:
	void threatSimulationThread();
	void updateAntiAirMissileInfo();
};

