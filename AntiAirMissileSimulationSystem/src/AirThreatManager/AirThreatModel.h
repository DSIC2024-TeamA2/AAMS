#pragma once
#include <chrono>
#include <functional>

class position
{
public:
    position(double x, double y) : x(x), y(y) {}

    double x;
    double y;
};

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

private:

    // startAirThreat Info
    double initSpeed;
    position initPos = position(0, 0);
    position targetPos = position(0, 0);
    // currentAirThreat Info
    position currentPos = position(0, 0);

    bool isThreadRunning = false;
    // IDLE=0, DETECTEING=1, CHASING=2, SUCCESS=3, FAIL=4
    // �� ���� ���� : �������� �߻�, �������ź �߻�, �ó����� ����, �������� Ž�� �� ����, ��� ����, ��� ����
    int scenarioState = 1;

private:
    void threatSimulationThread();
    void updateThreatPosition();
};
