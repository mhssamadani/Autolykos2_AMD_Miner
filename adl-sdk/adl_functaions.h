
#if defined (__unix__)
#	include <dlfcn.h>	//dyopen, dlsym, dlclose
#	include <stdlib.h>
#	include <string.h>	//memeset
#else
#	include <windows.h>
#	include <tchar.h>
#endif

#include "adl_sdk.h"


// Definitions of the used function pointers. Add more if you use other ADL APIs
typedef int(*ADL_MAIN_CONTROL_CREATE)(ADL_MAIN_MALLOC_CALLBACK, int);
typedef int(*ADL_MAIN_CONTROL_DESTROY)();
typedef int(*ADL_FLUSH_DRIVER_DATA)(int);
typedef int(*ADL2_ADAPTER_ACTIVE_GET) (ADL_CONTEXT_HANDLE, int, int*);

typedef int(*ADL_ADAPTER_ID_GET) (int iAdapterIndex, int *lpAdapterID);
typedef int(*ADL_MAIN_CONTROL_REFRESH) ();
typedef int(*ADL_ADAPTER_VIDEOBIOSINFO_GET) (int iAdapterIndex, ADLBiosInfo *lpBiosInfo);
typedef int(*ADL_DISPLAY_DISPLAYINFO_GET) (int iAdapterIndex, int *lpNumDisplays, ADLDisplayInfo **lppInfo, int iForceDetect);
typedef int(*ADL_ADAPTER_ACCESSIBILITY_GET) (int iAdapterIndex, int *lpAccessibility);

typedef int(*ADL_OVERDRIVE_CAPS) (int iAdapterIndex, int *iSupported, int *iEnabled, int *iVersion);


typedef int(*ADL_ADAPTER_NUMBEROFADAPTERS_GET) (int*);
typedef int(*ADL_ADAPTER_ADAPTERINFO_GET) (LPAdapterInfo, int);
typedef int(*ADL_ADAPTERX2_CAPS) (int, int*);
typedef int(*ADL2_OVERDRIVE_CAPS) (ADL_CONTEXT_HANDLE context, int iAdapterIndex, int * iSupported, int * iEnabled, int * iVersion);
typedef int(*ADL2_OVERDRIVEN_CAPABILITIESX2_GET)	(ADL_CONTEXT_HANDLE, int, ADLODNCapabilitiesX2*);
typedef int(*ADL2_OVERDRIVEN_PERFORMANCESTATUS_GET) (ADL_CONTEXT_HANDLE, int, ADLODNPerformanceStatus*);
typedef int(*ADL2_OVERDRIVEN_FANCONTROL_GET) (ADL_CONTEXT_HANDLE, int, ADLODNFanControl*);
typedef int(*ADL2_OVERDRIVEN_FANCONTROL_SET) (ADL_CONTEXT_HANDLE, int, ADLODNFanControl*);
typedef int(*ADL2_OVERDRIVEN_POWERLIMIT_GET) (ADL_CONTEXT_HANDLE, int, ADLODNPowerLimitSetting*);
typedef int(*ADL2_OVERDRIVEN_POWERLIMIT_SET) (ADL_CONTEXT_HANDLE, int, ADLODNPowerLimitSetting*);
typedef int(*ADL2_OVERDRIVEN_TEMPERATURE_GET) (ADL_CONTEXT_HANDLE, int, int, int*);
typedef int(*ADL2_OVERDRIVEN_SYSTEMCLOCKSX2_GET)	(ADL_CONTEXT_HANDLE, int, ADLODNPerformanceLevelsX2*);
typedef int(*ADL2_OVERDRIVEN_SYSTEMCLOCKSX2_SET)	(ADL_CONTEXT_HANDLE, int, ADLODNPerformanceLevelsX2*);
typedef int(*ADL2_OVERDRIVEN_MEMORYCLOCKSX2_GET)	(ADL_CONTEXT_HANDLE, int, ADLODNPerformanceLevelsX2*);
typedef int(*ADL2_OVERDRIVEN_MEMORYCLOCKSX2_SET)	(ADL_CONTEXT_HANDLE, int, ADLODNPerformanceLevelsX2*);

typedef int(*ADL2_OVERDRIVEN_MEMORYTIMINGLEVEL_GET) (ADL_CONTEXT_HANDLE context, int iAdapterIndex, int *lpSupport, int *lpCurrentValue, int *lpDefaultValue, int *lpNumberLevels, int **lppLevelList);
typedef int(*ADL2_OVERDRIVEN_MEMORYTIMINGLEVEL_SET) (ADL_CONTEXT_HANDLE context, int iAdapterIndex, int currentValue);
typedef int(*ADL2_OVERDRIVEN_ZERORPMFAN_GET) (ADL_CONTEXT_HANDLE context, int iAdapterIndex, int *lpSupport, int *lpCurrentValue, int *lpDefaultValue);
typedef int(*ADL2_OVERDRIVEN_ZERORPMFAN_SET) (ADL_CONTEXT_HANDLE context, int iAdapterIndex, int currentValue);

typedef int(*ADL2_OVERDRIVEN_SETTINGSEXT_GET) (ADL_CONTEXT_HANDLE context, int iAdapterIndex, int* lpOverdriveNExtCapabilities, int *lpNumberOfODNExtFeatures, ADLODNExtSingleInitSetting** lppInitSettingList, int** lppCurrentSettingList);
typedef int(*ADL2_OVERDRIVEN_SETTINGSEXT_SET) (ADL_CONTEXT_HANDLE context, int iAdapterIndex, int iNumberOfODNExtFeatures, int* itemValueValidList, int* lpItemValueList);
