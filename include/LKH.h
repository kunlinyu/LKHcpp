#ifndef _LKH_H_
#define _LKH_H_

#include <stddef.h>

#ifdef __cplusplus
extern "C" {
#endif

typedef void* LKH_Handle;

LKH_Handle LKH_Create();

void LKH_Destroy(LKH_Handle h);

const char* LKH_Solve(LKH_Handle h, const char* param, const char* tsp,
                      size_t* out_len);

void LKH_Stop(LKH_Handle h);

void LKH_FreeResult(const char* data);

#ifdef __cplusplus
}
#endif

#endif /* _LKH_H_ */