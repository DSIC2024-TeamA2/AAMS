#pragma once
#include "../Data/DataStruct.h"

class IAirThreatDetector
{
public:
	virtual void setRadarPosition(double latitude, double longitude) = 0;
	virtual void setTargetPosition(double latitude, double longitude) = 0;
	virtual bool detected() = 0;
	virtual void clear() = 0;
};