#include "AirThreatDetector.h"

bool AirThreatDetector::detected()
{
	int radius = AMSConfiguration::getInstance().getDetectionRadius();
	double radiusDouble = pow(radius, 2);
	double deltaX = radarPosition.latitude - targetPosition.latitude;
	double deltaY = radarPosition.longitude - targetPosition.longitude;
	double radX = pow(deltaX, 2);
	double radY = pow(deltaY, 2);
	if (radiusDouble >= (radX + radY))
	{
		return true;
	}

	return false;
}

void AirThreatDetector::setRadarInfo(Position radarInfo)
{
	this->radarPosition = radarInfo;
}

void AirThreatDetector::setTargetInfo(Position targetInfo)
{
	this->targetPosition = targetInfo;
}

void AirThreatDetector::clear() {
	this->radarPosition.latitude = 0;
	this->radarPosition.longitude = 0;
	this->targetPosition.latitude = 0;
	this->targetPosition.longitude = 0;
}