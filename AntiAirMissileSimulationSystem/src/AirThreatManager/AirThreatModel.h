#pragma once
#include <chrono>
#include <functional>

// class �����
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
	//�� ���� ���� : �������� �߻�, �������ź �߻�, �ó����� ����, �������� Ž�� �� ����, ��� ����, ��� ����

private:
	void threatSimulationThread();
	void updateThreatPosition();
};

