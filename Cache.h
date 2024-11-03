#pragma once

#include <array>
#include <intrin.h>
#include <iostream>
#include <map>
#include <string>
#include <windows.h>

#define AMD R"(AuthenticAMD)" 
#define Intel R"(GenuineIntel)"

using namespace std;

struct Cache {
    int count;       // ���������� �����
    int sizeKB;      // ������ ���� � ��
    int lineSize;    // ������ ������ ���� � ������
    int ways;        // ������� �����
    int sets;        // ���������� �������
    string type; // ��� ����

    Cache() : count(0), sizeKB(0), lineSize(0), ways(0), sets(0), type("") {} // ����������� �� ���������
};

class CacheInfo
{
public:
    CacheInfo();
    ~CacheInfo();

    /// <summary>
    /// ������ ������������� ����������
    /// </summary>
    /// <returns></returns>
    string getProcessorVendor();

    /// <summary>
    /// ������ ������ ����������
    /// </summary>
    /// <returns></returns>
    string getProcessorName();

    /// <summary>
    /// ����� ���������� � ���-������
    /// </summary>
    string getCacheInfo();
    
private:

    /// <summary>
    /// ������������� ��
    /// </summary>
    std::string vendor;

    /// <summary>
    /// �������� ��
    /// </summary>
    std::string model;

    /// <summary>
    /// ������ �� ��� ������ ����������� Intel
    /// </summary>
    string getCacheInfoForIntel();

    /// <summary>
    /// ������ �� ��� ������ ����������� AMD
    /// </summary>
    string getCacheInfoForAMD();

};