# Contributing to Project Guidelines

## Coding Style
The majority of the code in this repository adheres to [Google's C++ guidelines](https://google.github.io/styleguide/cppguide.html). Below are additional rules which we adhere to:

### 1. C Struct intialisation
If you're initialising a struct, please use member designators to identify the individual fields:
```c
iota_stor_pack_t pack = {.models = (void **)txs,
                         .capacity = 5,
                         .num_loaded = 0,
                         .insufficient_capacity = false};
```

You can find more information on the capabilities of C99 in this regard [here](https://en.cppreference.com/w/c/language/struct_initialization).

### 2. Put function prototypes inside an `extern "C"` block

When writing a header file in C, put function prototypes inside an `extern "C"` block like this:

```c
#ifdef __cplusplus
extern "C" {
#endif

#ifdef __cplusplus
}
#endif
```

It will avoid having function symbols mangled leading to linkage errors when used in a C++ project. You can find more information about this [here](https://stackoverflow.com/questions/3789340/combining-c-and-c-how-does-ifdef-cplusplus-work).

### 3. Parameters ordering

- Always put the main component as first parameter.
- If any, always put tangle as second parameter.

```c
retcode_t iota_api_store_transactions(
    iota_api_t const *const api, tangle_t *const tangle,
    store_transactions_req_t const *const req);

retcode_t iota_consensus_ledger_validator_update_snapshot(
    ledger_validator_t const *const lv, tangle_t *const tangle,
    iota_milestone_t *const milestone, bool *const has_snapshot);
```
