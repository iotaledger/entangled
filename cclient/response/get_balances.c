#include "get_balances.h"

get_balances_res_t* get_balances_res_new() {
  get_balances_res_t* res =
      (get_balances_res_t*)malloc(sizeof(get_balances_res_t));

  res->balances = int_array_new();
  utarray_new(res->milestone, &ut_str_icd);

  return res;
}

void get_balances_res_free(get_balances_res_t* res) {
  if (res) {
    int_array_free(res->balances);
    utarray_free(res->milestone);
    utarray_free(res->milestone);

    free(res);
  }
}

int get_balances_res_balances_at(get_balances_res_t* in, int index) {
  if (in->balances->size > index) {
    return *(in->balances->array + index);
  }
  return NULL;
}

char* get_balances_res_milestone_at(get_balances_res_t* in, int index) {
  if (utarray_len(in->milestone) > index) {
    char** p = NULL;
    p = (char**)utarray_eltptr(in->milestone, index);
    if (p) {
      return *p;
    }
  }
  return '\0';
}
