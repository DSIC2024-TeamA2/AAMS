#pragma once

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
