#include "AirthreatModel.h"
#include <math.h>
#include <iostream>

// �ʱ�ȭ
AirthreatModel::AirthreatModel()
{
	initX = 0;
	initY = 0;
	initSpeed = 0;
	targetX = 0;
	targetY = 0;
}


// class ������
// ������ �������� ���� �������� (�����, �ӵ�, ������)
void AirthreatModel::GetCurrentAirThreat(double stx, double sty, double speed, double edx, double edy)
{
	initX = stx;
	initY = sty;
	initSpeed = speed / 3600; //[km/h]
	targetX = edx;
	targetY = edy;
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
	currentX = initX;
	currentY = initY;

	// detecting�̰ų� chasing�̸� ���
	// �ó����� �̺�Ʈ�� �ǽð����� �޾ƿ��� �� �߰��ؾ���
	// GetCurrentPosition(info);
	if (scenarioState == 1 | scenarioState == 2)
	{
		isThreadRunning = true;
	}
	while (isThreadRunning) {

		if ((initX < targetX && currentX >= targetX) || (initX > targetX && currentX <= targetX)) // ������ ����. fail
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
	double deltaX, deltaY;
	double theta;

	deltaX = targetX - initX;
	deltaY = targetY - initY;

	if (deltaX == 0.0 || deltaY == 0.0)
		return 0;

	theta = abs(atan(deltaY / deltaX));
	return theta;
}

// ��ǥ ���
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

