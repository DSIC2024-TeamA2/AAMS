#include "antiairmissile_controller.h"
#include <math.h>
#include <iostream>

AntiAirMissileController::AntiAirMissileController()
{

	long currentTime = 0;
	double currentSpeed = 100;
	double currentAngle = 100;

}
AntiAirMissileController::~AntiAirMissileController()
{
	std::cout << "wonyeong" << std::endl;
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

}