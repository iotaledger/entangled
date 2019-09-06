# PCurl

Fast reference implementation of vectorized IOTA Curl transform function; tests and benchmarks.

# Design overview

PCurl runs `n` Curl instances at once following SIMD idea. A trit is represented with 2 bits `(low,high)`; there are `4! = 24` such representations.
`n` trits thus can be packed into two `word`s: one containing `n` `low` bits and the other -- `n` `high` bits, provided `n` bits fit `word`.
A pair of `word`s is called `dword`. PCurl `state` consists of 743 `dwords`. The building block for a Sponge construction is `transform : state -> state` mapping which
applies simpler transform `pcurl_sbox : state -> state` `r` times. The following property can be stated regarding `i`-th output state trit of `pcurl_sbox`:

```
pcurl_sbox(s)[i] == pcurl_s2(s[a(i)], s[b(i)]),
```

where `pcurl_s2 : dword x dword -> dword` is a building block of PCurl that mixes two input trits into one output trit. `a, b : Z_743 -> Z_743` are just fixed
permutations that determine dependencies between input and output trits. The circuit evaluating `pcurl_s2` depends on trit representation and on availability
of corresponding processor instructions.

# Optimizations

There are a number of optimization parameters that can be selected at compile time. They are compared against the "basic" PCurl implementation in iotaledger/entangled.

## `PTRIT_CVT`
Trit representation.

- `PTRIT_CVT_ANDN`: `-1 -> (1,0)`, `0 -> (1,1)`, `+1 -> (0,1)`; optimizes `pcurl_s2` for `andn` instruction; used in the "basic" PCurl impl.

- `PTRIT_CVT_ORN`: `-1 -> (0,0)`, `0 -> (0,1)`, `+1 -> (1,1)`; optimizes `pcurl_s2` for `orn` instruction.

## `PTRIT_PLATFORM`
A straight-forward idea is to SIMD intrinsics to extend capacity of `word` and use available instructions for circuit evaluation.

- `PTRIT_NEON`: 128-bit NEON intrinsics on ARMv7-a targets; enforces the use of `PTRIT_CVT_ORN`.

- `PTRIT_AVX512`: 512-bit AVX512F intrinsics on Intel/AMD targets; enforces the use of `PTRIT_CVT_ANDN`.

- `PTRIT_AVX2`: 256-bit AVX2 intrinsics on Intel/AMD targets; enforces the use of `PTRIT_CVT_ANDN`.

- `PTRIT_SSE2`: 128-bit SSE2 intrinsics on Intel/AMD targets; enforces the use of `PTRIT_CVT_ANDN`.

- `PTRIT_64`: `word` is just 64-bit `uint64_t` type and standard C syntax is used to express `pcurl_s2` circuit.

## `PCURL_S2_CIRCUIT`
Enables the use of certain `pcurl_s2` circuit.

- `PCURL_S2_CIRCUIT4`: optimized 4-instruction circuits found with `circuit-solver`.
  With `PTRIT_CVT_ANDN` -- `(Xor AH (Andn BL AL),Xor AL (And BH (Xor AH (Andn BL AL))))`.
  With `PTRIT_CVT_OR` -- `(Xor AH (Orn BL AL),Xor AL (Orn BH (Xor AH (Orn BL AL))))`.

- `PCURL_S2_CIRCUIT5`: 5-instruction circuit used in the "basic" PCurl impl.

This option is not available and is listed here for the sake of completeness. `PCURL_S2_CIRCUIT4` is permanently enabled.

## `PCURL_S2_ARGS`
Determines the way arguments `s[a(i)]` and `s[b(i)]` to `pcurl_s2` are selected, ie. the way `a` and `b` permutations are implemented.

- `PCURL_S2_ARGS_PTR`: through pointer arithmetic.

- `PCURL_S2_ARGS_LUT`: through look-up table as in the "basic" PCurl impl.

This option is not available and is listed here for the sake of completeness. `PCURL_S2_ARGS_PTR` is permanently enabled.

## `PCURL_SBOX_UNWIND`
Unwind factor for `pcurl_sbox` internal loop, ie. the number of call to `pcurl_s2` per loop. Not all unwind factors are allowed with certain configurations.

- `PCURL_SBOX_UNWIND_1`: 1, ie. do not unwind.

- `PCURL_SBOX_UNWIND_2`: 2.

- `PCURL_SBOX_UNWIND_4`: 4.

- `PCURL_SBOX_UNWIND_8`: 8.

## `PCURL_STATE`
`pcurl_sbox` requires additional memory as output trits are highly interconnected with input.

- `PCURL_STATE_SHORT`: use roughly half of state as additional. It is achieved through different `pcurl_sbox`es as they access memory in different fashion.

- `PCURL_STATE_DOUBLE`: trivially use a full state as additional as in the "basic" PCurl impl.

## TODO: Fuse rounds of `pcurl_sbox`
Make a fused `pcurl_sboxN` for `N=2,3,4`, find an optimized `pcurl_s2xN` circuit with `2^N` input arguments, determine pointer schedule for `pcurl_sbox`.

# Build and Run

See [bench_arm.sh](bench_arm.sh), [bench_x64_msvc.bat](bench_x64_msvc.bat), and [bench_x64_gcc.bat](bench_x64_gcc.bat) for examples. You might need to adjust your `PATH` environment variable.

# Benchmark results

`Time` is measured using `clock` function, benchmark results have quite significant variance due to platform specifics (mobile PC, noisy environment).
Time presented is "average" of several benchmark runs. `Speed` is measured in transactions per second. Transaction size is 8019 trits, or 33 `rate`s, 
ie. it takes 33 transforms to hash a transaction. SIMD vectorization factor is to the left in the `Distance` column.
Curl-81 was used to hash transactions in the benchmarks.

Short configuration has the following format:

```
<compiler>_<platform>_<cvt>_<circuit>_<s2_args>_<unwind>_<state>
```

where
- `<compiler>`: `gcc`, `clang`, or `msvc`; version omitted.
- `<platform>`: `neon`=`PTRIT_NEON`, `avx512`=`PTRIT_AVX512`, `avx2`=`PTRIT_AVX2`, `sse2`=`PTRIT_SSE2`, `64`=`PTRIT_64`.
- `<cvt>`: `andn`=`PTRIT_CVT_ANDN`, `orn`=`PTRIT_CVT_ORN`.
- `<circuit>`: `c4`=`PCURL_S2_CIRCUIT4`, `c5`=`PCURL_S2_CIRCUIT5`.
- `<s2_args>`: `ptr`=`PCURL_S2_ARGS_PTR`, `lut`=`PCURL_S2_ARGS_LUT`.
- `<unwind>`: `uX`=`PCURL_SBOX_UNWIND_X`.
- `<state>`: `ss`=`PCURL_STATE_SHORT`, `sd`=`PCURL_STATE_DOUBLE`.

## MacBook Pro

Environment: 2,6 GHz Intel Core i7, 2400 MHz DDR4, Darwin 18.7.0, Apple LLVM 10.0.1.

*Speedup*: 3.56x.

| Configuration                  | Speed, tx/s | Distance, tx  | Time, ms |
| ------------------------------ | ----------: | ------------: | -------: |
| `gcc_avx2_andn_c4_ptr_u8_sd`   | 108598 tx/s | 256 x 2500 tx |  5893 ms |
|                                | 108864 tx/s | 256 x 2500 tx |  5878 ms |
|                                | 109836 tx/s | 256 x 2500 tx |  5826 ms |
| `gcc_avx2_andn_c4_ptr_u4_ss`   | 107704 tx/s | 256 x 2500 tx |  5942 ms |
|                                | 107957 tx/s | 256 x 2500 tx |  5928 ms |
|                                | 107222 tx/s | 256 x 2500 tx |  5968 ms |
| `gcc_avx2_andn_c4_ptr_u4_sd`   | 105338 tx/s | 256 x 2500 tx |  6075 ms |
|                                | 106084 tx/s | 256 x 2500 tx |  6032 ms |
|                                | 106209 tx/s | 256 x 2500 tx |  6025 ms |
| `gcc_avx2_andn_c4_ptr_u2_sd`   | 102943 tx/s | 256 x 2500 tx |  6217 ms |
|                                | 102129 tx/s | 256 x 2500 tx |  6266 ms |
|                                | 102387 tx/s | 256 x 2500 tx |  6250 ms |
| `gcc_sse2_andn_c4_ptr_u4_ss`   |  74331 tx/s | 128 x 5000 tx |  8610 ms |
|                                |  74476 tx/s | 128 x 5000 tx |  8593 ms |
|                                |  75133 tx/s | 128 x 5000 tx |  8518 ms |
| `gcc_64_andn_c4_ptr_u4_ss`     |  37366 tx/s | 64 x 10000 tx | 17127 ms |
|                                |  37691 tx/s | 64 x 10000 tx | 16980 ms |
|                                |  37681 tx/s | 64 x 10000 tx | 16984 ms |
| `gcc_64_andn_c5_lut_u1_sd`     |  30482 tx/s | 64 x 10000 tx | 20995 ms |
|                                |  30497 tx/s | 64 x 10000 tx | 20985 ms |
|                                |  29735 tx/s | 64 x 10000 tx | 21523 ms |

## Laptop ThinkPad T-490s

Environment: Intel Core i7-8565U, Ubuntu 18.04, gcc-7.4.0.

*Speedup*: 3.75x.

| Configuration                  | Speed, tx/s | Distance, tx | Time, ms |
| ------------------------------ | ---------: | ------------: | -------: |
| `gcc_avx2_andn_c4_ptr_u8_sd`   | 95345 tx/s | 256 x 2500 tx |  6712 ms |
|                                | 91675 tx/s | 256 x 2500 tx |  6981 ms |
|                                | 88421 tx/s | 256 x 2500 tx |  7238 ms |
| `gcc_avx2_andn_c4_ptr_u4_ss`   | 89393 tx/s | 256 x 2500 tx |  7159 ms |
|                                | 88847 tx/s | 256 x 2500 tx |  7203 ms |
|                                | 90907 tx/s | 256 x 2500 tx |  7040 ms |
| `gcc_avx2_andn_c4_ptr_u2_sd`   | 82903 tx/s | 256 x 2500 tx |  7719 ms |
|                                | 84771 tx/s | 256 x 2500 tx |  7549 ms |
|                                | 83058 tx/s | 256 x 2500 tx |  7705 ms |
| `gcc_avx2_andn_c4_ptr_u4_sd`   | 88255 tx/s | 256 x 2500 tx |  7251 ms |
|                                | 85882 tx/s | 256 x 2500 tx |  7452 ms |
|                                | 83931 tx/s | 256 x 2500 tx |  7625 ms |
| `gcc_sse2_andn_c4_ptr_u4_ss`   | 70663 tx/s | 128 x 5000 tx |  9057 ms |
|                                | 71107 tx/s | 128 x 5000 tx |  9000 ms |
|                                | 72600 tx/s | 128 x 5000 tx |  8815 ms |
| `gcc_64_andn_c4_ptr_u4_ss`     | 42553 tx/s | 64 x 10000 tx | 15039 ms |
|                                | 40221 tx/s | 64 x 10000 tx | 15912 ms |
|                                | 39609 tx/s | 64 x 10000 tx | 16157 ms |
| `gcc_64_andn_c5_lut_u1_sd`     | 24019 tx/s | 64 x 10000 tx | 26645 ms |
|                                | 24765 tx/s | 64 x 10000 tx | 25842 ms |
|                                | 24478 tx/s | 64 x 10000 tx | 26145 ms |


## Laptop HP ProBook

Environment: Intel Core i7 4510U, Windows 8.1, msvc-14.1, gcc-8.0.1.
AVX512 implementation was tested with Intel sde-8.35.0.

*Speedup*: 2.71x.

| Configuration                  | Speed, tx/s | Distance, tx | Time, ms |
| ------------------------------ | ---------: | ------------: | -------: |
| `gcc_avx512_andn_c4_ptr_u4_ss` |   575 tx/s |   512 x 10 tx |  8897 ms |
|                                |   579 tx/s |   512 x 10 tx |  8836 ms |
|                                |   572 tx/s |   512 x 10 tx |  8940 ms |
| `gcc_sse2_andn_c4_ptr_u4_ss`   | 55541 tx/s | 128 x 5000 tx | 11523 ms |
|                                | 48724 tx/s | 128 x 5000 tx | 13135 ms |
|                                | 46299 tx/s | 128 x 5000 tx | 13823 ms |
| `msvc_sse2_andn_c4_ptr_u4_ss`  | 48799 tx/s | 128 x 5000 tx | 13115 ms |
|                                | 48167 tx/s | 128 x 5000 tx | 13287 ms |
|                                | 46879 tx/s | 128 x 5000 tx | 13652 ms |
| `msvc_avx2_andn_c4_ptr_u4_ss`  | 47672 tx/s | 256 x 2500 tx | 13425 ms |
|                                | 47693 tx/s | 256 x 2500 tx | 13419 ms |
|                                | 47038 tx/s | 256 x 2500 tx | 13606 ms |
| `gcc_avx2_andn_c4_ptr_u4_ss`   | 47107 tx/s | 256 x 2500 tx | 13586 ms |
|                                | 46931 tx/s | 256 x 2500 tx | 13637 ms |
|                                | 47368 tx/s | 256 x 2500 tx | 13511 ms |
| `msvc_avx2_andn_c4_ptr_u4_sd`  | 45133 tx/s | 256 x 2500 tx | 14180 ms |
|                                | 45045 tx/s | 256 x 2500 tx | 14208 ms |
|                                | 45169 tx/s | 256 x 2500 tx | 14169 ms |
| `msvc_avx2_andn_c4_ptr_u2_sd`  | 43354 tx/s | 256 x 2500 tx | 14762 ms |
|                                | 43950 tx/s | 256 x 2500 tx | 14562 ms |
|                                | 43281 tx/s | 256 x 2500 tx | 14787 ms |
| `gcc_avx2_andn_c4_ptr_u2_sd`   | 42780 tx/s | 256 x 2500 tx | 14960 ms |
|                                | 42108 tx/s | 256 x 2500 tx | 15199 ms |
|                                | 37941 tx/s | 256 x 2500 tx | 16868 ms |
| `gcc_avx2_andn_c4_ptr_u8_sd`   | 41674 tx/s | 256 x 2500 tx | 15357 ms |
|                                | 40145 tx/s | 256 x 2500 tx | 15942 ms |
|                                | 36527 tx/s | 256 x 2500 tx | 17521 ms |
| `gcc_avx2_andn_c4_ptr_u4_sd`   | 39438 tx/s | 256 x 2500 tx | 16228 ms |
|                                | 39885 tx/s | 256 x 2500 tx | 16046 ms |
|                                | 40231 tx/s | 256 x 2500 tx | 15908 ms |
| `msvc_64_andn_c4_ptr_u4_ss`    | 24781 tx/s | 64 x 10000 tx | 25826 ms |
|                                | 29723 tx/s | 64 x 10000 tx | 21532 ms |
|                                | 29684 tx/s | 64 x 10000 tx | 21560 ms |
| `gcc_64_andn_c4_ptr_u4_ss`     | 28567 tx/s | 64 x 10000 tx | 22403 ms |
|                                | 28731 tx/s | 64 x 10000 tx | 22275 ms |
|                                | 28519 tx/s | 64 x 10000 tx | 22441 ms |
| `gcc_64_andn_c5_lut_u1_sd`     | 18489 tx/s | 64 x 10000 tx | 34615 ms |
|                                | 18003 tx/s | 64 x 10000 tx | 35549 ms |
|                                | 16341 tx/s | 64 x 10000 tx | 39164 ms |
| `msvc_64_andn_c5_lut_u1_sd`    | 16291 tx/s | 64 x 10000 tx | 39284 ms |
|                                | 16296 tx/s | 64 x 10000 tx | 39272 ms |
|                                | 16044 tx/s | 64 x 10000 tx | 39890 ms |

## Raspberry Pi 3

Environment: ARMv7l, raspbian (4.4.13-v7+), gcc-6.1.0.

*Speedup*: 1.90x.

| Configuration                  | Speed, tx/s | Distance, tx | Time, ms |
| ------------------------------ | ---------- | ------------- | -------- |
| `gcc_64_andn_c4_ptr_u8_sd`     |  3494 tx/s |  64 x 1000 tx | 18316 ms |
|                                |  3492 tx/s |  64 x 1000 tx | 18323 ms |
|                                |  3487 tx/s |  64 x 1000 tx | 18348 ms |
| `gcc_64_andn_c4_ptr_u4_ss`     |  3320 tx/s |  64 x 1000 tx | 19273 ms |
|                                |  3329 tx/s |  64 x 1000 tx | 19219 ms |
|                                |  3331 tx/s |  64 x 1000 tx | 19210 ms |
| `gcc_neon_orn_c4_ptr_u4_ss`    |  3303 tx/s |  128 x 500 tx | 19371 ms |
|                                |  3306 tx/s |  128 x 500 tx | 19355 ms |
|                                |  3305 tx/s |  128 x 500 tx | 19359 ms |
| `gcc_neon_orn_c4_ptr_u8_sd`    |  3270 tx/s |  128 x 500 tx | 19571 ms |
|                                |  3271 tx/s |  128 x 500 tx | 19561 ms |
|                                |  3271 tx/s |  128 x 500 tx | 19560 ms |
| `gcc_64_orn_c4_ptr_u4_ss`      |  2738 tx/s |  64 x 1000 tx | 23367 ms |
|                                |  2723 tx/s |  64 x 1000 tx | 23501 ms |
|                                |  2732 tx/s |  64 x 1000 tx | 23419 ms |
| `gcc_64_andn_c5_lut_u1_sd`     |  1832 tx/s |  64 x 1000 tx | 34924 ms |
|                                |  1832 tx/s |  64 x 1000 tx | 34923 ms |
|                                |  1832 tx/s |  64 x 1000 tx | 34925 ms |

## Samsung Grand Prime

Environment: ARMv8, Android 5.1.1, clang-8.0.0.

*Speedup*: 2.37x.

| Configuration                  | Speed, tx/s | Distance, tx | Time, ms |
| ------------------------------ | ---------- | ------------- | -------- |
| `clang_neon_orn_c4_ptr_u4_ss`  |  3345 tx/s |  128 x 500 tx | 19132 ms |
|                                |  3350 tx/s |  128 x 500 tx | 19099 ms |
|                                |  3350 tx/s |  128 x 500 tx | 19099 ms |
| `clang_neon_orn_c4_ptr_u8_sd`  |  3086 tx/s |  128 x 500 tx | 20733 ms |
|                                |  3083 tx/s |  128 x 500 tx | 20757 ms |
|                                |  3069 tx/s |  128 x 500 tx | 20850 ms |
| `clang_64_andn_c4_ptr_u4_ss`   |  1791 tx/s |  64 x 1000 tx | 35732 ms |
|                                |  1791 tx/s |  64 x 1000 tx | 35725 ms |
|                                |  1790 tx/s |  64 x 1000 tx | 35750 ms |
| `clang_64_orn_c4_ptr_u4_ss`    |  1561 tx/s |  64 x 1000 tx | 40991 ms |
|                                |  1560 tx/s |  64 x 1000 tx | 41018 ms |
|                                |  1561 tx/s |  64 x 1000 tx | 40981 ms |
| `clang_64_andn_c4_ptr_u8_sd`   |  1490 tx/s |  64 x 1000 tx | 42928 ms |
|                                |  1491 tx/s |  64 x 1000 tx | 42916 ms |
|                                |  1490 tx/s |  64 x 1000 tx | 42946 ms |
| `clang_64_andn_c5_lut_u1_sd`   |  1413 tx/s |  64 x 1000 tx | 45287 ms |
|                                |  1413 tx/s |  64 x 1000 tx | 45274 ms |
|                                |  1413 tx/s |  64 x 1000 tx | 45281 ms |

