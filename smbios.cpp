#include "SMBIOS.h"

SMBios::SMBios()
{
    GetSMBInfo(this);
}

const char* LocateStringA (const char* str, UINT i)
{
    static const char strNull[] = "Null String";

    if (0 == i || 0 == *str) return strNull;

    while (--i) {
        str += strlen ((char*)str) + 1;
    }

    return str;
}

const wchar_t* LocateStringW (const char* str, UINT i)
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


bool ProcBIOSInfo (void* p, SMBios* smb)
{
    PBIOSInfo pBIOS = (PBIOSInfo)p;
    const char *str = toPointString (p);

    smb->Bios.Vendor = toString (LocateString (str, pBIOS->Vendor));
    smb->Bios.Version = toString (LocateString (str, pBIOS->Version));
    smb->Bios.ReleaseDate = toString (LocateString (str, pBIOS->ReleaseDate));
    smb->Bios.ROMSize = toString ((((pBIOS->ROMSize) + 1) * 64));

    if (pBIOS->Header.Length > 0x14) {   // for spec v2.4 and later
        smb->Bios.MajorRelease = toString ((UINT)pBIOS->MajorRelease);
        smb->Bios.MinorRelease = toString ((UINT)pBIOS->MinorRelease);
        smb->Bios.ECFirmwareMajor = toString ((UINT)pBIOS->ECFirmwareMajor);
        smb->Bios.ECFirmwareMinor = toString ((UINT)pBIOS->ECFirmwareMinor);
    }

    return true;
}

bool ProcSysInfo (void* p, SMBios* smb)
{
    PSystemInfo pSystem = (PSystemInfo)p;
    const char *str = toPointString (p);

    smb->System.Manufacturer = toString (LocateString (str, pSystem->Manufacturer));
    smb->System.ProductName = toString (LocateString (str, pSystem->ProductName));
    smb->System.Version = toString (LocateString (str, pSystem->Version));
    smb->System.SN = toString (LocateString (str, pSystem->SN));

    if (pSystem->Header.Length > 0x19) {
        // fileds for spec. 2.4
        smb->System.SKUNumber = toString (LocateString (str, pSystem->SKUNumber));
        smb->System.Family = toString (LocateString (str, pSystem->Family));
    }

    return true;
}

bool ProcBoardInfo (void* p, SMBios* smb)
{
    PBoardInfo pBoard = (PBoardInfo)p;
    const char *str = toPointString (p);

    smb->MotherBoard.Manufacturer = toString (LocateString (str, pBoard->Manufacturer));
    smb->MotherBoard.Product = toString (LocateString (str, pBoard->Product));
    smb->MotherBoard.Version = toString (LocateString (str, pBoard->Version));
    smb->MotherBoard.SN = toString (LocateString (str, pBoard->SN));
    smb->MotherBoard.AssetTag = toString (LocateString (str, pBoard->AssetTag));

    if (pBoard->Header.Length > 0x08) {
        smb->MotherBoard.LocationInChassis = toString (LocateString (str, pBoard->LocationInChassis));
    }

    return true;
}

bool ProcSystemEnclosure (void* p, SMBios* smb)
{
    PSystemEnclosure pSysEnclosure = (PSystemEnclosure)p;
    const char *str = toPointString (p);

    smb->SystemEnclosure.Manufacturer = toString (LocateString (str, pSysEnclosure->Manufacturer));
    smb->SystemEnclosure.Version = toString (LocateString (str, pSysEnclosure->Version));
    smb->SystemEnclosure.SN = toString (LocateString (str, pSysEnclosure->SN));
    smb->SystemEnclosure.AssetTag = toString (LocateString (str, pSysEnclosure->AssetTag));

    return true;
}

bool ProcProcessorInfo (void* p, SMBios* smb)
{
    PProcessorInfo	pProcessor = (PProcessorInfo)p;
    const char *str = toPointString (p);

    smb->CPU.SocketDesignation = toString (LocateString (str, pProcessor->SocketDesignation));
    smb->CPU.CoreCount = toString ((UINT)pProcessor->CoreCount);
    smb->CPU.Type = toString (LocateString (str, pProcessor->Type));
    smb->CPU.Manufacturer = toString (LocateString (str, pProcessor->Manufacturer));
    smb->CPU.ID = toString ((ULONG64)pProcessor->ID);
    smb->CPU.SerialNumber = toString (LocateString (str, pProcessor->SerialNumber));
    smb->CPU.PartNumber = toString (LocateString (str, pProcessor->PartNumber));
    smb->CPU.ExtClock = toString (pProcessor->ExtClock);
    smb->CPU.MaxSpeed = toString (pProcessor->MaxSpeed);
    smb->CPU.CurrentSpeed = toString (pProcessor->CurrentSpeed);

    return true;
}

bool ProcMemModuleInfo (void* p, SMBios* smb)
{
    PMemModuleInfo	pMemModule = (PMemModuleInfo)p;
    const char *str = toPointString (p);

    smb->MemModule.SocketDesignation = toString (LocateString (str, pMemModule->SocketDesignation));
    smb->MemModule.CurrentSpeed = toString ((UINT)pMemModule->CurrentSpeed);

    return true;
}

bool ProcCacheInfo (void* p, SMBios* smb)
{
    PCacheInfo	pCache = (PCacheInfo)p;
    const char* str = toPointString (p);

    smb->Cash.SocketDesignation = toString (LocateString (str, pCache->SocketDesignation));

    return true;
}

bool ProcOEMString (void* p, SMBios* smb)
{
    //PSMBIOSHEADER pHdr = (PSMBIOSHEADER)p;
    const char *str = toPointString (p);

    smb->OEMString.oemString = toString (LocateString (str, *(((char*)p) + 4)));

    return true;
}

bool ProcMemoryDevice (void* p, SMBios* smb)
{
    PMemoryDevice pMD = (PMemoryDevice)p;
    const char *str = toPointString (p);

    smb->MemoryDevice.TotalWidth = toString (pMD->TotalWidth);
    smb->MemoryDevice.DataWidth = toString (pMD->DataWidth);
    smb->MemoryDevice.DeviceLocator = toString (LocateString (str, pMD->DeviceLocator));
    smb->MemoryDevice.BankLocator = toString (LocateString (str, pMD->BankLocator));

    if (pMD->Header.Length > 0x15) {
        smb->MemoryDevice.Speed = toString (pMD->Speed);
        smb->MemoryDevice.Manufacturer = toString (LocateString (str, pMD->Manufacturer));
        smb->MemoryDevice.SN = toString (LocateString (str, pMD->SN));
        smb->MemoryDevice.AssetTag = toString (LocateString (str, pMD->AssetTag));
        smb->MemoryDevice.PN = toString (LocateString (str, pMD->PN));
    }

    return true;
}

bool ProcMemoryArrayMappedAddress (void* p, SMBios* smb)
{
    PMemoryArrayMappedAddress pMAMA = (PMemoryArrayMappedAddress)p;
    //const char *str = toPointString (p);

    smb->MemoryArrayMappedAdress.Starting = toString ((UINT)pMAMA->Starting);
    smb->MemoryArrayMappedAdress.Ending = toString ((UINT)pMAMA->Ending);
    smb->MemoryArrayMappedAdress.Handle = toString ((UINT)pMAMA->Handle);
    smb->MemoryArrayMappedAdress.PartitionWidth = toString ((UINT)pMAMA->PartitionWidth);

    return true;
}

bool ProcPortableBattery (void* p, SMBios* smb)
{
    PPortableBattery pPB = (PPortableBattery)p;
    const char *str = toPointString (p);

    smb->PortableBattery.Location = toString (LocateString (str, pPB->Location));
    smb->PortableBattery.Manufacturer = toString (LocateString (str, pPB->Manufacturer));
    smb->PortableBattery.Date = toString (LocateString (str, pPB->Date));
    smb->PortableBattery.SN = toString (LocateString (str, pPB->SN));

    return true;
}

int SMBios::GetSMBInfo (SMBios* smb)
{
    typedef struct
    {
        BYTE t;
        bool (*Proc)(void*, SMBios*);
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

            for (UINT i = 0; i < sizeof(tpfunc) / sizeof(TPFUNC); i++)
            {
                if (tpfunc[i].t == pHeader->Type)
                    tpfunc[i].Proc ((void*)pHeader, smb);
            }

            PBYTE nt = p + pHeader->Length; // point to struct end

            while (0 != (*nt | *(nt + 1))) nt++; // skip string area
            nt += 2;

            if ((DWORD)nt >= lastAddress)
                break;
            p = nt;
        }
    }
    else
        smb->error = "Can not allocate memory for recevice SMBIOS/DMI table.";

    if (pBuff)
        free (pBuff);

    return 0;
}
