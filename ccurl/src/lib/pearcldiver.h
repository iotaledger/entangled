
#ifndef _PEARCLDIVER_H_
#define _PEARCLDIVER_H_
#include "claccess/clcontext.h"
#include "pearl_diver.h"
//#include "hash.h"

typedef struct {
  CLContext cl;
  PearlDiver pd;
  size_t num_groups;
  size_t loop_count;
} PearCLDiver;

int init_pearcl(PearCLDiver* pd);
void pearcl_search(
		PearCLDiver* pdcl, 
		curl_t* const curl, 
		size_t offset, 
		size_t min_weight_magnitude
		);
#endif /* _PEARCLDIVER_H_ */
