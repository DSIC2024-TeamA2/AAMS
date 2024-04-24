#pragma once
#include <iostream>
#include <fstream>
#include <string>

class AMSConfiguration {
public:
    static AMSConfiguration& getInstance();
    int getUpdateDuration() const;
    int getDetectionRadius() const;

private:
    AMSConfiguration();
    void Init();
    int updateDuration;
    int detectionRadius;
    AMSConfiguration(const AMSConfiguration&) = delete;
    AMSConfiguration& operator=(const AMSConfiguration&) = delete;
    static const std::string CONF_FILE_PATH; 
};
