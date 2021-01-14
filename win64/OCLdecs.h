#pragma once

#ifndef OCLDECS_H
#define OCLDECS_H

typedef ulong        cl_ulong;
typedef uint        cl_uint;

#include "OCLdefs.h"////problem with relative path --


#define  fn_mulHi(Val1, Val2, cv, Result,ret)  \
do  \
{  \
	cl_ulong tmp = (cl_ulong)Val1 * (cl_ulong)Val2 + (cl_ulong)+cv;\
	Result = tmp >> 32;\
	ret = 0;\
}\
while(0)


#define fn_mulLow(Val1, Val2, Result,ret)                                    \  
do                                                                           \
{                                                                            \
cl_ulong tmp = (cl_ulong)Val1 * (cl_ulong)Val2; \
Result = tmp; \
ret = tmp >> 32; \
}                                                                            \
while (0)

#define fn_Add(Val1, Val2, cv, Result,ret) \
do \
{  \
	cl_ulong tmp = (cl_ulong)Val1 + (cl_ulong)Val2 + (cl_ulong)cv; \
	Result = tmp; \
	ret = tmp >> 32;\
}  \
while(0)



#define fn_Sub(Val1, Val2, cv, Result,ret) \
do \
{  \
	cl_uint newcv = 0; \
	if (Val1 < Val2 && Val2 - Val1>cv)newcv = 1; \
	Result = Val1 - Val2 - cv; \
	ret = newcv; \
} \
while(0)

#define fn_MadLo(Val1, Val2, cv, Result,ret) \
do \
{ \
	cl_ulong tmp = Result; \
	/*r[1]*/tmp += (cl_ulong)(Val1*Val2) + cv; \
	Result = tmp; \
	tmp = tmp >> 32; \
	ret = tmp; \
} \
while(0)




#define fn_MadHi(Val1, Val2, cv, Result,ret)  \
do \
{ \
	cl_ulong tmp = Result; \
	tmp += (((cl_ulong)Val1 * (cl_ulong)Val2) >> 32) + cv; \
	Result = tmp; \
	ret= tmp >> 32; \
} \
while(0)\

#endif //
