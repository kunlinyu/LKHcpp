// @file main.c
// @brief
// @author Kunlin Yu
// @date 2026/4/13

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "LKH.h"

static char* read_all(const char* path, size_t* out_len) {
  FILE* f = fopen(path, "rb");
  if (!f) return NULL;
  fseek(f, 0, SEEK_END);
  long len = ftell(f);
  if (len < 0) {
    fclose(f);
    return NULL;
  }
  fseek(f, 0, SEEK_SET);
  char* buf = (char*)malloc((size_t)len + 1);
  if (!buf) {
    fclose(f);
    return NULL;
  }
  if (fread(buf, 1, (size_t)len, f) != (size_t)len) {
    free(buf);
    fclose(f);
    return NULL;
  }
  buf[len] = 0;
  fclose(f);
  if (out_len) *out_len = (size_t)len;
  return buf;
}

int main(int argc, char* argv[]) {
  if (argc < 3 || argc > 4) {
    printf("Usage:  %s <param_file> <tsplib_file> [tour_file]\n", argv[0]);
    return 1;
  }

  const char* param_path = argv[1];
  const char* tsplib_path = argv[2];
  const char* tour_path = (argc == 4) ? argv[3] : NULL;

  size_t param_len = 0, tsplib_len = 0;
  char* param_text = read_all(param_path, &param_len);
  if (!param_text) {
    fprintf(stderr, "Failed to read param file: %s\n", param_path);
    return 1;
  }
  char* tsplib_text = read_all(tsplib_path, &tsplib_len);
  if (!tsplib_text) {
    fprintf(stderr, "Failed to read tsplib file: %s\n", tsplib_path);
    free(param_text);
    return 1;
  }

  LKH_Handle hdl = LKH_Create();
  LKH_Enable_LOG();
  if (!hdl) {
    fprintf(stderr, "Failed to create LKH handle\n");
    free(param_text);
    free(tsplib_text);
    return 1;
  }

  size_t tour_len = 0;
  const char* tour_data = LKH_Solve(hdl, param_text, tsplib_text, &tour_len);

  free(param_text);
  free(tsplib_text);

  if (!tour_data || tour_len == 0) {
    fprintf(stderr, "No tour found or error occurred\n");
    LKH_Destroy(hdl);
    return 1;
  }

  int ret = 0;
  if (tour_path) {
    FILE* out = fopen(tour_path, "wb");
    if (!out) {
      fprintf(stderr, "Failed to open tour file for writing: %s\n", tour_path);
      ret = 1;
    } else {
      fwrite(tour_data, 1, tour_len, out);
      fclose(out);
    }
  } else {
    fwrite(tour_data, 1, tour_len, stdout);
    fflush(stdout);
  }

  LKH_FreeResult(tour_data);
  LKH_Destroy(hdl);
  return ret;
}