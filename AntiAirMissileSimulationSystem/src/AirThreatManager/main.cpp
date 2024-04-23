#include "AirthreatModel.h"
#include <math.h>
#include <iostream>

// 초기화
AirthreatModel::AirthreatModel()
{
	initX = 0;
	initY = 0;
	initSpeed = 0;
	targetX = 0;
	targetY = 0;
}


// class 구현부
// 설정한 공중위협 정보 가져오기 (출발지, 속도, 목적지)
void AirthreatModel::GetCurrentAirThreat(double stx, double sty, double speed, double edx, double edy)
{
	initX = stx;
	initY = sty;
	initSpeed = speed / 3600; //[km/h]
	targetX = edx;
	targetY = edy;
}


// 시나리오 상태 업데이트
// 요격되면 공중위협 멈추도록
void AirthreatModel::GetScenarioInfo(int info)
{
	scenarioState = info;
}

// 시나리오 상태 내보내기
// 목적지 도착
int AirthreatModel::PubScenarioInfo()
{
	std::cout << "scenarioState : " << scenarioState << std::endl;
	return scenarioState;

}


void AirthreatModel::start()
{
	// 초기 위치 설정
	currentX = initX;
	currentY = initY;

	// detecting이거나 chasing이면 출발
	// 시나리오 이벤트를 실시간으로 받아오는 것 추가해야함
	// GetCurrentPosition(info);
	if (scenarioState == 1 | scenarioState == 2)
	{
		isThreadRunning = true;
	}
	while (isThreadRunning) {

		if ((initX < targetX && currentX >= targetX) || (initX > targetX && currentX <= targetX)) // 목적지 도착. fail
		{
			isThreadRunning = false;
			scenarioState = 4;
			PubScenarioInfo();
			break;
		}
		if (scenarioState == 3) // 요격되면 
		{
			isThreadRunning = false;
			break;
		}
		// 아니면 계속 좌표 업데이트
		GetCurrentPosition();
	}
}


// 방향 계산
double AirthreatModel::GetCurrenAngle()
{
	double deltaX, deltaY;
	double theta;

	deltaX = targetX - initX;
	deltaY = targetY - initY;

	if (deltaX == 0.0 || deltaY == 0.0)
		return 0;

	theta = abs(atan(deltaY / deltaX));
	return theta;
}

// 좌표 계산
void AirthreatModel::GetCurrentPosition()
{
	double theta = GetCurrenAngle();

	if (initX < targetX)
		currentX = currentX + initSpeed * cos(theta);
	else
		currentX = currentX - initSpeed * cos(theta);

	if (initY < targetY)
		currentY = currentY + initSpeed * sin(theta);
	else
		currentY = currentY - initSpeed * sin(theta);

	std::cout << "Threat position : (" << currentX << ", " << currentY << ")" << std::endl;
}


// ----------------------------------------------------------------------------
int main() {
	AirthreatModel atm;
	atm.GetCurrentAirThreat(30, 40, 100, 0, 10);
	atm.GetScenarioInfo(1);
	atm.start();
	///int pub_scenario = atm->scenarioState;
}

