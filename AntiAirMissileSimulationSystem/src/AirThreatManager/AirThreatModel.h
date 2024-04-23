#pragma once
#include <chrono>
#include <functional>

// class 선언부
class AirthreatModel
{
public:
	AirthreatModel();

	void GetCurrentAirThreat(double stx, double sty, double speed, double edx, double edy);
	void GetCurrentPosition();
	double GetCurrenAngle();
	void GetScenarioInfo(int info);
	int PubScenarioInfo();
	void start();
	void stop();


private:
	// startAirThreat Info
	double initX, initY, initSpeed;
	double targetX, targetY;
	// currentAirThreat Info
	double currentX, currentY;

	bool isThreadRunning = false;
	int scenarioState = 1;
	//IDLE=0, DETECTEING=1,CHASING=2, SUCCESS=3, FAIL=4
	//※ 동작 정보 : 공중위협 발사, 대공유도탄 발사, 시나리오 중지, 공중위협 탐지 및 추적, 요격 성공, 요격 실패

private:
	void threatSimulationThread();
	void updateThreatPosition();
};

