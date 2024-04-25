/*****************************************************************//**
 * \file		AirThreatManager.h
 * \brief		공중위협 모의기 정보 통신을 정의한 파일
 * \details		공중위협 모의기가 송수신할 정보를 지정하거나 정보를 네트워크로 송수신한다.
 * \author		남아리
 * \date		2024.04.25
 *********************************************************************/
#pragma once
#include <nFramework/BaseManager.h>
#include <nFramework/mec/MECComponent.h>
#include <nFramework/nom/NOMMain.h>
#include "../AirThreatManger/AirthreatController.h"

using namespace nframework;
using namespace nom;

/**
 * @class		AirThreatManager
 * @brief		공중위협 모의기 정보 통신 클래스
 * @details		공중위협 모의가 송수신할 정보를 지정 및 송수신을 담당한다.
 */
class BASEMGRDLL_API AirThreatManager : public BaseManager
{
public:
	AirThreatManager(void);
	~AirThreatManager(void);

public:
	// inherited from the BaseManager class
	virtual shared_ptr<NOM> registerMsg(tstring) override;
	virtual void discoverMsg(shared_ptr<NOM>) override;
	virtual void updateMsg(shared_ptr<NOM>) override;
	/**
	* @brief		NOM 처리하는 가상 함수
	* @details		메시지를 수신하고 처리한다.
	* @param[in]	NOM
	* @return		void
	*/
	virtual void reflectMsg(shared_ptr<NOM>) override;
	virtual void deleteMsg(shared_ptr<NOM>) override;
	virtual void removeMsg(shared_ptr<NOM>) override;
	virtual void sendMsg(shared_ptr<NOM>) override;
	virtual void recvMsg(shared_ptr<NOM>) override;
	virtual void setUserName(tstring) override;
	virtual tstring getUserName() override;
	virtual void setData(void*) override;
	virtual bool start() override;
	virtual bool stop() override;
	virtual void setMEBComponent(IMEBComponent*) override;
private:
	void init();
	void release();
	void sendAirThreatInfoMsg(AirThreatInfo& airThreatInfo);
	void sendSimulationStatusInfoMsg(int status);

private:
	IMEBComponent* meb;
	MECComponent* mec;
	tstring name;
	map<unsigned int, shared_ptr<NOM>> registeredMsg;
	map<unsigned int, shared_ptr<NOM>> discoveredMsg;
	map<tstring, function<void(AirthreatController&, shared_ptr<NOM>)>> msgFuncMap;

	shared_ptr<NOM> simulationStatusInfoMsg;
	shared_ptr<NOM> scenarioInfoMsg;
	shared_ptr<NOM> airThreatInfoMsg;

	AirthreatController airThreatController;
};