
/*
 * Copyright (c) 2018 IOTA Stiftung
 * https://github.com/iotaledger/entangled
 *
 * MAM is based on an original implementation & specification by apmi.bsu.by
 [ITSec Lab]

 *
 *
 * Refer to the LICENSE file for licensing information
 */

/*!
\file mam2.c
\brief MAM2 layer.
*/
#include "mam/v2/mam.h"
#include "mam/v2/pb3.h"

MAM2_SAPI err_t mam2_mss_create(mam2_ialloc *ma, mss_t *m, prng_t *p,
                                mss_mt_height_t d, trits_t nonce1,
                                trits_t nonce2) {
  err_t e = err_internal_error;
  MAM2_ASSERT(ma);
  MAM2_ASSERT(m);

  do {
    err_bind(mss_create(m, d));

    m->nonce1 = trits_null();
    if (!trits_is_empty(nonce1)) {
      m->nonce1 = trits_alloc(trits_size(nonce1));
      err_guard(!trits_is_null(m->nonce1), err_bad_alloc);
    }

    m->nonce2 = trits_null();
    if (!trits_is_empty(nonce2)) {
      m->nonce2 = trits_alloc(trits_size(nonce2));
      err_guard(!trits_is_null(m->nonce2), err_bad_alloc);
    }

    m->sponge = ma->create_sponge();
    err_guard(m->sponge, err_bad_alloc);

    m->wots = malloc(sizeof(iwots));
    err_guard(m->wots, err_bad_alloc);
    err_bind(wots_create(m->wots));

    m->wots->s = ma->create_sponge();
    err_guard(m->wots->s, err_bad_alloc);
    wots_init(m->wots, m->wots->s);

    mss_init(m, p, m->sponge, m->wots, d, m->nonce1, m->nonce2);

    e = err_ok;
  } while (0);

  return e;
}

MAM2_SAPI void mam2_mss_destroy(mam2_ialloc *ma, mss_t *m) {
  MAM2_ASSERT(ma);
  MAM2_ASSERT(m);

  m->prng = NULL;

  trits_free(m->nonce1);
  trits_free(m->nonce2);

  if (m->wots) {
    ma->destroy_sponge(m->wots->s);
    m->wots->s = 0;
  }
  wots_destroy(m->wots);
  m->wots = NULL;

  ma->destroy_sponge(m->sponge);
  m->sponge = NULL;

  mss_destroy(m);
}

MAM2_SAPI trits_t mam2_channel_id(mam2_channel *ch) {
  return trits_from_rep(MAM2_CHANNEL_ID_SIZE, ch->id);
}
MAM2_SAPI trits_t mam2_channel_name(mam2_channel *ch) { return ch->m->nonce1; }
static size_t mam2_channel_sig_size(mam2_channel *ch) {
  return MAM2_MSS_SIG_SIZE(ch->m->height);
}

MAM2_SAPI err_t mam2_channel_create(
    mam2_ialloc *ma, /*!< [in] Allocator. */
    prng_t *
        p, /*! [in] Shared PRNG interface used to generate WOTS private keys. */
    mss_mt_height_t d, /*!< [in] MSS MT height. */
    trits_t ch_name,   /*!< [in] Channel name. */
    mam2_channel *ch   /*!< [out] Channel. */
) {
  err_t e = err_internal_error;

  MAM2_ASSERT(ma);
  MAM2_ASSERT(ch);

  do {
    err_bind(mam2_mss_create(ma, ch->m, p, d, ch_name, trits_null()));
    mss_gen(ch->m, mam2_channel_id(ch));

    e = err_ok;
  } while (0);

  return e;
}

MAM2_SAPI void mam2_channel_destroy(mam2_ialloc *ma, /*!< [in] Allocator. */
                                    mam2_channel *ch /*!< [out] Channel. */
) {
  MAM2_ASSERT(ma);
  MAM2_ASSERT(ch);

  mam2_mss_destroy(ma, ch->m);
}

MAM2_API trits_t mam2_endpoint_id(mam2_endpoint *ep) {
  return trits_from_rep(MAM2_ENDPOINT_ID_SIZE, ep->id);
}
MAM2_API trits_t mam2_endpoint_chname(mam2_endpoint *ep) {
  return ep->m->nonce1;
}
MAM2_API trits_t mam2_endpoint_name(mam2_endpoint *ep) { return ep->m->nonce2; }
static size_t mam2_endpoint_sig_size(mam2_endpoint *ep) {
  return MAM2_MSS_SIG_SIZE(ep->m->height);
}

MAM2_SAPI err_t mam2_endpoint_create(
    mam2_ialloc *ma, /*!< [in] Allocator. */
    prng_t *
        p, /*! [in] Shared PRNG interface used to generate WOTS private keys. */
    mss_mt_height_t d, /*!< [in] MSS MT height. */
    trits_t ch_name,   /*!< [in] Channel name. */
    trits_t ep_name,   /*!< [in] Endpoint name. */
    mam2_endpoint *ep  /*!< [out] Endpoint. */
) {
  err_t e = err_internal_error;

  MAM2_ASSERT(ma);
  MAM2_ASSERT(ep);

  do {
    err_bind(mam2_mss_create(ma, ep->m, p, d, ch_name, ep_name));
    mss_gen(ep->m, mam2_endpoint_id(ep));

    e = err_ok;
  } while (0);

  return e;
}

MAM2_SAPI void mam2_endpoint_destroy(mam2_ialloc *ma,  /*!< [in] Allocator. */
                                     mam2_endpoint *ep /*!< [out] Endpoint. */
) {
  MAM2_ASSERT(ma);
  MAM2_ASSERT(ep);

  mam2_mss_destroy(ma, ep->m);
}

MAM2_SAPI trits_t mam2_psk_id(mam2_psk *p) {
  return trits_from_rep(MAM2_PSK_ID_SIZE, p->id);
}
MAM2_SAPI trits_t mam2_psk_trits(mam2_psk *p) {
  return trits_from_rep(MAM2_PSK_SIZE, p->psk);
}

MAM2_SAPI trits_t mam2_ntru_pk_trits(mam2_ntru_pk *p) {
  return trits_from_rep(MAM2_NTRU_PK_SIZE, p->pk);
}
MAM2_SAPI trits_t mam2_ntru_pk_id(mam2_ntru_pk *p) {
  return trits_from_rep(MAM2_NTRU_ID_SIZE, p->pk);
}

MAM2_SAPI size_t mam2_send_msg_size(mam2_send_msg_context *cfg) {
  size_t sz = 0;

  // channel
  //  tryte ver;
  sz += pb3_sizeof_tryte();
  //  external tryte chid[81];
  sz += 0;

  // endpoint
  //  oneof pubkey {
  sz += pb3_sizeof_oneof();
  if (cfg->ep)
    //  tryte epid[81] = 2;
    sz += MAM2_ENDPOINT_ID_SIZE;
  else if (cfg->ch1)
    //  tryte chid1[81] = 1;
    sz += MAM2_CHANNEL_ID_SIZE;
  else
    //  null chid = 0;
    sz += 0;

  if (cfg->ep_sig) {
    if (cfg->ep)
      // sign with ep
      sz += pb3_sizeof_trytes(mam2_endpoint_sig_size(cfg->ep));
    else
      // sign with ch
      sz += pb3_sizeof_trytes(mam2_channel_sig_size(cfg->ch));
  } else
    sz += pb3_sizeof_trytes(0);

  // header
  //  tryte nonce[27];
  sz += pb3_sizeof_ntrytes(MAM2_HEADER_NONCE_SIZE / 3);
  //  forkhash repeated oneof keyload {
  {
    size_t keyload_count = 0;
    mam2_psk_node *ipsk;
    mam2_ntru_pk_node *intru_pk;

    if (cfg->key_plain) {
      ++keyload_count;
      //  message KeyloadPlain
      sz += pb3_sizeof_oneof()
            //  tryte key[81];
            + pb3_sizeof_ntrytes(MAM2_SPONGE_KEY_SIZE / 3);
    }

    for (ipsk = cfg->psks.begin; ipsk; ipsk = ipsk->next) {
      ++keyload_count;
      //  message KeyloadPSK
      sz += pb3_sizeof_oneof()
            //  tryte id[27];
            + pb3_sizeof_ntrytes(MAM2_PSK_ID_SIZE / 3)
            //  external secret tryte psk[81];
            + 0
            //  encrypted tryte ekey[81];
            + pb3_sizeof_ntrytes(MAM2_SPONGE_KEY_SIZE / 3);
    }

    for (intru_pk = cfg->ntru_pks.begin; intru_pk; intru_pk = intru_pk->next) {
      ++keyload_count;
      //  message KeyloadNTRU
      sz += pb3_sizeof_oneof()
            //  tryte id[27];
            + pb3_sizeof_ntrytes(MAM2_NTRU_ID_SIZE / 3)
            //  tryte ekey[3072];
            + pb3_sizeof_ntrytes(MAM2_NTRU_EKEY_SIZE / 3);
    }

    sz += pb3_sizeof_repeated(keyload_count);
  }
  //  external secret tryte key[81];
  sz += 0;

  // packets
  return sz;
}

static trits_t mam2_send_msg_cfg_chid(mam2_send_msg_context *cfg) {
  MAM2_ASSERT(cfg);
  MAM2_ASSERT(cfg->ch);
  return mam2_channel_id(cfg->ch);
}
static trits_t mam2_send_msg_cfg_chid1(mam2_send_msg_context *cfg) {
  MAM2_ASSERT(cfg);
  MAM2_ASSERT(cfg->ch1);
  return mam2_channel_id(cfg->ch1);
}
static trits_t mam2_send_msg_cfg_epid(mam2_send_msg_context *cfg) {
  MAM2_ASSERT(cfg);
  MAM2_ASSERT(cfg->ep);
  return mam2_endpoint_id(cfg->ep);
}
static trits_t mam2_send_msg_cfg_key(mam2_send_msg_context *cfg) {
  MAM2_ASSERT(cfg);
  return trits_from_rep(MAM2_SPONGE_KEY_SIZE, cfg->key);
}

MAM2_SAPI err_t mam2_send_msg(mam2_send_msg_context *cfg, trits_t *msg) {
  err_t e = err_internal_error;

  isponge *s;
  isponge *fork;
  trits_t b0, *b;

  MAM2_TRITS_DEF(skn, MAM2_MSS_SKN_SIZE);
  trits_t _msg = trits_null();
  size_t msg_size = 0;

  MAM2_ASSERT(cfg);
  MAM2_ASSERT(cfg->ch);
  MAM2_ASSERT(cfg->ma);
  MAM2_ASSERT(cfg->s);
  s = cfg->s;
  fork = cfg->fork;

  do {
    err_guard(msg, err_invalid_argument);
    msg_size = mam2_send_msg_size(cfg);
    if (trits_is_null(*msg)) {
      _msg = trits_alloc(msg_size);
      err_guard(!trits_is_null(_msg), err_bad_alloc);
      *msg = _msg;
    } else
      err_guard(!(trits_size(*msg) < msg_size), err_buffer_too_small);
    b = msg;

    if (cfg->ep)
      mss_skn(cfg->ep->m, skn);
    else
      mss_skn(cfg->ch->m, skn);
    prng_gen3(cfg->rng, MAM2_PRNG_DST_SECKEY, mam2_channel_name(cfg->ch),
              cfg->ep ? mam2_endpoint_name(cfg->ep) : trits_null(), skn,
              mam2_send_msg_cfg_key(cfg));

    // choose recipient

    /*
    message Msg {
      Channel channel;
      Endpoint endpoint;
      Header header;
      Packet packets[];
    };
    */

    // wrap Channel
    /*
    message Channel {
      tryte ver;
      external tryte chid[81];
    };
    */
    {
      // tryte ver;
      b0 = *b;
      pb3_encode_tryte(0, b);
      pb3_wrap_data(s, trits_diff(b0, *b));

      // external required tryte chid[81];
      pb3_wrap_data(s, mam2_send_msg_cfg_chid(cfg));
    }

    // wrap Endpoint
    /*
    message Endpoint {
      oneof pubkey {
        null chid = 0;
        tryte chid1[81] = 1;
        tryte epid[81] = 2;
      };
      donthash trytes sig;
    };
    */
    {
      tryte_t pubkey = 0;
      if (cfg->ep)
        pubkey = 2;
      else if (cfg->ch1)
        pubkey = 1;
      else
        pubkey = 0;

      b0 = *b;
      pb3_encode_oneof(pubkey, b);
      pb3_wrap_data(s, trits_diff(b0, *b));

      b0 = *b;
      if (cfg->ep) {
        pb3_encode_ntrytes(mam2_send_msg_cfg_epid(cfg), b);
        pb3_wrap_data(s, trits_diff(b0, *b));
      } else if (cfg->ch1) {
        pb3_encode_ntrytes(mam2_send_msg_cfg_chid1(cfg), b);
        pb3_wrap_data(s, trits_diff(b0, *b));
      } else
        // null, don't waste sponge calls
        ;

      if (cfg->ep_sig) {
        size_t n;
        trits_t sig;
        MAM2_TRITS_DEF(H, MAM2_MSS_HASH_SIZE);

        sponge_squeeze(s, MAM2_SPONGE_CTL_HASH, H);

        if (cfg->ep)
          n = mam2_endpoint_sig_size(cfg->ep);
        else
          n = mam2_channel_sig_size(cfg->ch);

        MAM2_ASSERT(0 == n % 3);
        // manually `pb3_encode_trytes`
        pb3_encode_sizet(n / 3, b);
        sig = trits_take(*b, n);
        *b = trits_drop(*b, n);

        // sign with ep
        if (cfg->ep)
          mss_sign(cfg->ep->m, H, sig);
        else
          mss_sign(cfg->ch->m, H, sig);
      } else
        // empty sig = no sig
        pb3_encode_trytes(trits_null(), b);
    }

    // header
    /*
    message Header {
      tryte nonce[27];
      forkhash repeated oneof keyload {
        KeyloadPlain plain = 0;
        KeyloadPSK psk = 1;
        KeyloadNTRU ntru = 2;
      };
      external secret tryte key[81];
    };
    message KeyloadPlain {
      tryte key[81];
    };
    message KeyloadPSK {
      tryte id[27];
      external secret tryte psk[81];
      encrypted tryte ekey[81];
    };
    message KeyloadNTRU {
      tryte id[27];
      tryte ekey[3072];
    };
    */
    {
      //  tryte nonce[27];
      b0 = *b;
      pb3_encode_ntrytes(cfg->nonce, b);
      pb3_wrap_data(s, trits_diff(b0, *b));

      {
        size_t keyload_count = 0;
        mam2_psk_node *ipsk;
        mam2_ntru_pk_node *intru_pk;

        if (cfg->key_plain) ++keyload_count;
        for (ipsk = cfg->psks.begin; ipsk; ipsk = ipsk->next) ++keyload_count;
        for (intru_pk = cfg->ntru_pks.begin; intru_pk;
             intru_pk = intru_pk->next)
          ++keyload_count;

        //  forkhash repeated oneof keyload
        b0 = *b;
        pb3_encode_repeated(keyload_count, b);
        pb3_wrap_data(s, trits_diff(b0, *b));

        if (cfg->key_plain) {
          //  message KeyloadPlain
          sponge_fork(s, fork);

          b0 = *b;
          pb3_encode_oneof(0, b);  // oneof = 0;
          pb3_wrap_data(fork, trits_diff(b0, *b));

          //  tryte key[81];
          b0 = *b;
          pb3_encode_ntrytes(mam2_send_msg_cfg_key(cfg), b);
          pb3_wrap_data(fork, trits_diff(b0, *b));
        }

        for (ipsk = cfg->psks.begin; ipsk; ipsk = ipsk->next) {
          //  message KeyloadPSK
          sponge_fork(s, fork);

          b0 = *b;
          pb3_encode_oneof(1, b);  // oneof = 1;
          pb3_wrap_data(fork, trits_diff(b0, *b));

          //  tryte id[27];
          b0 = *b;
          pb3_encode_ntrytes(mam2_psk_id(&ipsk->info), b);
          pb3_wrap_data(fork, trits_diff(b0, *b));

          //  external secret tryte psk[81];
          pb3_wrap_secret(fork, mam2_psk_trits(&ipsk->info));

          //  encrypted tryte ekey[81];
          b0 = *b;
          pb3_encode_ntrytes(mam2_send_msg_cfg_key(cfg), b);
          pb3_wrap_encrypted(fork, trits_diff(b0, *b));
        }

        for (intru_pk = cfg->ntru_pks.begin; intru_pk;
             intru_pk = intru_pk->next) {
          //  message KeyloadNTRU
          sponge_fork(s, fork);

          b0 = *b;
          pb3_encode_oneof(2, b);  // oneof = 2;
          pb3_wrap_data(fork, trits_diff(b0, *b));

          //  tryte id[27];
          b0 = *b;
          pb3_encode_ntrytes(mam2_ntru_pk_id(&intru_pk->info), b);
          pb3_wrap_data(fork, trits_diff(b0, *b));

          //  tryte ekey[3072];
          b0 = trits_take(*b, MAM2_NTRU_EKEY_SIZE);
          // pb3_encode_ntrytes(Y, b);
          ntru_encr(mam2_ntru_pk_trits(&intru_pk->info), cfg->rng,
                    mam2_send_msg_cfg_key(cfg), cfg->nonce, b0);
          *b = trits_drop(*b, MAM2_NTRU_EKEY_SIZE);
          pb3_wrap_data(fork, b0);
        }
      }
    }

    _msg = trits_null();
    e = err_ok;
  } while (0);

  trits_free(_msg);
  trits_set_zero(mam2_send_msg_cfg_key(cfg));

  return e;
}

MAM2_SAPI size_t mam2_send_packet_size(mam2_send_packet_context *cfg,
                                       size_t payload_size) {
  size_t sz = 0;
  MAM2_ASSERT(cfg);
  MAM2_ASSERT(0 == payload_size % 3);
  sz = 0
       //  donthash long trint ord;
       + pb3_sizeof_longtrint()
       //  encrypted trytes payload;
       + pb3_sizeof_trytes(payload_size / 3)
       //  donthash oneof checksum
       + pb3_sizeof_oneof();

  if (0 == cfg->checksum)
    //    null none = 0;
    sz += 0;
  else if (1 == cfg->checksum)
    //    tryte mac [81] = 1;
    sz += pb3_sizeof_ntrytes(MAM2_SPONGE_MAC_SIZE / 3);
  else if (2 == cfg->checksum) {
    MAM2_ASSERT(cfg->m);
    //    trytes sig = 2;
    sz += pb3_sizeof_trytes(MAM2_MSS_SIG_SIZE(cfg->m->height) / 3);
  } else
    MAM2_ASSERT(0);
}

MAM2_SAPI err_t mam2_send_packet(mam2_send_packet_context *cfg, trits_t payload,
                                 trits_t *packet) {
  err_t e = err_internal_error;
  isponge *s;
  trits_t b0, *b = packet;

  MAM2_ASSERT(cfg);
  MAM2_ASSERT(packet);
  s = cfg->s;

  do {
    /*
    message Packet {
      donthash long trint ord;
      encrypted trytes payload;
      donthash oneof checksum {
        null none = 0;
        tryte mac [81] = 1;
        trytes sig = 2;
      }
    }
    */

    //  donthash long trint ord;
    pb3_encode_longtrint(cfg->ord, b);

    //  encrypted trytes payload;
    b0 = *b;
    pb3_encode_trytes(payload, b);
    pb3_wrap_encrypted(s, trits_diff(b0, *b));

    //  donthash oneof checksum
    b0 = *b;
    pb3_encode_oneof(cfg->checksum, b);
    pb3_wrap_data(s, trits_diff(b0, *b));

    if (1 == cfg->checksum) {
      //    tryte mac [81] = 1;
      // pb3_encode_ntrytes(mac, b);
      b0 = trits_take(*b, MAM2_SPONGE_MAC_SIZE);
      sponge_squeeze(s, MAM2_SPONGE_CTL_HASH, b0);
      *b = trits_drop(*b, MAM2_SPONGE_MAC_SIZE);
    } else if (2 == cfg->checksum) {
      //    trytes sig = 2;
      size_t n;
      trits_t sig;
      MAM2_TRITS_DEF(H, MAM2_MSS_HASH_SIZE);

      sponge_squeeze(s, MAM2_SPONGE_CTL_HASH, H);

      n = MAM2_MSS_SIG_SIZE(cfg->m->height);
      // manually `pb3_encode_trytes`
      pb3_encode_sizet(n / 3, b);
      sig = trits_take(*b, n);
      *b = trits_drop(*b, n);

      // sign
      mss_sign(cfg->m, H, sig);
    } else
      //    null none = 0;
      ;

    e = err_ok;
  } while (0);

  return e;
}

static trits_t mam2_recv_msg_cfg_chid(mam2_recv_msg_context *cfg) {
  MAM2_ASSERT(cfg);
  MAM2_ASSERT(cfg->chid);
  return trits_from_rep(MAM2_CHANNEL_ID_SIZE, cfg->chid);
}
static trits_t mam2_recv_msg_cfg_chid1(mam2_recv_msg_context *cfg) {
  MAM2_ASSERT(cfg);
  MAM2_ASSERT(cfg->chid1);
  return trits_from_rep(MAM2_CHANNEL_ID_SIZE, cfg->chid1);
}
static trits_t mam2_recv_msg_cfg_epid(mam2_recv_msg_context *cfg) {
  MAM2_ASSERT(cfg);
  MAM2_ASSERT(cfg->epid);
  return trits_from_rep(MAM2_ENDPOINT_ID_SIZE, cfg->epid);
}
static trits_t mam2_recv_msg_cfg_nonce(mam2_recv_msg_context *cfg) {
  MAM2_ASSERT(cfg);
  return trits_from_rep(MAM2_HEADER_NONCE_SIZE, cfg->nonce);
}
static trits_t mam2_recv_msg_cfg_key(mam2_recv_msg_context *cfg) {
  MAM2_ASSERT(cfg);
  return trits_from_rep(MAM2_SPONGE_KEY_SIZE, cfg->key);
}
static trits_t mam2_recv_msg_cfg_psk_id(mam2_recv_msg_context *cfg) {
  MAM2_ASSERT(cfg);
  return trits_from_rep(MAM2_PSK_ID_SIZE, cfg->psk_id);
}
static trits_t mam2_recv_msg_cfg_ntru_id(mam2_recv_msg_context *cfg) {
  MAM2_ASSERT(cfg);
  return trits_from_rep(MAM2_NTRU_ID_SIZE, cfg->ntru_id);
}

MAM2_SAPI err_t mam2_recv_msg(mam2_recv_msg_context *cfg, trits_t *msg) {
  err_t e = err_internal_error;

  isponge *s;
  isponge *fork;
  trits_t b0, *b;

  MAM2_ASSERT(cfg);
  MAM2_ASSERT(cfg->chid);
  MAM2_ASSERT(cfg->chid1);
  MAM2_ASSERT(cfg->epid);
  MAM2_ASSERT(cfg->ma);
  MAM2_ASSERT(cfg->s);
  s = cfg->s;
  fork = cfg->fork;
  b = msg;

  do {
    // unwrap Channel
    {
      tryte_t ver = 0;
      err_guard(trits_size(*b) >= pb3_sizeof_tryte(), err_pb3_eof);

      // tryte ver;
      b0 = *b;
      err_bind(pb3_decode_tryte(&ver, b));
      err_guard(0 == ver, err_mam2_version_not_supported);
      pb3_unwrap_data(s, trits_diff(b0, *b));

      // external required tryte chid[81];
      pb3_unwrap_data(s, trits_from_rep(MAM2_CHANNEL_ID_SIZE, cfg->chid));
    }

    // unwrap Endpoint
    {
      tryte_t pubkey = -1;

      b0 = *b;
      err_bind(pb3_decode_oneof(&pubkey, b));
      err_guard(0 <= pubkey && pubkey <= 2, err_pb3_bad_oneof);
      pb3_unwrap_data(s, trits_diff(b0, *b));
      cfg->pubkey = pubkey;

      if (1 == pubkey) {  // chid1
        b0 = *b;
        err_bind(pb3_decode_ntrytes(mam2_recv_msg_cfg_chid1(cfg), b));
        pb3_unwrap_data(s, trits_diff(b0, *b));
      } else if (2 == pubkey) {  // epid
        b0 = *b;
        err_bind(pb3_decode_ntrytes(mam2_recv_msg_cfg_epid(cfg), b));
        pb3_unwrap_data(s, trits_diff(b0, *b));
      } else
        // null, skip
        ;

      // unwrap sig
      {
        size_t n = 0;
        err_bind(pb3_decode_sizet(&n, b));
        err_guard(trits_size(*b) >= pb3_sizeof_ntrytes(n), err_pb3_eof);

        if (0 < n) {
          trits_t sig;
          MAM2_TRITS_DEF(H, MAM2_MSS_HASH_SIZE);

          sponge_squeeze(s, MAM2_SPONGE_CTL_HASH, H);

          err_guard(trits_size(*b) >= n, err_pb3_eof);
          sig = trits_take(*b, n);
          *b = trits_drop(*b, n);

          // TODO: verify that signer is trusted
          if (2 == pubkey)
            // signed with ep
            mss_verify(cfg->ms, cfg->ws, H, sig, mam2_recv_msg_cfg_epid(cfg));
          else
            // signed with ch
            mss_verify(cfg->ms, cfg->ws, H, sig, mam2_recv_msg_cfg_chid(cfg));

          // TODO: record new channel/endpoint
        }
      }
    }

    // unwrap header
    {
      //  tryte nonce[27];
      b0 = *b;
      err_bind(pb3_decode_ntrytes(mam2_recv_msg_cfg_nonce(cfg), b));
      pb3_unwrap_data(s, trits_diff(b0, *b));

      //  forkhash repeated oneof keyload
      {
        size_t keyload_count = 0;
        bool_t key_found = 0;

        b0 = *b;
        err_bind(pb3_decode_repeated(&keyload_count, b));
        pb3_unwrap_data(s, trits_diff(b0, *b));

        for (e = err_ok; e == err_ok && keyload_count--;) {
          tryte_t keyload = -1;

          // forkhash
          sponge_fork(s, fork);

          // oneof keyload {
          b0 = *b;
          err_bind(pb3_decode_tryte(&keyload, b));
          pb3_unwrap_data(fork, trits_diff(b0, *b));

          if (0 == keyload) {  //  KeyloadPlain plain = 0;

            //  tryte key[81];
            b0 = *b;
            pb3_decode_ntrytes(mam2_recv_msg_cfg_key(cfg), b);
            pb3_unwrap_data(fork, trits_diff(b0, *b));

            err_guard(!key_found, err_mam2_keyload_overloaded);
            key_found = 1;
          } else if (1 == keyload) {  //  KeyloadPSK psk = 1;

            //  tryte id[27];
            b0 = *b;
            pb3_decode_ntrytes(mam2_recv_msg_cfg_psk_id(cfg), b);
            pb3_wrap_data(fork, trits_diff(b0, *b));

            if (cfg->psk && trits_cmp_eq(mam2_psk_id(cfg->psk),
                                         mam2_recv_msg_cfg_psk_id(cfg))) {
              err_guard(!key_found, err_mam2_keyload_overloaded);
              key_found = 1;

              //  external secret tryte psk[81];
              pb3_unwrap_secret(s, mam2_psk_trits(cfg->psk));

              //  encrypted tryte ekey[81];
              b0 = *b;
              err_bind(pb3_decode_ntrytes(mam2_recv_msg_cfg_key(cfg), b));
              pb3_unwrap_encrypted(s, mam2_recv_msg_cfg_key(cfg));
            } else {
              // skip
              //  external secret tryte psk[81];
              //  encrypted tryte ekey[81];
              *b = trits_drop(*b, pb3_sizeof_ntrytes(MAM2_SPONGE_KEY_SIZE / 3));
            }
          } else if (2 == keyload) {  //  KeyloadNTRU ntru = 2;

            //  tryte id[27];
            b0 = *b;
            pb3_decode_ntrytes(mam2_recv_msg_cfg_ntru_id(cfg), b);
            pb3_wrap_data(fork, trits_diff(b0, *b));

            if (cfg->ntru && trits_cmp_eq(ntru_id_trits(cfg->ntru),
                                          mam2_recv_msg_cfg_ntru_id(cfg))) {
              err_guard(!key_found, err_mam2_keyload_overloaded);
              key_found = 1;

              //  tryte ekey[3072];
              err_guard(trits_size(*b) >= MAM2_NTRU_EKEY_SIZE, err_pb3_eof);
              b0 = trits_take(*b, MAM2_NTRU_EKEY_SIZE);
              err_guard(ntru_decr(cfg->ntru, b0, mam2_recv_msg_cfg_key(cfg)),
                        err_pb3_bad_ekey);
              *b = trits_drop(*b, MAM2_NTRU_EKEY_SIZE);
              pb3_unwrap_data(fork, b0);
            } else {
              // skip
              //  tryte ekey[3072];
              *b = trits_drop(*b, pb3_sizeof_ntrytes(MAM2_NTRU_EKEY_SIZE / 3));
            }
          } else
            err_guard(0, err_pb3_bad_oneof);
        }
      }
    }

    e = err_ok;
  } while (0);

  return e;
}

MAM2_SAPI err_t mam2_recv_packet(mam2_recv_packet_context *cfg, trits_t *packet,
                                 trits_t *payload) {
  err_t e = err_internal_error;
  isponge *s;
  trits_t b0, *b = packet;

  MAM2_ASSERT(cfg);
  MAM2_ASSERT(cfg->s);
  MAM2_ASSERT(cfg->ma);
  MAM2_ASSERT(packet);
  MAM2_ASSERT(payload);
  s = cfg->s;

  do {
    tryte_t checksum = -1;

    //  donthash long trint ord;
    err_bind(pb3_decode_longtrint(&cfg->ord, b));
    // TODO: check ord

    //  encrypted trytes payload;
    b0 = *b;
    err_bind(pb3_decode_trytes2(payload, b));
    pb3_unwrap_encrypted(s, trits_diff(b0, *b));

    //  donthash oneof checksum
    b0 = *b;
    err_bind(pb3_decode_oneof(&checksum, b));
    err_guard(0 <= checksum && checksum <= 2, err_pb3_bad_oneof);
    pb3_wrap_data(s, trits_diff(b0, *b));

    if (1 == checksum) {
      //    tryte mac [81] = 1;
      MAM2_TRITS_DEF(mac, MAM2_SPONGE_MAC_SIZE);
      // pb3_encode_ntrytes(mac, b);
      err_guard(trits_size(*b) >= MAM2_SPONGE_MAC_SIZE, err_pb3_eof);
      b0 = trits_take(*b, MAM2_SPONGE_MAC_SIZE);
      sponge_squeeze(s, MAM2_SPONGE_CTL_HASH, mac);
      *b = trits_drop(*b, MAM2_SPONGE_MAC_SIZE);

      err_guard(trits_cmp_eq(mac, b0), err_pb3_bad_mac);
    } else if (2 == checksum) {
      //    trytes sig = 2;
      size_t n;
      trits_t sig;
      MAM2_TRITS_DEF(H, MAM2_MSS_HASH_SIZE);

      // manually `pb3_decode_trytes`
      err_bind(pb3_decode_sizet(&n, b));
      err_guard(trits_size(*b) >= n, err_pb3_eof);
      sig = trits_take(*b, n);
      *b = trits_drop(*b, n);

      sponge_squeeze(s, MAM2_SPONGE_CTL_HASH, H);

      // verify
      err_guard(mss_verify(cfg->ms, cfg->ws, H, sig, cfg->pk), err_pb3_bad_sig);
    } else
      //    null none = 0;
      ;

    e = err_ok;
  } while (0);

  return e;
}
