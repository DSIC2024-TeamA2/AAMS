/*****************************************************************//**
 * \file        ams_configuration.h
 * \brief       대공유도탄 모의시스템이 설정한 환경 정보 파일의 정보를 관리하는 파일
 * \details		대공유도탄 모의시스템의 환경 정보 파일을 불러와 저장한다.
 * \author      남아리
 * \date        2024.04.25
 *********************************************************************/
#pragma once
#include <iostream>
#include <fstream>
#include <string>

/**
* @class     AMSConfiguration
* @brief     대공유도탄 모의시스템의 환경 정보를 관리하는 클래스
* @details   대공유도탄 모의시스템의 환경 정보 파일을 불러와 저장한다.
*/
class AMSConfiguration {
public:
    /**
     * @brief   대공유도탄 모의시스템의 환경 정보를 관리하는 객체를 불러오는 함수
     * @return  대공유도탄 모의시스템의 환경 정보를 관리하는 객체
     */
    static AMSConfiguration& getInstance();

    int getUpdateDuration() const;      /// 모의기 통신 주기
    int getDetectionRadius() const;     /// 대공유도탄 모의기의 탐지 범위

private:
    /**
    * @brief    설정한 시나리오 정보를 초기화
     */
    AMSConfiguration();

    /**
     * @brief   대공유도탄 모의시스템의 환경 정보로 초기화하는 함수
     */
    void Init();

    int updateDuration;     /// 모의기 통신 주기
    int detectionRadius;    /// 대공유도탄 모의기의 탐지 범위

    /**
     * @brief       복사 생성자로 사용 제한하는 함수
     * @param[in]   AMSConfiguration
     */
    AMSConfiguration(const AMSConfiguration&) = delete;

    /**
     * @brief       대입 연산자로 사용 제한하는 함수
     * @param[in]   AMSConfiguration
     */
    AMSConfiguration& operator=(const AMSConfiguration&) = delete;

    static const std::string CONF_FILE_PATH;    /// 대공유도탄 모의시스템의 환경 정보 파일 경로
};
