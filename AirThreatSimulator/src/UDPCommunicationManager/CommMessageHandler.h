#pragma once
#include <map>
#include <nFramework/util/util.h>
#include <nFramework/nom/NOM.h>

using namespace nframework;
using namespace nframework::nom;

class CommMessageHandler
{
public:
	CommMessageHandler();
	~CommMessageHandler();

	tstring getMsgName(unsigned short msgID);
	bool containsIDNameTable(unsigned short msgID);
	void setNOM(shared_ptr<NOM>);
	shared_ptr<NOM> getNOM(tstring msgName);

private:
	void initialize();
	void release();
	void setIDNameTable(unsigned short msgID, tstring msgName);
	
private:
	map<unsigned short, tstring> IDNameTable;
	map<tstring, shared_ptr<NOM>> ObjNOMTable;
};

