#pragma once
#include<iostream>
#include<cstdio>
#include<string>

class AntiAirMissileController
{
public:
	AntiAirMissileController();
	~AntiAirMissileController();
	void GetCurrentAntiAirMissile();
	void GetCurrentPosition();
	void GetCurrentAngle();

private:
	void startAntiAirMissile(double x, double y);
	void currentAntiAirMissile(double x, double y);
	void scenario();

};

class AntiAirMissileConfiguration
{
public:
	AntiAirMissileConfiguration();
	~AntiAirMissileConfiguration();
private:
	std::string confFilePath = "wonyeong";
	long updateDuration;
	long detectionRadius;
};


class Position {
public:
	Position(double lat, double lon) : latitude(lat), longitude(lon) {}

	// Getter 메서드들
	double getLatitude() const { return latitude; }
	double getLongitude() const { return longitude; }
private:
	double latitude;
	double longitude;
};