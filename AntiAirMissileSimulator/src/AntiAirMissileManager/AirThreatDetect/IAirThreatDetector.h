#pragma once
#include "../Data/DataStruct.h"

template <typename T1, typename T2>
class IAirThreatDetector
{
public:
	IAirThreatDetector() {}
	virtual void setRadarInfo(T1 radarInfo) = 0;
	virtual void setTargetInfo(T2 targetInfo) = 0;
	virtual bool detected() = 0;
	virtual void clear() = 0;
};