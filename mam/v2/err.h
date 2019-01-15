/*!
\file err.h
\brief MAM2 errors.
*/
#pragma once

typedef enum _err_t {
  err_ok = 0,
  err_bad_alloc,        /* end of memory (alloc failed) */
  err_buffer_too_small, /* output (non-io) buffer is too small (see
                           `pb3_decode_trytes`) */
  err_invalid_argument, /* invalid function argument */
  err_invalid_value,
  err_negative_value, /* more specific than `err_invalid_value` */
  err_internal_error, /* internal logic error, mam/v2 is broken */
  err_not_implemented,
  err_pb3_eof,          /* end of file (io buffer) */
  err_pb3_bad_oneof,    /* oneof key is invalid */
  err_pb3_bad_optional, /* not 0 or 1 */
  err_pb3_bad_repeated, /* repeat count undecodable */
  err_pb3_bad_mac,      /* decoded mac is invalid */
  err_pb3_bad_sig,      /* sig verification failed */
  err_pb3_bad_ekey,     /* ekey decryption failed */
  err_mam_channel_not_found,
  err_mam_endpoint_not_found,
  err_mam_version_not_supported,
  err_mam_channel_not_trusted,
  err_mam_endpoint_not_trusted,
  err_mam_keyload_irrelevant, /* recipient can't detect suitable key in keyload
                               */
  err_mam_keyload_overloaded, /* recipient detected multiple suitable keys in
                                 keyload */

  _err_count
} err_t;

/*! MonadError bind operator mimicking.
\note Local variable `err_t e` must be available at call site.
\note Macro `err_bind` can only be called within a loop,
  usually do-while.
\note `continue` is used in case of trigger to break execution.
\example
```
err_t f();
err_t run_f()
{
  err_t e = err_internal_error;
  do {
    err_bind(f());
    e = err_ok;
  } while(0);
  return e;
}
```
*/
#define err_bind(expr)        \
  if (err_ok != (e = (expr))) \
    continue;                 \
  else

/*! MonadError guard mimicking.
\note Local variable `err_t e` must be available at call site.
\note Macro `err_guard` can only be called within a loop,
  usually do-while.
\note `continue` is used in case of trigger to break execution.
\example
```
bool_t f();
err_t err_f_failed;
err_t run_f()
{
  err_t e = err_internal_error;
  do {
    err_guard(f(), err_f_failed);
    e = err_ok;
  } while(0);
  return e;
}
```
*/
#define err_guard(expr, err) \
  if (!(expr)) {             \
    e = (err);               \
    continue;                \
  } else
