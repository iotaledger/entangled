#include "get_inclusion_state.h"

get_inclusion_state_res_t* get_inclusion_state_res_new() {
  get_inclusion_state_res_t* res =
      (get_inclusion_state_res_t*)malloc(sizeof(get_inclusion_state_res_t));
  utarray_new(res->states, &ut_int_icd);
  return res;
}

bool get_inclusion_state_res_bool_at(get_inclusion_state_res_t* in, int index) {
  bool* b = (bool*)utarray_eltptr(in->states, index);
  return *b;
}

int get_inclusion_state_res_bool_num(get_inclusion_state_res_t* in) {
  return utarray_len(in->states);
}

void get_inclusion_state_res_free(get_inclusion_state_res_t** res) {
  if (*res) {
    utarray_free((*res)->states);
    free(*res);
    *res = NULL;
  }
}
