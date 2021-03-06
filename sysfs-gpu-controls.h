#ifndef __SYSFS_GPU_CONTROLS_H
#define __SYSFS_GPU_CONTROLS_H

#include <stdbool.h>
#ifdef __unix__
#include <sys/time.h>
#include "OCLdecs.h"
#else
#include <time.h>
#endif
#include <stdint.h>
#include "adl.h"
typedef struct _gpu_sysfs_info
{
  char *HWMonPath;
  uint32_t MinFanSpeed;
  uint32_t MaxFanSpeed;
  uint32_t OverHeatTemp;
  uint32_t TargetTemp;
  float TgtFanSpeed;
  int LastFanSpeed;
  float LastTemp;
  float LastPower;
  int LastFanPercent;
  uint8_t pcie_index[3];
} gpu_sysfs_info;

enum algorithm_t {
  ALG1,
  ALG2,
};

struct cgpu_info {
  char *name;  /* GPU family codename. */
  char *device_path;
  void *device_data;

  bool sysfs_hwcontrols;
  bool adl_hwcontrols;
  ADLInfo adl_info;
  gpu_sysfs_info sysfs_info;
};

#define MAX_GPUDEVICES 16
#define MAX_DEVICES 4096

extern bool has_sysfs_hwcontrols;
extern bool opt_reorder;
extern int opt_hysteresis;
extern int opt_targettemp;
extern int opt_overheattemp;
static cgpu_info gpusList[MAX_GPUDEVICES];
//
//float (*gpu_temp)(int);
//int (*gpu_engineclock)(int);
//int (*gpu_memclock)(int);
//float (*gpu_vddc)(int);
//int (*gpu_activity)(int);
//int (*gpu_fanspeed)(int);
//float (*gpu_fanpercent)(int);
//int (*set_powertune)(int, int);
//int (*set_fanspeed)(int, float);
//int (*set_vddc)(int, float);
//int (*set_engineclock)(int, int);
//int (*set_memoryclock)(int, int);
//bool (*gpu_stats)(int, float *, int *, int *, float *, int *, int *, int *, int *);
//void (*gpu_autotune) (int, enum dev_enable *);

bool init_sysfs_hwcontrols(int nDevs);
float sysfs_gpu_temp(int gpu);
float sysfs_gpu_power(int gpu);
int sysfs_gpu_engineclock(int gpu);
int sysfs_gpu_memclock(int gpu);
float sysfs_gpu_vddc(int gpu);
int sysfs_gpu_activity(int gpu);
int sysfs_gpu_fanspeed(int gpu);
float sysfs_gpu_fanpercent(int gpu);
int sysfs_set_powertune(int gpu, int iPercentage);

bool sysfs_gpu_stats(int gpu, float *temp,float *power, int *engineclock, int *memclock, float *vddc,
               int *activity, int *fanspeed, int *fanpercent, int *powertune);
bool sysfs_gpu_stats(int gpu, float *temp,float *power,int *fanspeed, int *fanpercent);


#endif
