#pragma once
#include <nFramework/mec/MECComponent.h>
#include <nFramework/comm/NCommInterface.h>
#include <nFramework/nom/NOMMain.h>
#include "CommMessageHandler.h"

const int commCount = 2;

class BASEMGRDLL_API UDPCommunicationManager : public BaseManager
{
public:
	UDPCommunicationManager(void);
	~UDPCommunicationManager(void);

public:

	virtual shared_ptr<NOM> registerMsg(tstring) override;
	virtual void discoverMsg(shared_ptr<NOM>) override;
	virtual void updateMsg(shared_ptr<NOM>) override;
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

public:
	void processRecvMessage(unsigned char* data, int size);

private:
	void init();
	void release();

private:
	IMEBComponent* meb;
	MECComponent* mec;
	tstring name;

	map<unsigned int, shared_ptr<NOM>> registeredMsg;
	map<unsigned int, shared_ptr<NOM>> discoveredMsg;

	shared_ptr<NOM> scenarioInfoMsg;
	shared_ptr<NOM> simulationStatusInfoMsg;

	CommunicationInterface* commInterface[commCount];
	CommunicationConfig* commConfig[commCount];
	CommMessageHandler commMsgHandler;
};

