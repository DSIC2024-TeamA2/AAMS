#include "antiairmissile_controller.h"
#include <math.h>
#include <iostream>
class AntiAirMissileConfiguration 
{
public:
	AntiAirMissileConfiguration();
	~AntiAirMissileConfiguration();
	void GetInstance();
	void Init();
private:
	std::string confFilePath;
	long updateDuration;
	long detectionRadius;


};


AntiAirMissileController::AntiAirMissileController()
{
	long currentTime = 0;
	double currentSpeed = 100;
	double currentAngle = 100;

}
AntiAirMissileController::~AntiAirMissileController()
{
	
}

void GetCurrentAntiAirMissile(double x, double y)
{

}

void GetCurrentPosition(double x, double y)
{
	Position cur_position(x, y);
}

void GetCurrentAngle(double a)
{
	AntiAirMissileConfiguration(a);

}