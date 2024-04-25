/*****************************************************************//**
 * \file        ams_configuration.h
 * \brief       �������ź ���ǽý����� ������ ȯ�� ���� ������ ������ �����ϴ� ����
 * \details		�������ź ���ǽý����� ȯ�� ���� ������ �ҷ��� �����Ѵ�.
 * \author      ���Ƹ�
 * \date        2024.04.25
 *********************************************************************/
#pragma once
#include <iostream>
#include <fstream>
#include <string>

/**
* @class     AMSConfiguration
* @brief     �������ź ���ǽý����� ȯ�� ������ �����ϴ� Ŭ����
* @details   �������ź ���ǽý����� ȯ�� ���� ������ �ҷ��� �����Ѵ�.
*/
class AMSConfiguration {
public:
    /**
     * @brief   �������ź ���ǽý����� ȯ�� ������ �����ϴ� ��ü�� �ҷ����� �Լ�
     * @return  �������ź ���ǽý����� ȯ�� ������ �����ϴ� ��ü
     */
    static AMSConfiguration& getInstance();

    int getUpdateDuration() const;      /// ���Ǳ� ��� �ֱ�
    int getDetectionRadius() const;     /// �������ź ���Ǳ��� Ž�� ����

private:
    /**
    * @brief    ������ �ó����� ������ �ʱ�ȭ
     */
    AMSConfiguration();

    /**
     * @brief   �������ź ���ǽý����� ȯ�� ������ �ʱ�ȭ�ϴ� �Լ�
     */
    void Init();

    int updateDuration;     /// ���Ǳ� ��� �ֱ�
    int detectionRadius;    /// �������ź ���Ǳ��� Ž�� ����

    /**
     * @brief       ���� �����ڷ� ��� �����ϴ� �Լ�
     * @param[in]   AMSConfiguration
     */
    AMSConfiguration(const AMSConfiguration&) = delete;

    /**
     * @brief       ���� �����ڷ� ��� �����ϴ� �Լ�
     * @param[in]   AMSConfiguration
     */
    AMSConfiguration& operator=(const AMSConfiguration&) = delete;

    static const std::string CONF_FILE_PATH;    /// �������ź ���ǽý����� ȯ�� ���� ���� ���
};
