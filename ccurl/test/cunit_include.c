#include "cunit_include.h"
#ifdef __APPLE__
#include "CUnit/Basic.h"
#else
#include "BCUnit/Basic.h"
#endif
#include <assert.h>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>

int run_tests(CU_SuiteInfo* suites) {
  if (CUE_SUCCESS != CU_initialize_registry())
    return CU_get_error();

  CU_ErrorCode error;

  error = CU_register_suites(suites);
  if (CUE_SUCCESS != error) {
    CU_cleanup_registry();
    fprintf(stderr, "suite registration failed - %s\n", CU_get_error_msg());
    exit(EXIT_FAILURE);
  }
  CU_basic_set_mode(CU_BRM_VERBOSE);
  CU_basic_run_tests();
  CU_cleanup_registry();

  // CU_automated_run_tests();
  // CU_list_tests_to_file();
  return CU_get_error();
}
