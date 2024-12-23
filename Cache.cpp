#include "Cache.h"

using namespace std;

CacheInfo::CacheInfo()
{
    vendor = getProcessorVendor();

    model = getProcessorName();
}

CacheInfo::~CacheInfo()
{
}

void CacheInfo::getCacheInfo()
{
    getCacheInfoForAMD();
}

vector<string> CacheInfo::getVector()
{
    return vector<string>(infoCache);
}

string CacheInfo::getProcessorVendor()
{
    int cpuInfo[4];
    __cpuid(cpuInfo, 0); // �������� ���������� � ������������� ����������

    // ��������� ������������� �� �������� EBX, EDX, ECX
    char vendor[13]{};
    *((int*)&vendor[0]) = cpuInfo[1]; // EBX
    *((int*)&vendor[4]) = cpuInfo[3]; // EDX
    *((int*)&vendor[8]) = cpuInfo[2]; // ECX
    vendor[12] = '\0'; // ��������� ������
    
    return string(vendor);
}

string CacheInfo::getProcessorName()
{
    array<int, 4> cpuInfo{};
    char cpuBrandString[48] = { 0 }; // ����� ��� �������� ����������, ������ 48 ����

    // �������� ���������� � �������������
    __cpuid(cpuInfo.data(), 0x80000000);
    unsigned int nIds = cpuInfo[0];

    // �������� ���������� � ������ ����������
    if (nIds >= 0x80000004) {
        __cpuid((int*)&cpuBrandString[0], 0x80000002);
        __cpuid((int*)&cpuBrandString[16], 0x80000003);
        __cpuid((int*)&cpuBrandString[32], 0x80000004);
    }
    return string(cpuBrandString);
}

void CacheInfo::getCacheInfoForAMD() 
{
    DWORD bufferSize = 0;
    string cinfo, bytes[4];    
    short k = 0, s = 0;
    GetLogicalProcessorInformation(nullptr, &bufferSize); // �������� ����������� ������ ������

    SYSTEM_LOGICAL_PROCESSOR_INFORMATION* buffer = (SYSTEM_LOGICAL_PROCESSOR_INFORMATION*)malloc(bufferSize);

    if (buffer == nullptr) 
    {
        cinfo = "������ ��������� ������\n";
        infoCache.push_back(cinfo);
    }

    if (GetLogicalProcessorInformation(buffer, &bufferSize)) 
    {
        // ������� ��� �������� ���������� � �����
        map<string, Cache> cacheInfo;

        for (DWORD i = 0; i < bufferSize / sizeof(SYSTEM_LOGICAL_PROCESSOR_INFORMATION); ++i) 
        {
            if (buffer[i].Relationship == RelationCache) 
            {
                string cacheType;
                int lineSize = buffer[i].Cache.LineSize; // ������ ������ ����
                int cacheSizeKB = buffer[i].Cache.Size / 1024; // ������ ���� � ��
                int ways = buffer[i].Cache.Associativity;
                int sets = (cacheSizeKB * 1024) / (lineSize * ways); // ���������� �������

                switch (buffer[i].Cache.Level) 
                {
                case 1:
                    if (buffer[i].Cache.Type == CacheData) {
                        cacheType = "������� ����: L1";
                    }
                    else if (buffer[i].Cache.Type == CacheInstruction) {
                        cacheType = "������� ����: L1 ";
                    }
                    break;
                case 2:
                    cacheType = "������� ����: L2";
                    break;
                case 3:
                    cacheType = "������� ����: L3";
                    break;
                }

                if (!cacheType.empty())
                {
                    auto& info = cacheInfo[cacheType];
                    info.count++; // ����������� ����������
                    info.sizeKB = cacheSizeKB; // ���������� ������
                    info.lineSize = lineSize; // ���������� ������ ������
                    info.ways = ways; // ���������� ������� �����
                    info.sets = sets; // ���������� ���������� �������

                    // ���������� ��� ����
                    if (buffer[i].Cache.Type == CacheData) {
                        info.type = "��� ������";
                    }
                    else if (buffer[i].Cache.Type == CacheInstruction) {
                        info.type = "��� ����������";
                    }
                    else {
                        info.type = "������������ ���";
                    }
                }
            }
        }

        // ������� ����������
        for (const auto& entry : cacheInfo) 
        {
            const string& type = entry.first;
            const Cache& info = entry.second;

            bytes[k] = (info.sizeKB / 1024) > 0 ? " �����\n" : " �����\n";
            s = (info.sizeKB / 1024) > 0 ? (info.sizeKB / 1024) : info.sizeKB;

            cinfo = ""; // ��������� ������

            cinfo += type + '\n'
                + "  ���: " + info.type + '\n'
                + "  ������: " + to_string(s) + bytes[k++]
                + "  ������ ������: " + to_string(info.lineSize) + string(" �����\n")
                + "  ���������� �����: " + to_string(info.ways) + '\n'
                + "  ���������� �������: " + to_string(info.sets) + '\n'
                + "  ���������� ��������: " + to_string(info.count) + '\n';
            
            infoCache.push_back(cinfo);
        }
    }
    else {
        cinfo += "������ ��������� ����������: \n";
        infoCache.push_back(cinfo);
    }

    free(buffer); // ����������� ������
}

void CacheInfo::getCacheInfoForIntel() 
{
    std::array<int, 4> cpuInfo; // ������ ��� �������� �������� ��������� EAX, EBX, ECX � EDX ����� ������ CPUID
    string info;

    // ���� ��� ������ ������� ���� �� L1 �� L3, ��� ������� ������ ����������� CPUID � ECX = i
    for (int i = 0; i < 4; i++) 
    {
        info = "";

        // �������� CPUID � ������� ���������� ������� __cpuidex
        __cpuidex(cpuInfo.data(), 4, i);

        // ���� ������� 5 ��� EAX ����� 0, ��� ������, ��� ������� ���� ������ ���, ��������� ����
        if ((cpuInfo[0] & 0x1F) == 0) {
            break;
        }

        // ���������� ��������� ����, �������� ������ �� ��������������� ���������
        int cacheType = cpuInfo[0] & 0x1F;                  // ��� ���� (������, ���������� ��� ������������) �� ������� 5 ����� EAX
        int cacheLevel = (cpuInfo[0] >> 5) & 0x7;           // ������� ���� (L1, L2, L3), ���� 5-7 � EAX
        int cacheLineSize = (cpuInfo[1] & 0xFFF) + 1;       // ������ ������ ���� (� ������) �� ����� 0-11 � EBX
        int cachePartitions = ((cpuInfo[1] >> 0xC) & 0x15) + 1; // ���������� �������� (partitions) �� ����� 12-21 � EBX
        int cacheWays = ((cpuInfo[1] >> 22) & 0x3FF) + 1;   // ���������� ����� (associativity) �� ����� 22-31 � EBX
        int cacheSets = cpuInfo[2] + 1;                     // ���������� ������� (sets) �� �������� ECX

        // ������������ ����� ������ ����, ������� ��� ���������
        int cacheSize = cacheWays * cachePartitions * cacheLineSize * cacheSets;

        // ���������� ��� ���� � ���� ������
        std::string cacheTypeStr;
        switch (cacheType) {
        case 1: cacheTypeStr = "��� ������"; break;      // ���� cacheType = 1, �� ��� ��� ������
        case 2: cacheTypeStr = "��� ����������"; break;   // ���� cacheType = 2, �� ��� ��� ����������
        case 3: cacheTypeStr = "������������ ���"; break; // ���� cacheType = 3, �� ��� ������������ ���
        default: cacheTypeStr = "����������� ��� ����"; break; // � ��������� ������� ��� ����������
        }
        
        // ������� ���������� � ������� ������ ����
        info += "������� ����: L" + to_string(cacheLevel) + '\n';             // ������� ���� (L1, L2, L3)
        info += "  ���: " + cacheTypeStr + '\n';                             // ��� ���� (������, ���������� ��� ������������)
        info += "  ���������� ��������: " + to_string(cachePartitions) + '\n'; // ���������� �������� (partitions)
        info += "  ������: " + to_string(cacheSize / 1024) + string(" ��") + '\n'; // ������ ���� � ����������
        info += "  ������ ������: " + to_string(cacheLineSize) + string(" ����") + '\n'; // ������ ������ ���� � ������
        info += "  ���������� �����: " + to_string(cacheWays) + '\n';            // ���������� ����� (associativity)
        info += "  ���������� �������: " + to_string(cacheSets) + '\n';          // ���������� �������
        infoCache.push_back(info);
    }
}

std::string ReplaceNewlines(const std::string& input)
{
    std::string output;
    for (char c : input) {
        if (c == '\n') {
            output += "\r\n"; // ��������� \r\n ������ \n
        }
        else {
            output += c;
        }
    }
    return output;
}