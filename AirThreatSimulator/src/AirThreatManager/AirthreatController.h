/*****************************************************************//**
 * \file		AirthreatController.h
 * \brief		�������� ���ǱⰡ �������� ������ ������ �� �ֵ��� ����� ����
 * \details		�������� ���ǰ��� �ó����� ������ �����ϰ� �������� ������ ����� �����մϴ�.
 * \author		���Ƹ�
 * \date		2024.04.25
 *********************************************************************/
#pragma once
#include <thread>
#include <chrono>
#include <functional>
#include "Data/ams_configuration.h"

#ifndef M_PI
#define M_PI 3.14159265358979323846
#endif

/**
* @struct		AirThreatInfo
* @brief		�������� ���ǱⰡ ���� �������� ���� �������� ����ü
* @details		�������� �⵿ �� ����ð�, �������� ���� ����, �������� ���� �浵, �������� ���� �ӷ�, �������� ���� ������ ������ �ֽ��ϴ�.
*/
struct AirThreatInfo
{
	int currentTime;
	double currentLatitude;
	double currentLongitude;
	double currentSpeed;
	double currentAngle;
};

/**
* @struct		ScenarioInfo
* @brief		���ǽý��� �ó����� �������� ����ü
* @details		���� ���� �� ����ð�, �������� ������ ����, �������� ������ �浵, �������� ������ ����, �������� ������ �浵, �������� ���� �ӵ�, �������ź ������ ����, �������ź ������ �浵, �������ź ���� �ӵ� ������ �ֽ��ϴ�.
*/
struct ScenarioInfo
{
	int startTime;
	double airThreatStartLatitude;
	double airThreatStartLongitude;
	double airThreatEndLatitude;
	double airThreatEndLongitude;
	double airThreatSpeed;
	double antiAirMissileLatitude;
	double antiAirMissileLongitude;
	double antiAirMissileSpeed;
};

/**
* @enum			SimulationStatus
* @brief		���ǽý��� ���� �� �߻��ϴ� ���µ��� ������
* @details		���α׷� �ʱ� ����, �������� Ž�� ����, �������� ���� ����, �������� ��� ���� ����, �������� ��� ���� ���� ������ �ֽ��ϴ�.
*/
enum SimulationStatus
{
	IDLE = 1, DETECTING = 2, CHASING = 3, SUCCESS = 4, FAIL = 5
};

/**
 * @class		AirthreatController
 * @brief		�������� ���� Ŭ����
 * @details		�������� ���Ǳ�� ������ �ó����� �������� ����� �������� �����Ѵ�.
 */
class AirthreatController
{
public:
	/**
	 * @brief		�������� ���� Ŭ������ ��ü�� �����ϴ� �Լ�	
	 */
	AirthreatController();

	/**
	 * @brief		�������� ���� Ŭ������ ��ü�� �Ҹ��Ű�� �Լ�
	 */
	~AirthreatController();

	/**
	* @brief		�ó����� ������ ������ �����ϴ� �Լ�
	* @details		���������κ��� �ó����� ������ ������ �����Ѵ�.
	* @param[in]	scenarioInfo
	* @return		void
	*/
	void setScenarioInfo(ScenarioInfo scenarioInfo);

	/**
	* @brief		���� ���� �� �߻��ϴ� ���°��� ������ �����ϴ� �Լ�
	* @details		���������κ��� ������ �ó����� ���� �� ���� ���� �� �߻��ϴ� ���°��� ������ �����Ѵ�.
	* @param[in]	status
	* @return		void
	*/
	void setSimulationStatus(SimulationStatus status);

	/**
	* @brief		���� ���۽� ���� �ó����� �����ϰ� ���� �ֱ⸶�� ���������� ����� �����ϴ� �Լ�
	* @details		���������κ��� ������ �ó����� ������ �������� �ʱ�ȭ�ϰ� ������ �ֱ⸶�� ���������κ��� �ó����� ������ �����Ѵ�.
	* @return		void
	*/
	void start();

	/**
	* @brief		���� ����� �������� ���� �ʱ�ȭ�ϰ� ������������ ����� ���� �Լ�
	* @details		���������κ��� ���� ���� ��ȣ�� ������ �������� ������ �ʱ�ȭ�ϰ� ����� ���´�.
	* @return		void
	*/
	void stop();

	/**
	* @brief		�������� ������ ���� �ֱ⸶�� �����ϴ� �Լ�
	* @details		�������� ���Ǳ��� ���� �������� ������ ��������⿡ ������ �ֱ⸶�� �����Ѵ�.
	* @param[in]	sendAirThreatInfo
	* @return		void
	*/
	void setSendAirThreatInfoMethod(std::function<void(AirThreatInfo&)> sendAirThreatInfo);

	/**
	* @brief		�������� ���Ǳ��� �ó����� ���� ���� ������ �����ϴ� �Լ�
	* @details		���� ���� �� �������� ���Ǳ⿡ �߻��� �ó����� ���� ������ ������ �ֱ⸶�� �����Ѵ�.
	* @param[in]	sendSimulationStatusInfo
	* @return		void
	*/
	void setSendSimulationStatusInfoMethod(std::function<void(int)> sendSimulationStatusInfo);

	/**
	* @brief		�������� ������ ������ �ó����� ������ �ʱ�ȭ�ϴ� �Լ�
	* @details		���������κ��� ������ �ó����� ������ �������� ������ �ʱ� �����Ѵ�.
	* @return		void
	*/
	void InitCurrentAirThreat();

private:
	AirThreatInfo airThreatInfo;	/// �������� ����
	ScenarioInfo scenarioInfo;		/// ������ �ó����� ����
	SimulationStatus status;		/// ���� ���� ���� ����

	std::function<void(AirThreatInfo&)> sendAirThreatInfo;		/// �������� ������ ���� �Լ��� ������ ��ü
	std::function<void(int)> sendSimulationStatusInfo;			/// ���� ���� ������ ���� �Լ��� ������ ��ü

	int timerHandle;	/// Ntimer �ڵ鷯

private:
	/**
	* @brief		���������� �����ϴ� �Լ�
	* @details		���������κ��� ������ ������ ������� �������� ������ �����Ѵ�.
	* @return		void
	*/
	void threatSimulationThread();

	/**
	* @brief		�������� ������ �ֽ����� �����ϴ� �Լ�
	* @details		�������� ���ǱⰡ ������ ���������� ������ �ֽ� ������ �����Ѵ�.
	* @return		void
	*/
	void updateAirThreatInfo();
};

