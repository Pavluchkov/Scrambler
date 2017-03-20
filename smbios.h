#ifndef SMBIOS_H
#define SMBIOS_H

#include <QObject>
#include "string"
#include "ostream"
#include"QVariant"
#include "QVector"

#pragma pack(push)
#pragma pack(1)

typedef struct _RawSMBIOSData
{
    unsigned char       Used20CallingMethod;
    unsigned char       SMBIOSMajorVersion;
    unsigned char       SMBIOSMinorVersion;
    unsigned char       DmiRevision;
    unsigned long int	Length;
    unsigned char	SMBIOSTableData;
} RawSMBIOSData, *PRawSMBIOSData;

typedef struct _SMBIOSHEADER_
{
    unsigned char       Type;
    unsigned char       Length;
    unsigned long int   Handle;
} SMBIOSHEADER, *PSMBIOSHEADER;

typedef struct _TYPE_0_
{
    SMBIOSHEADER	Header;
    unsigned char	Vendor;
    unsigned char	Version;
    unsigned short	StartingAddrSeg;
    unsigned char	ReleaseDate;
    unsigned char	ROMSize;
    unsigned __int64 Characteristics;
    unsigned char	Extension[2]; // spec. 2.3
    unsigned char	MajorRelease;
    unsigned char	MinorRelease;
    unsigned char	ECFirmwareMajor;
    unsigned char	ECFirmwareMinor;
} BIOSInfo, *PBIOSInfo;


typedef struct _TYPE_1_
{
    SMBIOSHEADER	Header;
    unsigned char	Manufacturer;
    unsigned char	ProductName;
    unsigned char	Version;
    unsigned char	SN;
    unsigned char	UUID[16];
    unsigned char	WakeUpType;
    unsigned char	SKUNumber;
    unsigned char	Family;
} SystemInfo, *PSystemInfo;

typedef struct _TYPE_2_
{
    SMBIOSHEADER	Header;
    unsigned char	Manufacturer;
    unsigned char	Product;
    unsigned char	Version;
    unsigned char	SN;
    unsigned char	AssetTag;
    unsigned char	FeatureFlags;
    unsigned char	LocationInChassis;
    unsigned short	ChassisHandle;
    unsigned char	Type;
    unsigned char	NumObjHandle;
    unsigned short	*pObjHandle;
} BoardInfo, *PBoardInfo;

typedef struct _TYPE_3_
{
    SMBIOSHEADER Header;
    unsigned char	Manufacturer;
    unsigned char	Type;
    unsigned char	Version;
    unsigned char	SN;
    unsigned char	AssetTag;
    unsigned char	BootupState;
    unsigned char	PowerSupplyState;
    unsigned char	ThermalState;
    unsigned char	SecurityStatus;
    unsigned int	OEMDefine;
    unsigned char	Height;
    unsigned char	NumPowerCord;
    unsigned char	ElementCount;
    unsigned char	ElementRecordLength;
    unsigned char	pElements;
} SystemEnclosure, *PSystemEnclosure;

typedef struct _TYPE_4_
{
    SMBIOSHEADER Header;
    unsigned char	SocketDesignation;
    unsigned char	Type;
    unsigned char	Family;
    unsigned char	Manufacturer;
    unsigned __int64 ID;
    unsigned char	Version;
    unsigned char	Voltage;
    unsigned short	ExtClock;
    unsigned short	MaxSpeed;
    unsigned short	CurrentSpeed;
    unsigned char   Status;
    unsigned char   ProcessorUpgrade;
    unsigned short  L1cashe;
    unsigned short  L2cashe;
    unsigned short  L3cashe;
    unsigned char   SerialNumber;
    unsigned char   AssetTag;
    unsigned char   PartNumber;
    unsigned char   CoreCount;
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
    unsigned char	SocketDesignation;
    unsigned char	BankConnections;
    unsigned char	CurrentSpeed;
    // Todo, Here
} MemModuleInfo, *PMemModuleInfo;

typedef struct _TYPE_7_
{
    SMBIOSHEADER Header;
    unsigned char	SocketDesignation;
    unsigned short	Configuration;
    unsigned short	MaxSize;
    unsigned short	InstalledSize;
    unsigned short	SupportSRAMType;
    unsigned short	CurrentSRAMType;
    unsigned char	Speed;
    unsigned char	ErrorCorrectionType;
    unsigned char	SystemCacheType;
    unsigned char	Associativity;
} CacheInfo, *PCacheInfo;

typedef struct _TYPE_17_
{
    SMBIOSHEADER Header;
    unsigned short	PhysicalArrayHandle;
    unsigned short	ErrorInformationHandle;
    unsigned short	TotalWidth;
    unsigned short	DataWidth;
    unsigned short	Size;
    unsigned char	FormFactor;
    unsigned char	DeviceSet;
    unsigned char	DeviceLocator;
    unsigned char	BankLocator;
    unsigned char	MemoryType;
    unsigned short	TypeDetail;
    unsigned short	Speed;
    unsigned char	Manufacturer;
    unsigned char	SN;
    unsigned char	AssetTag;
    unsigned char	PN;
    unsigned char	Attributes;
} MemoryDevice, *PMemoryDevice;

typedef struct _TYPE_19_
{
    SMBIOSHEADER Header;
    unsigned int	Starting;
    unsigned int	Ending;
    unsigned short	Handle;
    unsigned char	PartitionWidth;
} MemoryArrayMappedAddress, *PMemoryArrayMappedAddress;

typedef struct _TYPE_22_
{
    SMBIOSHEADER Header;
    unsigned char	Location;
    unsigned char	Manufacturer;
    unsigned char	Date;
    unsigned char	SN;
    unsigned char	DeviceName;

} PortableBattery, *PPortableBattery;

#pragma pack(push)


#ifdef UNICODE
#define getHeaderString  getHeaderStringW
#define LocateString	LocateStringW
#else
#define getHeaderString  getHeaderStringA
#define LocateString	LocateStringA
#endif

QString toString (QVariant val)
{
    return val.toString();
}

class SMBios
{
public:
    SMBios();
    const char* LocateStringA (const char*, unsigned int);
    const wchar_t* LocateStringW (const char*, unsigned int);
    const char* toPointString (void*);
    bool ProcBIOSInfo (void*, QVector<QString> &);
    bool ProcSysInfo (void*, QVector<QString> &);
    bool ProcBoardInfo (void*, QVector<QString> &);
    bool ProcSystemEnclosure (void*, QVector<QString> &);
    bool ProcProcessorInfo (void*, QVector<QString> &);
    bool ProcMemModuleInfo (void*, QVector<QString> &);
    bool ProcCacheInfo (void *, QVector<QString> &);
    bool ProcOEMString (void*, QVector<QString> &);
    bool ProcMemoryDevice (void*, QVector<QString> &);
    bool ProcMemoryArrayMappedAddress (void*, QVector<QString> &);
    bool ProcPortableBattery (void*, QVector<QString> &);
    int GetSMBInfo (int, QVector<QString> &);
};

#endif // SMBIOS_H
