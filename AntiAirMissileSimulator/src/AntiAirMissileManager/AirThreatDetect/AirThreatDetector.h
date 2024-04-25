#pragma once
#include <cmath>
#include "IAirThreatDetector.h"
#include "../Data/ams_configuration.h"

class AirThreatDetector : public IAirThreatDetector<Position, Position>
{
public:
	AirThreatDetector() {}
	virtual void setRadarInfo(Position radarInfo);
	virtual void setTargetInfo(Position targetInfo);
	virtual bool detected();
	virtual void clear();
private:
	Position radarPosition;
	Position targetPosition;
};