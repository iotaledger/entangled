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

#include <stdlib.h>
#include <string.h>

#include "mam/v2/mam/mam.h"
#include "mam/v2/pb3/pb3.h"

retcode_t mam_mss_create(mam_ialloc_t *ma, mss_t *m, prng_t *p,
                         mss_mt_height_t d, trits_t N1, trits_t N2) {
  retcode_t e = RC_MAM2_INTERNAL_ERROR;
  MAM2_ASSERT(ma);
  MAM2_ASSERT(m);

  do {
    err_bind(mss_create(m, d));

    m->N1 = trits_null();
    if (!trits_is_empty(N1)) {
      m->N1 = trits_alloc(trits_size(N1));
      err_guard(!trits_is_null(m->N1), RC_OOM);
      trits_copy(N1, m->N1);
    }

    m->N2 = trits_null();
    if (!trits_is_empty(N2)) {
      m->N2 = trits_alloc(trits_size(N2));
      err_guard(!trits_is_null(m->N2), RC_OOM);
      trits_copy(N2, m->N2);
    }

    m->sg->sponge = ma->create_sponge();
    err_guard(m->sg->sponge, RC_OOM);

    m->w = malloc(sizeof(wots_t));
    err_guard(m->w, RC_OOM);
    err_bind(wots_create(m->w));

    m->w->sg->sponge = ma->create_sponge();
    err_guard(m->w->sg->sponge, RC_OOM);
    wots_init(m->w, m->w->sg->sponge);

    mss_init(m, p, m->sg->sponge, m->w, d, m->N1, m->N2);

    e = RC_OK;
  } while (0);

  return e;
}

void mam_mss_destroy(mam_ialloc_t *ma, mss_t *m) {
  MAM2_ASSERT(ma);
  MAM2_ASSERT(m);

  m->p = 0;

  trits_free(m->N1);
  trits_free(m->N2);

  if (m->w) {
    ma->destroy_sponge(m->w->sg->sponge);
    m->w->sg->sponge = 0;
  }
  wots_destroy(m->w);
  m->w = 0;

  ma->destroy_sponge(m->sg->sponge);
  m->sg->sponge = 0;

  mss_destroy(m);
}

trits_t mam_channel_id(mam_channel_t *ch) {
  return trits_from_rep(MAM2_CHANNEL_ID_SIZE, ch->id);
}
trits_t mam_channel_name(mam_channel_t *ch) { return ch->m->N1; }
static size_t mam_channel_sig_size(mam_channel_t *ch) {
  return MAM2_MSS_SIG_SIZE(ch->m->d);
}

retcode_t mam_channel_create(mam_ialloc_t *ma, /*!< [in] Allocator. */
                             prng_t *p, /*! [in] Shared PRNG interface used to
                                          generate WOTS private keys. */
                             mss_mt_height_t d, /*!< [in] MSS MT height. */
                             trits_t ch_name,   /*!< [in] Channel name. */
                             mam_channel_t *ch  /*!< [out] Channel. */
) {
  retcode_t e = RC_MAM2_INTERNAL_ERROR;

  MAM2_ASSERT(ma);
  MAM2_ASSERT(ch);

  do {
    err_bind(mam_mss_create(ma, ch->m, p, d, ch_name, trits_null()));
    mss_gen(ch->m, mam_channel_id(ch));

    e = RC_OK;
  } while (0);

  return e;
}

void mam_channel_destroy(mam_ialloc_t *ma, /*!< [in] Allocator. */
                         mam_channel_t *ch /*!< [out] Channel. */
) {
  MAM2_ASSERT(ma);
  MAM2_ASSERT(ch);

  mam_mss_destroy(ma, ch->m);
}

trits_t mam_endpoint_id(mam_endpoint_t *ep) {
  return trits_from_rep(MAM2_ENDPOINT_ID_SIZE, ep->id);
}
trits_t mam_endpoint_chname(mam_endpoint_t *ep) { return ep->m->N1; }
trits_t mam_endpoint_name(mam_endpoint_t *ep) { return ep->m->N2; }
static size_t mam_endpoint_sig_size(mam_endpoint_t *ep) {
  return MAM2_MSS_SIG_SIZE(ep->m->d);
}

retcode_t mam_endpoint_create(mam_ialloc_t *ma, /*!< [in] Allocator. */
                              prng_t *p, /*! [in] Shared PRNG interface used to
                                           generate WOTS private keys. */
                              mss_mt_height_t d, /*!< [in] MSS MT height. */
                              trits_t ch_name,   /*!< [in] Channel name. */
                              trits_t ep_name,   /*!< [in] Endpoint name. */
                              mam_endpoint_t *ep /*!< [out] Endpoint. */
) {
  retcode_t e = RC_MAM2_INTERNAL_ERROR;

  MAM2_ASSERT(ma);
  MAM2_ASSERT(ep);

  do {
    err_bind(mam_mss_create(ma, ep->m, p, d, ch_name, ep_name));
    mss_gen(ep->m, mam_endpoint_id(ep));

    e = RC_OK;
  } while (0);

  return e;
}

void mam_endpoint_destroy(mam_ialloc_t *ma,  /*!< [in] Allocator. */
                          mam_endpoint_t *ep /*!< [out] Endpoint. */
) {
  MAM2_ASSERT(ma);
  MAM2_ASSERT(ep);

  mam_mss_destroy(ma, ep->m);
}

trits_t mam_psk_id(mam_psk_t *p) {
  return trits_from_rep(MAM2_PSK_ID_SIZE, p->id);
}
trits_t mam_psk_trits(mam_psk_t *p) {
  return trits_from_rep(MAM2_PSK_SIZE, p->psk);
}

trits_t mam_ntru_pk_trits(mam_ntru_pk_t *p) {
  return trits_from_rep(MAM2_NTRU_PK_SIZE, p->pk);
}
trits_t mam_ntru_pk_id(mam_ntru_pk_t *p) {
  return trits_from_rep(MAM2_NTRU_ID_SIZE, p->pk);
}

/* MAC, MSSig, SignedId */

static size_t mam_wrap_mac_size() {
  return 0
         /*  commit; */
         /*  squeeze tryte mac[81]; */
         + pb3_sizeof_ntrytes(81);
}
static void mam_wrap_mac(spongos_t *s, trits_t *b) {
  MAM2_ASSERT(mam_wrap_mac_size() <= trits_size(*b));
  /*  commit; */
  spongos_commit(s);
  /*  squeeze tryte mac[81]; */
  pb3_wrap_squeeze_ntrytes(s, b, 81);
}
static retcode_t mam_unwrap_mac(spongos_t *s, trits_t *b) {
  retcode_t e = RC_MAM2_INTERNAL_ERROR;

  do {
    /*  commit; */
    spongos_commit(s);
    /*  squeeze tryte mac[81]; */
    err_bind(pb3_unwrap_squeeze_ntrytes(s, b, 81));
    e = RC_OK;
  } while (0);

  return e;
}

static size_t mam_wrap_mssig_size(mss_t *m) {
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
static void mam_wrap_mssig(spongos_t *s, trits_t *b, mss_t *m) {
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
static retcode_t mam_unwrap_mssig(spongos_t *s, trits_t *b, spongos_t *ms,
                                  spongos_t *ws, trits_t pk) {
  retcode_t e = RC_MAM2_INTERNAL_ERROR;
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
    err_guard(pb3_sizeof_ntrytes(sz) <= trits_size(*b), RC_MAM2_PB3_EOF);
    r = mss_verify(ms, ws, mac, pb3_trits_take(b, pb3_sizeof_ntrytes(sz)), pk);
    err_guard(r, RC_MAM2_PB3_BAD_SIG);
    e = RC_OK;
  } while (0);

  return e;
}

static size_t mam_wrap_signedid_size(mss_t *m) {
  size_t const sz = MAM2_MSS_SIG_SIZE(m->d) / 3;
  return 0
         /*  absorb tryte id[81]; */
         + pb3_sizeof_ntrytes(81)
         /*  MSSig mssig; */
         + mam_wrap_mssig_size(m);
}
static void mam_wrap_signedid(spongos_t *s, trits_t *b, trits_t id, mss_t *m) {
  MAM2_ASSERT(mam_wrap_signedid_size(m) <= trits_size(*b));
  MAM2_ASSERT(pb3_sizeof_ntrytes(81) == trits_size(id));

  /*  absorb tryte id[81]; */
  pb3_wrap_absorb_ntrytes(s, b, id);
  /*  MSSig mssig; */
  mam_wrap_mssig(s, b, m);
}
static retcode_t mam_unwrap_signedid(spongos_t *s, trits_t *b, trits_t id,
                                     spongos_t *ms, spongos_t *ws, trits_t pk) {
  retcode_t e = RC_MAM2_INTERNAL_ERROR;
  bool_t r = 1;

  do {
    /*  absorb tryte id[81]; */
    MAM2_ASSERT(pb3_sizeof_ntrytes(81) == trits_size(id));
    err_bind(pb3_unwrap_absorb_ntrytes(s, b, id));
    /*  MSSig mssig; */
    err_bind(mam_unwrap_mssig(s, b, ms, ws, pk));
    e = RC_OK;
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
void mam_wrap_channel(spongos_t *s, trits_t *b, tryte_t ver, trits_t chid) {
  MAM2_ASSERT(mam_wrap_channel_size() <= trits_size(*b));
  MAM2_ASSERT(pb3_sizeof_ntrytes(81) == trits_size(chid));

  /*  absorb tryte ver; */
  pb3_wrap_absorb_tryte(s, b, ver);
  /*  absorb external tryte chid[81]; */
  pb3_absorb_external_ntrytes(s, chid);
}
retcode_t mam_unwrap_channel(spongos_t *s, trits_t *b, tryte_t *ver,
                             trits_t chid) {
  retcode_t e = RC_MAM2_INTERNAL_ERROR;
  do {
    /*  absorb tryte ver; */
    err_bind(pb3_unwrap_absorb_tryte(s, b, ver));
    /*  absorb external tryte chid[81]; */
    MAM2_ASSERT(pb3_sizeof_ntrytes(81) == trits_size(chid));
    pb3_absorb_external_ntrytes(s, chid);
    e = RC_OK;
  } while (0);
  return e;
}

/* Endpoint */

size_t mam_wrap_pubkey_chid_size() {
  return 0
         /*  absorb null chid; */
         + 0;
}
void mam_wrap_pubkey_chid(spongos_t *s, trits_t *b) { /*  absorb null chid; */
}
retcode_t mam_unwrap_pubkey_chid(spongos_t *s, trits_t *b) {
  /*  absorb null chid; */
  return RC_OK;
}

size_t mam_wrap_pubkey_epid_size() {
  return 0
         /*  absorb tryte epid[81]; */
         + pb3_sizeof_ntrytes(81);
}
void mam_wrap_pubkey_epid(spongos_t *s, trits_t *b, trits_t epid) {
  MAM2_ASSERT(mam_wrap_pubkey_epid_size() <= trits_size(*b));
  MAM2_ASSERT(pb3_sizeof_ntrytes(81) == trits_size(epid));

  /*  absorb tryte epid[81]; */
  pb3_wrap_absorb_ntrytes(s, b, epid);
}
retcode_t mam_unwrap_pubkey_epid(spongos_t *s, trits_t *b, trits_t epid) {
  retcode_t e = RC_MAM2_INTERNAL_ERROR;
  do {
    /*  absorb tryte epid[81]; */
    MAM2_ASSERT(pb3_sizeof_ntrytes(81) == trits_size(epid));
    err_bind(pb3_unwrap_absorb_ntrytes(s, b, epid));
    e = RC_OK;
  } while (0);
  return e;
}

size_t mam_wrap_pubkey_chid1_size(mss_t *m) {
  return mam_wrap_signedid_size(m);
}
void mam_wrap_pubkey_chid1(spongos_t *s, trits_t *b, trits_t chid1, mss_t *m) {
  mam_wrap_signedid(s, b, chid1, m);
}
retcode_t mam_unwrap_pubkey_chid1(spongos_t *s, trits_t *b, trits_t chid1,
                                  spongos_t *ms, spongos_t *ws, trits_t pk) {
  return mam_unwrap_signedid(s, b, chid1, ms, ws, pk);
}

size_t mam_wrap_pubkey_epid1_size(mss_t *m) {
  return mam_wrap_signedid_size(m);
}
void mam_wrap_pubkey_epid1(spongos_t *s, trits_t *b, trits_t epid1, mss_t *m) {
  mam_wrap_signedid(s, b, epid1, m);
}
retcode_t mam_unwrap_pubkey_epid1(spongos_t *s, trits_t *b, trits_t epid1,
                                  spongos_t *ms, spongos_t *ws, trits_t pk) {
  return mam_unwrap_signedid(s, b, epid1, ms, ws, pk);
}

/* Keyload */

size_t mam_wrap_keyload_plain_size() {
  return 0
         /*  absorb tryte key[81]; */
         + pb3_sizeof_ntrytes(81);
}
void mam_wrap_keyload_plain(spongos_t *s, trits_t *b, trits_t key) {
  MAM2_ASSERT(mam_wrap_keyload_plain_size() <= trits_size(*b));
  MAM2_ASSERT(pb3_sizeof_ntrytes(81) == trits_size(key));

  /*  absorb tryte key[81]; */
  pb3_wrap_absorb_ntrytes(s, b, key);
}
retcode_t mam_unwrap_keyload_plain(spongos_t *s, trits_t *b, trits_t key) {
  retcode_t e = RC_MAM2_INTERNAL_ERROR;
  do {
    /*  absorb tryte key[81]; */
    MAM2_ASSERT(pb3_sizeof_ntrytes(81) == trits_size(key));
    err_bind(pb3_unwrap_absorb_ntrytes(s, b, key));
    e = RC_OK;
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
void mam_wrap_keyload_psk(spongos_t *s, trits_t *b, trits_t key, trits_t id,
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
retcode_t mam_unwrap_keyload_psk(spongos_t *s, trits_t *b, trits_t key,
                                 bool_t *key_found, trits_t pskid,
                                 trits_t psk) {
  retcode_t e = RC_MAM2_INTERNAL_ERROR;
  MAM2_TRITS_DEF0(id, MAM2_PSK_ID_SIZE);
  id = MAM2_TRITS_INIT(id, MAM2_PSK_ID_SIZE);

  MAM2_ASSERT(key_found);
  MAM2_ASSERT(pb3_sizeof_ntrytes(81) == trits_size(key));
  MAM2_ASSERT(trits_is_empty(pskid) ||
              pb3_sizeof_ntrytes(81) == trits_size(psk));

  do {
    /*  absorb tryte id[27]; */
    err_bind(pb3_unwrap_absorb_ntrytes(s, b, id));

    /* TODO: retcode_t (*lookup_psk)(void *ctx, trits_t id); */
    if (trits_cmp_eq(pskid, id)) {
      err_guard(!*key_found, RC_MAM2_KEYLOAD_OVERLOADED);
      *key_found = 1;

      /*  absorb external tryte psk[81]; */
      pb3_absorb_external_ntrytes(s, psk);
      /*  commit; */
      spongos_commit(s);
      /*  crypt tryte ekey[81]; */
      err_bind(pb3_unwrap_crypt_ntrytes(s, b, key));
    } else { /* skip */
      err_guard(MAM2_SPONGE_KEY_SIZE <= trits_size(*b), RC_MAM2_PB3_EOF);
      pb3_trits_take(b, MAM2_SPONGE_KEY_SIZE);
      /* spongos state is corrupted */
    }
    e = RC_OK;
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
void mam_wrap_keyload_ntru(spongos_t *s, trits_t *b, trits_t key, trits_t pk,
                           prng_t *p, spongos_t *ns, trits_t N) {
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
retcode_t mam_unwrap_keyload_ntru(spongos_t *s, trits_t *b, trits_t key,
                                  bool_t *key_found, trits_t pkid, ntru_t *n,
                                  spongos_t *ns) {
  retcode_t e = RC_MAM2_INTERNAL_ERROR;
  trits_t ekey;
  MAM2_TRITS_DEF0(id, 81);
  id = MAM2_TRITS_INIT(id, 81);

  MAM2_ASSERT(MAM2_NTRU_KEY_SIZE == trits_size(key));
  MAM2_ASSERT(MAM2_KEY_ID_SIZE == trits_size(pkid));

  do {
    /*  absorb tryte id[27]; */
    err_bind(pb3_unwrap_absorb_ntrytes(s, b, id));
    if (trits_cmp_eq(pkid, id)) {
      err_guard(!*key_found, RC_MAM2_KEYLOAD_OVERLOADED);
      *key_found = 1;

      /*  absorb tryte ekey[3072]; */
      err_guard(MAM2_NTRU_EKEY_SIZE <= trits_size(*b), RC_MAM2_PB3_EOF);
      ekey = pb3_trits_take(b, MAM2_NTRU_EKEY_SIZE);
      err_guard(trits_cmp_eq(pkid, id), RC_MAM2_KEYLOAD_IRRELEVANT);
      err_guard(ntru_decr(n, ns, ekey, key), RC_MAM2_PB3_BAD_EKEY);
      spongos_absorb(s, ekey);
    } else { /* skip */
      err_guard(MAM2_NTRU_EKEY_SIZE <= trits_size(*b), RC_MAM2_PB3_EOF);
      pb3_trits_take(b, MAM2_NTRU_EKEY_SIZE);
      /* spongos state is corrupted */
    }
    e = RC_OK;
  } while (0);

  return e;
}

/* Packet */

size_t mam_wrap_checksum_none_size() {
  return 0
         /*  absorb null none; */
         + 0;
}
void mam_wrap_checksum_none(spongos_t *s, trits_t *b) {
  /*  absorb null none; */
}
retcode_t mam_unwrap_checksum_none(spongos_t *s, trits_t *b) {
  /*  absorb null none; */
  return RC_OK;
}

size_t mam_wrap_checksum_mac_size() { return mam_wrap_mac_size(); }
void mam_wrap_checksum_mac(spongos_t *s, trits_t *b) { mam_wrap_mac(s, b); }
retcode_t mam_unwrap_checksum_mac(spongos_t *s, trits_t *b) {
  return mam_unwrap_mac(s, b);
}

size_t mam_wrap_checksum_mssig_size(mss_t *m) { return mam_wrap_mssig_size(m); }
void mam_wrap_checksum_mssig(spongos_t *s, trits_t *b, mss_t *m) {
  mam_wrap_mssig(s, b, m);
}
retcode_t mam_unwrap_checksum_mssig(spongos_t *s, trits_t *b, spongos_t *ms,
                                    spongos_t *ws, trits_t pk) {
  return mam_unwrap_mssig(s, b, ms, ws, pk);
}

size_t mam_send_msg_size(mam_send_msg_context_t *cfg) {
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

trits_t mam_send_msg_cfg_chid(mam_send_msg_context_t *cfg) {
  MAM2_ASSERT(cfg);
  MAM2_ASSERT(cfg->ch);
  return mam_channel_id(cfg->ch);
}
trits_t mam_send_msg_cfg_chid1(mam_send_msg_context_t *cfg) {
  MAM2_ASSERT(cfg);
  MAM2_ASSERT(cfg->ch1);
  return mam_channel_id(cfg->ch1);
}
trits_t mam_send_msg_cfg_epid(mam_send_msg_context_t *cfg) {
  MAM2_ASSERT(cfg);
  MAM2_ASSERT(cfg->ep);
  return mam_endpoint_id(cfg->ep);
}
trits_t mam_send_msg_cfg_epid1(mam_send_msg_context_t *cfg) {
  MAM2_ASSERT(cfg);
  MAM2_ASSERT(cfg->ep1);
  return mam_endpoint_id(cfg->ep1);
}
trits_t mam_send_msg_cfg_nonce(mam_send_msg_context_t *cfg) {
  MAM2_ASSERT(cfg);
  return trits_from_rep(MAM2_HEADER_NONCE_SIZE, cfg->nonce);
}
trits_t mam_send_msg_cfg_key(mam_send_msg_context_t *cfg) {
  MAM2_ASSERT(cfg);
  return trits_from_rep(MAM2_SPONGE_KEY_SIZE, cfg->key);
}

void mam_send_msg(mam_send_msg_context_t *cfg, trits_t *b) {
  spongos_t *s;
  spongos_t *fork;

  MAM2_TRITS_DEF0(skn, MAM2_MSS_SKN_SIZE);
  skn = MAM2_TRITS_INIT(skn, MAM2_MSS_SKN_SIZE);

  MAM2_ASSERT(cfg);
  MAM2_ASSERT(cfg->ch);
  MAM2_ASSERT(cfg->spongos->sponge);
  MAM2_ASSERT(cfg->fork->sponge);
  MAM2_ASSERT(b);
  s = cfg->spongos;
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
                              cfg->spongos_ntru, mam_send_msg_cfg_nonce(cfg));
      }
    }

    /*  absorb external tryte key[81]; */
    pb3_absorb_external_ntrytes(s, mam_send_msg_cfg_key(cfg));
    /*  commit; */
    spongos_commit(s);
  }

  trits_set_zero(mam_send_msg_cfg_key(cfg));
}

size_t mam_send_packet_size(mam_send_packet_context_t *cfg,
                            size_t payload_size) {
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
    MAM2_ASSERT(cfg->mss);
    /*    MSSig mssig = 2; */
    sz += mam_wrap_checksum_mssig_size(cfg->mss);
  } else
    MAM2_ASSERT(0);
  /*  commit; */
  return sz;
}

void mam_send_packet(mam_send_packet_context_t *cfg, trits_t payload,
                     trits_t *b) {
  spongos_t *s;
  tryte_t checksum;

  MAM2_ASSERT(cfg);
  MAM2_ASSERT(cfg->spongos->sponge);
  MAM2_ASSERT(b);
  s = cfg->spongos;

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
    mam_wrap_checksum_mssig(s, b, cfg->mss);
  else
    MAM2_ASSERT(0);

  /*  commit; */
  spongos_commit(s);
}

trits_t mam_recv_msg_cfg_chid(mam_recv_msg_context_t *cfg) {
  MAM2_ASSERT(cfg);
  MAM2_ASSERT(cfg->chid);
  return trits_from_rep(MAM2_CHANNEL_ID_SIZE, cfg->chid);
}
trits_t mam_recv_msg_cfg_chid1(mam_recv_msg_context_t *cfg) {
  MAM2_ASSERT(cfg);
  MAM2_ASSERT(cfg->chid1);
  return trits_from_rep(MAM2_CHANNEL_ID_SIZE, cfg->chid1);
}
trits_t mam_recv_msg_cfg_epid(mam_recv_msg_context_t *cfg) {
  MAM2_ASSERT(cfg);
  MAM2_ASSERT(cfg->epid);
  return trits_from_rep(MAM2_ENDPOINT_ID_SIZE, cfg->epid);
}
trits_t mam_recv_msg_cfg_epid1(mam_recv_msg_context_t *cfg) {
  MAM2_ASSERT(cfg);
  MAM2_ASSERT(cfg->epid1);
  return trits_from_rep(MAM2_ENDPOINT_ID_SIZE, cfg->epid1);
}
trits_t mam_recv_msg_cfg_nonce(mam_recv_msg_context_t *cfg) {
  MAM2_ASSERT(cfg);
  return trits_from_rep(MAM2_HEADER_NONCE_SIZE, cfg->nonce);
}
trits_t mam_recv_msg_cfg_key(mam_recv_msg_context_t *cfg) {
  MAM2_ASSERT(cfg);
  return trits_from_rep(MAM2_SPONGE_KEY_SIZE, cfg->key);
}
trits_t mam_recv_msg_cfg_psk_id(mam_recv_msg_context_t *cfg) {
  MAM2_ASSERT(cfg);
  return trits_from_rep(MAM2_PSK_ID_SIZE, cfg->psk_id);
}
trits_t mam_recv_msg_cfg_ntru_id(mam_recv_msg_context_t *cfg) {
  MAM2_ASSERT(cfg);
  return trits_from_rep(MAM2_NTRU_ID_SIZE, cfg->ntru_id);
}

retcode_t mam_recv_msg(mam_recv_msg_context_t *cfg, trits_t *b) {
  retcode_t e = RC_MAM2_INTERNAL_ERROR;

  spongos_t *s;
  spongos_t *fork;

  MAM2_ASSERT(cfg);
  MAM2_ASSERT(cfg->chid);
  MAM2_ASSERT(cfg->chid1);
  MAM2_ASSERT(cfg->epid);
  MAM2_ASSERT(cfg->epid1);
  MAM2_ASSERT(cfg->allocator);
  MAM2_ASSERT(cfg->spongos);
  s = cfg->spongos;
  fork = cfg->fork;

  do {
    spongos_init(s);

    /* unwrap Channel */
    {
      tryte_t ver = -1;
      err_bind(mam_unwrap_channel(s, b, &ver, mam_recv_msg_cfg_chid(cfg)));
      err_guard(0 == ver, RC_MAM2_VERSION_NOT_SUPPORTED);
    }

    /* unwrap Endpoint */
    {
      tryte_t pubkey = -1;
      err_bind(pb3_unwrap_absorb_tryte(s, b, &pubkey));
      err_guard(0 <= pubkey && pubkey <= 3, RC_MAM2_PB3_BAD_ONEOF);
      cfg->pubkey = (mam_msg_pubkey_t)pubkey;

      if (mam_msg_pubkey_chid1 == pubkey) { /*  SignedId chid1 = 2; */
        /*TODO: verify chid is trusted */
        err_bind(mam_unwrap_pubkey_chid1(s, b, mam_recv_msg_cfg_chid1(cfg),
                                         cfg->spongos_mss, cfg->spongos_wots,
                                         mam_recv_msg_cfg_chid(cfg)));
        /*TODO: record new channel/endpoint */
      } else if (mam_msg_pubkey_epid1 == pubkey) { /*  SignedId epid1 = 3; */
        /*TODO: verify chid is trusted */
        err_bind(mam_unwrap_pubkey_epid1(s, b, mam_recv_msg_cfg_epid1(cfg),
                                         cfg->spongos_mss, cfg->spongos_wots,
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

        for (e = RC_OK; e == RC_OK && keyload_count--;) {
          tryte_t keyload = -1;

          /*  absorb oneof keyload */
          err_bind(pb3_unwrap_absorb_tryte(s, b, &keyload));
          err_guard(0 <= keyload && keyload <= 2, RC_MAM2_PB3_BAD_ONEOF);
          /*  fork; */
          spongos_fork(s, fork);

          if (mam_msg_keyload_plain == keyload) { /*  KeyloadPlain plain = 0; */
            err_bind(
                mam_unwrap_keyload_plain(fork, b, mam_recv_msg_cfg_key(cfg)));
            err_guard(!key_found, RC_MAM2_KEYLOAD_OVERLOADED);
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
                  ntru_id_trits(cfg->ntru), cfg->ntru, cfg->spongos_ntru));
          } else
            err_guard(0, RC_MAM2_PB3_BAD_ONEOF);
        }

        err_guard(key_found, RC_MAM2_KEYLOAD_IRRELEVANT);
      }
    }

    /*  absorb external tryte key[81]; */
    pb3_absorb_external_ntrytes(s, mam_recv_msg_cfg_key(cfg));
    /*  commit; */
    spongos_commit(s);

    e = RC_OK;
  } while (0);

  return e;
}

retcode_t mam_recv_packet(mam_recv_packet_context_t *cfg, trits_t *b,
                          trits_t *payload) {
  retcode_t e = RC_MAM2_INTERNAL_ERROR;
  spongos_t *s;
  trits_t p = trits_null();

  MAM2_ASSERT(cfg);
  MAM2_ASSERT(cfg->spongos);
  MAM2_ASSERT(cfg->allocator);
  MAM2_ASSERT(b);
  MAM2_ASSERT(payload);
  s = cfg->spongos;

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
      p = trits_alloc(pb3_sizeof_ntrytes(sz));
      err_guard(!trits_is_null(p), RC_OOM);
    } else {
      err_guard(trits_size(*payload) <= pb3_sizeof_ntrytes(sz),
                RC_MAM2_BUFFER_TOO_SMALL);
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
      err_bind(mam_unwrap_checksum_mssig(s, b, cfg->spongos_mss,
                                         cfg->spongos_wots, cfg->pk));
    } else {
      err_guard(0, RC_MAM2_PB3_BAD_ONEOF);
    }

    /*  commit; */
    spongos_commit(s);

    if (trits_is_null(*payload)) *payload = p;
    p = trits_null();

    e = RC_OK;
  } while (0);

  trits_free(p);
  return e;
}
