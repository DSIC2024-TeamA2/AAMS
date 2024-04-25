#include "AirThreatDetector.h"

bool AirThreatDetector::detected()
{
	int radius = AMSConfiguration::getInstance().getDetectionRadius();
	double radiusDouble = pow(radius, 2);
	double deltaX = radarLatitude - targetLatitude;
	double deltaY = radarLongitude - targetLongitude;
	double radX = pow(deltaX, 2);
	double radY = pow(deltaY, 2);
	if (radiusDouble >= (radX + radY))
	{
		return true;
	}

	return false;
}

void AirThreatDetector::setRadarPosition(double latitude, double longitude)
{
	this->radarLatitude = latitude;
	this->radarLongitude = longitude;
}

void AirThreatDetector::setTargetPosition(double latitude, double longitude)
{
	this->targetLatitude = latitude;
	this->targetLongitude = longitude;
}

void AirThreatDetector::clear() {
	this->radarLatitude = 0;
	this->radarLongitude = 0;
	this->targetLatitude = 0;
	this->targetLongitude = 0;
}