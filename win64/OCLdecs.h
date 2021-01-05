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

//
//void fn_DEVICE_B2B_H(ctx_t * ctx, cl_ulong *aux)
//{
//	cl_uint CV = 0;																
//		CV = fn_Add((cl_uint)(ctx->t[0]), 128, CV, (cl_uint *)(ctx->t[0]));			
//		CV = fn_Add((cl_uint)(ctx->t[1]), 0, CV, (cl_uint *)(ctx->t[1]));              
//		CV = fn_Add((cl_uint)(ctx->t[2]), 0, CV, (cl_uint *)(ctx->t[2]));              
//		CV = fn_Add((cl_uint)(ctx->t[3]), 0, CV, (cl_uint *)(ctx->t[3]));            
//		B2B_INIT(ctx, aux);                                                        
//		B2B_FINAL(ctx, aux);                                                       
//		((ctx_t *)(ctx))->c = 0;                                                   
//
//}
//
//void fn_DEVICE_B2B_H_LAST(ctx_t * ctx, cl_ulong *aux)
//{
//	cl_uint CV = 0;                                                               
//		CV = fn_Add((cl_uint)(ctx->t[0]), ctx->c, CV, (cl_uint *)(ctx->t[0]));       
//		CV = fn_Add((cl_uint)(ctx->t[1]), 0, CV, (cl_uint *)(ctx->t[1]));              
//		CV = fn_Add((cl_uint)(ctx->t[2]), 0, CV, (cl_uint *)(ctx->t[2]));             
//		CV = fn_Add((cl_uint)(ctx->t[3]), 0, CV, (cl_uint *)(ctx->t[3]));             
//		while (((ctx_t *)(ctx))->c < BUF_SIZE_8)                                      
//		{                                                                             
//			((ctx_t *)(ctx))->b[((ctx_t *)(ctx))->c++] = 0;                           
//		}                                                                            
//			B2B_INIT(ctx, aux);                                                              
//			((cl_ulong *)(aux))[14] = ~((cl_ulong *)(aux))[14];                                 
//			B2B_FINAL(ctx, aux);                                                            
//}
//
//
//#define DEVICE_B2B_H(ctx, aux)                                                 \
//do                                                                             \
//{                                                                              \
//cl_uint CV = 0;																\
//CV = fn_Add((cl_uint)(ctx->t[0]), 128, CV, (cl_uint *)(ctx->t[0]));			\
//CV = fn_Add((cl_uint)(ctx->t[1]), 0, CV, (cl_uint *)(ctx->t[1]));              \
//CV = fn_Add((cl_uint)(ctx->t[2]), 0, CV, (cl_uint *)(ctx->t[2]));              \
//CV = fn_Add((cl_uint)(ctx->t[3]), 0, CV, (cl_uint *)(ctx->t[3]));            \
//    B2B_INIT(ctx, aux);                                                        \
//    B2B_FINAL(ctx, aux);                                                       \
//    ((ctx_t *)(ctx))->c = 0;                                                   \
//}                                                                              \
//while (0)
//
//// blake2b last mixing procedure
//#define DEVICE_B2B_H_LAST(ctx, aux)                                            \
//do                                                                             \
//{                                                                              \
//cl_uint CV = 0;                                                               \
//CV = fn_Add((cl_uint)(ctx->t[0]), ctx->c, CV, (cl_uint *)(ctx->t[0]));       \
//CV = fn_Add((cl_uint)(ctx->t[1]), 0, CV, (cl_uint *)(ctx->t[1]));              \
//CV = fn_Add((cl_uint)(ctx->t[2]), 0, CV, (cl_uint *)(ctx->t[2]));             \
//CV = fn_Add((cl_uint)(ctx->t[3]), 0, CV, (cl_uint *)(ctx->t[3]));             \
//while (((ctx_t *)(ctx))->c < BUF_SIZE_8)                                        \
//{                                                                              \
//	((ctx_t *)(ctx))->b[((ctx_t *)(ctx))->c++] = 0;                                \
//}                                                                                 \
//                                                                              \
//B2B_INIT(ctx, aux);                                                              \
//                                                                                    \
//((cl_ulong *)(aux))[14] = ~((cl_ulong *)(aux))[14];                                 \
//                                                                                  \
//B2B_FINAL(ctx, aux);                                                            \
//}                                                                           \
//while (0)

#endif //
