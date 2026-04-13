#ifndef LKH_H_
#define LKH_H_

#include <stddef.h>

#ifdef __cplusplus
extern "C" {
#endif

typedef void* LKH_Handle;

LKH_Handle LKH_Create();

void LKH_Destroy(LKH_Handle hdl);

const char* LKH_Solve(LKH_Handle hdl, const char* param_json,
                      const char* tsplib_text, size_t* out_len);

void LKH_Stop(LKH_Handle hdl);

void LKH_FreeResult(const char* data);

#ifdef __cplusplus
}
#endif

#endif /* LKH_H_ */