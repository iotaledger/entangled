#ifndef _CUNIT_INCLUDE_H_
#define _CUNIT_INCLUDE_H_

#ifdef __APPLE__
#include "CUnit/CUnit.h"
#else
#include "BCUnit/BCUnit.h"
#endif

int run_tests(CU_SuiteInfo* suites);

#endif
