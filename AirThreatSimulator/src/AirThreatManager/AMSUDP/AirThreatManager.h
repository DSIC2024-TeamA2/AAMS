/*****************************************************************//**
 * \file		AirThreatManager.h
 * \brief		�������� ���Ǳ� ���� ����� ������ ����
 * \details		�������� ���ǱⰡ �ۼ����� ������ �����ϰų� ������ ��Ʈ��ũ�� �ۼ����Ѵ�.
 * \author		���Ƹ�
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
 * @brief		�������� ���Ǳ� ���� ��� Ŭ����
 * @details		�������� ���ǰ� �ۼ����� ������ ���� �� �ۼ����� ����Ѵ�.
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
	* @brief		NOM ó���ϴ� ���� �Լ�
	* @details		�޽����� �����ϰ� ó���Ѵ�.
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