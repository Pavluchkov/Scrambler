#include "smbios.h"
#include "Windows.h"

SMBios::SMBios()
{

}

const char* LocateStringA (const char* str, unsigned int i)
{
    static const char strNull[] = "Null String";

    if (0 == i || 0 == *str) return strNull;

    while (--i) {
        str += strlen ((char*)str) + 1;
    }

    return str;
}

const wchar_t* LocateStringW (const char* str, unsigned int i)
{
    static wchar_t buff[2048];
    const char *pStr = LocateStringA (str, i);
    SecureZeroMemory (buff, sizeof (buff));
    MultiByteToWideChar (CP_OEMCP, 0, pStr, strlen (pStr), buff, sizeof (buff));

    return buff;
}

const char* toPointString (void* p)
{
    return (char*)p + ((PSMBIOSHEADER)p)->Length;
}


bool ProcBIOSInfo (void* p, QVector<QString> &v)
{
    PBIOSInfo pBIOS = (PBIOSInfo)p;
    const char *str = toPointString (p);


    v.push_back ("Поставщик: " + toString (LocateString (str, pBIOS->Vendor)));
    v.push_back ("Версия: " + toString (LocateString (str, pBIOS->Version)));
    v.push_back ("Дата выпуска: " + toString (LocateString (str, pBIOS->ReleaseDate)));
    v.push_back ("Размер: " + toString ((((pBIOS->ROMSize) + 1) * 64)) + " K");
    if (pBIOS->Header.Length > 0x14) {   // for spec v2.4 and later
        v.push_back ("Версия системы BIOS: " + toString ((UINT)pBIOS->MajorRelease) + "." + toString ((UINT)pBIOS->MinorRelease));
        v.push_back ("Версия EC Firmware: " + toString ((UINT)pBIOS->ECFirmwareMajor) + "." + toString ((UINT)pBIOS->ECFirmwareMinor));
    }

    return true;
}

bool ProcSysInfo (void* p, QVector<QString> &v)
{
    PSystemInfo pSystem = (PSystemInfo)p;
    const char *str = toPointString (p);

    v.push_back ("Производитель: " + toString (LocateString (str, pSystem->Manufacturer)));
    v.push_back ("Наименование продукта: " + toString (LocateString (str, pSystem->ProductName)));
    v.push_back ("Версия: " + toString (LocateString (str, pSystem->Version)));
    v.push_back ("Серийный номер: " + toString (LocateString (str, pSystem->SN)));

    if (pSystem->Header.Length > 0x19) {
        // fileds for spec. 2.4
        v.push_back ("Номер SKU: " + toString (LocateString (str, pSystem->SKUNumber)));
        v.push_back ("Семейство: " + toString (LocateString (str, pSystem->Family)));
    }

    return true;
}

bool ProcBoardInfo (void* p, QVector<QString> &v)
{
    PBoardInfo pBoard = (PBoardInfo)p;
    const char *str = toPointString (p);

    v.push_back ("Производитель: " + toString (LocateString (str, pBoard->Manufacturer)));
    v.push_back ("Наименование продукта: " + toString (LocateString (str, pBoard->Product)));
    v.push_back ("Версия: " + toString (LocateString (str, pBoard->Version)));
    v.push_back ("Серийный номер: " + toString (LocateString (str, pBoard->SN)));
    v.push_back ("Asset Tag Number: " + toString (LocateString (str, pBoard->AssetTag)));

    if (pBoard->Header.Length > 0x08) {
        v.push_back ("Расположение на шасси: " +
            toString (LocateString (str, pBoard->LocationInChassis)));
    }

    return true;
}

bool ProcSystemEnclosure (void* p, QVector<QString> &v)
{
    PSystemEnclosure pSysEnclosure = (PSystemEnclosure)p;
    const char *str = toPointString (p);

    v.push_back ("Производитель: " + toString (LocateString (str, pSysEnclosure->Manufacturer)));
    v.push_back ("Версия: " + toString (LocateString (str, pSysEnclosure->Version)));
    v.push_back ("Серийный номер: " + toString (LocateString (str, pSysEnclosure->SN)));
    v.push_back ("Asset Tag Number: " + toString (LocateString (str, pSysEnclosure->AssetTag)));

    return true;
}

bool ProcProcessorInfo (void* p, QVector<QString> &v)
{
    PProcessorInfo	pProcessor = (PProcessorInfo)p;
    const char *str = toPointString (p);

    v.push_back ("Тип сокета: " + toString (LocateString (str, pProcessor->SocketDesignation)) +
        "; Количество ядер: " + toString ((UINT)pProcessor->CoreCount));
    v.push_back ("Тип процессора: " + toString (LocateString (str, pProcessor->Type)));
    v.push_back ("Производитель процессора: " + toString (LocateString (str, pProcessor->Manufacturer)));
    v.push_back ("Идентификационный номер: " + toString ((ULONG64)pProcessor->ID));
    v.push_back ("Серийный номер: " + toString (LocateString (str, pProcessor->SerialNumber)));
    v.push_back ("Номер партии: " + toString (LocateString (str, pProcessor->PartNumber)));
    v.push_back ("Внешняя тактовая частота: " + toString (pProcessor->ExtClock) + " MГц");
    v.push_back ("Максимальная тактовая частота : " + toString (pProcessor->MaxSpeed) + " MГц");
    v.push_back ("Текущая тактовая частота: " + toString (pProcessor->CurrentSpeed) + " MГц");

    return true;
}

bool ProcMemModuleInfo (void* p, QVector<QString> &v)
{
    PMemModuleInfo	pMemModule = (PMemModuleInfo)p;
    const char *str = toPointString (p);

    v.push_back ("Тип сокета памяти: " + toString (LocateString (str, pMemModule->SocketDesignation)));
    v.push_back ("Текущая скорость: " + toString ((UINT)pMemModule->CurrentSpeed) + " нс");

    return true;
}

bool ProcCacheInfo (void* p, QVector<QString> &v)
{
    PCacheInfo	pCache = (PCacheInfo)p;
    const char* str = toPointString (p);

    //v.push_back ("Кэш процессора: " + toString (LocateString (str, pCache->SocketDesignation)));
    v.push_back(toString(LocateString(str, pCache->SocketDesignation)) + "; ");
    return true;
}

bool ProcOEMString (void* p, QVector<QString> &v)
{
    PSMBIOSHEADER pHdr = (PSMBIOSHEADER)p;
    const char *str = toPointString (p);

    v.push_back ("OEM String: " + toString (LocateString (str, *(((char*)p) + 4))));

    return true;
}

bool ProcMemoryDevice (void* p, QVector<QString> &v)
{
    PMemoryDevice pMD = (PMemoryDevice)p;
    const char *str = toPointString (p);

    v.push_back ("Total Width: " + toString (pMD->TotalWidth) + " bits");
    v.push_back ("Data Width: " + toString (pMD->DataWidth) + " bits");
    v.push_back ("Device Locator: " + toString (LocateString (str, pMD->DeviceLocator)));
    v.push_back ("Bank Locator: " + toString (LocateString (str, pMD->BankLocator)));

    if (pMD->Header.Length > 0x15) {
        v.push_back ("Speed: " + toString (pMD->Speed));
        v.push_back ("Manufacturer: " + toString (LocateString (str, pMD->Manufacturer)));
        v.push_back ("Serial Number: " + toString (LocateString (str, pMD->SN)));
        v.push_back ("Asset Tag Number: " + toString (LocateString (str, pMD->AssetTag)));
        v.push_back ("Part Number: " + toString (LocateString (str, pMD->PN)));
    }

    return true;
}

bool ProcMemoryArrayMappedAddress (void*	p, QVector<QString> &v)
{
    PMemoryArrayMappedAddress pMAMA = (PMemoryArrayMappedAddress)p;
    const char *str = toPointString (p);

    v.push_back ("Starting Address: 0x" + toString ((unsigned int)pMAMA->Starting));
    v.push_back ("Ending Address: 0x" + toString ((unsigned int)pMAMA->Ending));
    v.push_back ("Memory Array Handle: 0x" + toString ((unsigned int)pMAMA->Handle));
    v.push_back ("Partition Width: 0x" + toString ((unsigned int)pMAMA->PartitionWidth));

    return true;
}

bool ProcPortableBattery (void* p, QVector<QString> &v)
{
    PPortableBattery pPB = (PPortableBattery)p;
    const char *str = toPointString (p);

    v.push_back ("Location: " + toString (LocateString (str, pPB->Location)));
    v.push_back ("Manufacturer: " + toString (LocateString (str, pPB->Manufacturer)));
    v.push_back ("Manufacturer Date: " + toString (LocateString (str, pPB->Date)));
    v.push_back ("Serial Number: " + toString (LocateString (str, pPB->SN)));

    return true;
}

int GetSMBInfo (int func, QVector<QString> &info)
{
    typedef struct
    {
        BYTE t;
        bool (*Proc)(void*, QVector<QString> &);
    } TPFUNC;

    const TPFUNC	tpfunc[] = {
        { 0, ProcBIOSInfo },
        { 1, ProcSysInfo },
        { 2, ProcBoardInfo },
        { 3, ProcSystemEnclosure },
        { 4, ProcProcessorInfo },
        { 6, ProcMemModuleInfo },
        { 7, ProcCacheInfo },
        { 11, ProcOEMString },
        { 17, ProcMemoryDevice },
        { 19, ProcMemoryArrayMappedAddress },
        { 22, ProcPortableBattery }

    };

    DWORD needBufferSize = 0;
    // the seqence just for x86, but don't worry we know SMBIOS/DMI only exist on x86 platform
    const BYTE byteSignature[] = { 'B', 'M', 'S', 'R' };
    const DWORD Signature = *((DWORD*)byteSignature);
    LPBYTE pBuff = NULL;

    needBufferSize = GetSystemFirmwareTable (Signature, 0, NULL, 0);

    pBuff = (LPBYTE)malloc (needBufferSize);

    if (pBuff) {
        GetSystemFirmwareTable (Signature, 0, pBuff, needBufferSize);

        const PRawSMBIOSData pDMIData = (PRawSMBIOSData)pBuff;

        LPBYTE p = (LPBYTE)(&(pDMIData->SMBIOSTableData));
        const DWORD lastAddress = ((DWORD)p) + pDMIData->Length;
        PSMBIOSHEADER pHeader;

        for (;;) {
            pHeader = (PSMBIOSHEADER)p;

            if (tpfunc[func].t == pHeader->Type)
                tpfunc[func].Proc ((void*)pHeader, info);

            PBYTE nt = p + pHeader->Length; // point to struct end

            while (0 != (*nt | *(nt + 1))) nt++; // skip string area
            nt += 2;

            if ((DWORD)nt >= lastAddress)
                break;
            p = nt;
        }
    }
    else
        info.push_back ("Can not allocate memory for recevice SMBIOS/DMI table.");

    if (pBuff)
        free (pBuff);

    return 0;
}
