/*
 * Copyright (c) 2018 IOTA Stiftung
 * https://github.com/iotaledger/entangled
 *
 * MAM is based on an original implementation & specification by apmi.bsu.by
 * [ITSec Lab]
 *
 * Refer to the LICENSE file for licensing information
 */

#include <stdlib.h>
#include <string.h>

#include "mam/v2/mam/mam.h"
#include "mam/v2/pb3/pb3.h"

/* MAC, MSSig, SignedId */

static size_t mam_wrap_mac_size() {
  return 0
         /*  commit; */
         /*  squeeze tryte mac[81]; */
         + pb3_sizeof_ntrytes(81);
}

static void mam_wrap_mac(mam_spongos_t *s, trits_t *b) {
  MAM2_ASSERT(mam_wrap_mac_size() <= trits_size(*b));
  /*  commit; */
  mam_spongos_commit(s);
  /*  squeeze tryte mac[81]; */
  pb3_wrap_squeeze_ntrytes(s, b, 81);
}

static retcode_t mam_unwrap_mac(mam_spongos_t *s, trits_t *b) {
  retcode_t e = RC_OK;

  /*  commit; */
  mam_spongos_commit(s);
  /*  squeeze tryte mac[81]; */
  ERR_BIND_RETURN(pb3_unwrap_squeeze_ntrytes(s, b, 81), e);

  return e;
}

static size_t mam_wrap_mssig_size(mss_t *m) {
  size_t const sz = MAM2_MSS_SIG_SIZE(m->height) / 3;
  return 0
         /*  commit; */
         /*  external squeeze tryte mac[78]; */
         + 0
         /*  absorb size_t sz; */
         + pb3_sizeof_size_t(sz)
         /*  skip tryte sig[sz]; */
         + pb3_sizeof_ntrytes(sz);
}

static void mam_wrap_mssig(mam_spongos_t *s, trits_t *b, mss_t *m) {
  MAM2_TRITS_DEF0(mac, MAM2_MSS_HASH_SIZE);
  size_t const sz = MAM2_MSS_SIG_SIZE(m->height) / 3;
  mac = MAM2_TRITS_INIT(mac, MAM2_MSS_HASH_SIZE);

  MAM2_ASSERT(mam_wrap_mssig_size(m) <= trits_size(*b));

  /*  commit; */
  mam_spongos_commit(s);
  /*  external squeeze tryte mac[78]; */
  pb3_squeeze_external_ntrytes(s, mac);
  /*  absorb size_t sz; */
  pb3_wrap_absorb_size_t(s, b, sz);
  /*  skip tryte sig[sz]; */
  mss_sign(m, mac, pb3_trits_take(b, pb3_sizeof_ntrytes(sz)));
}

static retcode_t mam_unwrap_mssig(mam_spongos_t *s, trits_t *b,
                                  mam_spongos_t *ms, mam_spongos_t *ws,
                                  trits_t pk) {
  retcode_t e = RC_OK;

  MAM2_TRITS_DEF0(mac, MAM2_MSS_HASH_SIZE);
  size_t sz;
  mac = MAM2_TRITS_INIT(mac, MAM2_MSS_HASH_SIZE);

  /*  commit; */
  mam_spongos_commit(s);
  /*  external squeeze tryte mac[78]; */
  pb3_squeeze_external_ntrytes(s, mac);
  /*  absorb size_t sz; */
  ERR_BIND_RETURN(pb3_unwrap_absorb_size_t(s, b, &sz), e);
  /*  skip tryte sig[sz]; */
  ERR_GUARD_RETURN(pb3_sizeof_ntrytes(sz) <= trits_size(*b), RC_MAM2_PB3_EOF,
                   e);
  ERR_GUARD_RETURN(
      mss_verify(ms, ws, mac, pb3_trits_take(b, pb3_sizeof_ntrytes(sz)), pk),
      RC_MAM2_PB3_BAD_SIG, e);

  return e;
}

static size_t mam_wrap_signedid_size(mss_t *m) {
  return 0
         /*  absorb tryte id[81]; */
         + pb3_sizeof_ntrytes(81)
         /*  MSSig mssig; */
         + mam_wrap_mssig_size(m);
}

static void mam_wrap_signedid(mam_spongos_t *s, trits_t *b, trits_t id,
                              mss_t *m) {
  MAM2_ASSERT(mam_wrap_signedid_size(m) <= trits_size(*b));
  MAM2_ASSERT(pb3_sizeof_ntrytes(81) == trits_size(id));

  /*  absorb tryte id[81]; */
  pb3_wrap_absorb_ntrytes(s, b, id);
  /*  MSSig mssig; */
  mam_wrap_mssig(s, b, m);
}

static retcode_t mam_unwrap_signedid(mam_spongos_t *s, trits_t *b, trits_t id,
                                     mam_spongos_t *ms, mam_spongos_t *ws,
                                     trits_t pk) {
  retcode_t e = RC_OK;

  /*  absorb tryte id[81]; */
  MAM2_ASSERT(pb3_sizeof_ntrytes(81) == trits_size(id));
  ERR_BIND_RETURN(pb3_unwrap_absorb_ntrytes(s, b, id), e);
  /*  MSSig mssig; */
  ERR_BIND_RETURN(mam_unwrap_mssig(s, b, ms, ws, pk), e);

  return e;
}

/* Endpoint */

size_t mam_wrap_pubkey_chid_size() {
  return 0
         /*  absorb null chid; */
         + 0;
}

void mam_wrap_pubkey_chid(mam_spongos_t *s,
                          trits_t *b) { /*  absorb null chid; */
}

retcode_t mam_unwrap_pubkey_chid(mam_spongos_t *s, trits_t *b) {
  /*  absorb null chid; */
  return RC_OK;
}

size_t mam_wrap_pubkey_epid_size() {
  return 0
         /*  absorb tryte epid[81]; */
         + pb3_sizeof_ntrytes(81);
}

void mam_wrap_pubkey_epid(mam_spongos_t *s, trits_t *b, trits_t epid) {
  MAM2_ASSERT(mam_wrap_pubkey_epid_size() <= trits_size(*b));
  MAM2_ASSERT(pb3_sizeof_ntrytes(81) == trits_size(epid));

  /*  absorb tryte epid[81]; */
  pb3_wrap_absorb_ntrytes(s, b, epid);
}

retcode_t mam_unwrap_pubkey_epid(mam_spongos_t *s, trits_t *b, trits_t epid) {
  retcode_t e = RC_OK;

  /*  absorb tryte epid[81]; */
  MAM2_ASSERT(pb3_sizeof_ntrytes(81) == trits_size(epid));
  ERR_BIND_RETURN(pb3_unwrap_absorb_ntrytes(s, b, epid), e);

  return e;
}

size_t mam_wrap_pubkey_chid1_size(mss_t *m) {
  return mam_wrap_signedid_size(m);
}

void mam_wrap_pubkey_chid1(mam_spongos_t *s, trits_t *b, trits_t chid1,
                           mss_t *m) {
  mam_wrap_signedid(s, b, chid1, m);
}

retcode_t mam_unwrap_pubkey_chid1(mam_spongos_t *s, trits_t *b, trits_t chid1,
                                  mam_spongos_t *ms, mam_spongos_t *ws,
                                  trits_t pk) {
  return mam_unwrap_signedid(s, b, chid1, ms, ws, pk);
}

size_t mam_wrap_pubkey_epid1_size(mss_t *m) {
  return mam_wrap_signedid_size(m);
}

void mam_wrap_pubkey_epid1(mam_spongos_t *s, trits_t *b, trits_t epid1,
                           mss_t *m) {
  mam_wrap_signedid(s, b, epid1, m);
}

retcode_t mam_unwrap_pubkey_epid1(mam_spongos_t *s, trits_t *b, trits_t epid1,
                                  mam_spongos_t *ms, mam_spongos_t *ws,
                                  trits_t pk) {
  return mam_unwrap_signedid(s, b, epid1, ms, ws, pk);
}

/* Keyload */

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

void mam_wrap_keyload_psk(mam_spongos_t *s, trits_t *b, trits_t key, trits_t id,
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
  mam_spongos_commit(s);
  /*  crypt tryte ekey[81]; */
  pb3_wrap_crypt_ntrytes(s, b, key);
}

retcode_t mam_unwrap_keyload_psk(mam_spongos_t *s, trits_t *b, trits_t key,
                                 bool *key_found, mam_psk_t *p) {
  retcode_t e = RC_OK;
  MAM2_TRITS_DEF0(id, MAM2_PSK_ID_SIZE);
  id = MAM2_TRITS_INIT(id, MAM2_PSK_ID_SIZE);

  trits_t pskid = mam_psk_id(p);
  trits_t psk = mam_psk_trits(p);

  MAM2_ASSERT(key_found);
  MAM2_ASSERT(pb3_sizeof_ntrytes(81) == trits_size(key));
  MAM2_ASSERT(trits_is_empty(pskid) ||
              pb3_sizeof_ntrytes(81) == trits_size(psk));

  /*  absorb tryte id[27]; */
  ERR_BIND_RETURN(pb3_unwrap_absorb_ntrytes(s, b, id), e);

  /* TODO: retcode_t (*lookup_psk)(void *ctx, trits_t id); */
  if (trits_cmp_eq(pskid, id)) {
    ERR_GUARD_RETURN(!*key_found, RC_MAM2_KEYLOAD_OVERLOADED, e);
    *key_found = 1;

    /*  absorb external tryte psk[81]; */
    pb3_absorb_external_ntrytes(s, psk);
    /*  commit; */
    mam_spongos_commit(s);
    /*  crypt tryte ekey[81]; */
    ERR_BIND_RETURN(pb3_unwrap_crypt_ntrytes(s, b, key), e);
  } else { /* skip */
    ERR_GUARD_RETURN(MAM2_SPONGE_KEY_SIZE <= trits_size(*b), RC_MAM2_PB3_EOF,
                     e);
    pb3_trits_take(b, MAM2_SPONGE_KEY_SIZE);
    /* spongos state is corrupted */
  }

  return e;
}

size_t mam_wrap_keyload_ntru_size() {
  return 0
         /*  absorb tryte id[27]; */
         + pb3_sizeof_ntrytes(27)
         /*  absorb tryte ekey[3072]; */
         + pb3_sizeof_ntrytes(3072);
}

void mam_wrap_keyload_ntru(mam_spongos_t *s, trits_t *b, trits_t key,
                           trits_t pk, mam_prng_t *p, mam_spongos_t *ns,
                           trits_t N) {
  trits_t ekey;

  MAM2_ASSERT(mam_wrap_keyload_ntru_size() <= trits_size(*b));
  MAM2_ASSERT(MAM2_NTRU_KEY_SIZE == trits_size(key));
  MAM2_ASSERT(MAM2_NTRU_PK_SIZE == trits_size(pk));

  /*  absorb tryte id[27]; */
  pb3_wrap_absorb_ntrytes(s, b, trits_take(pk, pb3_sizeof_ntrytes(27)));
  /*  absorb tryte ekey[3072]; */
  ekey = pb3_trits_take(b, MAM2_NTRU_EKEY_SIZE);
  ntru_encr(pk, p, ns, key, N, ekey);
  mam_spongos_absorb(s, ekey);
}

retcode_t mam_unwrap_keyload_ntru(mam_spongos_t *s, trits_t *b, trits_t key,
                                  bool *key_found, mam_ntru_sk_t *n,
                                  mam_spongos_t *ns) {
  retcode_t e = RC_OK;
  trits_t ekey;
  MAM2_TRITS_DEF0(id, 81);
  id = MAM2_TRITS_INIT(id, 81);
  trits_t pkid = ntru_id_trits(n);

  MAM2_ASSERT(MAM2_NTRU_KEY_SIZE == trits_size(key));
  MAM2_ASSERT(MAM2_NTRU_ID_SIZE == trits_size(pkid));

  /*  absorb tryte id[27]; */
  ERR_BIND_RETURN(pb3_unwrap_absorb_ntrytes(s, b, id), e);
  if (trits_cmp_eq(pkid, id)) {
    ERR_GUARD_RETURN(!*key_found, RC_MAM2_KEYLOAD_OVERLOADED, e);
    *key_found = 1;

    /*  absorb tryte ekey[3072]; */
    ERR_GUARD_RETURN(MAM2_NTRU_EKEY_SIZE <= trits_size(*b), RC_MAM2_PB3_EOF, e);
    ekey = pb3_trits_take(b, MAM2_NTRU_EKEY_SIZE);
    ERR_GUARD_RETURN(trits_cmp_eq(pkid, id), RC_MAM2_KEYLOAD_IRRELEVANT, e);
    ERR_GUARD_RETURN(ntru_decr(n, ns, ekey, key), RC_MAM2_PB3_BAD_EKEY, e);
    mam_spongos_absorb(s, ekey);
  } else { /* skip */
    ERR_GUARD_RETURN(MAM2_NTRU_EKEY_SIZE <= trits_size(*b), RC_MAM2_PB3_EOF, e);
    pb3_trits_take(b, MAM2_NTRU_EKEY_SIZE);
    /* spongos state is corrupted */
  }

  return e;
}

/* Packet */

size_t mam_wrap_checksum_none_size() {
  return 0
         /*  absorb null none; */
         + 0;
}

void mam_wrap_checksum_none(mam_spongos_t *s, trits_t *b) {
  /*  absorb null none; */
}

retcode_t mam_unwrap_checksum_none(mam_spongos_t *s, trits_t *b) {
  /*  absorb null none; */
  return RC_OK;
}

size_t mam_wrap_checksum_mac_size() { return mam_wrap_mac_size(); }

void mam_wrap_checksum_mac(mam_spongos_t *s, trits_t *b) { mam_wrap_mac(s, b); }

retcode_t mam_unwrap_checksum_mac(mam_spongos_t *s, trits_t *b) {
  return mam_unwrap_mac(s, b);
}

size_t mam_wrap_checksum_mssig_size(mss_t *m) { return mam_wrap_mssig_size(m); }

void mam_wrap_checksum_mssig(mam_spongos_t *s, trits_t *b, mss_t *m) {
  mam_wrap_mssig(s, b, m);
}

retcode_t mam_unwrap_checksum_mssig(mam_spongos_t *s, trits_t *b,
                                    mam_spongos_t *ms, mam_spongos_t *ws,
                                    trits_t pk) {
  return mam_unwrap_mssig(s, b, ms, ws, pk);
}

size_t mam_send_msg_size(mam_channel_t *ch, mam_endpoint_t *ep,
                         mam_channel_t *ch1, mam_endpoint_t *ep1,
                         mam_psk_t_set_t psks, mam_ntru_pk_t_set_t ntru_pks) {
  size_t sz = 0;

  MAM2_ASSERT(ch);

  /* channel */
  sz += mam_channel_wrap_size();

  /* endpoint */
  /*  absorb oneof pubkey */
  sz += pb3_sizeof_oneof();

  if (ch1) {
    // SignedId chid1 = 2;
    sz += mam_wrap_pubkey_chid1_size(&ch->mss);
  } else if (ep1) {
    // SignedId epid1 = 3;
    sz += mam_wrap_pubkey_epid1_size(&ch->mss);
  } else if (ep) {
    // absorb tryte epid[81] = 1;
    sz += mam_wrap_pubkey_epid_size();
  } else {
    //  null chid = 0;
    sz += mam_wrap_pubkey_chid_size();
  }

  /* header */
  /*  absorb tryte msg_id[27]; */
  sz += pb3_sizeof_ntrytes(MAM2_MSG_ID_SIZE / 3);
  /*  absorb trint typeid; */
  sz += pb3_sizeof_trint();
  {
    size_t keyload_count = 0;

    size_t num_pre_shared_keys = mam_psk_t_set_size(psks);
    keyload_count += num_pre_shared_keys;
    sz += (pb3_sizeof_oneof() + mam_wrap_keyload_psk_size()) *
          num_pre_shared_keys;

    size_t num_pre_ntru_keys = mam_ntru_pk_t_set_size(ntru_pks);
    keyload_count += num_pre_ntru_keys;
    sz +=
        (pb3_sizeof_oneof() + mam_wrap_keyload_ntru_size()) * num_pre_ntru_keys;

    /*  absorb repeated */
    sz += pb3_sizeof_repeated(keyload_count);
  }
  /*  external secret tryte key[81]; */
  sz += 0;

  /* packets */
  return sz;
}

void mam_send_msg(mam_send_context_t *ctx, mam_prng_t *prng, mam_channel_t *ch,
                  mam_endpoint_t *ep, mam_channel_t *ch1, mam_endpoint_t *ep1,
                  trits_t msg_id, trint9_t msg_type_id, mam_psk_t_set_t psks,
                  mam_ntru_pk_t_set_t ntru_pks, trits_t *msg) {
  trit_t session_key_trits[MAM2_SPONGE_KEY_SIZE];
  trits_t session_key = trits_from_rep(MAM2_SPONGE_KEY_SIZE, session_key_trits);

  MAM2_TRITS_DEF0(skn, MAM2_MSS_SKN_SIZE);
  skn = MAM2_TRITS_INIT(skn, MAM2_MSS_SKN_SIZE);

  MAM2_ASSERT(ctx);
  MAM2_ASSERT(ch);
  MAM2_ASSERT(msg);

  MAM2_ASSERT(!(trits_size(*msg) <
                mam_send_msg_size(ch, ep, ch1, ep1, psks, ntru_pks)));

  if (ep) {
    mss_skn(&ep->mss, skn);
  } else {
    mss_skn(&ch->mss, skn);
  }

  /* generate session key */
  if (psks == NULL && ntru_pks == NULL) {  // public
    trits_set_zero(session_key);
  } else {
    mam_prng_gen3(prng, MAM2_PRNG_DST_SEC_KEY, mam_channel_name(ch),
                  ep ? mam_endpoint_name(ep) : trits_null(), skn, session_key);
  }

  /* choose recipient */
  mam_spongos_init(&ctx->spongos);

  /* wrap Channel */
  mam_channel_wrap(&ctx->spongos, msg, 0, mam_channel_id(ch));

  /* wrap Endpoint */
  {
    tryte_t pubkey;

    if (ch1) { /*  SignedId chid1 = 2; */
      pubkey = (tryte_t)mam_msg_pubkey_chid1;
      pb3_wrap_absorb_tryte(&ctx->spongos, msg, pubkey);
      mam_wrap_pubkey_chid1(&ctx->spongos, msg, mam_channel_id(ch1), &ch->mss);
    } else if (ep1) { /*  SignedId epid1 = 3; */
      pubkey = (tryte_t)mam_msg_pubkey_epid1;
      pb3_wrap_absorb_tryte(&ctx->spongos, msg, pubkey);
      mam_wrap_pubkey_epid1(&ctx->spongos, msg, mam_endpoint_id(ep1), &ch->mss);
    } else if (ep) { /*  absorb tryte epid[81] = 1; */
      pubkey = (tryte_t)mam_msg_pubkey_epid;
      pb3_wrap_absorb_tryte(&ctx->spongos, msg, pubkey);
      mam_wrap_pubkey_epid(&ctx->spongos, msg, mam_endpoint_id(ep));
    } else { /*  absorb null chid = 0; */
      pubkey = (tryte_t)mam_msg_pubkey_chid;
      pb3_wrap_absorb_tryte(&ctx->spongos, msg, pubkey);
      mam_wrap_pubkey_chid(&ctx->spongos, msg);
    }
  }

  /* wrap Header */
  {
    /*  absorb tryte msgid[27]; */
    pb3_wrap_absorb_ntrytes(&ctx->spongos, msg, msg_id);
    /*  absorb trint typeid; */
    pb3_wrap_absorb_trint(&ctx->spongos, msg, msg_type_id);

    {
      size_t keyload_count = 0;
      tryte_t keyload;
      mam_spongos_t spongos_ntru;
      mam_spongos_t spongos_fork;

      keyload_count += mam_psk_t_set_size(psks);
      keyload_count += mam_ntru_pk_t_set_size(ntru_pks);
      /*  repeated */
      pb3_wrap_absorb_size_t(&ctx->spongos, msg, keyload_count);

      mam_psk_t_set_entry_t *curr_entry_psk = NULL;
      mam_psk_t_set_entry_t *tmp_entry_psk = NULL;

      HASH_ITER(hh, psks, curr_entry_psk, tmp_entry_psk) {
        /*  absorb oneof keyload */
        keyload = (tryte_t)mam_msg_keyload_psk;
        pb3_wrap_absorb_tryte(&ctx->spongos, msg, keyload);
        /*  fork; */
        mam_mam_spongos_fork(&ctx->spongos, &spongos_fork);
        /*  KeyloadPSK psk = 1; */
        mam_wrap_keyload_psk(&spongos_fork, msg, session_key,
                             mam_psk_id(&curr_entry_psk->value),
                             mam_psk_trits(&curr_entry_psk->value));
      }

      mam_ntru_pk_t_set_entry_t *curr_entry_ntru = NULL;
      mam_ntru_pk_t_set_entry_t *tmp_entry_ntru = NULL;

      HASH_ITER(hh, ntru_pks, curr_entry_ntru, tmp_entry_ntru) {
        /*  absorb oneof keyload */
        keyload = (tryte_t)mam_msg_keyload_ntru;
        pb3_wrap_absorb_tryte(&ctx->spongos, msg, keyload);
        /*  fork; */
        mam_mam_spongos_fork(&ctx->spongos, &spongos_fork);
        /*  KeyloadNTRU ntru = 2; */
        mam_wrap_keyload_ntru(&spongos_fork, msg, session_key,
                              mam_ntru_pk_trits(&curr_entry_ntru->value), prng,
                              &spongos_ntru, msg_id);
      }
    }

    /*  absorb external tryte key[81]; */
    pb3_absorb_external_ntrytes(&ctx->spongos, session_key);
    /*  commit; */
    mam_spongos_commit(&ctx->spongos);
  }

  trits_set_zero(session_key);
}

size_t mam_send_packet_size(mam_msg_checksum_t checksum, mss_t *mss,
                            size_t payload_size) {
  size_t sz = 0;
  MAM2_ASSERT(0 == payload_size % 3);
  sz = 0
       /*  absorb long trint ord; */
       + pb3_sizeof_longtrint()
       /*  absorb tryte sz; */
       + pb3_sizeof_size_t(payload_size / 3)
       /*  crypt tryte payload[sz]; */
       + pb3_sizeof_ntrytes(payload_size / 3)
       /*  absorb oneof checksum */
       + pb3_sizeof_oneof();

  if (mam_msg_checksum_none == checksum) {
    // absorb null none = 0;
    sz += mam_wrap_checksum_none_size();
  } else if (mam_msg_checksum_mac == checksum) {
    //  MAC mac = 1;
    sz += mam_wrap_checksum_mac_size();
  } else if (mam_msg_checksum_mssig == checksum) {
    //  MSSig mssig = 2;
    MAM2_ASSERT(mss);
    sz += mam_wrap_checksum_mssig_size(mss);
  } else {
    /*  commit; */
    MAM2_ASSERT(0);
  }

  return sz;
}

void mam_send_packet(mam_send_context_t *ctx, mam_msg_checksum_t checksum,
                     trits_t payload, trits_t *b) {
  MAM2_ASSERT(ctx);
  MAM2_ASSERT(b);

  MAM2_ASSERT(!(trits_size(*b) <
                mam_send_packet_size(checksum, ctx->mss, trits_size(payload))));

  /*  absorb long trint ord; */
  pb3_wrap_absorb_longtrint(&ctx->spongos, b, ctx->ord);

  /*  absorb tryte sz; */
  pb3_wrap_absorb_size_t(&ctx->spongos, b, trits_size(payload) / 3);
  /*  crypt tryte payload[sz]; */
  pb3_wrap_crypt_ntrytes(&ctx->spongos, b, payload);

  /*  absorb oneof checksum */
  pb3_wrap_absorb_tryte(&ctx->spongos, b, (tryte_t)checksum);
  if (mam_msg_checksum_none == checksum) {
    /*    absorb null none = 0; */
    mam_wrap_checksum_none(&ctx->spongos, b);
  } else if (mam_msg_checksum_mac == checksum) {
    /*    MAC mac = 1; */
    mam_wrap_checksum_mac(&ctx->spongos, b);
  } else if (mam_msg_checksum_mssig == checksum) {
    /*    MSSig mssig = 2; */
    mam_wrap_checksum_mssig(&ctx->spongos, b, ctx->mss);
  } else {
    MAM2_ASSERT(0);
  }
  /*  commit; */
  mam_spongos_commit(&ctx->spongos);
}

trits_t mam_recv_msg_cfg_chid(mam_recv_msg_context_t const *const cfg) {
  MAM2_ASSERT(cfg);
  MAM2_ASSERT(cfg->chid);
  return trits_from_rep(MAM2_CHANNEL_ID_SIZE, cfg->chid);
}

trits_t mam_recv_msg_cfg_chid1(mam_recv_msg_context_t const *const cfg) {
  MAM2_ASSERT(cfg);
  MAM2_ASSERT(cfg->chid1);
  return trits_from_rep(MAM2_CHANNEL_ID_SIZE, cfg->chid1);
}

trits_t mam_recv_msg_cfg_epid(mam_recv_msg_context_t const *const cfg) {
  MAM2_ASSERT(cfg);
  MAM2_ASSERT(cfg->epid);
  return trits_from_rep(MAM2_ENDPOINT_ID_SIZE, cfg->epid);
}

trits_t mam_recv_msg_cfg_epid1(mam_recv_msg_context_t const *const cfg) {
  MAM2_ASSERT(cfg);
  MAM2_ASSERT(cfg->epid1);
  return trits_from_rep(MAM2_ENDPOINT_ID_SIZE, cfg->epid1);
}

trits_t mam_recv_msg_cfg_msg_id(mam_recv_msg_context_t const *const cfg) {
  MAM2_ASSERT(cfg);
  return trits_from_rep(MAM2_MSG_ID_SIZE, cfg->msg_id);
}

retcode_t mam_recv_msg(mam_recv_msg_context_t *cfg, trits_t *b) {
  retcode_t e = RC_OK;
  mam_spongos_t *s;
  trit_t session_key_trits[MAM2_SPONGE_KEY_SIZE];
  trits_t session_key = trits_from_rep(MAM2_SPONGE_KEY_SIZE, session_key_trits);

  MAM2_ASSERT(cfg);
  MAM2_ASSERT(cfg->chid);
  MAM2_ASSERT(cfg->chid1);
  MAM2_ASSERT(cfg->epid);
  MAM2_ASSERT(cfg->epid1);
  MAM2_ASSERT(cfg->spongos);
  s = cfg->spongos;

  mam_spongos_init(s);

  /* unwrap Channel */
  {
    tryte_t ver = -1;
    ERR_BIND_RETURN(mam_channel_unwrap(s, b, &ver, mam_recv_msg_cfg_chid(cfg)),
                    e);
    ERR_GUARD_RETURN(0 == ver, RC_MAM2_VERSION_NOT_SUPPORTED, e);
  }

  /* unwrap Endpoint */
  {
    tryte_t pubkey = -1;
    ERR_BIND_RETURN(pb3_unwrap_absorb_tryte(s, b, &pubkey), e);
    ERR_GUARD_RETURN(0 <= pubkey && pubkey <= 3, RC_MAM2_PB3_BAD_ONEOF, e);
    cfg->pubkey = (mam_msg_pubkey_t)pubkey;

    if (mam_msg_pubkey_chid1 == pubkey) { /*  SignedId chid1 = 2; */
      /*TODO: verify chid is trusted */
      ERR_BIND_RETURN(mam_unwrap_pubkey_chid1(
                          s, b, mam_recv_msg_cfg_chid1(cfg), cfg->spongos_mss,
                          cfg->spongos_wots, mam_recv_msg_cfg_chid(cfg)),
                      e);
      /*TODO: record new channel/endpoint */
    } else if (mam_msg_pubkey_epid1 == pubkey) { /*  SignedId epid1 = 3; */
      /*TODO: verify chid is trusted */
      ERR_BIND_RETURN(mam_unwrap_pubkey_epid1(
                          s, b, mam_recv_msg_cfg_epid1(cfg), cfg->spongos_mss,
                          cfg->spongos_wots, mam_recv_msg_cfg_chid(cfg)),
                      e);
      /*TODO: record new channel/endpoint */
    } else if (mam_msg_pubkey_epid ==
               pubkey) { /*  absorb tryte epid[81] = 1; */
      ERR_BIND_RETURN(mam_unwrap_pubkey_epid(s, b, mam_recv_msg_cfg_epid(cfg)),
                      e);
    } else if (mam_msg_pubkey_chid == pubkey) { /*  absorb null chid = 0; */
      ERR_BIND_RETURN(mam_unwrap_pubkey_chid(s, b), e);
    } else
      MAM2_ASSERT(0);
  }

  /* unwrap Header */
  {
    /*  absorb tryte msg_id[27]; */
    ERR_BIND_RETURN(
        pb3_unwrap_absorb_ntrytes(s, b, mam_recv_msg_cfg_msg_id(cfg)), e);
    /*  absorb trint typeid; */
    ERR_BIND_RETURN(pb3_unwrap_absorb_trint(s, b, &cfg->msg_type_id), e);

    {
      /*  repeated */
      size_t keyload_count = 0;
      bool key_found = false;
      mam_spongos_t spongos_ntru;

      ERR_BIND_RETURN(pb3_unwrap_absorb_size_t(s, b, &keyload_count), e);

      if (0 < keyload_count) {
        mam_spongos_t spongos_fork;

        for (e = RC_OK; e == RC_OK && keyload_count--;) {
          tryte_t keyload = -1;

          /*  absorb oneof keyload */
          ERR_BIND_RETURN(pb3_unwrap_absorb_tryte(s, b, &keyload), e);
          ERR_GUARD_RETURN(1 <= keyload && keyload <= 2, RC_MAM2_PB3_BAD_ONEOF,
                           e);
          /*  fork; */
          mam_mam_spongos_fork(s, &spongos_fork);

          if (mam_msg_keyload_psk == keyload) { /*  KeyloadPSK psk = 1; */
            if (cfg->psk) {
              ERR_BIND_RETURN(
                  mam_unwrap_keyload_psk(&spongos_fork, b, session_key,
                                         &key_found, cfg->psk),
                  e);
            }
          } else if (mam_msg_keyload_ntru ==
                     keyload) { /*  KeyloadNTRU ntru = 2; */
            if (cfg->ntru) {
              ERR_BIND_RETURN(
                  mam_unwrap_keyload_ntru(&spongos_fork, b, session_key,
                                          &key_found, cfg->ntru, &spongos_ntru),
                  e);
            }
          } else
            ERR_GUARD_RETURN(0, RC_MAM2_PB3_BAD_ONEOF, e);
        }

        ERR_GUARD_RETURN(key_found, RC_MAM2_KEYLOAD_IRRELEVANT, e);
      } else
        /* no recipients => public mode */
        trits_set_zero(session_key);
    }
  }

  /*  absorb external tryte key[81]; */
  pb3_absorb_external_ntrytes(s, session_key);
  /*  commit; */
  mam_spongos_commit(s);

  return e;
}

retcode_t mam_recv_packet(mam_recv_packet_context_t *cfg, trits_t *b,
                          trits_t *payload) {
  retcode_t e = RC_OK;
  mam_spongos_t *s;
  trits_t p = trits_null();

  MAM2_ASSERT(cfg);
  MAM2_ASSERT(cfg->spongos);
  MAM2_ASSERT(b);
  MAM2_ASSERT(payload);
  s = cfg->spongos;

  size_t sz = 0;
  tryte_t checksum = -1;

  /*  absorb long trint ord; */
  ERR_BIND_RETURN(pb3_unwrap_absorb_longtrint(s, b, &cfg->ord), e);
  /*TODO: check ord */

  /*  absorb tryte sz; */
  ERR_BIND_RETURN(pb3_unwrap_absorb_size_t(s, b, &sz), e);
  /*  crypt tryte payload[sz]; */
  if (trits_is_null(*payload)) {
    p = trits_alloc(pb3_sizeof_ntrytes(sz));
    ERR_GUARD_RETURN(!trits_is_null(p), RC_OOM, e);
  } else {
    ERR_GUARD_GOTO(trits_size(*payload) <= pb3_sizeof_ntrytes(sz),
                   RC_MAM2_BUFFER_TOO_SMALL, e, cleanup);
    p = pb3_trits_take(payload, pb3_sizeof_ntrytes(sz));
  }
  ERR_BIND_GOTO(pb3_unwrap_crypt_ntrytes(s, b, p), e, cleanup);

  /*  absorb oneof checksum */
  ERR_BIND_GOTO(pb3_unwrap_absorb_tryte(s, b, &checksum), e, cleanup);

  if (mam_msg_checksum_none == checksum) {
    /*    absorb null none = 0; */
    ERR_BIND_GOTO(mam_unwrap_checksum_none(s, b), e, cleanup);
  } else if (mam_msg_checksum_mac == checksum) {
    /*    MAC mac = 1; */
    ERR_BIND_GOTO(mam_unwrap_checksum_mac(s, b), e, cleanup);
  } else if (mam_msg_checksum_mssig == checksum) {
    /*    MSSig mssig = 2; */
    ERR_BIND_GOTO(mam_unwrap_checksum_mssig(s, b, cfg->spongos_mss,
                                            cfg->spongos_wots, cfg->pk),
                  e, cleanup);
  } else {
    ERR_GUARD_GOTO(0, RC_MAM2_PB3_BAD_ONEOF, e, cleanup);
  }

  /*  commit; */
  mam_spongos_commit(s);

  if (trits_is_null(*payload)) *payload = p;
  p = trits_null();

cleanup:
  trits_free(p);
  return e;
}
