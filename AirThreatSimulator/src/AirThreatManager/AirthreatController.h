/*****************************************************************//**
 * \file		AirthreatController.h
 * \brief		공중위협 모의기가 공중위협 동작을 제어할 수 있도록 만드는 파일
 * \details		공중위협 모의가는 시나리오 정보를 수신하고 공중위협 동작을 계산해 제어합니다.
 * \author		남아리
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
* @brief		공중위협 모의기가 가진 공중위협 현재 정보들의 구조체
* @details		공중위협 기동 후 경과시간, 공중위협 현재 위도, 공중위협 현재 경도, 공중위협 현재 속력, 공중위협 현재 방위각 정보가 있습니다.
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
* @brief		모의시스템 시나리오 정보들의 구조체
* @details		모의 시작 후 경과시간, 공중위협 시작지 위도, 공중위협 시작지 경도, 공중위협 목적지 위도, 공중위협 목적지 경도, 공중위협 설정 속도, 대공유도탄 시작지 위도, 대공유도탄 시작지 경도, 대공유도탄 설정 속도 정보가 있습니다.
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
* @brief		모의시스템 진행 중 발생하는 상태들의 열거형
* @details		프로그램 초기 상태, 공중위협 탐지 상태, 공중위협 추적 상태, 공중위협 요격 성공 상태, 공중위협 요격 실패 상태 정보가 있습니다.
*/
enum SimulationStatus
{
	IDLE = 1, DETECTING = 2, CHASING = 3, SUCCESS = 4, FAIL = 5
};

/**
 * @class		AirthreatController
 * @brief		공중위협 제어 클래스
 * @details		공중위협 모의기는 수신한 시나리오 정보으로 계산해 공중위협 제어한다.
 */
class AirthreatController
{
public:
	/**
	 * @brief		공중위협 제어 클래스의 객체를 생성하는 함수	
	 */
	AirthreatController();

	/**
	 * @brief		공중위협 제어 클래스의 객체를 소멸시키는 함수
	 */
	~AirthreatController();

	/**
	* @brief		시나리오 정보를 수신해 저장하는 함수
	* @details		운용통제기로부터 시나리오 정보를 수신해 저장한다.
	* @param[in]	scenarioInfo
	* @return		void
	*/
	void setScenarioInfo(ScenarioInfo scenarioInfo);

	/**
	* @brief		모의 진행 중 발생하는 상태값을 수신헤 저장하는 함수
	* @details		운용통제기로부터 수신한 시나리오 정보 중 모의 진행 중 발생하는 상태값을 수신해 저장한다.
	* @param[in]	status
	* @return		void
	*/
	void setSimulationStatus(SimulationStatus status);

	/**
	* @brief		모의 시작시 설정 시나리오 적용하고 일정 주기마다 운용통제기와 통신을 시작하는 함수
	* @details		운용통제기로부터 수신한 시나리오 정보로 변수들을 초기화하고 설정한 주기마다 운용통제기로부터 시나리오 정보를 수신한다.
	* @return		void
	*/
	void start();

	/**
	* @brief		모의 종료시 공중위협 정보 초기화하고 운용통제기와의 통신을 끊는 함수
	* @details		운용통제기로부터 모의 중지 신호를 수신해 공중위협 정보를 초기화하고 통신을 끊는다.
	* @return		void
	*/
	void stop();

	/**
	* @brief		공중위협 정보를 일정 주기마다 전송하는 함수
	* @details		공중위협 모의기의 현재 공중위협 정보를 운용통제기에 설정한 주기마다 전송한다.
	* @param[in]	sendAirThreatInfo
	* @return		void
	*/
	void setSendAirThreatInfoMethod(std::function<void(AirThreatInfo&)> sendAirThreatInfo);

	/**
	* @brief		공중위협 모의기의 시나리오 진행 상태 정보를 전송하는 함수
	* @details		모의 진행 중 공중위협 모의기에 발생한 시나리오 상태 정보를 설정한 주기마다 전송한다.
	* @param[in]	sendSimulationStatusInfo
	* @return		void
	*/
	void setSendSimulationStatusInfoMethod(std::function<void(int)> sendSimulationStatusInfo);

	/**
	* @brief		공중위협 정보를 수신한 시나리오 정보로 초기화하는 함수
	* @details		운용통제기로부터 수신한 시나리오 정보를 공중위협 정보에 초기 저장한다.
	* @return		void
	*/
	void InitCurrentAirThreat();

private:
	AirThreatInfo airThreatInfo;	/// 공중위협 정보
	ScenarioInfo scenarioInfo;		/// 설정한 시나리오 정보
	SimulationStatus status;		/// 모의 진행 상태 정보

	std::function<void(AirThreatInfo&)> sendAirThreatInfo;		/// 공중위협 정보를 전송 함수를 내포한 객체
	std::function<void(int)> sendSimulationStatusInfo;			/// 모의 상태 정보를 전송 함수를 내포한 객체

	int timerHandle;	/// Ntimer 핸들러

private:
	/**
	* @brief		공중위협을 제어하는 함수
	* @details		운용통제기로부터 수신한 정보를 기반으로 공중위협 동작을 제어한다.
	* @return		void
	*/
	void threatSimulationThread();

	/**
	* @brief		공중위협 정보를 최신으로 갱신하는 함수
	* @details		공중위협 모의기가 제어한 공중위협의 정보를 최신 정보로 갱신한다.
	* @return		void
	*/
	void updateAirThreatInfo();
};

