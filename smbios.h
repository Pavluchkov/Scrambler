#ifndef H_SMBIOS
#define H_SMBIOS

#include <windows.h>
#include <vector>
#include "QString"
#include <sstream>

#pragma pack(push) 
#pragma pack(1)

typedef struct _RawSMBIOSData
{
	BYTE	Used20CallingMethod;
	BYTE	SMBIOSMajorVersion;
	BYTE	SMBIOSMinorVersion;
	BYTE	DmiRevision;
	DWORD	Length;
	PBYTE	SMBIOSTableData;
} RawSMBIOSData, *PRawSMBIOSData;

typedef struct _SMBIOSHEADER_
{
	BYTE Type;
	BYTE Length;
	WORD Handle;
} SMBIOSHEADER, *PSMBIOSHEADER;

typedef struct _TYPE_0_
{
	SMBIOSHEADER	Header;
	UCHAR	Vendor;
	UCHAR	Version;
	UINT16	StartingAddrSeg;
	UCHAR	ReleaseDate;
	UCHAR	ROMSize;
	ULONG64 Characteristics;
	UCHAR	Extension[2]; // spec. 2.3
	UCHAR	MajorRelease;
	UCHAR	MinorRelease;
	UCHAR	ECFirmwareMajor;
	UCHAR	ECFirmwareMinor;
} BIOSInfo, *PBIOSInfo;


typedef struct _TYPE_1_
{
	SMBIOSHEADER	Header;
	UCHAR	Manufacturer;
	UCHAR	ProductName;
	UCHAR	Version;
	UCHAR	SN;
	UCHAR	UUID[16];
	UCHAR	WakeUpType;
	UCHAR	SKUNumber;
	UCHAR	Family;
} SystemInfo, *PSystemInfo;

typedef struct _TYPE_2_
{
	SMBIOSHEADER	Header;
	UCHAR	Manufacturer;
	UCHAR	Product;
	UCHAR	Version;
	UCHAR	SN;
	UCHAR	AssetTag;
	UCHAR	FeatureFlags;
	UCHAR	LocationInChassis;
	UINT16	ChassisHandle;
	UCHAR	Type;
	UCHAR	NumObjHandle;
	UINT16	*pObjHandle;
} BoardInfo, *PBoardInfo;

typedef struct _TYPE_3_
{
	SMBIOSHEADER Header;
	UCHAR	Manufacturer;
	UCHAR	Type;
	UCHAR	Version;
	UCHAR	SN;
	UCHAR	AssetTag;
	UCHAR	BootupState;
	UCHAR	PowerSupplyState;
	UCHAR	ThermalState;
	UCHAR	SecurityStatus;
	ULONG32	OEMDefine;
	UCHAR	Height;
	UCHAR	NumPowerCord;
	UCHAR	ElementCount;
	UCHAR	ElementRecordLength;
	UCHAR	pElements;
} SystemEnclosure, *PSystemEnclosure;

typedef struct _TYPE_4_
{
	SMBIOSHEADER Header;
	UCHAR	SocketDesignation;
	UCHAR	Type;
	UCHAR	Family;
	UCHAR	Manufacturer;
	ULONG64 ID;
	UCHAR	Version;
	UCHAR	Voltage;
	UINT16	ExtClock;
	UINT16	MaxSpeed;
	UINT16	CurrentSpeed;
	UCHAR   Status;
	UCHAR   ProcessorUpgrade;
	UINT16  L1cashe;
	UINT16  L2cashe;
	UINT16  L3cashe;
	UCHAR   SerialNumber;
	UCHAR   AssetTag;
	UCHAR   PartNumber;
	UCHAR   CoreCount;
	// Todo, Here

} ProcessorInfo, *PProcessorInfo;

typedef struct _TYPE_5_
{
	SMBIOSHEADER Header;
	// Todo, Here

} MemCtrlInfo, *PMemCtrlInfo;

typedef struct _TYPE_6_
{
	SMBIOSHEADER Header;
	UCHAR	SocketDesignation;
	UCHAR	BankConnections;
	UCHAR	CurrentSpeed;
	// Todo, Here
} MemModuleInfo, *PMemModuleInfo;

typedef struct _TYPE_7_
{
	SMBIOSHEADER Header;
	UCHAR	SocketDesignation;
	UINT16	Configuration;
	UINT16	MaxSize;
	UINT16	InstalledSize;
	UINT16	SupportSRAMType;
	UINT16	CurrentSRAMType;
	UCHAR	Speed;
	UCHAR	ErrorCorrectionType;
	UCHAR	SystemCacheType;
	UCHAR	Associativity;
} CacheInfo, *PCacheInfo;

typedef struct _TYPE_17_
{
	SMBIOSHEADER Header;
	UINT16	PhysicalArrayHandle;
	UINT16	ErrorInformationHandle;
	UINT16	TotalWidth;
	UINT16	DataWidth;
	UINT16	Size;
	UCHAR	FormFactor;
	UCHAR	DeviceSet;
	UCHAR	DeviceLocator;
	UCHAR	BankLocator;
	UCHAR	MemoryType;
	UINT16	TypeDetail;
	UINT16	Speed;
	UCHAR	Manufacturer;
	UCHAR	SN;
	UCHAR	AssetTag;
	UCHAR	PN;
	UCHAR	Attributes;
} MemoryDevice, *PMemoryDevice;

typedef struct _TYPE_19_
{
	SMBIOSHEADER Header;
	ULONG32	Starting;
	ULONG32	Ending;
	UINT16	Handle;
	UCHAR	PartitionWidth;
} MemoryArrayMappedAddress, *PMemoryArrayMappedAddress;

typedef struct _TYPE_22_
{
	SMBIOSHEADER Header;
	UCHAR	Location;
	UCHAR	Manufacturer;
	UCHAR	Date;
	UCHAR	SN;
	UCHAR	DeviceName;

} PortableBattery, *PPortableBattery;

#pragma pack(push) 


#ifndef UNICODE
#define getHeaderString  getHeaderStringW
#define LocateString	LocateStringW
#else
#define getHeaderString  getHeaderStringA
#define LocateString	LocateStringA
#endif

template <typename T> QString toString (T val)
{
	std::ostringstream oss;
	oss << val;
    QString str = oss.str().c_str();
    //return oss.str ();
    return str;
}

//template <typename T> T fromString (const std::string &s)
//{
//	std::istringstream iss;
//	T res;
//	iss >> res;
//	return res;
//}

class SMBios
{
public:
    SMBios();
    //virtual getInfo();
    const char* LocateStringA (const char*, UINT);
    const wchar_t* LocateStringW (const char*, UINT);
    const char* toPointString (void*);
    bool ProcBIOSInfo (void*);
    int GetSMBInfo (int);
};

class Bios:public SMBios
{
public:
    Bios();
    void getInfo();

    QString Vendor;
    QString Version;
    QString ReleaseDate;
    QString ROMSize;
    QString MajorRelease;
    QString MinorRelease;
    QString ECFirmwareMajor;
    QString ECFirmwareMinor;
};


//bool ProcSysInfo (void*, std::vector<std::string> &);
//bool ProcBoardInfo (void*, std::vector<std::string> &);
//bool ProcSystemEnclosure (void*, std::vector<std::string> &);
//bool ProcProcessorInfo (void*, std::vector<std::string> &);
//bool ProcMemModuleInfo (void*, std::vector<std::string> &);
//bool ProcCacheInfo (void *, std::vector<std::string> &);
//bool ProcOEMString (void*, std::vector<std::string> &);
//bool ProcMemoryDevice (void*, std::vector<std::string> &);
//bool ProcMemoryArrayMappedAddress (void*, std::vector<std::string> &);
//bool ProcPortableBattery (void*, std::vector<std::string> &);


#endif
