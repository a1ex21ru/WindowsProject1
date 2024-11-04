#pragma once

#include <array>
#include <vector>
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

struct CPAndCacheInfo
{
    string vendor;
    string model;

    string L1_data;
    string L1_code;

    string L2;
    string L3;


    CPAndCacheInfo() = default;
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
    /// ��������� ���������� � ���-������
    /// </summary>
    void getCacheInfo();

    /// <summary>
    /// ������ ���������� � ���-������
    /// </summary>
    /// <returns></returns>
    vector<string> getVector();
    
private:

    /// <summary>
    /// ������ �������� ����
    /// </summary>
    vector<string> infoCache;

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
    void getCacheInfoForIntel();

    /// <summary>
    /// ������ �� ��� ������ ����������� AMD
    /// </summary>
    void getCacheInfoForAMD();

};

std::string ReplaceNewlines(const std::string& input);