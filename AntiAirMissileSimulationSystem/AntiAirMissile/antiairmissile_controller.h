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



class AntiAirMissileInfo
{
public:
	AntiAirMissileInfo();
	~AntiAirMissileInfo();
private:
	long currentTime;
	Position currentPosition;
	double currentSpeed;
	double currentAngle;
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

class AirThreatInfo
{
public:
	AirThreatInfo();
	~AirThreatInfo();
private:
	long currentTime;
	Position currentPostion;
	double currentSpeed;
	double currentAngle;

};