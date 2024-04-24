#include "UDPCommunicationManager.h"

/************************************************************************
	Constructor / Destructor
************************************************************************/
UDPCommunicationManager::UDPCommunicationManager(void)
{
	init();
}

UDPCommunicationManager::~UDPCommunicationManager(void)
{
	release();
}

/************************************************************************
	initialize / release
************************************************************************/
void
UDPCommunicationManager::init()
{
	setUserName(_T("UDPCommunicationManager"));

	// by contract
	mec = new MECComponent;
	mec->setUser(this);

	commConfig = new CommunicationConfig;
	commConfig->setIni(_T("CommLinkInfo.ini"));
	commInterface = new NCommInterface(this);

	tcout << _T("UDP Communication") << endl;
}

void
UDPCommunicationManager::release()
{
	delete commConfig;
	delete commInterface;

	meb = nullptr;
	delete mec;
	mec = nullptr;
}

/************************************************************************
	Inherit Function
************************************************************************/
shared_ptr<NOM>
UDPCommunicationManager::registerMsg(tstring msgName)
{
	shared_ptr<NOM> nomMsg = mec->registerMsg(msgName);
	registeredMsg.insert(pair<unsigned int, shared_ptr<NOM>>(nomMsg->getInstanceID(), nomMsg));

	return nomMsg;
}

void
UDPCommunicationManager::discoverMsg(shared_ptr<NOM> nomMsg)
{
	discoveredMsg.insert(pair<unsigned int, shared_ptr<NOM>>(nomMsg->getInstanceID(), nomMsg));
}

void
UDPCommunicationManager::updateMsg(shared_ptr<NOM> nomMsg)
{
	mec->updateMsg(nomMsg);
}

void
UDPCommunicationManager::reflectMsg(shared_ptr<NOM> nomMsg)
{
	if (nomMsg->getName() == _T("AirThreatInfo"))
	{
		nomMsg->setValue(_T("MessageHeader.ID"), &NUShort(9992));
		nomMsg->setValue(_T("MessageHeader.SIZE"), &NUShort(32));
	}
	else if (nomMsg->getName() == _T("SimulationStatusInfo"))
	{
		nomMsg->setValue(_T("MessageHeader.ID"), &NUShort(9994));
		nomMsg->setValue(_T("MessageHeader.SIZE"), &NUShort(8));
	}
	commInterface->sendCommMsg(nomMsg);
}

void
UDPCommunicationManager::deleteMsg(shared_ptr<NOM> nomMsg)
{
	mec->deleteMsg(nomMsg);
	registeredMsg.erase(nomMsg->getInstanceID());
}

void
UDPCommunicationManager::removeMsg(shared_ptr<NOM> nomMsg)
{
	discoveredMsg.erase(nomMsg->getInstanceID());
}

void
UDPCommunicationManager::sendMsg(shared_ptr<NOM> nomMsg)
{
	mec->sendMsg(nomMsg);
}

void
UDPCommunicationManager::recvMsg(shared_ptr<NOM> nomMsg)
{
	// if need be, write your code
	commInterface->sendCommMsg(nomMsg);
}

void
UDPCommunicationManager::setUserName(tstring userName)
{
	name = userName; 
}

tstring
UDPCommunicationManager::getUserName()
{
	return name;
}

void
UDPCommunicationManager::setData(void* data)
{
	// if need be, write your code
}

bool
UDPCommunicationManager::start()
{
	commInterface->setMEBComponent(meb);
	MessageProcessor msgProcessor = bind(&UDPCommunicationManager::processRecvMessage, this, placeholders::_1, placeholders::_2);
	commConfig->setMsgProcessor(msgProcessor);
	commInterface->initNetEnv(commConfig);

	simulationStatusInfoMsg = this->registerMsg(_T("SimulationStatusInfo"));
	commMsgHandler.setNOM(simulationStatusInfoMsg);
	scenarioInfoMsg = this->registerMsg(_T("ScenarioInfo"));
	commMsgHandler.setNOM(scenarioInfoMsg);
	
	return true;
}

bool
UDPCommunicationManager::stop()
{
	commInterface->releaseNetEnv(commConfig);
	this->deleteMsg(simulationStatusInfoMsg);
	this->deleteMsg(scenarioInfoMsg);

	return true;
}

void
UDPCommunicationManager::setMEBComponent(IMEBComponent* realMEB)
{
	meb = realMEB;
	mec->setMEB(meb);
}

void
UDPCommunicationManager::processRecvMessage(unsigned char* data, int size)
{
	auto IDSize = commConfig->getHeaderIDSize();
	auto IDPos = commConfig->getHeaderIDPos();

	unsigned short tmpMsgID;
	memcpy(&tmpMsgID, data + IDPos, IDSize);
	auto msgID = ntohs(tmpMsgID);

	tcerr << _T("msgID: ") << msgID << endl;
	tcerr << _T("size: ") << size << endl;
	auto nomMsg = meb->getNOMInstance(name, commMsgHandler.getMsgName(msgID));

	if (nomMsg.get())
	{
		if (nomMsg->getType() == NOM_TYPE_OBJECT)
		{
			auto objNOM = commMsgHandler.getNOM(nomMsg->getName());
			objNOM->deserialize(data, size);;
			this->updateMsg(objNOM);
		}
		else
		{
			auto nomMsgCP = nomMsg->clone();
			nomMsgCP->deserialize(data, size);
			this->sendMsg(nomMsgCP);
		}
	}
	else
	{
		tcerr << _T("not defined message") << endl;
	}
}

/************************************************************************
	Export Function
************************************************************************/
extern "C" BASEMGRDLL_API
BaseManager* createObject()
{
	return new UDPCommunicationManager;
}

extern "C" BASEMGRDLL_API
void deleteObject(BaseManager* userManager)
{
	delete userManager;
}

