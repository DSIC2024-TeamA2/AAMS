#pragma once
#include <cmath>
#include "IAirThreatDetector.h"
#include "../Data/ams_configuration.h"

class AirThreatDetector : public IAirThreatDetector
{
public:
	virtual void setRadarPosition(double latitude, double longitude);
	virtual void setTargetPosition(double latitude, double longitude);
	virtual bool detected();
	virtual void clear();
private:
	double radarLatitude;
	double radarLongitude;
	double targetLatitude;
	double targetLongitude;
};