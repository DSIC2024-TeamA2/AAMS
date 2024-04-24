#include "UDPCommunicationManager2.h"

/************************************************************************
	Constructor / Destructor
************************************************************************/
UDPCommunicationManager2::UDPCommunicationManager2(void)
{
	init();
}

UDPCommunicationManager2::~UDPCommunicationManager2(void)
{
	release();
}

/************************************************************************
	initialize / release
************************************************************************/
void
UDPCommunicationManager2::init()
{
	setUserName(_T("UDPCommunicationManager2"));

	// by contract
	mec = new MECComponent;
	mec->setUser(this);

	commConfig = new CommunicationConfig;
	commConfig->setIni(_T("CommLinkInfo.ini"));
	commInterface = new NCommInterface(this);
	tcout << _T("UDP Communication") << endl;
}

void
UDPCommunicationManager2::release()
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
UDPCommunicationManager2::registerMsg(tstring msgName)
{
	shared_ptr<NOM> nomMsg = mec->registerMsg(msgName);
	registeredMsg.insert(pair<unsigned int, shared_ptr<NOM>>(nomMsg->getInstanceID(), nomMsg));

	return nomMsg;
}

void
UDPCommunicationManager2::discoverMsg(shared_ptr<NOM> nomMsg)
{
	discoveredMsg.insert(pair<unsigned int, shared_ptr<NOM>>(nomMsg->getInstanceID(), nomMsg));
}

void
UDPCommunicationManager2::updateMsg(shared_ptr<NOM> nomMsg)
{
	mec->updateMsg(nomMsg);
}

void
UDPCommunicationManager2::reflectMsg(shared_ptr<NOM> nomMsg)
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
UDPCommunicationManager2::deleteMsg(shared_ptr<NOM> nomMsg)
{
	mec->deleteMsg(nomMsg);
	registeredMsg.erase(nomMsg->getInstanceID());
}

void
UDPCommunicationManager2::removeMsg(shared_ptr<NOM> nomMsg)
{
	discoveredMsg.erase(nomMsg->getInstanceID());
}

void
UDPCommunicationManager2::sendMsg(shared_ptr<NOM> nomMsg)
{
	mec->sendMsg(nomMsg);
}

void
UDPCommunicationManager2::recvMsg(shared_ptr<NOM> nomMsg)
{
	// if need be, write your code
	commInterface->sendCommMsg(nomMsg);
}

void
UDPCommunicationManager2::setUserName(tstring userName)
{
	name = userName; 
}

tstring
UDPCommunicationManager2::getUserName()
{
	return name;
}

void
UDPCommunicationManager2::setData(void* data)
{
	// if need be, write your code
}

bool
UDPCommunicationManager2::start()
{
	commInterface->setMEBComponent(meb);
	MessageProcessor msgProcessor = bind(&UDPCommunicationManager2::processRecvMessage, this, placeholders::_1, placeholders::_2);
	commConfig->setMsgProcessor(msgProcessor);
	commInterface->initNetEnv(commConfig);

	simulationStatusInfoMsg = this->registerMsg(_T("SimulationStatusInfo"));
	commMsgHandler.setNOM(simulationStatusInfoMsg);
	scenarioInfoMsg = this->registerMsg(_T("ScenarioInfo"));
	commMsgHandler.setNOM(scenarioInfoMsg);

	return true;
}

bool
UDPCommunicationManager2::stop()
{
	commInterface->releaseNetEnv(commConfig);
	this->deleteMsg(simulationStatusInfoMsg);
	this->deleteMsg(scenarioInfoMsg);

	return true;
}

void
UDPCommunicationManager2::setMEBComponent(IMEBComponent* realMEB)
{
	meb = realMEB;
	mec->setMEB(meb);
}

void
UDPCommunicationManager2::processRecvMessage(unsigned char* data, int size)
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
	return new UDPCommunicationManager2;
}

extern "C" BASEMGRDLL_API
void deleteObject(BaseManager* userManager)
{
	delete userManager;
}

