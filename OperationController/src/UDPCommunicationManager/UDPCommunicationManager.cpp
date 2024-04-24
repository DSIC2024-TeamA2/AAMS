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

	for (int i = 0; i < commCount; i++)
	{
		commConfig[i] = new CommunicationConfig;
		std::wstringstream wss;
		wss << L"CommLinkInfo" << (i + 1) << L".ini";
		commConfig[i]->setIni(wss.str());
		commInterface[i] = new NCommInterface(this);
	}

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
	// if need be, write your code
	if (nomMsg->getName() == _T("ScenarioInfo"))
	{
		nomMsg->setValue(_T("MessageHeader.ID"), &NUShort(9991));
		nomMsg->setValue(_T("MessageHeader.SIZE"), &NUShort(64));
	}
	else if (nomMsg->getName() == _T("AirThreatInfo"))
	{
		nomMsg->setValue(_T("MessageHeader.ID"), &NUShort(9992));
		nomMsg->setValue(_T("MessageHeader.SIZE"), &NUShort(32));
	}
	else if (nomMsg->getName() == _T("AntiAirMissileInfo"))
	{
		nomMsg->setValue(_T("MessageHeader.ID"), &NUShort(9993));
		nomMsg->setValue(_T("MessageHeader.SIZE"), &NUShort(32));
	}
	else if (nomMsg->getName() == _T("SimulationStatusInfo"))
	{
		nomMsg->setValue(_T("MessageHeader.ID"), &NUShort(9994));
		nomMsg->setValue(_T("MessageHeader.SIZE"), &NUShort(8));
	}

	for (int i = 0; i < commCount; i++)
	{
		commInterface[i]->sendCommMsg(nomMsg);
	}
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
	for (int i = 0; i < commCount; i++)
	{
		commInterface[i]->sendCommMsg(nomMsg);
	}
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
	for (int i = 0; i < commCount; i++)
	{
		commInterface[i]->setMEBComponent(meb);
		MessageProcessor msgProcessor = bind(&UDPCommunicationManager::processRecvMessage, this, placeholders::_1, placeholders::_2);
		commConfig[i]->setMsgProcessor(msgProcessor);
		commInterface[i]->initNetEnv(commConfig[i]);
	}

	antiAirMissileInfoMsg = this->registerMsg(_T("AntiAirMissileInfo"));
	airThreatInfoMsg = this->registerMsg(_T("AirThreatInfo"));
	simulationStatusInfoMsg = this->registerMsg(_T("SimulationStatusInfo"));
	commMsgHandler.setNOM(antiAirMissileInfoMsg);
	commMsgHandler.setNOM(airThreatInfoMsg);
	commMsgHandler.setNOM(simulationStatusInfoMsg);

	return true;
}

bool
UDPCommunicationManager::stop()
{
	for (int i = 0; i < commCount; i++)
	{
		commInterface[i]->releaseNetEnv(commConfig[i]);
	}
	this->deleteMsg(antiAirMissileInfoMsg);
	this->deleteMsg(airThreatInfoMsg);
	this->deleteMsg(simulationStatusInfoMsg);

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
	auto IDSize = commConfig[0]->getHeaderIDSize();
	auto IDPos = commConfig[0]->getHeaderIDPos();

	unsigned short tmpMsgID;
	memcpy(&tmpMsgID, data + IDPos, IDSize);
	auto msgID = ntohs(tmpMsgID);

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

