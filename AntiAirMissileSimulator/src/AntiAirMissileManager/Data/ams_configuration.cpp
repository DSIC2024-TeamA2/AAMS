#pragma once
#include "ams_configuration.h"

const std::string AMSConfiguration::CONF_FILE_PATH = "../../conf";

AMSConfiguration& AMSConfiguration::getInstance() {
    static AMSConfiguration instance;
    return instance;
}

int AMSConfiguration::getUpdateDuration() const {
    return updateDuration;
}

int AMSConfiguration::getDetectionRadius() const {
    return detectionRadius;
}

AMSConfiguration::AMSConfiguration() {
    Init();
}

void AMSConfiguration::Init() {
    std::ifstream file(CONF_FILE_PATH);

    if (file.is_open()) {
        std::string line;
        std::getline(file, line);
        updateDuration = std::stoi(line);
        std::getline(file, line);
        detectionRadius = std::stoi(line);
        file.close();
    }
    else {
        std::cerr << "Error opening configuration file: " << CONF_FILE_PATH << std::endl;
    }
}