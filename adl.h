
#ifndef _ADL_
#define _ADL_

#include <stdio.h>

#if defined (__unix__)
# include <dlfcn.h>
# include <stdlib.h>
# include <unistd.h>
#else /* WIN32 */
# include <windows.h>
# include <tchar.h>
#endif
#include "adl-sdk/adl_sdk.h"
#include "adl-sdk/adl_structures.h"
#include "adl-sdk/adl_functaions.h"


// Comment out one of the two lines below to allow or supress diagnostic messages
// #define PRINTF
#define PRINTF printf

#if defined (__unix__)
  static void *hDLL;  // Handle to .so library
#else
  static  HINSTANCE hDLL;   // Handle to DLL
#endif

static ADL_MAIN_CONTROL_CREATE          ADL_Main_Control_Create = NULL;
static ADL_MAIN_CONTROL_DESTROY         ADL_Main_Control_Destroy = NULL;
static ADL_ADAPTER_NUMBEROFADAPTERS_GET ADL_Adapter_NumberOfAdapters_Get = NULL;
static ADL_ADAPTER_ADAPTERINFO_GET      ADL_Adapter_AdapterInfo_Get = NULL;

static ADL_ADAPTER_ID_GET      ADL_Adapter_ID_Get = NULL;
static ADL_MAIN_CONTROL_REFRESH    ADL_Main_Control_Refresh = NULL;
static ADL_ADAPTER_VIDEOBIOSINFO_GET   ADL_Adapter_VideoBiosInfo_Get = NULL;
static ADL_DISPLAY_DISPLAYINFO_GET   ADL_Display_DisplayInfo_Get = NULL;
static ADL_ADAPTER_ACCESSIBILITY_GET   ADL_Adapter_Accessibility_Get = NULL;

static ADL_OVERDRIVE_CAPS      ADL_Overdrive_Caps = NULL;


static ADL_ADAPTERX2_CAPS ADL_AdapterX2_Caps = NULL;
static ADL2_ADAPTER_ACTIVE_GET				ADL2_Adapter_Active_Get = NULL;
static ADL2_OVERDRIVEN_CAPABILITIESX2_GET ADL2_OverdriveN_CapabilitiesX2_Get = NULL;
static ADL2_OVERDRIVEN_SYSTEMCLOCKSX2_GET ADL2_OverdriveN_SystemClocksX2_Get = NULL;
static ADL2_OVERDRIVEN_SYSTEMCLOCKSX2_SET ADL2_OverdriveN_SystemClocksX2_Set = NULL;
static ADL2_OVERDRIVEN_PERFORMANCESTATUS_GET ADL2_OverdriveN_PerformanceStatus_Get = NULL;
static ADL2_OVERDRIVEN_FANCONTROL_GET ADL2_OverdriveN_FanControl_Get = NULL;
static ADL2_OVERDRIVEN_FANCONTROL_SET ADL2_OverdriveN_FanControl_Set = NULL;
static ADL2_OVERDRIVEN_POWERLIMIT_GET ADL2_OverdriveN_PowerLimit_Get = NULL;
static ADL2_OVERDRIVEN_POWERLIMIT_SET ADL2_OverdriveN_PowerLimit_Set = NULL;
static ADL2_OVERDRIVEN_MEMORYCLOCKSX2_GET ADL2_OverdriveN_MemoryClocksX2_Get = NULL;
static ADL2_OVERDRIVEN_MEMORYCLOCKSX2_SET ADL2_OverdriveN_MemoryClocksX2_Set = NULL;
static ADL2_OVERDRIVE_CAPS ADL2_Overdrive_Caps = NULL;
static ADL2_OVERDRIVEN_TEMPERATURE_GET ADL2_OverdriveN_Temperature_Get = NULL;

static ADL2_OVERDRIVEN_MEMORYTIMINGLEVEL_GET ADL2_OverdriveN_MemoryTimingLevel_Get = NULL;
static ADL2_OVERDRIVEN_MEMORYTIMINGLEVEL_SET ADL2_OverdriveN_MemoryTimingLevel_Set = NULL;
static ADL2_OVERDRIVEN_ZERORPMFAN_GET ADL2_OverdriveN_ZeroRPMFan_Get = NULL;
static ADL2_OVERDRIVEN_ZERORPMFAN_SET ADL2_OverdriveN_ZeroRPMFan_Set = NULL;

static ADL2_OVERDRIVEN_SETTINGSEXT_GET ADL2_OverdriveN_SettingsExt_Get = NULL;
static ADL2_OVERDRIVEN_SETTINGSEXT_SET ADL2_OverdriveN_SettingsExt_Set = NULL;

// Memory allocation function
static void* __stdcall ADL_Main_Memory_Alloc(int iSize)
{
    void* lpBuffer = malloc(iSize);
    return lpBuffer;
}

// Optional Memory de-allocation function
static void __stdcall ADL_Main_Memory_Free(void** lpBuffer)
{
    if (NULL != *lpBuffer)
    {
        free(*lpBuffer);
        *lpBuffer = NULL;
    }
}

#if defined (__unix__)
// equivalent functions in linux
static void *GetProcAddress(void *pLibrary, const char *name)
{
  return dlsym( pLibrary, name);
}
#endif

static ADL_CONTEXT_HANDLE context = NULL;

static LPAdapterInfo   lpAdapterInfo = NULL;
static ADLInfo *pGPU_Info;
static int  iNumberAdapters;


static int initializeADL()
{

	  int result;

	#if defined (__unix__)
	  hDLL = dlopen( "libatiadlxx.so", RTLD_LAZY|RTLD_GLOBAL);
	#else
	  hDLL = LoadLibrary("atiadlxx.dll");
	  if (hDLL == NULL)
	    // A 32 bit calling application on 64 bit OS will fail to LoadLIbrary.
	    // Try to load the 32 bit library (atiadlxy.dll) instead
	    hDLL = LoadLibrary("atiadlxy.dll");
	#endif
	  if (hDLL == NULL) {
	    printf("\n Unable to load ATI ADL library. \n ");
	    return false;
	  }
    ADL_Main_Control_Create = (ADL_MAIN_CONTROL_CREATE)GetProcAddress(hDLL, "ADL_Main_Control_Create");
    ADL_Main_Control_Destroy = (ADL_MAIN_CONTROL_DESTROY)GetProcAddress(hDLL, "ADL_Main_Control_Destroy");
    ADL_Adapter_NumberOfAdapters_Get = (ADL_ADAPTER_NUMBEROFADAPTERS_GET)GetProcAddress(hDLL, "ADL_Adapter_NumberOfAdapters_Get");
    ADL_Adapter_AdapterInfo_Get = (ADL_ADAPTER_ADAPTERINFO_GET)GetProcAddress(hDLL, "ADL_Adapter_AdapterInfo_Get");

	ADL_Display_DisplayInfo_Get = (ADL_DISPLAY_DISPLAYINFO_GET)GetProcAddress(hDLL, "ADL_Display_DisplayInfo_Get");
	ADL_Adapter_ID_Get = (ADL_ADAPTER_ID_GET)GetProcAddress(hDLL, "ADL_Adapter_ID_Get");
	ADL_Main_Control_Refresh = (ADL_MAIN_CONTROL_REFRESH)GetProcAddress(hDLL, "ADL_Main_Control_Refresh");
	ADL_Adapter_VideoBiosInfo_Get = (ADL_ADAPTER_VIDEOBIOSINFO_GET)GetProcAddress(hDLL, "ADL_Adapter_VideoBiosInfo_Get");
	ADL_Overdrive_Caps = (ADL_OVERDRIVE_CAPS)GetProcAddress(hDLL, "ADL_Overdrive_Caps");


    ADL_AdapterX2_Caps = (ADL_ADAPTERX2_CAPS)GetProcAddress(hDLL, "ADL_AdapterX2_Caps");
    ADL2_Adapter_Active_Get = (ADL2_ADAPTER_ACTIVE_GET)GetProcAddress(hDLL, "ADL2_Adapter_Active_Get");
    ADL2_OverdriveN_CapabilitiesX2_Get = (ADL2_OVERDRIVEN_CAPABILITIESX2_GET)GetProcAddress(hDLL, "ADL2_OverdriveN_CapabilitiesX2_Get");
    ADL2_OverdriveN_SystemClocksX2_Get = (ADL2_OVERDRIVEN_SYSTEMCLOCKSX2_GET)GetProcAddress(hDLL, "ADL2_OverdriveN_SystemClocksX2_Get");
    ADL2_OverdriveN_SystemClocksX2_Set = (ADL2_OVERDRIVEN_SYSTEMCLOCKSX2_SET)GetProcAddress(hDLL, "ADL2_OverdriveN_SystemClocksX2_Set");
    ADL2_OverdriveN_MemoryClocksX2_Get = (ADL2_OVERDRIVEN_MEMORYCLOCKSX2_GET)GetProcAddress(hDLL, "ADL2_OverdriveN_MemoryClocksX2_Get");
    ADL2_OverdriveN_MemoryClocksX2_Set = (ADL2_OVERDRIVEN_MEMORYCLOCKSX2_SET)GetProcAddress(hDLL, "ADL2_OverdriveN_MemoryClocksX2_Set");
    ADL2_OverdriveN_PerformanceStatus_Get = (ADL2_OVERDRIVEN_PERFORMANCESTATUS_GET)GetProcAddress(hDLL, "ADL2_OverdriveN_PerformanceStatus_Get");
    ADL2_OverdriveN_FanControl_Get = (ADL2_OVERDRIVEN_FANCONTROL_GET)GetProcAddress(hDLL, "ADL2_OverdriveN_FanControl_Get");
    ADL2_OverdriveN_FanControl_Set = (ADL2_OVERDRIVEN_FANCONTROL_SET)GetProcAddress(hDLL, "ADL2_OverdriveN_FanControl_Set");
    ADL2_OverdriveN_PowerLimit_Get = (ADL2_OVERDRIVEN_POWERLIMIT_GET)GetProcAddress(hDLL, "ADL2_OverdriveN_PowerLimit_Get");
    ADL2_OverdriveN_PowerLimit_Set = (ADL2_OVERDRIVEN_POWERLIMIT_SET)GetProcAddress(hDLL, "ADL2_OverdriveN_PowerLimit_Set");
    ADL2_OverdriveN_Temperature_Get = (ADL2_OVERDRIVEN_TEMPERATURE_GET)GetProcAddress(hDLL, "ADL2_OverdriveN_Temperature_Get");
    ADL2_Overdrive_Caps = (ADL2_OVERDRIVE_CAPS)GetProcAddress(hDLL, "ADL2_Overdrive_Caps");

    ADL2_OverdriveN_MemoryTimingLevel_Get = (ADL2_OVERDRIVEN_MEMORYTIMINGLEVEL_GET)GetProcAddress(hDLL, "ADL2_OverdriveN_MemoryTimingLevel_Get");
    ADL2_OverdriveN_MemoryTimingLevel_Set = (ADL2_OVERDRIVEN_MEMORYTIMINGLEVEL_SET)GetProcAddress(hDLL, "ADL2_OverdriveN_MemoryTimingLevel_Set");
    ADL2_OverdriveN_ZeroRPMFan_Get = (ADL2_OVERDRIVEN_ZERORPMFAN_GET)GetProcAddress(hDLL, "ADL2_OverdriveN_ZeroRPMFan_Get");
    ADL2_OverdriveN_ZeroRPMFan_Set = (ADL2_OVERDRIVEN_ZERORPMFAN_SET)GetProcAddress(hDLL, "ADL2_OverdriveN_ZeroRPMFan_Set");

    ADL2_OverdriveN_SettingsExt_Get = (ADL2_OVERDRIVEN_SETTINGSEXT_GET)GetProcAddress(hDLL, "ADL2_OverdriveN_SettingsExt_Get");
    ADL2_OverdriveN_SettingsExt_Set = (ADL2_OVERDRIVEN_SETTINGSEXT_SET)GetProcAddress(hDLL, "ADL2_OverdriveN_SettingsExt_Set");

    if (NULL == ADL_Main_Control_Create ||
        NULL == ADL_Main_Control_Destroy ||
        NULL == ADL_Adapter_NumberOfAdapters_Get ||
        NULL == ADL_Adapter_AdapterInfo_Get ||
        NULL == ADL_AdapterX2_Caps ||
        NULL == ADL2_Adapter_Active_Get ||
        NULL == ADL2_OverdriveN_CapabilitiesX2_Get ||
        NULL == ADL2_OverdriveN_SystemClocksX2_Get ||
        NULL == ADL2_OverdriveN_SystemClocksX2_Set ||
        NULL == ADL2_OverdriveN_MemoryClocksX2_Get ||
        NULL == ADL2_OverdriveN_MemoryClocksX2_Set ||
        NULL == ADL2_OverdriveN_PerformanceStatus_Get ||
        NULL == ADL2_OverdriveN_FanControl_Get ||
        NULL == ADL2_OverdriveN_FanControl_Set ||
        NULL == ADL2_Overdrive_Caps ||
        NULL == ADL2_OverdriveN_MemoryTimingLevel_Get ||
        NULL == ADL2_OverdriveN_MemoryTimingLevel_Set ||
        NULL == ADL2_OverdriveN_ZeroRPMFan_Get ||
        NULL == ADL2_OverdriveN_ZeroRPMFan_Set ||
        NULL == ADL2_OverdriveN_SettingsExt_Get ||
        NULL == ADL2_OverdriveN_SettingsExt_Set
        )
    {
        PRINTF("Failed to get ADL function pointers\n");
        return false;
    }

    if (ADL_OK != ADL_Main_Control_Create(ADL_Main_Memory_Alloc, 1))
    {
        printf("Failed to initialize nested ADL2 context");
        return ADL_ERR;
    }

    return true;
}

static void deinitializeADL()
{
	ADL_Main_Memory_Free((void**)&lpAdapterInfo);

    ADL_Main_Control_Destroy();

#if defined (__unix__)
  dlclose(hDLL);
#else
  FreeLibrary(hDLL);
#endif

}


static int printODNFANParameters(ADLODNFanControl *odNFanControl,int *percent,int i = 0)
{
    int active = 0;;
    int iSupported, iEnabled, iVersion;

        if (lpAdapterInfo[i].iBusNumber > -1)
        {
            ADL2_Overdrive_Caps(context, lpAdapterInfo[i].iAdapterIndex, &iSupported, &iEnabled, &iVersion);
            if (iVersion == 7)
            {
				ADLODNCapabilitiesX2 overdriveCapabilities;
				memset(&overdriveCapabilities, 0, sizeof(ADLODNCapabilitiesX2));

				if (ADL_OK != ADL2_OverdriveN_CapabilitiesX2_Get(context, lpAdapterInfo[i].iAdapterIndex, &overdriveCapabilities))
				{
					PRINTF("ADL2_OverdriveN_Capabilities_Get is failed\n");
				}
				float range = overdriveCapabilities.fanSpeed.iMax - overdriveCapabilities.fanSpeed.iMin;

                memset(odNFanControl, 0, sizeof(ADLODNFanControl));

                if (ADL_OK != ADL2_OverdriveN_FanControl_Get(context, lpAdapterInfo[i].iAdapterIndex, odNFanControl))
                {
                    PRINTF("ADL2_OverdriveN_FanControl_Get is failed\n"); 
                }
                else
                {
					
					*percent = (int )((float)(odNFanControl->iCurrentFanSpeed  ) / overdriveCapabilities.fanSpeed.iMax * 100.f);
                    //PRINTF("---------Fan Current Speed--------------\n");
                    //PRINTF("odNFanControl.iCurrentFanSpeed : %d\n", odNFanControl->iCurrentFanSpeed);
                    //PRINTF("odNFanControl.iCurrentFanSpeedMode : %d\n", odNFanControl->iCurrentFanSpeedMode);
                    //PRINTF("-----------------------------------------\n");

                }
            }
        }


    return 0;
}
static int printODNTEMPParameters(int *temp,int i=0)
{

    int   active = 0;;
    int iSupported, iEnabled, iVersion;
    int ret = 0;
        if (lpAdapterInfo[i].iBusNumber > -1)
        {
            ADL2_Overdrive_Caps(context, lpAdapterInfo[i].iAdapterIndex, &iSupported, &iEnabled, &iVersion);
            if (iVersion == 7)
            {
                    ADL2_OverdriveN_Temperature_Get(context, lpAdapterInfo[i].iAdapterIndex, 1, temp);
                    //PRINTF("-------------------------------------------------\n");
                    //PRINTF("Current temperature : %d\n", *temp);
        }
    }

    return ret;
}
static int printODNZERORPMParameters(int i=0)
{
    int   active = 0;;
    int iSupported, iEnabled, iVersion;
    int ret = 0;
        if (lpAdapterInfo[i].iBusNumber > -1)
        {
            ADL2_Overdrive_Caps(context, lpAdapterInfo[i].iAdapterIndex, &iSupported, &iEnabled, &iVersion);
            if (iVersion == 7)
            {
                int support, currentValue, defaultValue;
                if (ADL_OK != ADL2_OverdriveN_ZeroRPMFan_Get(context, lpAdapterInfo[i].iAdapterIndex, &support, &currentValue, &defaultValue))
                {
                    PRINTF("ADL2_OverdriveN_ZeroRPMFan_Get is failed\n");
                }
                else
                {
                    //PRINTF("ADL2_OverdriveN_ZeroRPMFan_Get Data\n");
                    //PRINTF("-------------------------------------------------\n");
                    //PRINTF("Zero PRM Fan support : %d\n", support);
                    //PRINTF("Current value : %d\n", currentValue);
                    //PRINTF("Default value : %d\n", defaultValue);
                }
        }
    }

    return ret;
}

#include <vector>
static std::vector<ADLInfo> GPU_Lists;

static int initializeAdapters()
{

	//	Sleep(30000);


	if (initializeADL())
	{
		// Obtain the number of adapters for the system
		if (ADL_OK != ADL_Adapter_NumberOfAdapters_Get(&iNumberAdapters))
		{
			PRINTF("Cannot get the number of adapters!\n");
			return 0;
		}

		if (0 < iNumberAdapters)
		{
			lpAdapterInfo = (LPAdapterInfo)malloc(sizeof(AdapterInfo)* iNumberAdapters);
			memset(lpAdapterInfo, '\0', sizeof(AdapterInfo)* iNumberAdapters);

			pGPU_Info = (ADLInfo *)malloc(sizeof(ADLInfo)* iNumberAdapters);
			memset(pGPU_Info, '\0', sizeof(ADLInfo)* iNumberAdapters);

			// Get the AdapterInfo structure for all adapters in the system
			ADL_Adapter_AdapterInfo_Get(lpAdapterInfo, sizeof(AdapterInfo)* iNumberAdapters);
		}


		ADLBiosInfo BiosInfo;
		int last_adapter = -1;
		int i, j, devices = 0;
		/* Iterate over iNumberAdapters and find the lpAdapterID of real devices */
		for (i = 0; i < iNumberAdapters; i++) {
			int iAdapterIndex;
			int lpAdapterID;
			devices++;
			iAdapterIndex = lpAdapterInfo[i].iAdapterIndex;

			/* Get unique identifier of the adapter, 0 means not AMD */
			int result = ADL_Adapter_ID_Get(iAdapterIndex, &lpAdapterID);

			/* Each adapter may have multiple entries */
			if (lpAdapterID == last_adapter) {
				continue;
			}


			if (ADL_Adapter_VideoBiosInfo_Get(iAdapterIndex, &BiosInfo) == ADL_ERR) {
				printf("\n ADL index %d, id %d - FAILED to get BIOS info", iAdapterIndex, lpAdapterID);
			}
			else {
				//printf("\n ADL index %d, id %d - BIOS partno.: %s, version: %s, date: %s", iAdapterIndex, lpAdapterID, BiosInfo.strPartNumber, BiosInfo.strVersion, BiosInfo.strDate);
			}

			if (result != ADL_OK) {
				printf("\n Failed to ADL_Adapter_ID_Get. Error %d", result);
				if (result == -10)
					printf("\n (Device is not enabled.)");
				continue;
			}


			//printf("\n GPU %d assigned: "
			//	"iAdapterIndex:%d "
			//	"iPresent:%d "
			//	"strUDID:%s "
			//	"iBusNumber:%d "
			//	"iDeviceNumber:%d "
			//	"iFunctionNumber:%d "
			//	"iVendorID:%d "
			//	"name:%s",
			//	devices,
			//	lpAdapterInfo[i].iAdapterIndex,
			//	lpAdapterInfo[i].iPresent,
			//	lpAdapterInfo[i].strUDID,
			//	lpAdapterInfo[i].iBusNumber,
			//	lpAdapterInfo[i].iDeviceNumber,
			//	lpAdapterInfo[i].iFunctionNumber,
			//	lpAdapterInfo[i].iVendorID,
			//	lpAdapterInfo[i].strAdapterName);

			//	adapters[devices].iAdapterIndex = iAdapterIndex;
			//	adapters[devices].iBusNumber = lpAdapterInfo[i].iBusNumber;
			//	adapters[devices].id = i;


			last_adapter = lpAdapterID;

			if (!lpAdapterID) {
				printf("\n Adapter returns ID 0 meaning not AMD. Card order might be confused");
				continue;
			}

			memcpy(&pGPU_Info[i].ADL_AapterInfo, &lpAdapterInfo[i], sizeof(AdapterInfo));
			pGPU_Info[i].ADLODN_Temperature = -1;
			memset(&pGPU_Info[i].ADLODN_FanControl, 0, sizeof(ADLODNFanControl));
			pGPU_Info[i].GPU_Index = -1;
			GPU_Lists.insert(GPU_Lists.begin(), pGPU_Info[i]);



		}

	}


	return 0;
	
}
static int GetAdlInfo()
{
	
	size_t length = GPU_Lists.size();
	for (size_t i = 0; i < length; i++)
	{
		//printf("\n \n ");
		ADLODNFanControl odNFanControl;
		int FanPercent;
		printODNFANParameters(&GPU_Lists[i].ADLODN_FanControl,&GPU_Lists[i].ADLODN_FanPercent, i);
		//printf("\n \n ");
		int temp;
		printODNTEMPParameters(&GPU_Lists[i].ADLODN_Temperature, i);
		GPU_Lists[i].ADLODN_Temperature /= 1000;
		//printf("\n \n ");
		//printODNZERORPMParameters(i);
	}
	return 0;
}
#endif
