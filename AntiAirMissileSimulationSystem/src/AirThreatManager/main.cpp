#include "AirThreatModel.h"
#include <cmath>
#include <iostream>

// �ʱ�ȭ
AirthreatModel::AirthreatModel()
{
    initPos = position(0, 0);
    initSpeed = 0;
    targetPos = position(0, 0);
}

// ������ �������� ���� �������� (�����, �ӵ�, ������)
void AirthreatModel::GetCurrentAirThreat(double stx, double sty, double speed, double edx, double edy)
{
    initPos = position(stx, sty);
    initSpeed = speed / 3600; //[km/h]
    targetPos = position(edx, edy);
}

// �ó����� ���� ������Ʈ
// ��ݵǸ� �������� ���ߵ���
void AirthreatModel::GetScenarioInfo(int info)
{
    scenarioState = info;
}

// �ó����� ���� ��������
// ������ ����
int AirthreatModel::PubScenarioInfo()
{
    std::cout << "scenarioState : " << scenarioState << std::endl;
    return scenarioState;
}

void AirthreatModel::start()
{
    // �ʱ� ��ġ ����
    currentPos = initPos;

    // detecting�̰ų� chasing�̸� ���
    // �ó����� �̺�Ʈ�� �ǽð����� �޾ƿ��� �� �߰��ؾ���
    if (scenarioState == 1 || scenarioState == 2)
    {
        isThreadRunning = true;
    }
    while (isThreadRunning)
    {

        if ((initPos.x < targetPos.x && currentPos.x >= targetPos.x) || (initPos.x > targetPos.x && currentPos.x <= targetPos.x)) // ������ ����. fail
        {
            isThreadRunning = false;
            scenarioState = 4;
            PubScenarioInfo();
            break;
        }
        if (scenarioState == 3) // ��ݵǸ� 
        {
            isThreadRunning = false;
            break;
        }
        // �ƴϸ� ��� ��ǥ ������Ʈ
        GetCurrentPosition();
    }
}

// ���� ���
double AirthreatModel::GetCurrenAngle()
{
    double deltaX = targetPos.x - initPos.x;
    double deltaY = targetPos.y - initPos.y;

    if (deltaX == 0.0 || deltaY == 0.0)
        return 0;

    double theta = std::abs(std::atan(deltaY / deltaX));
    return theta;
}

// ��ǥ ���
void AirthreatModel::GetCurrentPosition()
{
    double theta = GetCurrenAngle();

    if (initPos.x < targetPos.x)
        currentPos.x = currentPos.x + initSpeed * std::cos(theta);
    else
        currentPos.x = currentPos.x - initSpeed * std::cos(theta);

    if (initPos.y < targetPos.y)
        currentPos.y = currentPos.y + initSpeed * std::sin(theta);
    else
        currentPos.y = currentPos.y - initSpeed * std::sin(theta);

    std::cout << "Threat position : (" << currentPos.x << ", " << currentPos.y << ")" << std::endl;
}

int main() {
    AirthreatModel atm;
    atm.GetCurrentAirThreat(30, 40, 100, 0, 10);
    atm.GetScenarioInfo(1);
    atm.start();
}
