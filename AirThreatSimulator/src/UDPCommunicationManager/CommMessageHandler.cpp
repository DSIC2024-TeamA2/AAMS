#include "CommMessageHandler.h"

/************************************************************************
	initialize / release
************************************************************************/
CommMessageHandler::CommMessageHandler()
{
	initialize();
}

CommMessageHandler::~CommMessageHandler()
{
	release();
}

void
CommMessageHandler::initialize()
{
	// if need be, write your code
	setIDNameTable(9991, _T("ScenarioInfo"));
	setIDNameTable(9992, _T("AirThreatInfo"));
	setIDNameTable(9994, _T("SimulationStatusInfo"));
}

void
CommMessageHandler::release()
{
	IDNameTable.clear();
}

/************************************************************************
	ID_Name table management
************************************************************************/
void
CommMessageHandler::setNOM(shared_ptr<NOM> nomMsg)
{
	ObjNOMTable.insert(pair<tstring, shared_ptr<NOM>>(nomMsg->getName(), nomMsg));
}

shared_ptr<NOM>
CommMessageHandler::getNOM(tstring msgName)
{
	auto itr = ObjNOMTable.find(msgName);
	shared_ptr<NOM> nomMsg;

	if (itr != ObjNOMTable.end())
	{
		nomMsg = itr->second;
	}

	return nomMsg;
}

void
CommMessageHandler::setIDNameTable(unsigned short msgID, tstring msgName)
{
	IDNameTable.insert(pair<unsigned short, tstring>(msgID, msgName));
}

tstring
CommMessageHandler::getMsgName(unsigned short msgID)
{
	tstring msgName;
	if (auto itr = IDNameTable.find(msgID); itr != IDNameTable.end())
	{
		msgName = itr->second;
	}
	else
	{
		msgName = _T("");
	}

	return msgName;
}

bool
CommMessageHandler::containsIDNameTable(unsigned short msgID)
{
	return IDNameTable.find(msgID) != IDNameTable.end();
}