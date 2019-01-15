/*
 * Copyright (c) 2018 IOTA Stiftung
 * https://github.com/iotaledger/entangled
 *
 * MAM is based on an original implementation & specification by apmi.bsu.by
 * [ITSec Lab]
 *
 * Refer to the LICENSE file for licensing information
 */

/*!
\file mam.c
\brief MAM2 layer.
*/

#include <string.h>

#include "mam/v2/mam.h"
#include "mam/v2/pb3/pb3.h"

err_t mam_mss_create(mam_ialloc *ma, imss *m, iprng *p, mss_mt_height_t d,
                     trits_t N1, trits_t N2) {
  err_t e = err_internal_error;
  ialloc *a;
  MAM2_ASSERT(ma);
  MAM2_ASSERT(m);
  a = ma->a;

  do {
    err_bind(mss_create(a, m, d));

    m->N1 = trits_null();
    if (!trits_is_empty(N1)) {
      m->N1 = trits_alloc(a, trits_size(N1));
      err_guard(!trits_is_null(m->N1), err_bad_alloc);
      trits_copy(N1, m->N1);
    }

    m->N2 = trits_null();
    if (!trits_is_empty(N2)) {
      m->N2 = trits_alloc(a, trits_size(N2));
      err_guard(!trits_is_null(m->N2), err_bad_alloc);
      trits_copy(N2, m->N2);
    }

    m->sg->s = ma->create_sponge(a);
    err_guard(m->sg->s, err_bad_alloc);

    m->w = mam_alloc(a, sizeof(iwots));
    err_guard(m->w, err_bad_alloc);
    err_bind(wots_create(a, m->w));

    m->w->sg->s = ma->create_sponge(a);
    err_guard(m->w->sg->s, err_bad_alloc);
    wots_init(m->w, m->w->sg->s);

    mss_init(m, p, m->sg->s, m->w, d, m->N1, m->N2);

    e = err_ok;
  } while (0);

  return e;
}

void mam_mss_destroy(mam_ialloc *ma, imss *m) {
  ialloc *a;
  MAM2_ASSERT(ma);
  MAM2_ASSERT(m);
  a = ma->a;

  m->p = 0;

  trits_free(a, m->N1);
  trits_free(a, m->N2);

  if (m->w) {
    ma->destroy_sponge(a, m->w->sg->s);
    m->w->sg->s = 0;
  }
  wots_destroy(a, m->w);
  m->w = 0;

  ma->destroy_sponge(a, m->sg->s);
  m->sg->s = 0;

  mss_destroy(a, m);
}

trits_t mam_channel_id(mam_channel *ch) {
  return trits_from_rep(MAM2_CHANNEL_ID_SIZE, ch->id);
}
trits_t mam_channel_name(mam_channel *ch) { return ch->m->N1; }
static size_t mam_channel_sig_size(mam_channel *ch) {
  return MAM2_MSS_SIG_SIZE(ch->m->d);
}

err_t mam_channel_create(mam_ialloc *ma, /*!< [in] Allocator. */
                         iprng *p,       /*! [in] Shared PRNG interface used to
                                            generate WOTS private keys. */
                         mss_mt_height_t d, /*!< [in] MSS MT height. */
                         trits_t ch_name,   /*!< [in] Channel name. */
                         mam_channel *ch    /*!< [out] Channel. */
) {
  err_t e = err_internal_error;

  MAM2_ASSERT(ma);
  MAM2_ASSERT(ch);

  do {
    err_bind(mam_mss_create(ma, ch->m, p, d, ch_name, trits_null()));
    mss_gen(ch->m, mam_channel_id(ch));

    e = err_ok;
  } while (0);

  return e;
}

void mam_channel_destroy(mam_ialloc *ma, /*!< [in] Allocator. */
                         mam_channel *ch /*!< [out] Channel. */
) {
  MAM2_ASSERT(ma);
  MAM2_ASSERT(ch);

  mam_mss_destroy(ma, ch->m);
}

trits_t mam_endpoint_id(mam_endpoint *ep) {
  return trits_from_rep(MAM2_ENDPOINT_ID_SIZE, ep->id);
}
trits_t mam_endpoint_chname(mam_endpoint *ep) { return ep->m->N1; }
trits_t mam_endpoint_name(mam_endpoint *ep) { return ep->m->N2; }
static size_t mam_endpoint_sig_size(mam_endpoint *ep) {
  return MAM2_MSS_SIG_SIZE(ep->m->d);
}

err_t mam_endpoint_create(mam_ialloc *ma, /*!< [in] Allocator. */
                          iprng *p,       /*! [in] Shared PRNG interface used to
                                             generate WOTS private keys. */
                          mss_mt_height_t d, /*!< [in] MSS MT height. */
                          trits_t ch_name,   /*!< [in] Channel name. */
                          trits_t ep_name,   /*!< [in] Endpoint name. */
                          mam_endpoint *ep   /*!< [out] Endpoint. */
) {
  err_t e = err_internal_error;

  MAM2_ASSERT(ma);
  MAM2_ASSERT(ep);

  do {
    err_bind(mam_mss_create(ma, ep->m, p, d, ch_name, ep_name));
    mss_gen(ep->m, mam_endpoint_id(ep));

    e = err_ok;
  } while (0);

  return e;
}

void mam_endpoint_destroy(mam_ialloc *ma,  /*!< [in] Allocator. */
                          mam_endpoint *ep /*!< [out] Endpoint. */
) {
  MAM2_ASSERT(ma);
  MAM2_ASSERT(ep);

  mam_mss_destroy(ma, ep->m);
}

trits_t mam_psk_id(mam_psk *p) {
  return trits_from_rep(MAM2_PSK_ID_SIZE, p->id);
}
trits_t mam_psk_trits(mam_psk *p) {
  return trits_from_rep(MAM2_PSK_SIZE, p->psk);
}

trits_t mam_ntru_pk_trits(mam_ntru_pk *p) {
  return trits_from_rep(MAM2_NTRU_PK_SIZE, p->pk);
}
trits_t mam_ntru_pk_id(mam_ntru_pk *p) {
  return trits_from_rep(MAM2_NTRU_ID_SIZE, p->pk);
}

/* MAC, MSSig, SignedId */

static size_t mam_wrap_mac_size() {
  return 0
         /*  commit; */
         /*  squeeze tryte mac[81]; */
         + pb3_sizeof_ntrytes(81);
}
static void mam_wrap_mac(ispongos *s, trits_t *b) {
  MAM2_ASSERT(mam_wrap_mac_size() <= trits_size(*b));
  /*  commit; */
  spongos_commit(s);
  /*  squeeze tryte mac[81]; */
  pb3_wrap_squeeze_ntrytes(s, b, 81);
}
static err_t mam_unwrap_mac(ispongos *s, trits_t *b) {
  err_t e = err_internal_error;

  do {
    /*  commit; */
    spongos_commit(s);
    /*  squeeze tryte mac[81]; */
    err_bind(pb3_unwrap_squeeze_ntrytes(s, b, 81));
    e = err_ok;
  } while (0);

  return e;
}

static size_t mam_wrap_mssig_size(imss *m) {
  size_t const sz = MAM2_MSS_SIG_SIZE(m->d) / 3;
  return 0
         /*  commit; */
         /*  external squeeze tryte mac[78]; */
         + 0
         /*  absorb sizet sz; */
         + pb3_sizeof_sizet(sz)
         /*  skip tryte sig[sz]; */
         + pb3_sizeof_ntrytes(sz);
}
static void mam_wrap_mssig(ispongos *s, trits_t *b, imss *m) {
  MAM2_TRITS_DEF0(mac, MAM2_MSS_HASH_SIZE);
  size_t const sz = MAM2_MSS_SIG_SIZE(m->d) / 3;
  mac = MAM2_TRITS_INIT(mac, MAM2_MSS_HASH_SIZE);

  MAM2_ASSERT(mam_wrap_mssig_size(m) <= trits_size(*b));

  /*  commit; */
  spongos_commit(s);
  /*  external squeeze tryte mac[78]; */
  pb3_squeeze_external_ntrytes(s, mac);
  /*  absorb sizet sz; */
  pb3_wrap_absorb_sizet(s, b, sz);
  /*  skip tryte sig[sz]; */
  mss_sign(m, mac, pb3_trits_take(b, pb3_sizeof_ntrytes(sz)));
}
static err_t mam_unwrap_mssig(ispongos *s, trits_t *b, ispongos *ms,
                              ispongos *ws, trits_t pk) {
  err_t e = err_internal_error;
  bool_t r = 1;

  MAM2_TRITS_DEF0(mac, MAM2_MSS_HASH_SIZE);
  size_t sz;
  mac = MAM2_TRITS_INIT(mac, MAM2_MSS_HASH_SIZE);

  do {
    /*  commit; */
    spongos_commit(s);
    /*  external squeeze tryte mac[78]; */
    pb3_squeeze_external_ntrytes(s, mac);
    /*  absorb sizet sz; */
    err_bind(pb3_unwrap_absorb_sizet(s, b, &sz));
    /*  skip tryte sig[sz]; */
    err_guard(pb3_sizeof_ntrytes(sz) <= trits_size(*b), err_pb3_eof);
    r = mss_verify(ms, ws, mac, pb3_trits_take(b, pb3_sizeof_ntrytes(sz)), pk);
    err_guard(r, err_pb3_bad_sig);
    e = err_ok;
  } while (0);

  return e;
}

static size_t mam_wrap_signedid_size(imss *m) {
  size_t const sz = MAM2_MSS_SIG_SIZE(m->d) / 3;
  return 0
         /*  absorb tryte id[81]; */
         + pb3_sizeof_ntrytes(81)
         /*  MSSig mssig; */
         + mam_wrap_mssig_size(m);
}
static void mam_wrap_signedid(ispongos *s, trits_t *b, trits_t id, imss *m) {
  MAM2_ASSERT(mam_wrap_signedid_size(m) <= trits_size(*b));
  MAM2_ASSERT(pb3_sizeof_ntrytes(81) == trits_size(id));

  /*  absorb tryte id[81]; */
  pb3_wrap_absorb_ntrytes(s, b, id);
  /*  MSSig mssig; */
  mam_wrap_mssig(s, b, m);
}
static err_t mam_unwrap_signedid(ispongos *s, trits_t *b, trits_t id,
                                 ispongos *ms, ispongos *ws, trits_t pk) {
  err_t e = err_internal_error;
  bool_t r = 1;

  do {
    /*  absorb tryte id[81]; */
    MAM2_ASSERT(pb3_sizeof_ntrytes(81) == trits_size(id));
    err_bind(pb3_unwrap_absorb_ntrytes(s, b, id));
    /*  MSSig mssig; */
    err_bind(mam_unwrap_mssig(s, b, ms, ws, pk));
    e = err_ok;
  } while (0);

  return e;
}

/* Channel */

size_t mam_wrap_channel_size() {
  return 0
         /*  absorb tryte ver; */
         + pb3_sizeof_tryte()
         /*  absorb external tryte chid[81]; */
         + 0;
}
void mam_wrap_channel(ispongos *s, trits_t *b, tryte_t ver, trits_t chid) {
  MAM2_ASSERT(mam_wrap_channel_size() <= trits_size(*b));
  MAM2_ASSERT(pb3_sizeof_ntrytes(81) == trits_size(chid));

  /*  absorb tryte ver; */
  pb3_wrap_absorb_tryte(s, b, ver);
  /*  absorb external tryte chid[81]; */
  pb3_absorb_external_ntrytes(s, chid);
}
err_t mam_unwrap_channel(ispongos *s, trits_t *b, tryte_t *ver, trits_t chid) {
  err_t e = err_internal_error;
  do {
    /*  absorb tryte ver; */
    err_bind(pb3_unwrap_absorb_tryte(s, b, ver));
    /*  absorb external tryte chid[81]; */
    MAM2_ASSERT(pb3_sizeof_ntrytes(81) == trits_size(chid));
    pb3_absorb_external_ntrytes(s, chid);
    e = err_ok;
  } while (0);
  return e;
}

/* Endpoint */

size_t mam_wrap_pubkey_chid_size() {
  return 0
         /*  absorb null chid; */
         + 0;
}
void mam_wrap_pubkey_chid(ispongos *s, trits_t *b) { /*  absorb null chid; */
}
err_t mam_unwrap_pubkey_chid(ispongos *s, trits_t *b) {
  /*  absorb null chid; */
  return err_ok;
}

size_t mam_wrap_pubkey_epid_size() {
  return 0
         /*  absorb tryte epid[81]; */
         + pb3_sizeof_ntrytes(81);
}
void mam_wrap_pubkey_epid(ispongos *s, trits_t *b, trits_t epid) {
  MAM2_ASSERT(mam_wrap_pubkey_epid_size() <= trits_size(*b));
  MAM2_ASSERT(pb3_sizeof_ntrytes(81) == trits_size(epid));

  /*  absorb tryte epid[81]; */
  pb3_wrap_absorb_ntrytes(s, b, epid);
}
err_t mam_unwrap_pubkey_epid(ispongos *s, trits_t *b, trits_t epid) {
  err_t e = err_internal_error;
  do {
    /*  absorb tryte epid[81]; */
    MAM2_ASSERT(pb3_sizeof_ntrytes(81) == trits_size(epid));
    err_bind(pb3_unwrap_absorb_ntrytes(s, b, epid));
    e = err_ok;
  } while (0);
  return e;
}

size_t mam_wrap_pubkey_chid1_size(imss *m) { return mam_wrap_signedid_size(m); }
void mam_wrap_pubkey_chid1(ispongos *s, trits_t *b, trits_t chid1, imss *m) {
  mam_wrap_signedid(s, b, chid1, m);
}
err_t mam_unwrap_pubkey_chid1(ispongos *s, trits_t *b, trits_t chid1,
                              ispongos *ms, ispongos *ws, trits_t pk) {
  return mam_unwrap_signedid(s, b, chid1, ms, ws, pk);
}

size_t mam_wrap_pubkey_epid1_size(imss *m) { return mam_wrap_signedid_size(m); }
void mam_wrap_pubkey_epid1(ispongos *s, trits_t *b, trits_t epid1, imss *m) {
  mam_wrap_signedid(s, b, epid1, m);
}
err_t mam_unwrap_pubkey_epid1(ispongos *s, trits_t *b, trits_t epid1,
                              ispongos *ms, ispongos *ws, trits_t pk) {
  return mam_unwrap_signedid(s, b, epid1, ms, ws, pk);
}

/* Keyload */

size_t mam_wrap_keyload_plain_size() {
  return 0
         /*  absorb tryte key[81]; */
         + pb3_sizeof_ntrytes(81);
}
void mam_wrap_keyload_plain(ispongos *s, trits_t *b, trits_t key) {
  MAM2_ASSERT(mam_wrap_keyload_plain_size() <= trits_size(*b));
  MAM2_ASSERT(pb3_sizeof_ntrytes(81) == trits_size(key));

  /*  absorb tryte key[81]; */
  pb3_wrap_absorb_ntrytes(s, b, key);
}
err_t mam_unwrap_keyload_plain(ispongos *s, trits_t *b, trits_t key) {
  err_t e = err_internal_error;
  do {
    /*  absorb tryte key[81]; */
    MAM2_ASSERT(pb3_sizeof_ntrytes(81) == trits_size(key));
    err_bind(pb3_unwrap_absorb_ntrytes(s, b, key));
    e = err_ok;
  } while (0);
  return e;
}

size_t mam_wrap_keyload_psk_size() {
  return 0
         /*  absorb tryte id[27]; */
         + pb3_sizeof_ntrytes(27)
         /*  absorb external tryte psk[81]; */
         + 0
         /*  commit; */
         /*  crypt tryte ekey[81]; */
         + pb3_sizeof_ntrytes(81);
}
void mam_wrap_keyload_psk(ispongos *s, trits_t *b, trits_t key, trits_t id,
                          trits_t psk) {
  MAM2_ASSERT(mam_wrap_keyload_psk_size() <= trits_size(*b));
  MAM2_ASSERT(pb3_sizeof_ntrytes(81) == trits_size(key));
  MAM2_ASSERT(pb3_sizeof_ntrytes(27) == trits_size(id));
  MAM2_ASSERT(pb3_sizeof_ntrytes(81) == trits_size(psk));

  /*  absorb tryte id[27]; */
  pb3_wrap_absorb_ntrytes(s, b, id);
  /*  absorb external tryte psk[81]; */
  pb3_absorb_external_ntrytes(s, psk);
  /*  commit; */
  spongos_commit(s);
  /*  crypt tryte ekey[81]; */
  pb3_wrap_crypt_ntrytes(s, b, key);
}
err_t mam_unwrap_keyload_psk(ispongos *s, trits_t *b, trits_t key,
                             bool_t *key_found, trits_t pskid, trits_t psk) {
  err_t e = err_internal_error;
  MAM2_TRITS_DEF0(id, MAM2_PSK_ID_SIZE);
  id = MAM2_TRITS_INIT(id, MAM2_PSK_ID_SIZE);

  MAM2_ASSERT(key_found);
  MAM2_ASSERT(pb3_sizeof_ntrytes(81) == trits_size(key));
  MAM2_ASSERT(trits_is_empty(pskid) ||
              pb3_sizeof_ntrytes(81) == trits_size(psk));

  do {
    /*  absorb tryte id[27]; */
    err_bind(pb3_unwrap_absorb_ntrytes(s, b, id));

    /* TODO: err_t (*lookup_psk)(void *ctx, trits_t id); */
    if (trits_cmp_eq(pskid, id)) {
      err_guard(!*key_found, err_mam_keyload_overloaded);
      *key_found = 1;

      /*  absorb external tryte psk[81]; */
      pb3_absorb_external_ntrytes(s, psk);
      /*  commit; */
      spongos_commit(s);
      /*  crypt tryte ekey[81]; */
      err_bind(pb3_unwrap_crypt_ntrytes(s, b, key));
    } else { /* skip */
      err_guard(MAM2_SPONGE_KEY_SIZE <= trits_size(*b), err_pb3_eof);
      pb3_trits_take(b, MAM2_SPONGE_KEY_SIZE);
      /* spongos state is corrupted */
    }
    e = err_ok;
  } while (0);

  return e;
}

size_t mam_wrap_keyload_ntru_size() {
  return 0
         /*  absorb tryte id[27]; */
         + pb3_sizeof_ntrytes(27)
         /*  absorb tryte ekey[3072]; */
         + pb3_sizeof_ntrytes(3072);
}
void mam_wrap_keyload_ntru(ispongos *s, trits_t *b, trits_t key, trits_t pk,
                           iprng *p, ispongos *ns, trits_t N) {
  trits_t ekey;

  MAM2_ASSERT(mam_wrap_keyload_ntru_size() <= trits_size(*b));
  MAM2_ASSERT(MAM2_NTRU_KEY_SIZE == trits_size(key));
  MAM2_ASSERT(MAM2_NTRU_PK_SIZE == trits_size(pk));

  /*  absorb tryte id[27]; */
  pb3_wrap_absorb_ntrytes(s, b, trits_take(pk, pb3_sizeof_ntrytes(27)));
  /*  absorb tryte ekey[3072]; */
  ekey = pb3_trits_take(b, MAM2_NTRU_EKEY_SIZE);
  ntru_encr(pk, p, ns, key, N, ekey);
  spongos_absorb(s, ekey);
}
err_t mam_unwrap_keyload_ntru(ispongos *s, trits_t *b, trits_t key,
                              bool_t *key_found, trits_t pkid, intru *n,
                              ispongos *ns) {
  err_t e = err_internal_error;
  trits_t ekey;
  MAM2_TRITS_DEF0(id, 81);
  id = MAM2_TRITS_INIT(id, 81);

  MAM2_ASSERT(MAM2_NTRU_KEY_SIZE == trits_size(key));
  MAM2_ASSERT(MAM2_KEY_ID_SIZE == trits_size(pkid));

  do {
    /*  absorb tryte id[27]; */
    err_bind(pb3_unwrap_absorb_ntrytes(s, b, id));
    if (trits_cmp_eq(pkid, id)) {
      err_guard(!*key_found, err_mam_keyload_overloaded);
      *key_found = 1;

      /*  absorb tryte ekey[3072]; */
      err_guard(MAM2_NTRU_EKEY_SIZE <= trits_size(*b), err_pb3_eof);
      ekey = pb3_trits_take(b, MAM2_NTRU_EKEY_SIZE);
      err_guard(trits_cmp_eq(pkid, id), err_mam_keyload_irrelevant);
      err_guard(ntru_decr(n, ns, ekey, key), err_pb3_bad_ekey);
      spongos_absorb(s, ekey);
    } else { /* skip */
      err_guard(MAM2_NTRU_EKEY_SIZE <= trits_size(*b), err_pb3_eof);
      pb3_trits_take(b, MAM2_NTRU_EKEY_SIZE);
      /* spongos state is corrupted */
    }
    e = err_ok;
  } while (0);

  return e;
}

/* Packet */

size_t mam_wrap_checksum_none_size() {
  return 0
         /*  absorb null none; */
         + 0;
}
void mam_wrap_checksum_none(ispongos *s, trits_t *b) {
  /*  absorb null none; */
}
err_t mam_unwrap_checksum_none(ispongos *s, trits_t *b) {
  /*  absorb null none; */
  return err_ok;
}

size_t mam_wrap_checksum_mac_size() { return mam_wrap_mac_size(); }
void mam_wrap_checksum_mac(ispongos *s, trits_t *b) { mam_wrap_mac(s, b); }
err_t mam_unwrap_checksum_mac(ispongos *s, trits_t *b) {
  return mam_unwrap_mac(s, b);
}

size_t mam_wrap_checksum_mssig_size(imss *m) { return mam_wrap_mssig_size(m); }
void mam_wrap_checksum_mssig(ispongos *s, trits_t *b, imss *m) {
  mam_wrap_mssig(s, b, m);
}
err_t mam_unwrap_checksum_mssig(ispongos *s, trits_t *b, ispongos *ms,
                                ispongos *ws, trits_t pk) {
  return mam_unwrap_mssig(s, b, ms, ws, pk);
}

size_t mam_send_msg_size(mam_send_msg_context *cfg) {
  size_t sz = 0;

  MAM2_ASSERT(cfg && cfg->ch);

  /* channel */
  sz += mam_wrap_channel_size();

  /* endpoint */
  /*  absorb oneof pubkey */
  sz += pb3_sizeof_oneof();
  if (cfg->ch1) /*    SignedId chid1 = 2; */
    sz += mam_wrap_pubkey_chid1_size(cfg->ch->m);
  else if (cfg->ep1)
    /*    SignedId epid1 = 3; */
    sz += mam_wrap_pubkey_epid1_size(cfg->ch->m);
  else if (cfg->ep)
    /*    absorb tryte epid[81] = 1; */
    sz += mam_wrap_pubkey_epid_size();
  else
    /*    null chid = 0; */
    sz += mam_wrap_pubkey_chid_size();

  /* header */
  /*  absorb tryte nonce[27]; */
  sz += pb3_sizeof_ntrytes(MAM2_HEADER_NONCE_SIZE / 3);
  {
    size_t keyload_count = 0;
    mam_psk_node *ipsk;
    mam_ntru_pk_node *intru_pk;

    if (cfg->key_plain) {
      ++keyload_count;
      /*  absorb oneof */
      sz += pb3_sizeof_oneof();
      /*  message KeyloadPlain */
      sz += mam_wrap_keyload_plain_size();
    }

    for (ipsk = cfg->psks.begin; ipsk; ipsk = ipsk->next) {
      ++keyload_count;
      /*  absorb oneof */
      sz += pb3_sizeof_oneof();
      /*  message KeyloadPSK */
      sz += mam_wrap_keyload_psk_size();
    }

    for (intru_pk = cfg->ntru_pks.begin; intru_pk; intru_pk = intru_pk->next) {
      ++keyload_count;
      /*  absorb oneof */
      sz += pb3_sizeof_oneof();
      /*  message KeyloadNTRU */
      sz += mam_wrap_keyload_ntru_size();
    }

    /*  absorb repeated */
    sz += pb3_sizeof_repeated(keyload_count);
  }
  /*  external secret tryte key[81]; */
  sz += 0;

  /* packets */
  return sz;
}

static trits_t mam_send_msg_cfg_chid(mam_send_msg_context *cfg) {
  MAM2_ASSERT(cfg);
  MAM2_ASSERT(cfg->ch);
  return mam_channel_id(cfg->ch);
}
static trits_t mam_send_msg_cfg_chid1(mam_send_msg_context *cfg) {
  MAM2_ASSERT(cfg);
  MAM2_ASSERT(cfg->ch1);
  return mam_channel_id(cfg->ch1);
}
static trits_t mam_send_msg_cfg_epid(mam_send_msg_context *cfg) {
  MAM2_ASSERT(cfg);
  MAM2_ASSERT(cfg->ep);
  return mam_endpoint_id(cfg->ep);
}
static trits_t mam_send_msg_cfg_epid1(mam_send_msg_context *cfg) {
  MAM2_ASSERT(cfg);
  MAM2_ASSERT(cfg->ep1);
  return mam_endpoint_id(cfg->ep1);
}
static trits_t mam_send_msg_cfg_nonce(mam_send_msg_context *cfg) {
  MAM2_ASSERT(cfg);
  return trits_from_rep(MAM2_HEADER_NONCE_SIZE, cfg->nonce);
}
static trits_t mam_send_msg_cfg_key(mam_send_msg_context *cfg) {
  MAM2_ASSERT(cfg);
  return trits_from_rep(MAM2_SPONGE_KEY_SIZE, cfg->key);
}

void mam_send_msg(mam_send_msg_context *cfg, trits_t *b) {
  ispongos *s;
  ispongos *fork;

  MAM2_TRITS_DEF0(skn, MAM2_MSS_SKN_SIZE);
  skn = MAM2_TRITS_INIT(skn, MAM2_MSS_SKN_SIZE);

  MAM2_ASSERT(cfg);
  MAM2_ASSERT(cfg->ch);
  MAM2_ASSERT(cfg->s->s);
  MAM2_ASSERT(cfg->fork->s);
  MAM2_ASSERT(b);
  s = cfg->s;
  fork = cfg->fork;

  MAM2_ASSERT(!(trits_size(*b) < mam_send_msg_size(cfg)));

  if (cfg->ep)
    mss_skn(cfg->ep->m, skn);
  else
    mss_skn(cfg->ch->m, skn);
  /* generate session key */
  prng_gen3(cfg->rng, MAM2_PRNG_DST_SECKEY, mam_channel_name(cfg->ch),
            cfg->ep ? mam_endpoint_name(cfg->ep) : trits_null(), skn,
            mam_send_msg_cfg_key(cfg));

  /* choose recipient */

  spongos_init(s);

  /* wrap Channel */
  mam_wrap_channel(s, b, 0, mam_send_msg_cfg_chid(cfg));

  /* wrap Endpoint */
  {
    tryte_t pubkey;

    if (cfg->ch1) { /*  SignedId chid1 = 2; */
      pubkey = (tryte_t)mam_msg_pubkey_chid1;
      pb3_wrap_absorb_tryte(s, b, pubkey);
      mam_wrap_pubkey_chid1(s, b, mam_send_msg_cfg_chid1(cfg), cfg->ch->m);
    } else if (cfg->ep1) { /*  SignedId epid1 = 3; */
      pubkey = (tryte_t)mam_msg_pubkey_epid1;
      pb3_wrap_absorb_tryte(s, b, pubkey);
      mam_wrap_pubkey_epid1(s, b, mam_send_msg_cfg_epid1(cfg), cfg->ch->m);
    } else if (cfg->ep) { /*  absorb tryte epid[81] = 1; */
      pubkey = (tryte_t)mam_msg_pubkey_epid;
      pb3_wrap_absorb_tryte(s, b, pubkey);
      mam_wrap_pubkey_epid(s, b, mam_send_msg_cfg_epid(cfg));
    } else { /*  absorb null chid = 0; */
      pubkey = (tryte_t)mam_msg_pubkey_chid;
      pb3_wrap_absorb_tryte(s, b, pubkey);
      mam_wrap_pubkey_chid(s, b);
    }
  }

  /* wrap Header */
  {
    /*  absorb tryte nonce[27]; */
    pb3_wrap_absorb_ntrytes(s, b, mam_send_msg_cfg_nonce(cfg));

    {
      size_t keyload_count = 0;
      tryte_t keyload;
      mam_psk_node *ipsk;
      mam_ntru_pk_node *intru_pk;

      if (cfg->key_plain) ++keyload_count;
      for (ipsk = cfg->psks.begin; ipsk; ipsk = ipsk->next) ++keyload_count;
      for (intru_pk = cfg->ntru_pks.begin; intru_pk; intru_pk = intru_pk->next)
        ++keyload_count;

      /*  repeated */
      pb3_wrap_absorb_sizet(s, b, keyload_count);

      if (cfg->key_plain) {
        /*  absorb oneof keyload */
        keyload = (tryte_t)mam_msg_keyload_plain;
        pb3_wrap_absorb_tryte(s, b, keyload);
        /*  fork; */
        spongos_fork(s, fork);
        /*  KeyloadPlain plain = 0; */
        mam_wrap_keyload_plain(fork, b, mam_send_msg_cfg_key(cfg));
      }

      for (ipsk = cfg->psks.begin; ipsk; ipsk = ipsk->next) {
        /*  absorb oneof keyload */
        keyload = (tryte_t)mam_msg_keyload_psk;
        pb3_wrap_absorb_tryte(s, b, keyload);
        /*  fork; */
        spongos_fork(s, fork);
        /*  KeyloadPSK psk = 1; */
        mam_wrap_keyload_psk(fork, b, mam_send_msg_cfg_key(cfg),
                             mam_psk_id(&ipsk->info),
                             mam_psk_trits(&ipsk->info));
      }

      for (intru_pk = cfg->ntru_pks.begin; intru_pk;
           intru_pk = intru_pk->next) {
        /*  absorb oneof keyload */
        keyload = (tryte_t)mam_msg_keyload_ntru;
        pb3_wrap_absorb_tryte(s, b, keyload);
        /*  fork; */
        spongos_fork(s, fork);
        /*  KeyloadNTRU ntru = 2; */
        mam_wrap_keyload_ntru(fork, b, mam_send_msg_cfg_key(cfg),
                              mam_ntru_pk_trits(&intru_pk->info), cfg->rng,
                              cfg->ns, mam_send_msg_cfg_nonce(cfg));
      }
    }

    /*  absorb external tryte key[81]; */
    pb3_absorb_external_ntrytes(s, mam_send_msg_cfg_key(cfg));
    /*  commit; */
    spongos_commit(s);
  }

  trits_set_zero(mam_send_msg_cfg_key(cfg));
}

size_t mam_send_packet_size(mam_send_packet_context *cfg, size_t payload_size) {
  size_t sz = 0;
  MAM2_ASSERT(cfg);
  MAM2_ASSERT(0 == payload_size % 3);
  sz = 0
       /*  absorb long trint ord; */
       + pb3_sizeof_longtrint()
       /*  absorb tryte sz; */
       + pb3_sizeof_sizet(payload_size / 3)
       /*  crypt tryte payload[sz]; */
       + pb3_sizeof_ntrytes(payload_size / 3)
       /*  absorb oneof checksum */
       + pb3_sizeof_oneof();

  if (mam_msg_checksum_none == cfg->checksum) /*    absorb null none = 0; */
    sz += mam_wrap_checksum_none_size();
  else if (mam_msg_checksum_mac == cfg->checksum)
    /*    MAC mac = 1; */
    sz += mam_wrap_checksum_mac_size();
  else if (mam_msg_checksum_mssig == cfg->checksum) {
    MAM2_ASSERT(cfg->m);
    /*    MSSig mssig = 2; */
    sz += mam_wrap_checksum_mssig_size(cfg->m);
  } else
    MAM2_ASSERT(0);
  /*  commit; */
  return sz;
}

void mam_send_packet(mam_send_packet_context *cfg, trits_t payload,
                     trits_t *b) {
  ispongos *s;
  tryte_t checksum;

  MAM2_ASSERT(cfg);
  MAM2_ASSERT(cfg->s->s);
  MAM2_ASSERT(b);
  s = cfg->s;

  MAM2_ASSERT(
      !(trits_size(*b) < mam_send_packet_size(cfg, trits_size(payload))));

  /*  absorb long trint ord; */
  pb3_wrap_absorb_longtrint(s, b, cfg->ord);

  /*  absorb tryte sz; */
  pb3_wrap_absorb_sizet(s, b, trits_size(payload) / 3);
  /*  crypt tryte payload[sz]; */
  pb3_wrap_crypt_ntrytes(s, b, payload);

  /*  absorb oneof checksum */
  checksum = (tryte_t)cfg->checksum;
  pb3_wrap_absorb_tryte(s, b, checksum);
  if (mam_msg_checksum_none == cfg->checksum) /*    absorb null none = 0; */
    mam_wrap_checksum_none(s, b);
  else if (mam_msg_checksum_mac == cfg->checksum)
    /*    MAC mac = 1; */
    mam_wrap_checksum_mac(s, b);
  else if (mam_msg_checksum_mssig == cfg->checksum)
    /*    MSSig mssig = 2; */
    mam_wrap_checksum_mssig(s, b, cfg->m);
  else
    MAM2_ASSERT(0);

  /*  commit; */
  spongos_commit(s);
}

static trits_t mam_recv_msg_cfg_chid(mam_recv_msg_context *cfg) {
  MAM2_ASSERT(cfg);
  MAM2_ASSERT(cfg->chid);
  return trits_from_rep(MAM2_CHANNEL_ID_SIZE, cfg->chid);
}
static trits_t mam_recv_msg_cfg_chid1(mam_recv_msg_context *cfg) {
  MAM2_ASSERT(cfg);
  MAM2_ASSERT(cfg->chid1);
  return trits_from_rep(MAM2_CHANNEL_ID_SIZE, cfg->chid1);
}
static trits_t mam_recv_msg_cfg_epid(mam_recv_msg_context *cfg) {
  MAM2_ASSERT(cfg);
  MAM2_ASSERT(cfg->epid);
  return trits_from_rep(MAM2_ENDPOINT_ID_SIZE, cfg->epid);
}
static trits_t mam_recv_msg_cfg_epid1(mam_recv_msg_context *cfg) {
  MAM2_ASSERT(cfg);
  MAM2_ASSERT(cfg->epid1);
  return trits_from_rep(MAM2_ENDPOINT_ID_SIZE, cfg->epid1);
}
static trits_t mam_recv_msg_cfg_nonce(mam_recv_msg_context *cfg) {
  MAM2_ASSERT(cfg);
  return trits_from_rep(MAM2_HEADER_NONCE_SIZE, cfg->nonce);
}
static trits_t mam_recv_msg_cfg_key(mam_recv_msg_context *cfg) {
  MAM2_ASSERT(cfg);
  return trits_from_rep(MAM2_SPONGE_KEY_SIZE, cfg->key);
}
static trits_t mam_recv_msg_cfg_psk_id(mam_recv_msg_context *cfg) {
  MAM2_ASSERT(cfg);
  return trits_from_rep(MAM2_PSK_ID_SIZE, cfg->psk_id);
}
static trits_t mam_recv_msg_cfg_ntru_id(mam_recv_msg_context *cfg) {
  MAM2_ASSERT(cfg);
  return trits_from_rep(MAM2_NTRU_ID_SIZE, cfg->ntru_id);
}

err_t mam_recv_msg(mam_recv_msg_context *cfg, trits_t *b) {
  err_t e = err_internal_error;

  ispongos *s;
  ispongos *fork;

  MAM2_ASSERT(cfg);
  MAM2_ASSERT(cfg->chid);
  MAM2_ASSERT(cfg->chid1);
  MAM2_ASSERT(cfg->epid);
  MAM2_ASSERT(cfg->epid1);
  MAM2_ASSERT(cfg->ma);
  MAM2_ASSERT(cfg->s);
  s = cfg->s;
  fork = cfg->fork;

  do {
    spongos_init(s);

    /* unwrap Channel */
    {
      tryte_t ver = -1;
      err_bind(mam_unwrap_channel(s, b, &ver, mam_recv_msg_cfg_chid(cfg)));
      err_guard(0 == ver, err_mam_version_not_supported);
    }

    /* unwrap Endpoint */
    {
      tryte_t pubkey = -1;
      err_bind(pb3_unwrap_absorb_tryte(s, b, &pubkey));
      err_guard(0 <= pubkey && pubkey <= 3, err_pb3_bad_oneof);
      cfg->pubkey = (mam_msg_pubkey_t)pubkey;

      if (mam_msg_pubkey_chid1 == pubkey) { /*  SignedId chid1 = 2; */
        /*TODO: verify chid is trusted */
        err_bind(mam_unwrap_pubkey_chid1(s, b, mam_recv_msg_cfg_chid1(cfg),
                                         cfg->ms, cfg->ws,
                                         mam_recv_msg_cfg_chid(cfg)));
        /*TODO: record new channel/endpoint */
      } else if (mam_msg_pubkey_epid1 == pubkey) { /*  SignedId epid1 = 3; */
        /*TODO: verify chid is trusted */
        err_bind(mam_unwrap_pubkey_epid1(s, b, mam_recv_msg_cfg_epid1(cfg),
                                         cfg->ms, cfg->ws,
                                         mam_recv_msg_cfg_chid(cfg)));
        /*TODO: record new channel/endpoint */
      } else if (mam_msg_pubkey_epid ==
                 pubkey) { /*  absorb tryte epid[81] = 1; */
        err_bind(mam_unwrap_pubkey_epid(s, b, mam_recv_msg_cfg_epid(cfg)));
      } else if (mam_msg_pubkey_chid == pubkey) { /*  absorb null chid = 0; */
        err_bind(mam_unwrap_pubkey_chid(s, b));
      } else
        MAM2_ASSERT(0);
    }

    /* unwrap Header */
    {
      /*  absorb tryte nonce[27]; */
      err_bind(pb3_unwrap_absorb_ntrytes(s, b, mam_recv_msg_cfg_nonce(cfg)));

      {
        /*  repeated */
        size_t keyload_count = 0;
        bool_t key_found = 0;

        err_bind(pb3_unwrap_absorb_sizet(s, b, &keyload_count));

        for (e = err_ok; e == err_ok && keyload_count--;) {
          tryte_t keyload = -1;

          /*  absorb oneof keyload */
          err_bind(pb3_unwrap_absorb_tryte(s, b, &keyload));
          err_guard(0 <= keyload && keyload <= 2, err_pb3_bad_oneof);
          /*  fork; */
          spongos_fork(s, fork);

          if (mam_msg_keyload_plain == keyload) { /*  KeyloadPlain plain = 0; */
            err_bind(
                mam_unwrap_keyload_plain(fork, b, mam_recv_msg_cfg_key(cfg)));
            err_guard(!key_found, err_mam_keyload_overloaded);
            key_found = 1;
          } else if (mam_msg_keyload_psk ==
                     keyload) { /*  KeyloadPSK psk = 1; */
            if (cfg->psk)
              err_bind(mam_unwrap_keyload_psk(
                  fork, b, mam_recv_msg_cfg_key(cfg), &key_found,
                  mam_psk_id(cfg->psk), mam_psk_trits(cfg->psk)));
          } else if (mam_msg_keyload_ntru ==
                     keyload) { /*  KeyloadNTRU ntru = 2; */
            if (cfg->ntru)
              err_bind(mam_unwrap_keyload_ntru(
                  fork, b, mam_recv_msg_cfg_key(cfg), &key_found,
                  ntru_id_trits(cfg->ntru), cfg->ntru, cfg->ns));
          } else
            err_guard(0, err_pb3_bad_oneof);
        }

        err_guard(key_found, err_mam_keyload_irrelevant);
      }
    }

    /*  absorb external tryte key[81]; */
    pb3_absorb_external_ntrytes(s, mam_recv_msg_cfg_key(cfg));
    /*  commit; */
    spongos_commit(s);

    e = err_ok;
  } while (0);

  return e;
}

err_t mam_recv_packet(mam_recv_packet_context *cfg, trits_t *b,
                      trits_t *payload) {
  err_t e = err_internal_error;
  ialloc *a;
  ispongos *s;
  trits_t p = trits_null();

  MAM2_ASSERT(cfg);
  MAM2_ASSERT(cfg->s);
  MAM2_ASSERT(cfg->ma);
  MAM2_ASSERT(b);
  MAM2_ASSERT(payload);
  s = cfg->s;
  a = cfg->ma->a;

  do {
    size_t sz = 0;
    tryte_t checksum = -1;

    /*  absorb long trint ord; */
    err_bind(pb3_unwrap_absorb_longtrint(s, b, &cfg->ord));
    /*TODO: check ord */

    /*  absorb tryte sz; */
    err_bind(pb3_unwrap_absorb_sizet(s, b, &sz));
    /*  crypt tryte payload[sz]; */
    if (trits_is_null(*payload)) {
      p = trits_alloc(a, pb3_sizeof_ntrytes(sz));
      err_guard(!trits_is_null(p), err_bad_alloc);
    } else {
      err_guard(trits_size(*payload) <= pb3_sizeof_ntrytes(sz),
                err_buffer_too_small);
      p = pb3_trits_take(payload, pb3_sizeof_ntrytes(sz));
    }
    err_bind(pb3_unwrap_crypt_ntrytes(s, b, p));

    /*  absorb oneof checksum */
    err_bind(pb3_unwrap_absorb_tryte(s, b, &checksum));

    if (mam_msg_checksum_none == checksum) {
      /*    absorb null none = 0; */
      err_bind(mam_unwrap_checksum_none(s, b));
    } else if (mam_msg_checksum_mac == checksum) {
      /*    MAC mac = 1; */
      err_bind(mam_unwrap_checksum_mac(s, b));
    } else if (mam_msg_checksum_mssig == checksum) {
      /*    MSSig mssig = 2; */
      err_bind(mam_unwrap_checksum_mssig(s, b, cfg->ms, cfg->ws, cfg->pk));
    } else {
      err_guard(0, err_pb3_bad_oneof);
    }

    /*  commit; */
    spongos_commit(s);

    if (trits_is_null(*payload)) *payload = p;
    p = trits_null();

    e = err_ok;
  } while (0);

  trits_free(a, p);
  return e;
}

static bool_t mam_test_generic(isponge *s, void *sponge_alloc_ctx,
                               isponge *(create_sponge)(void *ctx),
                               void (*destroy_sponge)(void *ctx, isponge *),
                               iprng *pa, iprng *pb) {
  err_t e = err_internal_error;
  mam_ialloc ma[1];
  ialloc *a = sponge_alloc_ctx;

  isponge *sa = 0, *forka = 0, *nsa = 0;
  isponge *sb = 0, *forkb = 0, *msb = 0, *wsb = 0, *nsb = 0;

  trits_t msg = trits_null(), packet = trits_null(), payload = trits_null();

  mss_mt_height_t d = 1;
  char const *cha_name_str = "CHANAME";
  char const *ch1a_name_str = "CHANAME9";
  char const *epa_name_str = "EPANAME";
  char const *ep1a_name_str = "EPANAME9";
  mam_channel *cha = 0, *ch1a = 0;
  mam_endpoint *epa = 0, *ep1a = 0;

  mam_psk_node pska[1], pskb[1];
  intru nb[1];
  mam_ntru_pk_node nbpk[1];

  mam_send_msg_context cfg_msga[1];
  mam_recv_msg_context cfg_msgb[1];
  mam_send_packet_context cfg_packeta[1];
  mam_recv_packet_context cfg_packetb[1];

  mam_msg_pubkey_t pubkey;     /* chid=0, epid=1, chid1=2, epid1=3 */
  mam_msg_keyload_t keyload;   /* plain=0, psk=1, ntru=2 */
  mam_msg_checksum_t checksum; /* none=0, mac=1, mssig=2 */

  /* init alloc */
  {
    ma->a = a;
    ma->create_sponge = create_sponge;
    ma->destroy_sponge = destroy_sponge;
  }
  /* create spongos */
  {
    sa = ma->create_sponge(ma->a);
    forka = ma->create_sponge(ma->a);
    nsa = ma->create_sponge(ma->a);

    msb = ma->create_sponge(ma->a);
    wsb = ma->create_sponge(ma->a);
    nsb = ma->create_sponge(ma->a);
    sb = ma->create_sponge(ma->a);
    forkb = ma->create_sponge(ma->a);
  }
  /* init rng */
  {
    MAM2_TRITS_DEF0(k, MAM2_PRNG_KEY_SIZE);
    k = MAM2_TRITS_INIT(k, MAM2_PRNG_KEY_SIZE);

    trits_from_str(k,
                   "SENDERPRNGKEYASENDERPRNGKEY"
                   "SENDERPRNGKEYASENDERPRNGKEY"
                   "SENDERPRNGKEYASENDERPRNGKEY");
    prng_init(pa, pa->s, k);

    trits_from_str(k,
                   "RECIPIPRNGKEYBRECIPIPRNGKEY"
                   "RECIPIPRNGKEYBRECIPIPRNGKEY"
                   "RECIPIPRNGKEYBRECIPIPRNGKEY");
    prng_init(pb, pb->s, k);
  }

  /* create channels */
  {
    trits_t cha_name = trits_alloc(a, 3 * strlen(cha_name_str));
    trits_from_str(cha_name, cha_name_str);

    cha = mam_alloc(ma->a, sizeof(mam_channel));
    MAM2_ASSERT(0 != cha);
    memset(cha, 0, sizeof(mam_channel));
    e = mam_channel_create(ma, pa, d, cha_name, cha);
    MAM2_ASSERT(err_ok == e);

    /* create endpoints */
    if (1) {
      trits_t epa_name = trits_alloc(a, 3 * strlen(epa_name_str));
      trits_from_str(epa_name, epa_name_str);

      epa = mam_alloc(ma->a, sizeof(mam_endpoint));
      MAM2_ASSERT(0 != epa);
      memset(epa, 0, sizeof(mam_endpoint));
      e = mam_endpoint_create(ma, pa, d, cha_name, epa_name, epa);
      MAM2_ASSERT(err_ok == e);
      trits_free(a, epa_name);
    }
    if (1) {
      trits_t ep1a_name = trits_alloc(a, 3 * strlen(ep1a_name_str));
      trits_from_str(ep1a_name, ep1a_name_str);

      ep1a = mam_alloc(ma->a, sizeof(mam_endpoint));
      MAM2_ASSERT(0 != ep1a);
      memset(ep1a, 0, sizeof(mam_endpoint));
      e = mam_endpoint_create(ma, pa, d, cha_name, ep1a_name, ep1a);
      MAM2_ASSERT(err_ok == e);
      trits_free(a, ep1a_name);
    }
    trits_free(a, cha_name);
  }
  if (1) {
    trits_t ch1a_name = trits_alloc(a, 3 * strlen(ch1a_name_str));
    trits_from_str(ch1a_name, ch1a_name_str);

    ch1a = mam_alloc(ma->a, sizeof(mam_channel));
    MAM2_ASSERT(0 != ch1a);
    memset(ch1a, 0, sizeof(mam_channel));
    e = mam_channel_create(ma, pa, d, ch1a_name, ch1a);
    MAM2_ASSERT(err_ok == e);
    trits_free(a, ch1a_name);
  }
  /* gen psk */
  {
    pska->prev = pska->next = 0;
    trits_from_str(mam_psk_id(&pska->info), "PSKIDAPSKIDAPSKIDAPSKIDAPSK");
    prng_gen_str(pa, MAM2_PRNG_DST_SECKEY, "PSKANONCE",
                 mam_psk_trits(&pska->info));

    pskb->prev = pskb->next = 0;
    trits_from_str(mam_psk_id(&pskb->info), "PSKIDBPSKIDBPSKIDBPSKIDBPSK");
    prng_gen_str(pb, MAM2_PRNG_DST_SECKEY, "PSKBNONCE",
                 mam_psk_trits(&pskb->info));
  }
  /* gen recipient's ntru keys */
  {
    MAM2_TRITS_DEF0(N, 30);
    N = MAM2_TRITS_INIT(N, 30);
    trits_from_str(N, "NTRUBNONCE");

    e = ntru_create(a, nb);
    ntru_gen(nb, pb, N, mam_ntru_pk_trits(&nbpk->info));
    MAM2_ASSERT(err_ok == e);
  }

  for (pubkey = 0; pubkey < 4; ++pubkey) /* chid=0, epid=1, chid1=2, epid1=3 */
    for (keyload = 0; keyload < 3; ++keyload) /* plain=0, psk=1, ntru=2 */
      for (checksum = 0; checksum < 3; ++checksum)
      /* none=0, mac=1, mssig=2 */
      {
        /* init send msg context */
        {
          mam_send_msg_context *cfg = cfg_msga;

          cfg->ma = 0;
          cfg->s->s = sa;
          cfg->fork->s = forka;
          cfg->prng = pa;
          cfg->rng = pa;
          cfg->ns->s = nsa;

          cfg->ch = cha;
          cfg->ch1 = 0;
          cfg->ep = 0;
          cfg->ep1 = 0;
          if (mam_msg_pubkey_epid == pubkey)
            cfg->ep = epa;
          else if (mam_msg_pubkey_chid1 == pubkey)
            cfg->ch1 = ch1a;
          else if (mam_msg_pubkey_epid1 == pubkey)
            cfg->ep1 = ep1a;
          else
            ;

          cfg->key_plain = 0;
          cfg->psks.begin = 0;
          cfg->psks.end = 0;
          cfg->ntru_pks.begin = 0;
          cfg->ntru_pks.end = 0;
          if (mam_msg_keyload_plain == keyload)
            cfg->key_plain = 1;
          else if (mam_msg_keyload_psk == keyload) {
            pska->prev = pska->next = 0;
            pskb->prev = pskb->next = 0;
            mam_list_insert_end(cfg->psks, pska);
            mam_list_insert_end(cfg->psks, pskb);
          } else if (mam_msg_keyload_ntru == keyload) {
            nbpk->prev = nbpk->next = 0;
            mam_list_insert_end(cfg->ntru_pks, nbpk);
          } else
            ;

          trits_from_str(mam_send_msg_cfg_nonce(cfg),
                         "SENDERNONCEAAAAASENDERNONCE");
        }
        /* init recv msg context */
        {
          mam_recv_msg_context *cfg = cfg_msgb;

          cfg->ma = ma;
          cfg->pubkey = -1;
          cfg->s->s = sb;
          cfg->fork->s = forkb;
          cfg->ms->s = msb;
          cfg->ws->s = wsb;
          cfg->ns->s = nsb;

          cfg->psk = &pskb->info;
          cfg->ntru = nb;

          trits_copy(mam_channel_id(cha), mam_recv_msg_cfg_chid(cfg));
        }

        /* send/recv msg */
        {
          size_t sz;
          sz = mam_send_msg_size(cfg_msga);
          msg = trits_alloc(ma->a, sz);
          MAM2_ASSERT(!trits_is_null(msg));

          mam_send_msg(cfg_msga, &msg);
          MAM2_ASSERT(trits_is_empty(msg));
          msg = trits_pickup(msg, sz);

          e = mam_recv_msg(cfg_msgb, &msg);
          MAM2_ASSERT(err_ok == e);
          MAM2_ASSERT(trits_is_empty(msg));
        }

        /* init send packet context */
        {
          mam_send_packet_context *cfg = cfg_packeta;

          cfg->s->s = sa;
          cfg->s->pos = cfg_msga->s->pos;
          cfg->ord = 0;
          cfg->checksum = checksum;
          cfg->m = 0;
          if (mam_msg_checksum_mssig == cfg->checksum) {
            if (mam_msg_pubkey_chid == pubkey)
              cfg->m = cha->m;
            else if (mam_msg_pubkey_epid == pubkey)
              cfg->m = epa->m;
            else if (mam_msg_pubkey_chid1 == pubkey)
              cfg->m = ch1a->m;
            else if (mam_msg_pubkey_epid1 == pubkey)
              cfg->m = ep1a->m;
            else
              ;
          }
        }
        /* init recv packet context */
        {
          mam_recv_packet_context *cfg = cfg_packetb;

          cfg->ma = ma;
          cfg->s->s = sb;
          cfg->s->pos = cfg_msgb->s->pos;
          cfg->ord = -1;
          cfg->pk = trits_null();
          if (mam_msg_pubkey_chid == cfg_msgb->pubkey)
            cfg->pk = mam_recv_msg_cfg_chid(cfg_msgb);
          else if (mam_msg_pubkey_epid == cfg_msgb->pubkey)
            cfg->pk = mam_recv_msg_cfg_epid(cfg_msgb);
          else if (mam_msg_pubkey_chid1 == cfg_msgb->pubkey)
            cfg->pk = mam_recv_msg_cfg_chid1(cfg_msgb);
          else if (mam_msg_pubkey_epid1 == cfg_msgb->pubkey)
            cfg->pk = mam_recv_msg_cfg_chid1(cfg_msgb);
          else
            ;
          cfg->ms->s = msb;
          cfg->ws->s = wsb;
        }

        /* send/recv packet */
        {
          size_t sz;
          char const *payload_str = "PAYLOAD9999";

          payload = trits_alloc(a, 3 * strlen(payload_str));
          MAM2_ASSERT(!trits_is_null(payload));
          trits_from_str(payload, payload_str);

          sz = mam_send_packet_size(cfg_packeta, trits_size(payload));
          packet = trits_alloc(ma->a, sz);
          MAM2_ASSERT(!trits_is_null(packet));

          mam_send_packet(cfg_packeta, payload, &packet);
          MAM2_ASSERT(trits_is_empty(packet));
          packet = trits_pickup(packet, sz);
          trits_set_zero(payload);
          /*trits_free(a, payload);*/

          e = mam_recv_packet(cfg_packetb, &packet, &payload);
          MAM2_ASSERT(err_ok == e);
          MAM2_ASSERT(trits_is_empty(packet));
          MAM2_ASSERT(trits_is_empty(payload));
          payload = trits_pickup_all(payload);
          MAM2_ASSERT(trits_cmp_eq_str(payload, payload_str));
        }

        /* destroy trits */
        {
          trits_free(a, msg);
          trits_free(a, packet);
          trits_free(a, payload);
        }
      }

  /* destroy spongos */
  {
    ma->destroy_sponge(ma->a, sa);
    ma->destroy_sponge(ma->a, forka);
    ma->destroy_sponge(ma->a, nsa);

    ma->destroy_sponge(ma->a, sb);
    ma->destroy_sponge(ma->a, forkb);
    ma->destroy_sponge(ma->a, msb);
    ma->destroy_sponge(ma->a, wsb);
    ma->destroy_sponge(ma->a, nsb);
  }
  /* destroy channels/endpoints */
  {
    if (cha) mam_channel_destroy(ma, cha);
    if (ch1a) mam_channel_destroy(ma, ch1a);
    if (epa) mam_endpoint_destroy(ma, epa);
    if (ep1a) mam_endpoint_destroy(ma, ep1a);
  }

  return e == err_ok;
}

static bool_t mam_test_msg_chid_psk_one_packet_with_mac(
    isponge *s, void *sponge_alloc_ctx, isponge *(create_sponge)(void *ctx),
    void (*destroy_sponge)(void *ctx, isponge *), iprng *pa, iprng *pb) {
  err_t e = err_internal_error;
  mam_ialloc ma[1];
  ialloc *a = sponge_alloc_ctx;

  isponge *sa = 0, *forka = 0;
  isponge *sb = 0, *forkb = 0, *msb = 0, *wsb = 0;

  trits_t msg = trits_null(), packet = trits_null(), payload = trits_null();

  mss_mt_height_t d = 1;
  char const *cha_name_str = "CHANAME";
  mam_channel *cha = 0;

  mam_psk_node pska[1], pskb[1];

  mam_send_msg_context cfg_msga[1];
  mam_recv_msg_context cfg_msgb[1];
  mam_send_packet_context cfg_packeta[1];
  mam_recv_packet_context cfg_packetb[1];

  mam_msg_checksum_t checksum; /* none=0, mac=1, mssig=2 */

  /* init alloc */
  {
    ma->a = a;
    ma->create_sponge = create_sponge;
    ma->destroy_sponge = destroy_sponge;
  }
  /* create spongos */
  {
    sa = ma->create_sponge(ma->a);
    forka = ma->create_sponge(ma->a);

    msb = ma->create_sponge(ma->a);
    wsb = ma->create_sponge(ma->a);
    sb = ma->create_sponge(ma->a);
    forkb = ma->create_sponge(ma->a);
  }
  /* init rng */
  {
    MAM2_TRITS_DEF0(k, MAM2_PRNG_KEY_SIZE);
    k = MAM2_TRITS_INIT(k, MAM2_PRNG_KEY_SIZE);

    trits_from_str(k,
                   "SENDERPRNGKEYASENDERPRNGKEY"
                   "SENDERPRNGKEYASENDERPRNGKEY"
                   "SENDERPRNGKEYASENDERPRNGKEY");
    prng_init(pa, pa->s, k);

    trits_from_str(k,
                   "RECIPIPRNGKEYBRECIPIPRNGKEY"
                   "RECIPIPRNGKEYBRECIPIPRNGKEY"
                   "RECIPIPRNGKEYBRECIPIPRNGKEY");
    prng_init(pb, pb->s, k);
  }

  /* create channels */
  {
    trits_t cha_name = trits_alloc(a, 3 * strlen(cha_name_str));
    trits_from_str(cha_name, cha_name_str);

    cha = mam_alloc(ma->a, sizeof(mam_channel));
    MAM2_ASSERT(0 != cha);
    memset(cha, 0, sizeof(mam_channel));
    e = mam_channel_create(ma, pa, d, cha_name, cha);
    MAM2_ASSERT(err_ok == e);

    trits_free(a, cha_name);
  }
  /* gen psk */
  {
    pska->prev = pska->next = 0;
    trits_from_str(mam_psk_id(&pska->info), "PSKIDAPSKIDAPSKIDAPSKIDAPSK");
    prng_gen_str(pa, MAM2_PRNG_DST_SECKEY, "PSKANONCE",
                 mam_psk_trits(&pska->info));

    pskb->prev = pskb->next = 0;
    trits_from_str(mam_psk_id(&pskb->info), "PSKIDBPSKIDBPSKIDBPSKIDBPSK");
    prng_gen_str(pb, MAM2_PRNG_DST_SECKEY, "PSKBNONCE",
                 mam_psk_trits(&pskb->info));
  }

  checksum = mam_msg_checksum_mac;
  /* none=0, mac=1, mssig=2 */
  {/* init send msg context */
   {mam_send_msg_context *cfg = cfg_msga;

  cfg->ma = 0;
  cfg->s->s = sa;
  cfg->fork->s = forka;
  cfg->prng = pa;
  cfg->rng = pa;

  cfg->ch = cha;
  cfg->ch1 = 0;
  cfg->ep = 0;
  cfg->ep1 = 0;

  cfg->key_plain = 0;
  cfg->psks.begin = 0;
  cfg->psks.end = 0;
  cfg->ntru_pks.begin = 0;
  cfg->ntru_pks.end = 0;
  {
    pska->prev = pska->next = 0;
    pskb->prev = pskb->next = 0;
    mam_list_insert_end(cfg->psks, pska);
    mam_list_insert_end(cfg->psks, pskb);
  }

  trits_from_str(mam_send_msg_cfg_nonce(cfg), "SENDERNONCEAAAAASENDERNONCE");
}
/* init recv msg context */
{
  mam_recv_msg_context *cfg = cfg_msgb;

  cfg->ma = ma;
  cfg->pubkey = -1;
  cfg->s->s = sb;
  cfg->fork->s = forkb;
  cfg->ms->s = msb;
  cfg->ws->s = wsb;
  cfg->ns->s = 0;

  cfg->psk = &pskb->info;
  cfg->ntru = 0;

  trits_copy(mam_channel_id(cha), mam_recv_msg_cfg_chid(cfg));
}

/* send/recv msg */
{
  size_t sz;
  sz = mam_send_msg_size(cfg_msga);
  msg = trits_alloc(ma->a, sz);
  MAM2_ASSERT(!trits_is_null(msg));

  mam_send_msg(cfg_msga, &msg);
  MAM2_ASSERT(trits_is_empty(msg));
  msg = trits_pickup(msg, sz);

  e = mam_recv_msg(cfg_msgb, &msg);
  MAM2_ASSERT(err_ok == e);
  MAM2_ASSERT(trits_is_empty(msg));
}

/* init send packet context */
{
  mam_send_packet_context *cfg = cfg_packeta;

  cfg->s->s = sa;
  cfg->s->pos = cfg_msga->s->pos;
  cfg->ord = 0;
  cfg->checksum = checksum;
  cfg->m = 0;
}
/* init recv packet context */
{
  mam_recv_packet_context *cfg = cfg_packetb;

  cfg->ma = ma;
  cfg->s->s = sb;
  cfg->s->pos = cfg_msgb->s->pos;
  cfg->ord = -1;
  cfg->pk = trits_null();
  MAM2_ASSERT(mam_msg_pubkey_chid == cfg_msgb->pubkey);
  cfg->pk = mam_recv_msg_cfg_chid(cfg_msgb);
  cfg->ms->s = msb;
  cfg->ws->s = wsb;
}

/* send/recv packet */
{
  size_t sz;
  char const *payload_str = "PAYLOAD9999";

  payload = trits_alloc(a, 3 * strlen(payload_str));
  MAM2_ASSERT(!trits_is_null(payload));
  trits_from_str(payload, payload_str);

  sz = mam_send_packet_size(cfg_packeta, trits_size(payload));
  packet = trits_alloc(ma->a, sz);
  MAM2_ASSERT(!trits_is_null(packet));

  mam_send_packet(cfg_packeta, payload, &packet);
  MAM2_ASSERT(trits_is_empty(packet));
  packet = trits_pickup(packet, sz);
  trits_set_zero(payload);
  /*trits_free(a, payload);*/

  e = mam_recv_packet(cfg_packetb, &packet, &payload);
  MAM2_ASSERT(err_ok == e);
  MAM2_ASSERT(trits_is_empty(packet));
  MAM2_ASSERT(trits_is_empty(payload));
  payload = trits_pickup_all(payload);
  MAM2_ASSERT(trits_cmp_eq_str(payload, payload_str));
}

/* destroy trits */
{
  trits_free(a, msg);
  trits_free(a, packet);
  trits_free(a, payload);
}
}

/* destroy spongos */
{
  ma->destroy_sponge(ma->a, sa);
  ma->destroy_sponge(ma->a, forka);

  ma->destroy_sponge(ma->a, sb);
  ma->destroy_sponge(ma->a, forkb);
  ma->destroy_sponge(ma->a, msb);
  ma->destroy_sponge(ma->a, wsb);
}
/* destroy channels/endpoints */
{
  if (cha) mam_channel_destroy(ma, cha);
}

return e == err_ok;
}

static bool_t mam_test_msg_epid_ntru_one_packet_with_sig(
    isponge *s, void *sponge_alloc_ctx, isponge *(create_sponge)(void *ctx),
    void (*destroy_sponge)(void *ctx, isponge *), iprng *pa, iprng *pb) {
  err_t e = err_internal_error;
  mam_ialloc ma[1];
  ialloc *a = sponge_alloc_ctx;

  isponge *sa = 0, *forka = 0, *nsa = 0;
  isponge *sb = 0, *forkb = 0, *msb = 0, *wsb = 0, *nsb = 0;

  trits_t msg = trits_null(), packet = trits_null(), payload = trits_null();

  mss_mt_height_t d = 1;
  char const *cha_name_str = "CHANAME";
  char const *epa_name_str = "EPANAME";
  mam_channel *cha = 0;
  mam_endpoint *epa = 0;

  intru nb[1];
  mam_ntru_pk_node nbpk[1];

  mam_send_msg_context cfg_msga[1];
  mam_recv_msg_context cfg_msgb[1];
  mam_send_packet_context cfg_packeta[1];
  mam_recv_packet_context cfg_packetb[1];

  mam_msg_checksum_t checksum; /* none=0, mac=1, mssig=2 */

  /* init alloc */
  {
    ma->a = a;
    ma->create_sponge = create_sponge;
    ma->destroy_sponge = destroy_sponge;
  }
  /* create spongos */
  {
    sa = ma->create_sponge(ma->a);
    forka = ma->create_sponge(ma->a);
    nsa = ma->create_sponge(ma->a);

    msb = ma->create_sponge(ma->a);
    wsb = ma->create_sponge(ma->a);
    nsb = ma->create_sponge(ma->a);
    sb = ma->create_sponge(ma->a);
    forkb = ma->create_sponge(ma->a);
  }
  /* init rng */
  {
    MAM2_TRITS_DEF0(k, MAM2_PRNG_KEY_SIZE);
    k = MAM2_TRITS_INIT(k, MAM2_PRNG_KEY_SIZE);

    trits_from_str(k,
                   "SENDERPRNGKEYASENDERPRNGKEY"
                   "SENDERPRNGKEYASENDERPRNGKEY"
                   "SENDERPRNGKEYASENDERPRNGKEY");
    prng_init(pa, pa->s, k);

    trits_from_str(k,
                   "RECIPIPRNGKEYBRECIPIPRNGKEY"
                   "RECIPIPRNGKEYBRECIPIPRNGKEY"
                   "RECIPIPRNGKEYBRECIPIPRNGKEY");
    prng_init(pb, pb->s, k);
  }

  /* create channels */
  {
    trits_t cha_name = trits_alloc(a, 3 * strlen(cha_name_str));
    trits_from_str(cha_name, cha_name_str);

    cha = mam_alloc(ma->a, sizeof(mam_channel));
    MAM2_ASSERT(0 != cha);
    memset(cha, 0, sizeof(mam_channel));
    e = mam_channel_create(ma, pa, d, cha_name, cha);
    MAM2_ASSERT(err_ok == e);

    /* create endpoints */
    if (1) {
      trits_t epa_name = trits_alloc(a, 3 * strlen(epa_name_str));
      trits_from_str(epa_name, epa_name_str);

      epa = mam_alloc(ma->a, sizeof(mam_endpoint));
      MAM2_ASSERT(0 != epa);
      memset(epa, 0, sizeof(mam_endpoint));
      e = mam_endpoint_create(ma, pa, d, cha_name, epa_name, epa);
      MAM2_ASSERT(err_ok == e);
      trits_free(a, epa_name);
    }
    trits_free(a, cha_name);
  }
  /* gen recipient's ntru keys */
  {
    MAM2_TRITS_DEF0(N, 30);
    N = MAM2_TRITS_INIT(N, 30);
    trits_from_str(N, "NTRUBNONCE");

    e = ntru_create(a, nb);
    ntru_gen(nb, pb, N, mam_ntru_pk_trits(&nbpk->info));
    MAM2_ASSERT(err_ok == e);
  }

  checksum = mam_msg_checksum_mssig;
  /* none=0, mac=1, mssig=2 */
  {/* init send msg context */
   {mam_send_msg_context *cfg = cfg_msga;

  cfg->ma = 0;
  cfg->s->s = sa;
  cfg->fork->s = forka;
  cfg->prng = pa;
  cfg->rng = pa;
  cfg->ns->s = nsa;

  cfg->ch = cha;
  cfg->ch1 = 0;
  cfg->ep = 0;
  cfg->ep1 = 0;
  cfg->ep = epa;

  cfg->key_plain = 0;
  cfg->psks.begin = 0;
  cfg->psks.end = 0;
  cfg->ntru_pks.begin = 0;
  cfg->ntru_pks.end = 0;
  {
    nbpk->prev = nbpk->next = 0;
    mam_list_insert_end(cfg->ntru_pks, nbpk);
  }

  trits_from_str(mam_send_msg_cfg_nonce(cfg), "SENDERNONCEAAAAASENDERNONCE");
}
/* init recv msg context */
{
  mam_recv_msg_context *cfg = cfg_msgb;

  cfg->ma = ma;
  cfg->pubkey = -1;
  cfg->s->s = sb;
  cfg->fork->s = forkb;
  cfg->ms->s = msb;
  cfg->ws->s = wsb;
  cfg->ns->s = nsb;

  cfg->psk = 0;
  cfg->ntru = nb;

  trits_copy(mam_channel_id(cha), mam_recv_msg_cfg_chid(cfg));
}

/* send/recv msg */
{
  size_t sz;
  sz = mam_send_msg_size(cfg_msga);
  msg = trits_alloc(ma->a, sz);
  MAM2_ASSERT(!trits_is_null(msg));

  mam_send_msg(cfg_msga, &msg);
  MAM2_ASSERT(trits_is_empty(msg));
  msg = trits_pickup(msg, sz);

  e = mam_recv_msg(cfg_msgb, &msg);
  MAM2_ASSERT(err_ok == e);
  MAM2_ASSERT(trits_is_empty(msg));
}

/* init send packet context */
{
  mam_send_packet_context *cfg = cfg_packeta;

  cfg->s->s = sa;
  cfg->s->pos = cfg_msga->s->pos;
  cfg->ord = 0;
  cfg->checksum = checksum;
  cfg->m = 0;
  { cfg->m = epa->m; }
}
/* init recv packet context */
{
  mam_recv_packet_context *cfg = cfg_packetb;

  cfg->ma = ma;
  cfg->s->s = sb;
  cfg->s->pos = cfg_msgb->s->pos;
  cfg->ord = -1;
  cfg->pk = trits_null();
  MAM2_ASSERT(mam_msg_pubkey_epid == cfg_msgb->pubkey);
  cfg->pk = mam_recv_msg_cfg_epid(cfg_msgb);
  cfg->ms->s = msb;
  cfg->ws->s = wsb;
}

/* send/recv packet */
{
  size_t sz;
  char const *payload_str = "PAYLOAD9999";

  payload = trits_alloc(a, 3 * strlen(payload_str));
  MAM2_ASSERT(!trits_is_null(payload));
  trits_from_str(payload, payload_str);

  sz = mam_send_packet_size(cfg_packeta, trits_size(payload));
  packet = trits_alloc(ma->a, sz);
  MAM2_ASSERT(!trits_is_null(packet));

  mam_send_packet(cfg_packeta, payload, &packet);
  MAM2_ASSERT(trits_is_empty(packet));
  packet = trits_pickup(packet, sz);
  trits_set_zero(payload);
  /*trits_free(a, payload);*/

  e = mam_recv_packet(cfg_packetb, &packet, &payload);
  MAM2_ASSERT(err_ok == e);
  MAM2_ASSERT(trits_is_empty(packet));
  MAM2_ASSERT(trits_is_empty(payload));
  payload = trits_pickup_all(payload);
  MAM2_ASSERT(trits_cmp_eq_str(payload, payload_str));
}

/* destroy trits */
{
  trits_free(a, msg);
  trits_free(a, packet);
  trits_free(a, payload);
}
}

/* destroy spongos */
{
  ma->destroy_sponge(ma->a, sa);
  ma->destroy_sponge(ma->a, forka);
  ma->destroy_sponge(ma->a, nsa);

  ma->destroy_sponge(ma->a, sb);
  ma->destroy_sponge(ma->a, forkb);
  ma->destroy_sponge(ma->a, msb);
  ma->destroy_sponge(ma->a, wsb);
  ma->destroy_sponge(ma->a, nsb);
}
/* destroy channels/endpoints */
{
  if (cha) mam_channel_destroy(ma, cha);
  if (epa) mam_endpoint_destroy(ma, epa);
}

return e == err_ok;
}

bool_t mam_test(isponge *s, void *sponge_alloc_ctx,
                isponge *(create_sponge)(void *ctx),
                void (*destroy_sponge)(void *ctx, isponge *), iprng *pa,
                iprng *pb) {
  bool_t r = 1;
  r = mam_test_msg_chid_psk_one_packet_with_mac(
          s, sponge_alloc_ctx, create_sponge, destroy_sponge, pa, pb) &&
      r;
  r = mam_test_msg_epid_ntru_one_packet_with_sig(
          s, sponge_alloc_ctx, create_sponge, destroy_sponge, pa, pb) &&
      r;
  r = mam_test_generic(s, sponge_alloc_ctx, create_sponge, destroy_sponge, pa,
                       pb) &&
      r;
  return r;
}
