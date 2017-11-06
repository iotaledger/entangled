#define HASH_LENGTH 243
#define OFFSET_LENGTH 4
#define NONCE_LENGTH HASH_LENGTH / 3
#define INT_LENGTH NONCE_LENGTH / 3
#define NONCE_START HASH_LENGTH - NONCE_LENGTH
#define NONCE_INIT_START NONCE_START + OFFSET_LENGTH
#define NONCE_INCREMENT_START NONCE_INIT_START + INT_LENGTH
#define NUMBER_OF_ROUNDS 81
#define STATE_LENGTH 3 * HASH_LENGTH
#define HALF_LENGTH 364
#define HIGH_BITS 0xFFFFFFFFFFFFFFFF
#define LOW_BITS 0x0000000000000000
//#define HIGH_BITS
//0b1111111111111111111111111111111111111111111111111111111111111111L
//#define LOW_BITS
//0b0000000000000000000000000000000000000000000000000000000000000000L
/**
 * t1 = j == 0? 0:(((j - 1)%2)+1)*HALF_LENGTH - ((j-1)>>1);
 */
__constant size_t INDEX[STATE_LENGTH + 1] = {
    0,   364, 728, 363, 727, 362, 726, 361, 725, 360, 724, 359, 723, 358, 722,
    357, 721, 356, 720, 355, 719, 354, 718, 353, 717, 352, 716, 351, 715, 350,
    714, 349, 713, 348, 712, 347, 711, 346, 710, 345, 709, 344, 708, 343, 707,
    342, 706, 341, 705, 340, 704, 339, 703, 338, 702, 337, 701, 336, 700, 335,
    699, 334, 698, 333, 697, 332, 696, 331, 695, 330, 694, 329, 693, 328, 692,
    327, 691, 326, 690, 325, 689, 324, 688, 323, 687, 322, 686, 321, 685, 320,
    684, 319, 683, 318, 682, 317, 681, 316, 680, 315, 679, 314, 678, 313, 677,
    312, 676, 311, 675, 310, 674, 309, 673, 308, 672, 307, 671, 306, 670, 305,
    669, 304, 668, 303, 667, 302, 666, 301, 665, 300, 664, 299, 663, 298, 662,
    297, 661, 296, 660, 295, 659, 294, 658, 293, 657, 292, 656, 291, 655, 290,
    654, 289, 653, 288, 652, 287, 651, 286, 650, 285, 649, 284, 648, 283, 647,
    282, 646, 281, 645, 280, 644, 279, 643, 278, 642, 277, 641, 276, 640, 275,
    639, 274, 638, 273, 637, 272, 636, 271, 635, 270, 634, 269, 633, 268, 632,
    267, 631, 266, 630, 265, 629, 264, 628, 263, 627, 262, 626, 261, 625, 260,
    624, 259, 623, 258, 622, 257, 621, 256, 620, 255, 619, 254, 618, 253, 617,
    252, 616, 251, 615, 250, 614, 249, 613, 248, 612, 247, 611, 246, 610, 245,
    609, 244, 608, 243, 607, 242, 606, 241, 605, 240, 604, 239, 603, 238, 602,
    237, 601, 236, 600, 235, 599, 234, 598, 233, 597, 232, 596, 231, 595, 230,
    594, 229, 593, 228, 592, 227, 591, 226, 590, 225, 589, 224, 588, 223, 587,
    222, 586, 221, 585, 220, 584, 219, 583, 218, 582, 217, 581, 216, 580, 215,
    579, 214, 578, 213, 577, 212, 576, 211, 575, 210, 574, 209, 573, 208, 572,
    207, 571, 206, 570, 205, 569, 204, 568, 203, 567, 202, 566, 201, 565, 200,
    564, 199, 563, 198, 562, 197, 561, 196, 560, 195, 559, 194, 558, 193, 557,
    192, 556, 191, 555, 190, 554, 189, 553, 188, 552, 187, 551, 186, 550, 185,
    549, 184, 548, 183, 547, 182, 546, 181, 545, 180, 544, 179, 543, 178, 542,
    177, 541, 176, 540, 175, 539, 174, 538, 173, 537, 172, 536, 171, 535, 170,
    534, 169, 533, 168, 532, 167, 531, 166, 530, 165, 529, 164, 528, 163, 527,
    162, 526, 161, 525, 160, 524, 159, 523, 158, 522, 157, 521, 156, 520, 155,
    519, 154, 518, 153, 517, 152, 516, 151, 515, 150, 514, 149, 513, 148, 512,
    147, 511, 146, 510, 145, 509, 144, 508, 143, 507, 142, 506, 141, 505, 140,
    504, 139, 503, 138, 502, 137, 501, 136, 500, 135, 499, 134, 498, 133, 497,
    132, 496, 131, 495, 130, 494, 129, 493, 128, 492, 127, 491, 126, 490, 125,
    489, 124, 488, 123, 487, 122, 486, 121, 485, 120, 484, 119, 483, 118, 482,
    117, 481, 116, 480, 115, 479, 114, 478, 113, 477, 112, 476, 111, 475, 110,
    474, 109, 473, 108, 472, 107, 471, 106, 470, 105, 469, 104, 468, 103, 467,
    102, 466, 101, 465, 100, 464, 99,  463, 98,  462, 97,  461, 96,  460, 95,
    459, 94,  458, 93,  457, 92,  456, 91,  455, 90,  454, 89,  453, 88,  452,
    87,  451, 86,  450, 85,  449, 84,  448, 83,  447, 82,  446, 81,  445, 80,
    444, 79,  443, 78,  442, 77,  441, 76,  440, 75,  439, 74,  438, 73,  437,
    72,  436, 71,  435, 70,  434, 69,  433, 68,  432, 67,  431, 66,  430, 65,
    429, 64,  428, 63,  427, 62,  426, 61,  425, 60,  424, 59,  423, 58,  422,
    57,  421, 56,  420, 55,  419, 54,  418, 53,  417, 52,  416, 51,  415, 50,
    414, 49,  413, 48,  412, 47,  411, 46,  410, 45,  409, 44,  408, 43,  407,
    42,  406, 41,  405, 40,  404, 39,  403, 38,  402, 37,  401, 36,  400, 35,
    399, 34,  398, 33,  397, 32,  396, 31,  395, 30,  394, 29,  393, 28,  392,
    27,  391, 26,  390, 25,  389, 24,  388, 23,  387, 22,  386, 21,  385, 20,
    384, 19,  383, 18,  382, 17,  381, 16,  380, 15,  379, 14,  378, 13,  377,
    12,  376, 11,  375, 10,  374, 9,   373, 8,   372, 7,   371, 6,   370, 5,
    369, 4,   368, 3,   367, 2,   366, 1,   365, 0};

/**
 * t2 = ((j%2)+1)*HALF_LENGTH - ((j)>>1);
 */

typedef long bc_trit_t;

void increment(__global bc_trit_t* mid_low, __global bc_trit_t* mid_high,
               __private size_t from_index, __private size_t to_index);
void copy_mid_to_state(__global bc_trit_t* mid_low, __global bc_trit_t* mid_high,
                       __global bc_trit_t* state_low, __global bc_trit_t* state_high,
                       __private size_t id, __private size_t l_size,
                       __private size_t l_trits);
void transform(__global bc_trit_t* state_low, __global bc_trit_t* state_high,
               __private size_t id, __private size_t l_size,
               __private size_t l_trits);
void check(__global bc_trit_t* state_low, __global bc_trit_t* state_high,
           __global volatile char* found,
           __constant size_t* min_weight_magnitude,
           __global bc_trit_t* nonce_probe, __private size_t gr_id);
void setup_ids(__private size_t* id, __private size_t* gid,
               __private size_t* gr_id, __private size_t* l_size,
               __private size_t* n_trits);

void increment(__global bc_trit_t* mid_low, __global bc_trit_t* mid_high,
               __private size_t from_index, __private size_t to_index) {
  size_t i;
  bc_trit_t carry = 1;
  bc_trit_t low, hi;
  for (i = from_index; i < to_index && carry != 0; i++) {
    low = mid_low[i];
    hi = mid_high[i];
    mid_low[i] = hi ^ low;
    mid_high[i] = low;
    carry = hi & (~low);
  }
}

void copy_mid_to_state(__global bc_trit_t* mid_low, __global bc_trit_t* mid_high,
                       __global bc_trit_t* state_low, __global bc_trit_t* state_high,
                       __private size_t id, __private size_t l_size,
                       __private size_t n_trits) {
  size_t i, j;
  for (i = 0; i < n_trits; i++) {
    j = id + i * l_size;
    state_low[j] = mid_low[j];
    state_high[j] = mid_high[j];
  }
}

void transform(__global bc_trit_t* state_low, __global bc_trit_t* state_high,
               __private size_t id, __private size_t l_size,
               __private size_t n_trits) {
  __private size_t round, i, j, k;
  __private bc_trit_t alpha, beta, gamma, delta, sp_low[3], sp_high[3];
  for (round = 0; round < NUMBER_OF_ROUNDS; round++) {
    for (i = 0; i < n_trits; i++) {
      j = id + i * l_size;
      k = j+1;
      alpha = state_low[INDEX[j]];
      beta = state_high[INDEX[j]];
      gamma = state_high[INDEX[k]];
      delta = (alpha | (~gamma)) & (state_low[INDEX[k]] ^ beta);

      sp_low[i] = ~delta;
      sp_high[i] = (alpha ^ gamma) | delta;
    }
    barrier(CLK_LOCAL_MEM_FENCE);
    for (i = 0; i < n_trits; i++) {
      j = id + i * l_size;
      state_low[j] = sp_low[i];
      state_high[j] = sp_high[i];
    }
    barrier(CLK_LOCAL_MEM_FENCE);
  }
}

void check(__global bc_trit_t* state_low, __global bc_trit_t* state_high,
           __global volatile char* found,
           __constant size_t* min_weight_magnitude,
           __global bc_trit_t* nonce_probe, __private size_t gr_id) {
  int i;
  *nonce_probe = HIGH_BITS;
  for (i = HASH_LENGTH - *min_weight_magnitude; i < HASH_LENGTH; i++) {
    *nonce_probe &= ~(state_low[i] ^ state_high[i]);
    if (*nonce_probe == 0)
      return;
  }
  if (*nonce_probe != 0) {
    //*nonce_probe = 1 << __builtin_ctzl(*nonce_probe);
    *found = gr_id + 1;
  }
}

void setup_ids(__private size_t* id, __private size_t* gid,
               __private size_t* gr_id, __private size_t* l_size,
               __private size_t* n_trits) {
  __private size_t l_rem;
  *id = get_local_id(0);
  *l_size = get_local_size(0);
  *gr_id = get_global_id(0) / *l_size;
  *gid = *gr_id * STATE_LENGTH;
  l_rem = STATE_LENGTH % *l_size;
  *n_trits = STATE_LENGTH / *l_size;
  *n_trits += l_rem == 0 ? 0 : 1;
  *n_trits -= (*n_trits) * (*id) < STATE_LENGTH ? 0 : 1;
}

__kernel void init(__global char* trit_hash, __global bc_trit_t* mid_low,
                   __global bc_trit_t* mid_high, __global bc_trit_t* state_low,
                   __global bc_trit_t* state_high,
                   __constant size_t* min_weight_magnitude,
                   __global volatile char* found, __global bc_trit_t* nonce_probe,
                   __constant size_t* loop_count) {
  __private size_t i, j, id, gid, gr_id, gl_off, l_size, n_trits;
  setup_ids(&id, &gid, &gr_id, &l_size, &n_trits);
  gl_off = get_global_offset(0);

  if (id == 0 && gr_id == 0) {
    *found = 0;
  }

  if (gr_id == 0)
    return;

  for (i = 0; i < n_trits; i++) {
    j = id + i * l_size;
    mid_low[gid + j] = mid_low[j];
    mid_high[gid + j] = mid_high[j];
  }

  if (id == 0) {
    for (i = 0; i < gr_id + gl_off; i++) {
      increment(&(mid_low[gid]), &(mid_high[gid]), NONCE_INIT_START,
                NONCE_INCREMENT_START);
    }
  }
}

__kernel void search(__global char* trit_hash, __global bc_trit_t* mid_low,
                     __global bc_trit_t* mid_high, __global bc_trit_t* state_low,
                     __global bc_trit_t* state_high,
                     __constant size_t* min_weight_magnitude,
                     __global volatile char* found,
                     __global bc_trit_t* nonce_probe,
                     __constant size_t* loop_count) {
  __private size_t i, id, gid, gr_id, l_size, n_trits;
  setup_ids(&id, &gid, &gr_id, &l_size, &n_trits);

  for (i = 0; i < *loop_count; i++) {
    if (id == 0)
      increment(&(mid_low[gid]), &(mid_high[gid]), NONCE_INCREMENT_START,
                HASH_LENGTH);

    barrier(CLK_LOCAL_MEM_FENCE);
    copy_mid_to_state(&(mid_low[gid]), &(mid_high[gid]), &(state_low[gid]),
                      &(state_high[gid]), id, l_size, n_trits);

    barrier(CLK_LOCAL_MEM_FENCE);
    transform(&(state_low[gid]), &(state_high[gid]), id, l_size, n_trits);

    barrier(CLK_LOCAL_MEM_FENCE);
    if (id == 0)
      check(&(state_low[gid]), &(state_high[gid]), found, min_weight_magnitude,
            &(nonce_probe[gr_id]), gr_id);

    barrier(CLK_LOCAL_MEM_FENCE);
    if (*found != 0)
      break;
  }
}

__kernel void finalize(__global char* trit_hash, __global bc_trit_t* mid_low,
                       __global bc_trit_t* mid_high, __global bc_trit_t* state_low,
                       __global bc_trit_t* state_high,
                       __constant size_t* min_weight_magnitude,
                       __global volatile char* found,
                       __global bc_trit_t* nonce_probe,
                       __constant size_t* loop_count) {
  __private size_t i, j, id, gid, gr_id, l_size, n_trits;
  setup_ids(&id, &gid, &gr_id, &l_size, &n_trits);

  if (gr_id == (size_t)(*found - 1) && nonce_probe[gr_id] != 0) {
    for (i = 0; i < n_trits; i++) {
      j = id + i * l_size;
      if (j < HASH_LENGTH) {
        trit_hash[j] =
            (mid_low[gid + j] & nonce_probe[gr_id]) == 0
                ? 1
                : (mid_high[gid + j] & nonce_probe[gr_id]) == 0 ? -1 : 0;
      }
    }
  }
}
