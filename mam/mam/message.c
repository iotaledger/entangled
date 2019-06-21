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

#include "mam/mam/mam_channel_t_set.h"
#include "mam/mam/message.h"
#include "mam/ntru/mam_ntru_sk_t_set.h"
#include "mam/pb3/pb3.h"
#include "utils/memset_safe.h"

/* MAC, MSSig, SignedId */

static size_t mam_msg_wrap_mac_size() {
  return 0
         /*  commit; */
         /*  squeeze tryte mac[81]; */
         + pb3_sizeof_ntrytes(81);
}

static void mam_msg_wrap_mac(mam_spongos_t *s, trits_t *b) {
  MAM_ASSERT(mam_msg_wrap_mac_size() <= trits_size(*b));
  /*  commit; */
  mam_spongos_commit(s);
  /*  squeeze tryte mac[81]; */
  pb3_wrap_squeeze_ntrytes(s, b, 81);
}

static retcode_t mam_msg_unwrap_mac(mam_spongos_t *s, trits_t *b) {
  retcode_t e = RC_OK;

  /*  commit; */
  mam_spongos_commit(s);
  /*  squeeze tryte mac[81]; */
  ERR_BIND_RETURN(pb3_unwrap_squeeze_ntrytes(s, b, 81), e);

  return e;
}

static size_t mam_msg_wrap_mssig_size(mam_mss_t const *const m) {
  size_t const sz = MAM_MSS_SIG_SIZE(m->height) / 3;
  return 0
         /*  commit; */
         /*  external squeeze tryte mac[78]; */
         + 0
         /*  absorb size_t sz; */
         + pb3_sizeof_size_t(sz)
         /*  skip tryte sig[sz]; */
         + pb3_sizeof_ntrytes(sz);
}

static retcode_t mam_msg_wrap_mssig(mam_spongos_t *s, trits_t *b, mam_mss_t *m) {
  MAM_TRITS_DEF(mac, MAM_MSS_HASH_SIZE);
  size_t const sz = MAM_MSS_SIG_SIZE(m->height) / 3;
  mac = MAM_TRITS_INIT(mac, MAM_MSS_HASH_SIZE);

  MAM_ASSERT(mam_msg_wrap_mssig_size(m) <= trits_size(*b));

  /*  commit; */
  mam_spongos_commit(s);
  /*  external squeeze tryte mac[78]; */
  pb3_squeeze_external_ntrytes(s, mac);
  /*  absorb size_t sz; */
  pb3_wrap_absorb_size_t(s, b, sz);
  /*  skip tryte sig[sz]; */
  return mam_mss_sign_and_next(m, mac, pb3_trits_take(b, pb3_sizeof_ntrytes(sz)));
}

static retcode_t mam_msg_unwrap_mssig(mam_spongos_t *s, trits_t *b, mam_spongos_t *ms, trits_t pk) {
  retcode_t e = RC_OK;

  MAM_TRITS_DEF(mac, MAM_MSS_HASH_SIZE);
  size_t sz;
  mac = MAM_TRITS_INIT(mac, MAM_MSS_HASH_SIZE);

  /*  commit; */
  mam_spongos_commit(s);
  /*  external squeeze tryte mac[78]; */
  pb3_squeeze_external_ntrytes(s, mac);
  /*  absorb size_t sz; */
  ERR_BIND_RETURN(pb3_unwrap_absorb_size_t(s, b, &sz), e);
  /*  skip tryte sig[sz]; */
  ERR_GUARD_RETURN(pb3_sizeof_ntrytes(sz) <= trits_size(*b), RC_MAM_PB3_EOF);
  ERR_GUARD_RETURN(mam_mss_verify(ms, mac, pb3_trits_take(b, pb3_sizeof_ntrytes(sz)), pk), RC_MAM_PB3_BAD_SIG);

  return e;
}

static size_t mam_msg_wrap_signedid_size(mam_mss_t const *const mss) {
  return 0
         /*  absorb tryte id[81]; */
         + pb3_sizeof_ntrytes(81)
         /*  MSSig mssig; */
         + mam_msg_wrap_mssig_size(mss);
}

static retcode_t mam_msg_wrap_signedid(mam_spongos_t *s, trits_t *b, trits_t id, mam_mss_t *m) {
  MAM_ASSERT(mam_msg_wrap_signedid_size(m) <= trits_size(*b));
  MAM_ASSERT(pb3_sizeof_ntrytes(81) == trits_size(id));

  /*  absorb tryte id[81]; */
  pb3_wrap_absorb_ntrytes(s, b, id);
  /*  MSSig mssig; */
  return mam_msg_wrap_mssig(s, b, m);
}

static retcode_t mam_msg_unwrap_signedid(mam_spongos_t *s, trits_t *b, trits_t id, mam_spongos_t *ms, trits_t pk) {
  retcode_t e = RC_OK;

  /*  absorb tryte id[81]; */
  MAM_ASSERT(pb3_sizeof_ntrytes(81) == trits_size(id));
  ERR_BIND_RETURN(pb3_unwrap_absorb_ntrytes(s, b, id), e);
  /*  MSSig mssig; */
  ERR_BIND_RETURN(mam_msg_unwrap_mssig(s, b, ms, pk), e);

  return e;
}

/* Channel */

static size_t mam_msg_channel_wrap_size() {
  // absorb tryte version + absorb external tryte channel_id[81]
  return pb3_sizeof_tryte() + 0;
}

static void mam_msg_channel_wrap(mam_spongos_t *const spongos, trits_t *const buffer, tryte_t const version,
                                 trits_t const channel_id) {
  MAM_ASSERT(mam_msg_channel_wrap_size() <= trits_size(*buffer));
  MAM_ASSERT(pb3_sizeof_ntrytes(81) == trits_size(channel_id));

  // absorb tryte version
  pb3_wrap_absorb_tryte(spongos, buffer, version);
  // absorb external tryte channel_id[81]
  pb3_absorb_external_ntrytes(spongos, channel_id);
}

static retcode_t mam_msg_channel_unwrap(mam_spongos_t *const spongos, trits_t *const buffer, tryte_t *const version,
                                        trits_t channel_id) {
  MAM_ASSERT(pb3_sizeof_ntrytes(81) == trits_size(channel_id));

  retcode_t ret = RC_OK;

  // absorb tryte version
  if ((ret = pb3_unwrap_absorb_tryte(spongos, buffer, version)) != RC_OK) {
    return ret;
  }

  // absorb external tryte channel_id[81]
  pb3_absorb_external_ntrytes(spongos, channel_id);

  return ret;
}

/* Endpoint */

static size_t mam_msg_wrap_pubkey_chid_size() {
  return 0
         /*  absorb null chid; */
         + 0;
}

static void mam_msg_wrap_pubkey_chid() { /*  absorb null chid; */
}

static retcode_t mam_msg_unwrap_pubkey_chid() {
  /*  absorb null chid; */
  return RC_OK;
}

static size_t mam_msg_wrap_pubkey_epid_size() {
  return 0
         /*  absorb tryte epid[81]; */
         + pb3_sizeof_ntrytes(81);
}

static void mam_msg_wrap_pubkey_epid(mam_spongos_t *s, trits_t *b, trits_t epid) {
  MAM_ASSERT(mam_msg_wrap_pubkey_epid_size() <= trits_size(*b));
  MAM_ASSERT(pb3_sizeof_ntrytes(81) == trits_size(epid));

  /*  absorb tryte epid[81]; */
  pb3_wrap_absorb_ntrytes(s, b, epid);
}

static retcode_t mam_msg_unwrap_pubkey_epid(mam_spongos_t *s, trits_t *b, trits_t epid) {
  retcode_t e = RC_OK;

  /*  absorb tryte epid[81]; */
  MAM_ASSERT(pb3_sizeof_ntrytes(81) == trits_size(epid));
  ERR_BIND_RETURN(pb3_unwrap_absorb_ntrytes(s, b, epid), e);

  return e;
}

static size_t mam_msg_wrap_pubkey_chid1_size(mam_mss_t const *const mss) { return mam_msg_wrap_signedid_size(mss); }

static retcode_t mam_msg_wrap_pubkey_chid1(mam_spongos_t *s, trits_t *b, trits_t chid1, mam_mss_t *m) {
  return mam_msg_wrap_signedid(s, b, chid1, m);
}

static retcode_t mam_msg_unwrap_pubkey_chid1(mam_spongos_t *s, trits_t *b, trits_t chid1, mam_spongos_t *ms,
                                             trits_t pk) {
  return mam_msg_unwrap_signedid(s, b, chid1, ms, pk);
}

static size_t mam_msg_wrap_pubkey_epid1_size(mam_mss_t const *const mss) { return mam_msg_wrap_signedid_size(mss); }

static retcode_t mam_msg_wrap_pubkey_epid1(mam_spongos_t *s, trits_t *b, trits_t epid1, mam_mss_t *m) {
  return mam_msg_wrap_signedid(s, b, epid1, m);
}

static retcode_t mam_msg_unwrap_pubkey_epid1(mam_spongos_t *s, trits_t *b, trits_t epid1, mam_spongos_t *ms,
                                             trits_t pk) {
  return mam_msg_unwrap_signedid(s, b, epid1, ms, pk);
}

/* Keyload */

static size_t mam_msg_wrap_keyload_psk_size() {
  return 0
         /*  absorb tryte id[27]; */
         + pb3_sizeof_ntrytes(27)
         /*  absorb external tryte psk[81]; */
         + 0
         /*  commit; */
         /*  crypt tryte ekey[81]; */
         + pb3_sizeof_ntrytes(81);
}

static void mam_msg_wrap_keyload_psk(mam_spongos_t *s, trits_t *b, trits_t key, trits_t id, trits_t psk) {
  MAM_ASSERT(mam_msg_wrap_keyload_psk_size() <= trits_size(*b));
  MAM_ASSERT(pb3_sizeof_ntrytes(81) == trits_size(key));
  MAM_ASSERT(pb3_sizeof_ntrytes(27) == trits_size(id));
  MAM_ASSERT(pb3_sizeof_ntrytes(81) == trits_size(psk));

  /*  absorb tryte id[27]; */
  pb3_wrap_absorb_ntrytes(s, b, id);
  /*  absorb external tryte psk[81]; */
  pb3_absorb_external_ntrytes(s, psk);
  /*  commit; */
  mam_spongos_commit(s);
  /*  crypt tryte ekey[81]; */
  pb3_wrap_crypt_ntrytes(s, b, key);
}

static retcode_t mam_msg_unwrap_keyload_psk(mam_spongos_t *s, trits_t *b, trits_t key, bool *key_found,
                                            mam_psk_t_set_t p) {
  retcode_t e = RC_OK;
  MAM_TRITS_DEF(id, MAM_PSK_ID_SIZE);
  id = MAM_TRITS_INIT(id, MAM_PSK_ID_SIZE);
  trit_t key2_trits[MAM_SPONGE_KEY_SIZE];
  trits_t key2 = trits_from_rep(MAM_SPONGE_KEY_SIZE, key2_trits);

  MAM_ASSERT(key_found);
  MAM_ASSERT(pb3_sizeof_ntrytes(81) == trits_size(key));

  /*  absorb tryte id[27]; */
  ERR_BIND_RETURN(pb3_unwrap_absorb_ntrytes(s, b, id), e);

  mam_psk_t_set_entry_t *entry = NULL;
  mam_psk_t_set_entry_t *tmp = NULL;
  bool psk_found = false;
  HASH_ITER(hh, p, entry, tmp) {
    if (trits_cmp_eq(id, mam_psk_id(&entry->value))) {
      psk_found = true;
      break;
    }
  }

  if (psk_found) {
    /*  absorb external tryte psk[81]; */
    pb3_absorb_external_ntrytes(s, mam_psk_key(&entry->value));
    /*  commit; */
    mam_spongos_commit(s);
    /*  crypt tryte ekey[81]; */
    ERR_BIND_RETURN(pb3_unwrap_crypt_ntrytes(s, b, key2), e);

    if (*key_found) {
      ERR_GUARD_RETURN(trits_cmp_eq(key, key2), RC_MAM_KEYLOAD_OVERLOADED);
    } else {
      trits_copy(key2, key);
      *key_found = true;
    }

  } else { /* skip */
    ERR_GUARD_RETURN(MAM_SPONGE_KEY_SIZE <= trits_size(*b), RC_MAM_PB3_EOF);
    pb3_trits_take(b, MAM_SPONGE_KEY_SIZE);
    /* spongos state is corrupted */
  }

  return e;
}

static size_t mam_msg_wrap_keyload_ntru_size() {
  return 0
         /*  absorb tryte id[27]; */
         + pb3_sizeof_ntrytes(27)
         /*  absorb tryte ekey[3072]; */
         + pb3_sizeof_ntrytes(3072);
}

static retcode_t mam_msg_wrap_keyload_ntru(mam_spongos_t *const spongos, trits_t *const buffer, trits_t key,
                                           mam_ntru_pk_t const *const ntru_pk, mam_prng_t const *const prng,
                                           mam_spongos_t *ntru_spongos, trits_t nonce) {
  retcode_t ret = RC_OK;
  trits_t ekey;

  MAM_ASSERT(mam_msg_wrap_keyload_ntru_size() <= trits_size(*buffer));
  MAM_ASSERT(MAM_NTRU_KEY_SIZE == trits_size(key));

  /*  absorb tryte id[27]; */
  pb3_wrap_absorb_ntrytes(spongos, buffer, mam_ntru_pk_id(ntru_pk));
  /*  absorb tryte ekey[3072]; */
  ekey = pb3_trits_take(buffer, MAM_NTRU_EKEY_SIZE);
  if ((ret = ntru_pk_encr(ntru_pk, prng, ntru_spongos, nonce, key, ekey)) != RC_OK) {
    return ret;
  }
  mam_spongos_absorb(spongos, ekey);

  return RC_OK;
}

static retcode_t mam_msg_unwrap_keyload_ntru(mam_spongos_t *s, trits_t *b, trits_t key, bool *key_found,
                                             mam_ntru_sk_t_set_t n, mam_spongos_t *ns) {
  retcode_t e = RC_OK;
  trits_t ekey;
  MAM_TRITS_DEF(id, 81);
  id = MAM_TRITS_INIT(id, 81);
  trit_t key2_trits[MAM_SPONGE_KEY_SIZE];
  trits_t key2 = trits_from_rep(MAM_SPONGE_KEY_SIZE, key2_trits);

  MAM_ASSERT(MAM_NTRU_KEY_SIZE == trits_size(key));

  /*  absorb tryte id[27]; */
  ERR_BIND_RETURN(pb3_unwrap_absorb_ntrytes(s, b, id), e);

  mam_ntru_sk_t_set_entry_t *entry = NULL;
  mam_ntru_sk_t_set_entry_t *tmp = NULL;
  bool ntru_found = false;
  HASH_ITER(hh, n, entry, tmp) {
    if (trits_cmp_eq(id, mam_ntru_pk_id(&entry->value.public_key))) {
      ntru_found = true;
      break;
    }
  }

  if (ntru_found) {
    /*  absorb tryte ekey[3072]; */
    ERR_GUARD_RETURN(MAM_NTRU_EKEY_SIZE <= trits_size(*b), RC_MAM_PB3_EOF);
    ekey = pb3_trits_take(b, MAM_NTRU_EKEY_SIZE);
    ERR_GUARD_RETURN(ntru_sk_decr(&entry->value, ns, ekey, key2), RC_MAM_PB3_BAD_EKEY);
    mam_spongos_absorb(s, ekey);

    if (*key_found) {
      ERR_GUARD_RETURN(trits_cmp_eq(key, key2), RC_MAM_KEYLOAD_OVERLOADED);
    } else {
      trits_copy(key2, key);
      *key_found = true;
    }

  } else { /* skip */
    ERR_GUARD_RETURN(MAM_NTRU_EKEY_SIZE <= trits_size(*b), RC_MAM_PB3_EOF);
    pb3_trits_take(b, MAM_NTRU_EKEY_SIZE);
    /* spongos state is corrupted */
  }

  return e;
}

/* Packet */

static size_t mam_msg_wrap_checksum_none_size() {
  return 0
         /*  absorb null none; */
         + 0;
}

static void mam_msg_wrap_checksum_none() { /*  absorb null none; */
}

static retcode_t mam_msg_unwrap_checksum_none() {
  /*  absorb null none; */
  return RC_OK;
}

static size_t mam_msg_wrap_checksum_mac_size() { return mam_msg_wrap_mac_size(); }

static void mam_msg_wrap_checksum_mac(mam_spongos_t *s, trits_t *b) { mam_msg_wrap_mac(s, b); }

static retcode_t mam_msg_unwrap_checksum_mac(mam_spongos_t *s, trits_t *b) { return mam_msg_unwrap_mac(s, b); }

static size_t mam_msg_wrap_checksum_mssig_size(mam_mss_t const *const m) { return mam_msg_wrap_mssig_size(m); }

static retcode_t mam_msg_wrap_checksum_mssig(mam_spongos_t *s, trits_t *b, mam_mss_t *m) {
  return mam_msg_wrap_mssig(s, b, m);
}

static retcode_t mam_msg_unwrap_checksum_mssig(mam_spongos_t *s, trits_t *b, mam_spongos_t *ms, trits_t pk) {
  return mam_msg_unwrap_mssig(s, b, ms, pk);
}

size_t mam_msg_header_size(mam_channel_t const *const ch, mam_endpoint_t const *const ep,
                           mam_channel_t const *const ch1, mam_endpoint_t const *const ep1, mam_psk_t_set_t const psks,
                           mam_ntru_pk_t_set_t const ntru_pks) {
  size_t sz = 0;

  MAM_ASSERT(ch);

  /* channel */
  sz += mam_msg_channel_wrap_size();

  /* endpoint */
  /*  absorb oneof pubkey */
  sz += pb3_sizeof_oneof();

  if (ch1) {
    // SignedId chid1 = 2;
    sz += mam_msg_wrap_pubkey_chid1_size(&ch->mss);
  } else if (ep1) {
    // SignedId epid1 = 3;
    sz += mam_msg_wrap_pubkey_epid1_size(&ch->mss);
  } else if (ep) {
    // absorb tryte epid[81] = 1;
    sz += mam_msg_wrap_pubkey_epid_size();
  } else {
    //  null chid = 0;
    sz += mam_msg_wrap_pubkey_chid_size();
  }

  /* header */
  /* absorb trint typeid; */
  sz += pb3_sizeof_trint();
  {
    size_t keyload_count = 0;

    size_t num_pre_shared_keys = mam_psk_t_set_size(psks);
    keyload_count += num_pre_shared_keys;
    sz += (pb3_sizeof_oneof() + mam_msg_wrap_keyload_psk_size()) * num_pre_shared_keys;

    size_t num_pre_ntru_keys = mam_ntru_pk_t_set_size(ntru_pks);
    keyload_count += num_pre_ntru_keys;
    sz += (pb3_sizeof_oneof() + mam_msg_wrap_keyload_ntru_size()) * num_pre_ntru_keys;

    /*  absorb repeated */
    sz += pb3_sizeof_repeated(keyload_count);
  }
  /*  external secret tryte key[81]; */
  sz += 0;

  /* packets */
  return sz;
}

retcode_t mam_msg_write_header(mam_msg_write_context_t *const ctx, mam_prng_t const *const prng,
                               mam_channel_t *const ch, mam_endpoint_t *const ep, mam_channel_t *const ch1,
                               mam_endpoint_t *const ep1, trits_t msg_id, trint9_t msg_type_id,
                               mam_psk_t_set_t const psks, mam_ntru_pk_t_set_t const ntru_pks, trits_t *const msg) {
  retcode_t ret;
  trit_t session_key_trits[MAM_SPONGE_KEY_SIZE];
  trits_t session_key = trits_from_rep(MAM_SPONGE_KEY_SIZE, session_key_trits);

  MAM_TRITS_DEF(skn, MAM_MSS_SKN_SIZE);
  skn = MAM_TRITS_INIT(skn, MAM_MSS_SKN_SIZE);

  MAM_ASSERT(ctx);
  MAM_ASSERT(ch);
  MAM_ASSERT(msg);

  MAM_ASSERT(!(trits_size(*msg) < mam_msg_header_size(ch, ep, ch1, ep1, psks, ntru_pks)));

  if (ep) {
    mam_mss_skn(&ep->mss, skn);
  } else {
    mam_mss_skn(&ch->mss, skn);
  }

  /* generate session key */
  if (psks == NULL && ntru_pks == NULL) {  // public
    trits_set_zero(session_key);
  } else {
    mam_prng_gen5(prng, MAM_PRNG_DST_SEC_KEY, mam_channel_name_size(ch), mam_channel_name(ch),
                  ep ? mam_endpoint_name_size(ep) : trits_null(), ep ? mam_endpoint_name(ep) : trits_null(), skn,
                  session_key);
  }

  /* choose recipient */
  mam_spongos_init(&ctx->spongos);

  /* wrap Channel */
  mam_msg_channel_wrap(&ctx->spongos, msg, 0, mam_channel_id(ch));

  /* wrap Endpoint */
  {
    tryte_t pubkey;

    if (ch1) { /*  SignedId chid1 = 2; */
      pubkey = (tryte_t)MAM_MSG_PUBKEY_CHID1;
      pb3_wrap_absorb_tryte(&ctx->spongos, msg, pubkey);
      ERR_BIND_RETURN(mam_msg_wrap_pubkey_chid1(&ctx->spongos, msg, mam_channel_id(ch1), &ch->mss), ret);
    } else if (ep1) { /*  SignedId epid1 = 3; */
      pubkey = (tryte_t)MAM_MSG_PUBKEY_EPID1;
      pb3_wrap_absorb_tryte(&ctx->spongos, msg, pubkey);
      ERR_BIND_RETURN(mam_msg_wrap_pubkey_epid1(&ctx->spongos, msg, mam_endpoint_id(ep1), &ch->mss), ret);
    } else if (ep) { /*  absorb tryte epid[81] = 1; */
      pubkey = (tryte_t)MAM_MSG_PUBKEY_EPID;
      pb3_wrap_absorb_tryte(&ctx->spongos, msg, pubkey);
      mam_msg_wrap_pubkey_epid(&ctx->spongos, msg, mam_endpoint_id(ep));
    } else { /*  absorb null chid = 0; */
      pubkey = (tryte_t)MAM_MSG_PUBKEY_CHID;
      pb3_wrap_absorb_tryte(&ctx->spongos, msg, pubkey);
      mam_msg_wrap_pubkey_chid();
    }
  }

  /* wrap Header */
  {
    /*  absorb tryte msgid[27]; */
    pb3_absorb_external_ntrytes(&ctx->spongos, msg_id);
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
        keyload = (tryte_t)MAM_MSG_KEYLOAD_PSK;
        pb3_wrap_absorb_tryte(&ctx->spongos, msg, keyload);
        /*  fork; */
        mam_mam_spongos_fork(&ctx->spongos, &spongos_fork);
        /*  KeyloadPSK psk = 1; */
        mam_msg_wrap_keyload_psk(&spongos_fork, msg, session_key, mam_psk_id(&curr_entry_psk->value),
                                 mam_psk_key(&curr_entry_psk->value));
      }

      mam_ntru_pk_t_set_entry_t *curr_entry_ntru = NULL;
      mam_ntru_pk_t_set_entry_t *tmp_entry_ntru = NULL;

      HASH_ITER(hh, ntru_pks, curr_entry_ntru, tmp_entry_ntru) {
        /*  absorb oneof keyload */
        keyload = (tryte_t)MAM_MSG_KEYLOAD_NTRU;
        pb3_wrap_absorb_tryte(&ctx->spongos, msg, keyload);
        /*  fork; */
        mam_mam_spongos_fork(&ctx->spongos, &spongos_fork);
        /*  KeyloadNTRU ntru = 2; */
        // TODO ERR_BIND_RETURN
        mam_msg_wrap_keyload_ntru(&spongos_fork, msg, session_key, &curr_entry_ntru->value, prng, &spongos_ntru,
                                  msg_id);
      }
    }

    /*  absorb external tryte key[81]; */
    pb3_absorb_external_ntrytes(&ctx->spongos, session_key);
    /*  commit; */
    mam_spongos_commit(&ctx->spongos);
  }

  memset_safe(trits_begin(session_key), trits_size(session_key), 0, trits_size(session_key));

  return RC_OK;
}

size_t mam_msg_packet_size(mam_msg_checksum_t checksum, mam_mss_t const *const mss, size_t payload_size) {
  size_t sz = 0;
  MAM_ASSERT(0 == payload_size % 3);
  sz = 0
       /*  absorb tryte sz; */
       + pb3_sizeof_size_t(payload_size / 3)
       /*  crypt tryte payload[sz]; */
       + pb3_sizeof_ntrytes(payload_size / 3)
       /*  absorb oneof checksum */
       + pb3_sizeof_oneof();

  if (MAM_MSG_CHECKSUM_NONE == checksum) {
    // absorb null none = 0;
    sz += mam_msg_wrap_checksum_none_size();
  } else if (MAM_MSG_CHECKSUM_MAC == checksum) {
    //  MAC mac = 1;
    sz += mam_msg_wrap_checksum_mac_size();
  } else if (MAM_MSG_CHECKSUM_SIG == checksum) {
    //  MSSig mssig = 2;
    MAM_ASSERT(mss);
    sz += mam_msg_wrap_checksum_mssig_size(mss);
  } else {
    /*  commit; */
    MAM_ASSERT(0);
  }

  return sz;
}

retcode_t mam_msg_write_packet(mam_msg_write_context_t *const ctx, mam_msg_checksum_t checksum, trits_t const payload,
                               trits_t *const buffer) {
  retcode_t ret;
  MAM_ASSERT(ctx);
  MAM_ASSERT(buffer);

  MAM_ASSERT(!(trits_size(*buffer) < mam_msg_packet_size(checksum, ctx->mss, trits_size(payload))));

  /*  absorb long trint ord; */
  {
    trit_t ord_trits[18];
    trits_t ord = trits_from_rep(18, ord_trits);
    trits_put18(ord, ctx->ord);
    pb3_absorb_external_ntrytes(&ctx->spongos, ord);
  }

  /*  absorb tryte sz; */
  pb3_wrap_absorb_size_t(&ctx->spongos, buffer, trits_size(payload) / 3);
  /*  crypt tryte payload[sz]; */
  pb3_wrap_crypt_ntrytes(&ctx->spongos, buffer, payload);

  /*  absorb oneof checksum */
  pb3_wrap_absorb_tryte(&ctx->spongos, buffer, (tryte_t)checksum);
  if (MAM_MSG_CHECKSUM_NONE == checksum) {
    /*    absorb null none = 0; */
    mam_msg_wrap_checksum_none();
  } else if (MAM_MSG_CHECKSUM_MAC == checksum) {
    /*    MAC mac = 1; */
    mam_msg_wrap_checksum_mac(&ctx->spongos, buffer);
  } else if (MAM_MSG_CHECKSUM_SIG == checksum) {
    /*    MSSig mssig = 2; */
    ERR_BIND_RETURN(mam_msg_wrap_checksum_mssig(&ctx->spongos, buffer, ctx->mss), ret);
  } else {
    MAM_ASSERT(0);
  }
  /*  commit; */
  mam_spongos_commit(&ctx->spongos);

  return RC_OK;
}

retcode_t mam_msg_read_header(mam_msg_read_context_t *const ctx, trits_t *const msg, mam_psk_t_set_t const psks,
                              mam_ntru_sk_t_set_t const ntru_sks, trits_t msg_id,
                              mam_pk_t_set_t *const trusted_channel_pks, mam_pk_t_set_t *const trusted_endpoint_pks) {
  retcode_t ret;

  MAM_ASSERT(ctx);

  trit_t chid[MAM_CHANNEL_ID_TRIT_SIZE];
  memcpy(chid, ctx->pk.key, MAM_CHANNEL_ID_TRIT_SIZE);

  mam_spongos_init(&ctx->spongos);

  trit_t session_key_trits[MAM_SPONGE_KEY_SIZE];
  trits_t session_key = trits_from_rep(MAM_SPONGE_KEY_SIZE, session_key_trits);

  /* unwrap Channel */
  {
    tryte_t ver = -1;
    ERR_BIND_RETURN(mam_msg_channel_unwrap(&ctx->spongos, msg, &ver, trits_from_rep(MAM_CHANNEL_ID_TRIT_SIZE, chid)),
                    ret);
    ERR_GUARD_RETURN(0 == ver, RC_MAM_VERSION_NOT_SUPPORTED);

    if (!mam_pk_t_set_contains(*trusted_channel_pks, &ctx->pk)) {
      return RC_MAM_CHANNEL_NOT_TRUSTED;
    }
  }

  /* unwrap Endpoint */
  {
    mam_spongos_t spongos_mss;
    tryte_t pubkey = -1;
    ERR_BIND_RETURN(pb3_unwrap_absorb_tryte(&ctx->spongos, msg, &pubkey), ret);
    ERR_GUARD_RETURN(0 <= pubkey && pubkey <= 3, RC_MAM_PB3_BAD_ONEOF);

    if (MAM_MSG_PUBKEY_CHID1 == pubkey) { /*  SignedId chid1 = 2; */
      ERR_BIND_RETURN(
          mam_msg_unwrap_pubkey_chid1(&ctx->spongos, msg, trits_from_rep(MAM_CHANNEL_ID_TRIT_SIZE, ctx->pk.key),
                                      &spongos_mss, trits_from_rep(MAM_CHANNEL_ID_TRIT_SIZE, chid)),
          ret);
      mam_pk_t_set_add(trusted_channel_pks, &ctx->pk);
    } else if (MAM_MSG_PUBKEY_EPID1 == pubkey) { /*  SignedId epid1 = 3; */

      ERR_BIND_RETURN(
          mam_msg_unwrap_pubkey_epid1(&ctx->spongos, msg, trits_from_rep(MAM_CHANNEL_ID_TRIT_SIZE, ctx->pk.key),
                                      &spongos_mss, trits_from_rep(MAM_CHANNEL_ID_TRIT_SIZE, chid)),
          ret);
      mam_pk_t_set_add(trusted_endpoint_pks, &ctx->pk);
    } else if (MAM_MSG_PUBKEY_EPID == pubkey) { /*  absorb tryte epid[81] = 1; */
      ERR_BIND_RETURN(
          mam_msg_unwrap_pubkey_epid(&ctx->spongos, msg, trits_from_rep(MAM_CHANNEL_ID_TRIT_SIZE, ctx->pk.key)), ret);
      if (!mam_pk_t_set_contains(*trusted_endpoint_pks, &ctx->pk)) {
        return RC_MAM_ENDPOINT_NOT_TRUSTED;
      }
    } else if (MAM_MSG_PUBKEY_CHID == pubkey) { /*  absorb null chid = 0; */
      ERR_BIND_RETURN(mam_msg_unwrap_pubkey_chid(), ret);
    } else
      MAM_ASSERT(0);
  }

  /* unwrap Header */
  {
    trint9_t msg_type_id;

    /*  absorb tryte msg_id[27]; */
    pb3_absorb_external_ntrytes(&ctx->spongos, msg_id);
    /*  absorb trint typeid; */
    ERR_BIND_RETURN(pb3_unwrap_absorb_trint(&ctx->spongos, msg, &msg_type_id), ret);
    // TODO switch case on the msg_type_id
    {
      /*  repeated */
      size_t keyload_count = 0;
      bool key_found = false;
      mam_spongos_t spongos_ntru;

      ERR_BIND_RETURN(pb3_unwrap_absorb_size_t(&ctx->spongos, msg, &keyload_count), ret);

      if (0 < keyload_count) {
        mam_spongos_t spongos_fork;

        for (ret = RC_OK; ret == RC_OK && keyload_count--;) {
          tryte_t keyload = -1;

          /*  absorb oneof keyload */
          ERR_BIND_RETURN(pb3_unwrap_absorb_tryte(&ctx->spongos, msg, &keyload), ret);
          ERR_GUARD_RETURN(1 <= keyload && keyload <= 2, RC_MAM_PB3_BAD_ONEOF);
          /*  fork; */
          mam_mam_spongos_fork(&ctx->spongos, &spongos_fork);

          if (MAM_MSG_KEYLOAD_PSK == keyload) { /*  KeyloadPSK psk = 1; */
            ERR_BIND_RETURN(mam_msg_unwrap_keyload_psk(&spongos_fork, msg, session_key, &key_found, psks), ret);

          } else if (MAM_MSG_KEYLOAD_NTRU == keyload) { /*  KeyloadNTRU ntru = 2; */
            ERR_BIND_RETURN(
                mam_msg_unwrap_keyload_ntru(&spongos_fork, msg, session_key, &key_found, ntru_sks, &spongos_ntru), ret);

          } else
            ERR_GUARD_RETURN(0, RC_MAM_PB3_BAD_ONEOF);
        }

        ERR_GUARD_RETURN(key_found, RC_MAM_KEYLOAD_IRRELEVANT);
      } else
        /* no recipients => public mode */
        trits_set_zero(session_key);
    }
  }

  /*  absorb external tryte key[81]; */
  pb3_absorb_external_ntrytes(&ctx->spongos, session_key);
  /*  commit; */
  mam_spongos_commit(&ctx->spongos);

  return ret;
}

retcode_t mam_msg_read_packet(mam_msg_read_context_t *const ctx, trits_t *const buffer, trits_t *const payload) {
  retcode_t e = RC_OK;
  trits_t p = trits_null();

  MAM_ASSERT(ctx);
  MAM_ASSERT(buffer);
  MAM_ASSERT(payload);

  size_t sz = 0;
  tryte_t checksum = -1;

  /*  absorb long trint ord; */
  {
    trit_t ord_trits[18];
    trits_t ord = trits_from_rep(18, ord_trits);
    trits_put18(ord, ctx->ord);
    pb3_absorb_external_ntrytes(&ctx->spongos, ord);
  }
  /*TODO: check ord */

  /*  absorb tryte sz; */
  ERR_BIND_RETURN(pb3_unwrap_absorb_size_t(&ctx->spongos, buffer, &sz), e);
  /*  crypt tryte payload[sz]; */
  if (trits_is_null(*payload)) {
    p = trits_alloc(pb3_sizeof_ntrytes(sz));
    ERR_GUARD_RETURN(!trits_is_null(p), RC_OOM);
  } else {
    ERR_GUARD_GOTO(trits_size(*payload) >= pb3_sizeof_ntrytes(sz), RC_MAM_BUFFER_TOO_SMALL, e, cleanup);
    p = pb3_trits_take(payload, pb3_sizeof_ntrytes(sz));
  }
  ERR_BIND_GOTO(pb3_unwrap_crypt_ntrytes(&ctx->spongos, buffer, p), e, cleanup);

  /*  absorb oneof checksum */
  ERR_BIND_GOTO(pb3_unwrap_absorb_tryte(&ctx->spongos, buffer, &checksum), e, cleanup);

  if (MAM_MSG_CHECKSUM_NONE == checksum) {
    /*    absorb null none = 0; */
    ERR_BIND_GOTO(mam_msg_unwrap_checksum_none(), e, cleanup);
  } else if (MAM_MSG_CHECKSUM_MAC == checksum) {
    /*    MAC mac = 1; */
    ERR_BIND_GOTO(mam_msg_unwrap_checksum_mac(&ctx->spongos, buffer), e, cleanup);
  } else if (MAM_MSG_CHECKSUM_SIG == checksum) {
    mam_spongos_t spongos_mss;
    /*    MSSig mssig = 2; */
    ERR_BIND_GOTO(mam_msg_unwrap_checksum_mssig(&ctx->spongos, buffer, &spongos_mss,
                                                trits_from_rep(MAM_CHANNEL_ID_TRIT_SIZE, ctx->pk.key)),
                  e, cleanup);
  } else {
    ERR_GUARD_GOTO(0, RC_MAM_PB3_BAD_ONEOF, e, cleanup);
  }

  /*  commit; */
  mam_spongos_commit(&ctx->spongos);

  if (trits_is_null(*payload)) *payload = p;
  p = trits_null();

cleanup:
  trits_free(p);
  return e;
}

size_t mam_msg_write_ctx_serialized_size(mam_msg_write_context_t const *const ctx) {
  return MAM_CHANNEL_ID_TRIT_SIZE + mam_spongos_serialized_size(&ctx->spongos) + MAM_MSG_ORD_SIZE + MAM_MSS_PK_SIZE;
}

void mam_msg_write_ctx_serialize(mam_msg_write_context_t const *const ctx, trits_t *const buffer) {
  pb3_encode_ntrytes(trits_from_rep(MAM_CHANNEL_ID_TRIT_SIZE, ctx->chid), buffer);
  mam_spongos_serialize(&ctx->spongos, buffer);
  trits_put18(*buffer, ctx->ord);
  trits_advance(buffer, MAM_MSG_ORD_SIZE);
  pb3_encode_ntrytes(trits_from_rep(MAM_MSS_PK_SIZE, ctx->mss->root), buffer);
}

retcode_t mam_msg_write_ctx_deserialize(trits_t *const buffer, mam_msg_write_context_t *const ctx,
                                        mam_channel_t_set_t const channels) {
  retcode_t ret = RC_OK;
  trit_t mss_root[MAM_MSS_PK_SIZE];
  mam_channel_t_set_entry_t *ch_entry = NULL;
  mam_channel_t_set_entry_t *ch_tmp = NULL;
  mam_endpoint_t_set_entry_t *ep_entry = NULL;
  mam_endpoint_t_set_entry_t *ep_tmp = NULL;

  ERR_BIND_RETURN(pb3_decode_ntrytes(trits_from_rep(MAM_CHANNEL_ID_TRIT_SIZE, ctx->chid), buffer), ret);
  ERR_BIND_RETURN(mam_spongos_deserialize(buffer, &ctx->spongos), ret);
  ctx->ord = trits_get18(*buffer);
  trits_advance(buffer, MAM_MSG_ORD_SIZE);
  ctx->mss = NULL;
  ERR_BIND_RETURN(pb3_decode_ntrytes(trits_from_rep(MAM_MSS_PK_SIZE, mss_root), buffer), ret);

  SET_ITER(channels, ch_entry, ch_tmp) {
    if (memcmp(trits_begin(mam_channel_id(&ch_entry->value)), mss_root, MAM_CHANNEL_ID_TRIT_SIZE) == 0) {
      ctx->mss = &ch_entry->value.mss;
      return RC_OK;
    } else {
      SET_ITER(ch_entry->value.endpoints, ep_entry, ep_tmp) {
        if (memcmp(trits_begin(mam_endpoint_id(&ep_entry->value)), mss_root, MAM_ENDPOINT_ID_TRIT_SIZE) == 0) {
          ctx->mss = &ep_entry->value.mss;
          return RC_OK;
        }
      }
    }
  }

  if (ctx->mss == NULL) {
    return RC_MAM_MSS_NOT_FOUND;
  }

  return ret;
}

size_t mam_msg_read_ctx_serialized_size(mam_msg_read_context_t const *const ctx) {
  return mam_spongos_serialized_size(&ctx->spongos) + MAM_CHANNEL_ID_TRIT_SIZE + MAM_MSG_ORD_SIZE;
}

void mam_msg_read_ctx_serialize(mam_msg_read_context_t const *const ctx, trits_t *const buffer) {
  mam_spongos_serialize(&ctx->spongos, buffer);
  pb3_encode_ntrytes(trits_from_rep(MAM_CHANNEL_ID_TRIT_SIZE, ctx->pk.key), buffer);
  trits_put18(*buffer, ctx->ord);
  trits_advance(buffer, MAM_MSG_ORD_SIZE);
}

retcode_t mam_msg_read_ctx_deserialize(trits_t *const buffer, mam_msg_read_context_t *const ctx) {
  retcode_t ret = RC_OK;

  ERR_BIND_RETURN(mam_spongos_deserialize(buffer, &ctx->spongos), ret);
  ERR_BIND_RETURN(pb3_decode_ntrytes(trits_from_rep(MAM_CHANNEL_ID_TRIT_SIZE, ctx->pk.key), buffer), ret);
  ctx->ord = trits_get18(*buffer);
  trits_advance(buffer, MAM_MSG_ORD_SIZE);

  return ret;
}
