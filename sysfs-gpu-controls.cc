#include <stdint.h>
#include <stdbool.h>
#include <dirent.h>
#include <stdio.h>
#include <string.h>
 #include <stdlib.h>

#include "sysfs-gpu-controls.h"
#define NULL 0



bool sysfs_hwcontrols = false;
bool opt_reorder = false;
int opt_hysteresis = 3;
int opt_targettemp = 80;
int opt_overheattemp = 90;

int sysfs_gpu_engineclock(int gpu) { }
int sysfs_gpu_memclock(int gpu) { }
float sysfs_gpu_vddc(int gpu) { }
int sysfs_gpu_activity(int gpu) { }
int sysfs_gpu_fanspeed(int gpu) { return(-1); }

int sysfs_set_vddc(int gpu, float fVddc) { }
int sysfs_set_engineclock(int gpu, int iEngineClock) { }
int sysfs_set_memoryclock(int gpu, int iMemoryClock) { }
int sysfs_set_powertune(int gpu, int iPercentage) { }

#ifndef __linux__

void GetGPUHWMonPath(char **HWMonPath, uint32_t GPUIdx)
{
  *HWMonPath = NULL;
}

#else

// Must be freed by caller unless NULL
void GetGPUHWMonPath(char **HWMonPath, uint32_t GPUIdx)
{
  DIR *hwmon;
  char TempPath[256];
  struct dirent *inner_hwmon;

  // Set it to NULL (indicating failure) until we need to use it
  *HWMonPath = NULL;

  sprintf(TempPath, "/sys/class/drm/card%d/device/hwmon", GPUIdx);

  hwmon = opendir(TempPath);
  if(hwmon == NULL)
    return;

  for(;;)
  {
    inner_hwmon = readdir(hwmon);
    if (inner_hwmon == NULL)
      return;
    if (inner_hwmon->d_type != DT_DIR)
      continue;
    if (!memcmp(inner_hwmon->d_name, "hwmon", 5))
      break;
  }

  *HWMonPath = (char *)malloc(sizeof(char) * (256 + strlen(inner_hwmon->d_name)));
  sprintf(*HWMonPath, "/sys/class/drm/card%d/device/hwmon/%s", GPUIdx, inner_hwmon->d_name);
  return;
}

#endif

int ReadSysFSFile(uint8_t *Buffer, char *Filename, uint32_t BufSize)
{
  FILE *fd;
  int BytesRead;

  fd = fopen(Filename, "rb");
  if(!fd)
    return -1;

  BytesRead = fread(Buffer, sizeof(uint8_t), BufSize, fd);

  fclose(fd);

  return BytesRead;
}

int WriteSysFSFile(uint8_t *Buffer, char *Filename, uint32_t BufSize)
{
  FILE *fd;
  int BytesWritten;

  fd = fopen(Filename, "wb");
  if(!fd)
    return -1;

  BytesWritten = fwrite(Buffer, sizeof(uint8_t), BufSize, fd);

  fclose(fd);

  return BytesWritten;
}

float sysfs_gpu_temp(int gpu)
{
  char TempFileName[513];
  char TempBuf[33];
  int BytesRead;

  snprintf(TempFileName, 512, "%s/temp1_input", gpusList[gpu].sysfs_info.HWMonPath);

  BytesRead = ReadSysFSFile((uint8_t *)TempBuf, TempFileName, 32);

  if (BytesRead <= 0)
    return 0.f;

  TempBuf[BytesRead] = 0x00;
  return strtof(TempBuf, NULL) / 1000.f;
}
float sysfs_gpu_power(int gpu)
{
	  char TempFileName[513];
	  char TempBuf[33];
	  int BytesRead;

	  snprintf(TempFileName, 512, "%s/power1_average", gpusList[gpu].sysfs_info.HWMonPath);

	  BytesRead = ReadSysFSFile((uint8_t *)TempBuf, TempFileName, 32);

	  if (BytesRead <= 0)
	    return 0.f;

	  TempBuf[BytesRead] = 0x00;
	  return strtof(TempBuf, NULL) / 1000000.f ;

}
int fanpercent_to_speed(int gpu, float percent)
{
  int min = gpusList[gpu].sysfs_info.MinFanSpeed;
  int max = gpusList[gpu].sysfs_info.MaxFanSpeed;

  float range = max - min;
  float res = percent / 100.f * range + min + 0.5f;
  return res;
}

float fanspeed_to_percent(int gpu, int speed)
{
  int min = gpusList[gpu].sysfs_info.MinFanSpeed;
  int max = gpusList[gpu].sysfs_info.MaxFanSpeed;

  float range = max - min;
  float res = (speed - min) / range * 100.f;
  return res;
}

float sysfs_gpu_fanpercent(int gpu)
{
  char PWM1FileName[513], SpeedStr[33];
  int BytesRead, Speed;

  snprintf(PWM1FileName, 512, "%s/pwm1", gpusList[gpu].sysfs_info.HWMonPath);

  BytesRead = ReadSysFSFile((uint8_t *)SpeedStr, PWM1FileName, 32);

  if(BytesRead <= 0)
    return -1;

  SpeedStr[BytesRead] = 0x00;

  Speed = strtoul(SpeedStr, NULL, 10);

  gpusList[gpu].sysfs_info.LastFanSpeed = Speed;
  return fanspeed_to_percent(gpu, Speed);
}

int sysfs_set_fanspeed(int gpu, float FanSpeed)
{
  char PWM1FileName[513], Setting[33];
  int speed = fanpercent_to_speed(gpu, FanSpeed);

  gpusList[gpu].sysfs_info.TgtFanSpeed = FanSpeed;

  snprintf(PWM1FileName, 512, "%s/pwm1", gpusList[gpu].sysfs_info.HWMonPath);
  snprintf(Setting, 32, "%d", speed);

  return (WriteSysFSFile((uint8_t *)Setting, PWM1FileName, strlen(Setting)) > 0 ? 0 : -1);
}

bool sysfs_gpu_stats(int gpu, float *temp,float *power, int *engineclock, int *memclock, float *vddc,
         int *activity, int *fanspeed, int *fanpercent, int *powertune)
{
  if (!gpusList[gpu].sysfs_hwcontrols)
    return false;

  *temp = sysfs_gpu_temp(gpu);
  *power = sysfs_gpu_power(gpu);
  *fanpercent = sysfs_gpu_fanpercent(gpu);
  *fanspeed = gpusList[gpu].sysfs_info.LastFanSpeed;
  *engineclock = 0;
  *memclock = 0;
  *vddc = 0;
  *activity = 0;

  return true;
}
bool sysfs_gpu_stats(int gpu, float *temp,float *power,int *fanspeed, int *fanpercent)
{
	  if (!gpusList[gpu].sysfs_hwcontrols)
	    return false;

	  *temp = sysfs_gpu_temp(gpu);
	  *power = sysfs_gpu_power(gpu);
	  *fanpercent = sysfs_gpu_fanpercent(gpu);
	  *fanspeed = gpusList[gpu].sysfs_info.LastFanSpeed;
	  return true;

}



bool init_sysfs_hwcontrols(int nDevs)
{
  for(int i = 0; i < nDevs; ++i)
  {
    // Set gpusList[i].sysfs_info.HWMonPath
    GetGPUHWMonPath(&gpusList[i].sysfs_info.HWMonPath, i);

    if(gpusList[i].sysfs_info.HWMonPath)
    {
      char PWMSettingBuf[33];
      char FileNameBuf[513];
      int BytesRead;
      bool MinFanReadSuccess = false;

      snprintf(FileNameBuf, 512, "%s/pwm1_min", gpusList[i].sysfs_info.HWMonPath);
      BytesRead = ReadSysFSFile((uint8_t *)PWMSettingBuf, FileNameBuf, 512);

      if(BytesRead > 0)
      {
        PWMSettingBuf[BytesRead] = 0x00;
        gpusList[i].sysfs_info.MinFanSpeed = strtoul(PWMSettingBuf, NULL, 10);
        MinFanReadSuccess = true;
      }

      snprintf(FileNameBuf, 512, "%s/pwm1_max", gpusList[i].sysfs_info.HWMonPath);
      BytesRead = ReadSysFSFile((uint8_t *)PWMSettingBuf, FileNameBuf, 512);

      if((BytesRead > 0) && MinFanReadSuccess)
      {
        gpusList[i].sysfs_hwcontrols = true;
        PWMSettingBuf[BytesRead] = 0x00;
        gpusList[i].sysfs_info.MaxFanSpeed = strtoul(PWMSettingBuf, NULL, 10);
      }

      gpusList[i].sysfs_info.LastTemp = sysfs_gpu_temp(i);
      if(!gpusList[i].sysfs_info.OverHeatTemp) gpusList[i].sysfs_info.OverHeatTemp = opt_overheattemp;
      if(!gpusList[i].sysfs_info.TargetTemp) gpusList[i].sysfs_info.TargetTemp = opt_targettemp;

      gpusList[i].sysfs_info.LastPower = sysfs_gpu_power(i);

    }

    if(gpusList[i].sysfs_hwcontrols) sysfs_hwcontrols = true;
  }

  if(sysfs_hwcontrols)
  {
//    gpu_temp = &sysfs_gpu_temp;
//    gpu_engineclock = &sysfs_gpu_engineclock;
//    gpu_memclock = &sysfs_gpu_memclock;
//    gpu_vddc = &sysfs_gpu_vddc;
//    gpu_activity = &sysfs_gpu_activity;
//    gpu_fanspeed = &sysfs_gpu_fanspeed;
//    gpu_fanpercent = &sysfs_gpu_fanpercent;
//    set_powertune = &sysfs_set_powertune;
//    set_vddc = &sysfs_set_vddc;
//    set_fanspeed = &sysfs_set_fanspeed;
//    set_engineclock = &sysfs_set_engineclock;
//    set_memoryclock = &sysfs_set_memoryclock;
//    gpu_stats = &sysfs_gpu_stats;
//    gpu_autotune = &sysfs_gpu_autotune;
  }

  return(sysfs_hwcontrols);
}



























