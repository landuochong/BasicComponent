#include <openssl/ssl.h>
#include <openssl/kdf.h>
#include "openssl/md5.h"
#include "utils/strutil.h"
#include "openssl/aes.h"
#include "crypt/ibase64.h"
#include "openssl/pem.h"

static std::string sha256Hex(unsigned char* str, int len)
{
	char buf[2];
	unsigned char hash[SHA256_DIGEST_LENGTH];
	SHA256_CTX sha256;
	SHA256_Init(&sha256);
	SHA256_Update(&sha256, str, len);
	SHA256_Final(hash, &sha256);
	std::string result = "";
	for (int i = 0; i < SHA256_DIGEST_LENGTH; i++)
	{
		sprintf(buf, "%02x", hash[i]);
		result = result + buf;
	}
	return result;
}

static void sha256(unsigned char* str, int len, unsigned char hash[SHA256_DIGEST_LENGTH])
{	
	SHA256_CTX sha256;
	SHA256_Init(&sha256);
	SHA256_Update(&sha256, str, len);
	SHA256_Final(hash, &sha256);
}

static int64_t padding(int n) {
	int64_t v = 1;
	for (int i = 0; i < n - 1; i++) {
		v = v * 10 + v;
	}
	return v;
}


typedef int32_t fe[10];
static const int64_t kBottom21Bits = 0x1fffffLL;
static const int64_t kBottom25Bits = 0x1ffffffLL;
static const int64_t kBottom26Bits = 0x3ffffffLL;
static const int64_t kTop39Bits = 0xfffffffffe000000LL;
static const int64_t kTop38Bits = 0xfffffffffc000000LL;

static uint64_t load_3(const uint8_t* in)
{
	uint64_t result;

	result = ((uint64_t)in[0]);
	result |= ((uint64_t)in[1]) << 8;
	result |= ((uint64_t)in[2]) << 16;
	return result;
}

static uint64_t load_4(const uint8_t* in)
{
	uint64_t result;

	result = ((uint64_t)in[0]);
	result |= ((uint64_t)in[1]) << 8;
	result |= ((uint64_t)in[2]) << 16;
	result |= ((uint64_t)in[3]) << 24;
	return result;
}
static void fe_frombytes(fe h, const uint8_t* s)
{
	/* Ignores top bit of h. */
	int64_t h0 = load_4(s);
	int64_t h1 = load_3(s + 4) << 6;
	int64_t h2 = load_3(s + 7) << 5;
	int64_t h3 = load_3(s + 10) << 3;
	int64_t h4 = load_3(s + 13) << 2;
	int64_t h5 = load_4(s + 16);
	int64_t h6 = load_3(s + 20) << 7;
	int64_t h7 = load_3(s + 23) << 5;
	int64_t h8 = load_3(s + 26) << 4;
	int64_t h9 = (load_3(s + 29) & 0x7fffff) << 2;
	int64_t carry0;
	int64_t carry1;
	int64_t carry2;
	int64_t carry3;
	int64_t carry4;
	int64_t carry5;
	int64_t carry6;
	int64_t carry7;
	int64_t carry8;
	int64_t carry9;

	carry9 = h9 + (1 << 24); h0 += (carry9 >> 25) * 19; h9 -= carry9 & kTop39Bits;
	carry1 = h1 + (1 << 24); h2 += carry1 >> 25; h1 -= carry1 & kTop39Bits;
	carry3 = h3 + (1 << 24); h4 += carry3 >> 25; h3 -= carry3 & kTop39Bits;
	carry5 = h5 + (1 << 24); h6 += carry5 >> 25; h5 -= carry5 & kTop39Bits;
	carry7 = h7 + (1 << 24); h8 += carry7 >> 25; h7 -= carry7 & kTop39Bits;

	carry0 = h0 + (1 << 25); h1 += carry0 >> 26; h0 -= carry0 & kTop38Bits;
	carry2 = h2 + (1 << 25); h3 += carry2 >> 26; h2 -= carry2 & kTop38Bits;
	carry4 = h4 + (1 << 25); h5 += carry4 >> 26; h4 -= carry4 & kTop38Bits;
	carry6 = h6 + (1 << 25); h7 += carry6 >> 26; h6 -= carry6 & kTop38Bits;
	carry8 = h8 + (1 << 25); h9 += carry8 >> 26; h8 -= carry8 & kTop38Bits;

	h[0] = (int32_t)h0;
	h[1] = (int32_t)h1;
	h[2] = (int32_t)h2;
	h[3] = (int32_t)h3;
	h[4] = (int32_t)h4;
	h[5] = (int32_t)h5;
	h[6] = (int32_t)h6;
	h[7] = (int32_t)h7;
	h[8] = (int32_t)h8;
	h[9] = (int32_t)h9;
}
static void fe_tobytes(uint8_t* s, const fe h)
{
	int32_t h0 = h[0];
	int32_t h1 = h[1];
	int32_t h2 = h[2];
	int32_t h3 = h[3];
	int32_t h4 = h[4];
	int32_t h5 = h[5];
	int32_t h6 = h[6];
	int32_t h7 = h[7];
	int32_t h8 = h[8];
	int32_t h9 = h[9];
	int32_t q;

	q = (19 * h9 + (((int32_t)1) << 24)) >> 25;
	q = (h0 + q) >> 26;
	q = (h1 + q) >> 25;
	q = (h2 + q) >> 26;
	q = (h3 + q) >> 25;
	q = (h4 + q) >> 26;
	q = (h5 + q) >> 25;
	q = (h6 + q) >> 26;
	q = (h7 + q) >> 25;
	q = (h8 + q) >> 26;
	q = (h9 + q) >> 25;

	/* Goal: Output h-(2^255-19)q, which is between 0 and 2^255-20. */
	h0 += 19 * q;
	/* Goal: Output h-2^255 q, which is between 0 and 2^255-20. */

	h1 += h0 >> 26; h0 &= kBottom26Bits;
	h2 += h1 >> 25; h1 &= kBottom25Bits;
	h3 += h2 >> 26; h2 &= kBottom26Bits;
	h4 += h3 >> 25; h3 &= kBottom25Bits;
	h5 += h4 >> 26; h4 &= kBottom26Bits;
	h6 += h5 >> 25; h5 &= kBottom25Bits;
	h7 += h6 >> 26; h6 &= kBottom26Bits;
	h8 += h7 >> 25; h7 &= kBottom25Bits;
	h9 += h8 >> 26; h8 &= kBottom26Bits;
	h9 &= kBottom25Bits;
	/* h10 = carry9 */

	/*
	 * Goal: Output h0+...+2^255 h10-2^255 q, which is between 0 and 2^255-20.
	 * Have h0+...+2^230 h9 between 0 and 2^255-1;
	 * evidently 2^255 h10-2^255 q = 0.
	 * Goal: Output h0+...+2^230 h9.
	 */
	s[0] = (uint8_t)(h0 >> 0);
	s[1] = (uint8_t)(h0 >> 8);
	s[2] = (uint8_t)(h0 >> 16);
	s[3] = (uint8_t)((h0 >> 24) | ((uint32_t)(h1) << 2));
	s[4] = (uint8_t)(h1 >> 6);
	s[5] = (uint8_t)(h1 >> 14);
	s[6] = (uint8_t)((h1 >> 22) | ((uint32_t)(h2) << 3));
	s[7] = (uint8_t)(h2 >> 5);
	s[8] = (uint8_t)(h2 >> 13);
	s[9] = (uint8_t)((h2 >> 21) | ((uint32_t)(h3) << 5));
	s[10] = (uint8_t)(h3 >> 3);
	s[11] = (uint8_t)(h3 >> 11);
	s[12] = (uint8_t)((h3 >> 19) | ((uint32_t)(h4) << 6));
	s[13] = (uint8_t)(h4 >> 2);
	s[14] = (uint8_t)(h4 >> 10);
	s[15] = (uint8_t)(h4 >> 18);
	s[16] = (uint8_t)(h5 >> 0);
	s[17] = (uint8_t)(h5 >> 8);
	s[18] = (uint8_t)(h5 >> 16);
	s[19] = (uint8_t)((h5 >> 24) | ((uint32_t)(h6) << 1));
	s[20] = (uint8_t)(h6 >> 7);
	s[21] = (uint8_t)(h6 >> 15);
	s[22] = (uint8_t)((h6 >> 23) | ((uint32_t)(h7) << 3));
	s[23] = (uint8_t)(h7 >> 5);
	s[24] = (uint8_t)(h7 >> 13);
	s[25] = (uint8_t)((h7 >> 21) | ((uint32_t)(h8) << 4));
	s[26] = (uint8_t)(h8 >> 4);
	s[27] = (uint8_t)(h8 >> 12);
	s[28] = (uint8_t)((h8 >> 20) | ((uint32_t)(h9) << 6));
	s[29] = (uint8_t)(h9 >> 2);
	s[30] = (uint8_t)(h9 >> 10);
	s[31] = (uint8_t)(h9 >> 18);
}

/* h = f */
static void fe_copy(fe h, const fe f)
{
	memmove(h, f, sizeof(int32_t) * 10);
}

/* h = 0 */
static void fe_0(fe h)
{
	memset(h, 0, sizeof(int32_t) * 10);
}

/* h = 1 */
static void fe_1(fe h)
{
	memset(h, 0, sizeof(int32_t) * 10);
	h[0] = 1;
}

static void fe_cswap(fe f, fe g, unsigned int b)
{
	size_t i;

	b = 0 - b;
	for (i = 0; i < 10; i++) {
		int32_t x = f[i] ^ g[i];
		x &= b;
		f[i] ^= x;
		g[i] ^= x;
	}
}
static void fe_sub(fe h, const fe f, const fe g)
{
	unsigned i;

	for (i = 0; i < 10; i++) {
		h[i] = f[i] - g[i];
	}
}
static void fe_add(fe h, const fe f, const fe g)
{
	unsigned i;

	for (i = 0; i < 10; i++) {
		h[i] = f[i] + g[i];
	}
}
static void fe_mul(fe h, const fe f, const fe g)
{
	int32_t f0 = f[0];
	int32_t f1 = f[1];
	int32_t f2 = f[2];
	int32_t f3 = f[3];
	int32_t f4 = f[4];
	int32_t f5 = f[5];
	int32_t f6 = f[6];
	int32_t f7 = f[7];
	int32_t f8 = f[8];
	int32_t f9 = f[9];
	int32_t g0 = g[0];
	int32_t g1 = g[1];
	int32_t g2 = g[2];
	int32_t g3 = g[3];
	int32_t g4 = g[4];
	int32_t g5 = g[5];
	int32_t g6 = g[6];
	int32_t g7 = g[7];
	int32_t g8 = g[8];
	int32_t g9 = g[9];
	int32_t g1_19 = 19 * g1; /* 1.959375*2^29 */
	int32_t g2_19 = 19 * g2; /* 1.959375*2^30; still ok */
	int32_t g3_19 = 19 * g3;
	int32_t g4_19 = 19 * g4;
	int32_t g5_19 = 19 * g5;
	int32_t g6_19 = 19 * g6;
	int32_t g7_19 = 19 * g7;
	int32_t g8_19 = 19 * g8;
	int32_t g9_19 = 19 * g9;
	int32_t f1_2 = 2 * f1;
	int32_t f3_2 = 2 * f3;
	int32_t f5_2 = 2 * f5;
	int32_t f7_2 = 2 * f7;
	int32_t f9_2 = 2 * f9;
	int64_t f0g0 = f0 * (int64_t)g0;
	int64_t f0g1 = f0 * (int64_t)g1;
	int64_t f0g2 = f0 * (int64_t)g2;
	int64_t f0g3 = f0 * (int64_t)g3;
	int64_t f0g4 = f0 * (int64_t)g4;
	int64_t f0g5 = f0 * (int64_t)g5;
	int64_t f0g6 = f0 * (int64_t)g6;
	int64_t f0g7 = f0 * (int64_t)g7;
	int64_t f0g8 = f0 * (int64_t)g8;
	int64_t f0g9 = f0 * (int64_t)g9;
	int64_t f1g0 = f1 * (int64_t)g0;
	int64_t f1g1_2 = f1_2 * (int64_t)g1;
	int64_t f1g2 = f1 * (int64_t)g2;
	int64_t f1g3_2 = f1_2 * (int64_t)g3;
	int64_t f1g4 = f1 * (int64_t)g4;
	int64_t f1g5_2 = f1_2 * (int64_t)g5;
	int64_t f1g6 = f1 * (int64_t)g6;
	int64_t f1g7_2 = f1_2 * (int64_t)g7;
	int64_t f1g8 = f1 * (int64_t)g8;
	int64_t f1g9_38 = f1_2 * (int64_t)g9_19;
	int64_t f2g0 = f2 * (int64_t)g0;
	int64_t f2g1 = f2 * (int64_t)g1;
	int64_t f2g2 = f2 * (int64_t)g2;
	int64_t f2g3 = f2 * (int64_t)g3;
	int64_t f2g4 = f2 * (int64_t)g4;
	int64_t f2g5 = f2 * (int64_t)g5;
	int64_t f2g6 = f2 * (int64_t)g6;
	int64_t f2g7 = f2 * (int64_t)g7;
	int64_t f2g8_19 = f2 * (int64_t)g8_19;
	int64_t f2g9_19 = f2 * (int64_t)g9_19;
	int64_t f3g0 = f3 * (int64_t)g0;
	int64_t f3g1_2 = f3_2 * (int64_t)g1;
	int64_t f3g2 = f3 * (int64_t)g2;
	int64_t f3g3_2 = f3_2 * (int64_t)g3;
	int64_t f3g4 = f3 * (int64_t)g4;
	int64_t f3g5_2 = f3_2 * (int64_t)g5;
	int64_t f3g6 = f3 * (int64_t)g6;
	int64_t f3g7_38 = f3_2 * (int64_t)g7_19;
	int64_t f3g8_19 = f3 * (int64_t)g8_19;
	int64_t f3g9_38 = f3_2 * (int64_t)g9_19;
	int64_t f4g0 = f4 * (int64_t)g0;
	int64_t f4g1 = f4 * (int64_t)g1;
	int64_t f4g2 = f4 * (int64_t)g2;
	int64_t f4g3 = f4 * (int64_t)g3;
	int64_t f4g4 = f4 * (int64_t)g4;
	int64_t f4g5 = f4 * (int64_t)g5;
	int64_t f4g6_19 = f4 * (int64_t)g6_19;
	int64_t f4g7_19 = f4 * (int64_t)g7_19;
	int64_t f4g8_19 = f4 * (int64_t)g8_19;
	int64_t f4g9_19 = f4 * (int64_t)g9_19;
	int64_t f5g0 = f5 * (int64_t)g0;
	int64_t f5g1_2 = f5_2 * (int64_t)g1;
	int64_t f5g2 = f5 * (int64_t)g2;
	int64_t f5g3_2 = f5_2 * (int64_t)g3;
	int64_t f5g4 = f5 * (int64_t)g4;
	int64_t f5g5_38 = f5_2 * (int64_t)g5_19;
	int64_t f5g6_19 = f5 * (int64_t)g6_19;
	int64_t f5g7_38 = f5_2 * (int64_t)g7_19;
	int64_t f5g8_19 = f5 * (int64_t)g8_19;
	int64_t f5g9_38 = f5_2 * (int64_t)g9_19;
	int64_t f6g0 = f6 * (int64_t)g0;
	int64_t f6g1 = f6 * (int64_t)g1;
	int64_t f6g2 = f6 * (int64_t)g2;
	int64_t f6g3 = f6 * (int64_t)g3;
	int64_t f6g4_19 = f6 * (int64_t)g4_19;
	int64_t f6g5_19 = f6 * (int64_t)g5_19;
	int64_t f6g6_19 = f6 * (int64_t)g6_19;
	int64_t f6g7_19 = f6 * (int64_t)g7_19;
	int64_t f6g8_19 = f6 * (int64_t)g8_19;
	int64_t f6g9_19 = f6 * (int64_t)g9_19;
	int64_t f7g0 = f7 * (int64_t)g0;
	int64_t f7g1_2 = f7_2 * (int64_t)g1;
	int64_t f7g2 = f7 * (int64_t)g2;
	int64_t f7g3_38 = f7_2 * (int64_t)g3_19;
	int64_t f7g4_19 = f7 * (int64_t)g4_19;
	int64_t f7g5_38 = f7_2 * (int64_t)g5_19;
	int64_t f7g6_19 = f7 * (int64_t)g6_19;
	int64_t f7g7_38 = f7_2 * (int64_t)g7_19;
	int64_t f7g8_19 = f7 * (int64_t)g8_19;
	int64_t f7g9_38 = f7_2 * (int64_t)g9_19;
	int64_t f8g0 = f8 * (int64_t)g0;
	int64_t f8g1 = f8 * (int64_t)g1;
	int64_t f8g2_19 = f8 * (int64_t)g2_19;
	int64_t f8g3_19 = f8 * (int64_t)g3_19;
	int64_t f8g4_19 = f8 * (int64_t)g4_19;
	int64_t f8g5_19 = f8 * (int64_t)g5_19;
	int64_t f8g6_19 = f8 * (int64_t)g6_19;
	int64_t f8g7_19 = f8 * (int64_t)g7_19;
	int64_t f8g8_19 = f8 * (int64_t)g8_19;
	int64_t f8g9_19 = f8 * (int64_t)g9_19;
	int64_t f9g0 = f9 * (int64_t)g0;
	int64_t f9g1_38 = f9_2 * (int64_t)g1_19;
	int64_t f9g2_19 = f9 * (int64_t)g2_19;
	int64_t f9g3_38 = f9_2 * (int64_t)g3_19;
	int64_t f9g4_19 = f9 * (int64_t)g4_19;
	int64_t f9g5_38 = f9_2 * (int64_t)g5_19;
	int64_t f9g6_19 = f9 * (int64_t)g6_19;
	int64_t f9g7_38 = f9_2 * (int64_t)g7_19;
	int64_t f9g8_19 = f9 * (int64_t)g8_19;
	int64_t f9g9_38 = f9_2 * (int64_t)g9_19;
	int64_t h0 = f0g0 + f1g9_38 + f2g8_19 + f3g7_38 + f4g6_19 + f5g5_38 + f6g4_19 + f7g3_38 + f8g2_19 + f9g1_38;
	int64_t h1 = f0g1 + f1g0 + f2g9_19 + f3g8_19 + f4g7_19 + f5g6_19 + f6g5_19 + f7g4_19 + f8g3_19 + f9g2_19;
	int64_t h2 = f0g2 + f1g1_2 + f2g0 + f3g9_38 + f4g8_19 + f5g7_38 + f6g6_19 + f7g5_38 + f8g4_19 + f9g3_38;
	int64_t h3 = f0g3 + f1g2 + f2g1 + f3g0 + f4g9_19 + f5g8_19 + f6g7_19 + f7g6_19 + f8g5_19 + f9g4_19;
	int64_t h4 = f0g4 + f1g3_2 + f2g2 + f3g1_2 + f4g0 + f5g9_38 + f6g8_19 + f7g7_38 + f8g6_19 + f9g5_38;
	int64_t h5 = f0g5 + f1g4 + f2g3 + f3g2 + f4g1 + f5g0 + f6g9_19 + f7g8_19 + f8g7_19 + f9g6_19;
	int64_t h6 = f0g6 + f1g5_2 + f2g4 + f3g3_2 + f4g2 + f5g1_2 + f6g0 + f7g9_38 + f8g8_19 + f9g7_38;
	int64_t h7 = f0g7 + f1g6 + f2g5 + f3g4 + f4g3 + f5g2 + f6g1 + f7g0 + f8g9_19 + f9g8_19;
	int64_t h8 = f0g8 + f1g7_2 + f2g6 + f3g5_2 + f4g4 + f5g3_2 + f6g2 + f7g1_2 + f8g0 + f9g9_38;
	int64_t h9 = f0g9 + f1g8 + f2g7 + f3g6 + f4g5 + f5g4 + f6g3 + f7g2 + f8g1 + f9g0;
	int64_t carry0;
	int64_t carry1;
	int64_t carry2;
	int64_t carry3;
	int64_t carry4;
	int64_t carry5;
	int64_t carry6;
	int64_t carry7;
	int64_t carry8;
	int64_t carry9;

	/* |h0| <= (1.65*1.65*2^52*(1+19+19+19+19)+1.65*1.65*2^50*(38+38+38+38+38))
	 *   i.e. |h0| <= 1.4*2^60; narrower ranges for h2, h4, h6, h8
	 * |h1| <= (1.65*1.65*2^51*(1+1+19+19+19+19+19+19+19+19))
	 *   i.e. |h1| <= 1.7*2^59; narrower ranges for h3, h5, h7, h9 */

	carry0 = h0 + (1 << 25); h1 += carry0 >> 26; h0 -= carry0 & kTop38Bits;
	carry4 = h4 + (1 << 25); h5 += carry4 >> 26; h4 -= carry4 & kTop38Bits;
	/* |h0| <= 2^25 */
	/* |h4| <= 2^25 */
	/* |h1| <= 1.71*2^59 */
	/* |h5| <= 1.71*2^59 */

	carry1 = h1 + (1 << 24); h2 += carry1 >> 25; h1 -= carry1 & kTop39Bits;
	carry5 = h5 + (1 << 24); h6 += carry5 >> 25; h5 -= carry5 & kTop39Bits;
	/* |h1| <= 2^24; from now on fits into int32 */
	/* |h5| <= 2^24; from now on fits into int32 */
	/* |h2| <= 1.41*2^60 */
	/* |h6| <= 1.41*2^60 */

	carry2 = h2 + (1 << 25); h3 += carry2 >> 26; h2 -= carry2 & kTop38Bits;
	carry6 = h6 + (1 << 25); h7 += carry6 >> 26; h6 -= carry6 & kTop38Bits;
	/* |h2| <= 2^25; from now on fits into int32 unchanged */
	/* |h6| <= 2^25; from now on fits into int32 unchanged */
	/* |h3| <= 1.71*2^59 */
	/* |h7| <= 1.71*2^59 */

	carry3 = h3 + (1 << 24); h4 += carry3 >> 25; h3 -= carry3 & kTop39Bits;
	carry7 = h7 + (1 << 24); h8 += carry7 >> 25; h7 -= carry7 & kTop39Bits;
	/* |h3| <= 2^24; from now on fits into int32 unchanged */
	/* |h7| <= 2^24; from now on fits into int32 unchanged */
	/* |h4| <= 1.72*2^34 */
	/* |h8| <= 1.41*2^60 */

	carry4 = h4 + (1 << 25); h5 += carry4 >> 26; h4 -= carry4 & kTop38Bits;
	carry8 = h8 + (1 << 25); h9 += carry8 >> 26; h8 -= carry8 & kTop38Bits;
	/* |h4| <= 2^25; from now on fits into int32 unchanged */
	/* |h8| <= 2^25; from now on fits into int32 unchanged */
	/* |h5| <= 1.01*2^24 */
	/* |h9| <= 1.71*2^59 */

	carry9 = h9 + (1 << 24); h0 += (carry9 >> 25) * 19; h9 -= carry9 & kTop39Bits;
	/* |h9| <= 2^24; from now on fits into int32 unchanged */
	/* |h0| <= 1.1*2^39 */

	carry0 = h0 + (1 << 25); h1 += carry0 >> 26; h0 -= carry0 & kTop38Bits;
	/* |h0| <= 2^25; from now on fits into int32 unchanged */
	/* |h1| <= 1.01*2^24 */

	h[0] = (int32_t)h0;
	h[1] = (int32_t)h1;
	h[2] = (int32_t)h2;
	h[3] = (int32_t)h3;
	h[4] = (int32_t)h4;
	h[5] = (int32_t)h5;
	h[6] = (int32_t)h6;
	h[7] = (int32_t)h7;
	h[8] = (int32_t)h8;
	h[9] = (int32_t)h9;
}
static void fe_sq(fe h, const fe f)
{
	int32_t f0 = f[0];
	int32_t f1 = f[1];
	int32_t f2 = f[2];
	int32_t f3 = f[3];
	int32_t f4 = f[4];
	int32_t f5 = f[5];
	int32_t f6 = f[6];
	int32_t f7 = f[7];
	int32_t f8 = f[8];
	int32_t f9 = f[9];
	int32_t f0_2 = 2 * f0;
	int32_t f1_2 = 2 * f1;
	int32_t f2_2 = 2 * f2;
	int32_t f3_2 = 2 * f3;
	int32_t f4_2 = 2 * f4;
	int32_t f5_2 = 2 * f5;
	int32_t f6_2 = 2 * f6;
	int32_t f7_2 = 2 * f7;
	int32_t f5_38 = 38 * f5; /* 1.959375*2^30 */
	int32_t f6_19 = 19 * f6; /* 1.959375*2^30 */
	int32_t f7_38 = 38 * f7; /* 1.959375*2^30 */
	int32_t f8_19 = 19 * f8; /* 1.959375*2^30 */
	int32_t f9_38 = 38 * f9; /* 1.959375*2^30 */
	int64_t f0f0 = f0 * (int64_t)f0;
	int64_t f0f1_2 = f0_2 * (int64_t)f1;
	int64_t f0f2_2 = f0_2 * (int64_t)f2;
	int64_t f0f3_2 = f0_2 * (int64_t)f3;
	int64_t f0f4_2 = f0_2 * (int64_t)f4;
	int64_t f0f5_2 = f0_2 * (int64_t)f5;
	int64_t f0f6_2 = f0_2 * (int64_t)f6;
	int64_t f0f7_2 = f0_2 * (int64_t)f7;
	int64_t f0f8_2 = f0_2 * (int64_t)f8;
	int64_t f0f9_2 = f0_2 * (int64_t)f9;
	int64_t f1f1_2 = f1_2 * (int64_t)f1;
	int64_t f1f2_2 = f1_2 * (int64_t)f2;
	int64_t f1f3_4 = f1_2 * (int64_t)f3_2;
	int64_t f1f4_2 = f1_2 * (int64_t)f4;
	int64_t f1f5_4 = f1_2 * (int64_t)f5_2;
	int64_t f1f6_2 = f1_2 * (int64_t)f6;
	int64_t f1f7_4 = f1_2 * (int64_t)f7_2;
	int64_t f1f8_2 = f1_2 * (int64_t)f8;
	int64_t f1f9_76 = f1_2 * (int64_t)f9_38;
	int64_t f2f2 = f2 * (int64_t)f2;
	int64_t f2f3_2 = f2_2 * (int64_t)f3;
	int64_t f2f4_2 = f2_2 * (int64_t)f4;
	int64_t f2f5_2 = f2_2 * (int64_t)f5;
	int64_t f2f6_2 = f2_2 * (int64_t)f6;
	int64_t f2f7_2 = f2_2 * (int64_t)f7;
	int64_t f2f8_38 = f2_2 * (int64_t)f8_19;
	int64_t f2f9_38 = f2 * (int64_t)f9_38;
	int64_t f3f3_2 = f3_2 * (int64_t)f3;
	int64_t f3f4_2 = f3_2 * (int64_t)f4;
	int64_t f3f5_4 = f3_2 * (int64_t)f5_2;
	int64_t f3f6_2 = f3_2 * (int64_t)f6;
	int64_t f3f7_76 = f3_2 * (int64_t)f7_38;
	int64_t f3f8_38 = f3_2 * (int64_t)f8_19;
	int64_t f3f9_76 = f3_2 * (int64_t)f9_38;
	int64_t f4f4 = f4 * (int64_t)f4;
	int64_t f4f5_2 = f4_2 * (int64_t)f5;
	int64_t f4f6_38 = f4_2 * (int64_t)f6_19;
	int64_t f4f7_38 = f4 * (int64_t)f7_38;
	int64_t f4f8_38 = f4_2 * (int64_t)f8_19;
	int64_t f4f9_38 = f4 * (int64_t)f9_38;
	int64_t f5f5_38 = f5 * (int64_t)f5_38;
	int64_t f5f6_38 = f5_2 * (int64_t)f6_19;
	int64_t f5f7_76 = f5_2 * (int64_t)f7_38;
	int64_t f5f8_38 = f5_2 * (int64_t)f8_19;
	int64_t f5f9_76 = f5_2 * (int64_t)f9_38;
	int64_t f6f6_19 = f6 * (int64_t)f6_19;
	int64_t f6f7_38 = f6 * (int64_t)f7_38;
	int64_t f6f8_38 = f6_2 * (int64_t)f8_19;
	int64_t f6f9_38 = f6 * (int64_t)f9_38;
	int64_t f7f7_38 = f7 * (int64_t)f7_38;
	int64_t f7f8_38 = f7_2 * (int64_t)f8_19;
	int64_t f7f9_76 = f7_2 * (int64_t)f9_38;
	int64_t f8f8_19 = f8 * (int64_t)f8_19;
	int64_t f8f9_38 = f8 * (int64_t)f9_38;
	int64_t f9f9_38 = f9 * (int64_t)f9_38;
	int64_t h0 = f0f0 + f1f9_76 + f2f8_38 + f3f7_76 + f4f6_38 + f5f5_38;
	int64_t h1 = f0f1_2 + f2f9_38 + f3f8_38 + f4f7_38 + f5f6_38;
	int64_t h2 = f0f2_2 + f1f1_2 + f3f9_76 + f4f8_38 + f5f7_76 + f6f6_19;
	int64_t h3 = f0f3_2 + f1f2_2 + f4f9_38 + f5f8_38 + f6f7_38;
	int64_t h4 = f0f4_2 + f1f3_4 + f2f2 + f5f9_76 + f6f8_38 + f7f7_38;
	int64_t h5 = f0f5_2 + f1f4_2 + f2f3_2 + f6f9_38 + f7f8_38;
	int64_t h6 = f0f6_2 + f1f5_4 + f2f4_2 + f3f3_2 + f7f9_76 + f8f8_19;
	int64_t h7 = f0f7_2 + f1f6_2 + f2f5_2 + f3f4_2 + f8f9_38;
	int64_t h8 = f0f8_2 + f1f7_4 + f2f6_2 + f3f5_4 + f4f4 + f9f9_38;
	int64_t h9 = f0f9_2 + f1f8_2 + f2f7_2 + f3f6_2 + f4f5_2;
	int64_t carry0;
	int64_t carry1;
	int64_t carry2;
	int64_t carry3;
	int64_t carry4;
	int64_t carry5;
	int64_t carry6;
	int64_t carry7;
	int64_t carry8;
	int64_t carry9;

	carry0 = h0 + (1 << 25); h1 += carry0 >> 26; h0 -= carry0 & kTop38Bits;
	carry4 = h4 + (1 << 25); h5 += carry4 >> 26; h4 -= carry4 & kTop38Bits;

	carry1 = h1 + (1 << 24); h2 += carry1 >> 25; h1 -= carry1 & kTop39Bits;
	carry5 = h5 + (1 << 24); h6 += carry5 >> 25; h5 -= carry5 & kTop39Bits;

	carry2 = h2 + (1 << 25); h3 += carry2 >> 26; h2 -= carry2 & kTop38Bits;
	carry6 = h6 + (1 << 25); h7 += carry6 >> 26; h6 -= carry6 & kTop38Bits;

	carry3 = h3 + (1 << 24); h4 += carry3 >> 25; h3 -= carry3 & kTop39Bits;
	carry7 = h7 + (1 << 24); h8 += carry7 >> 25; h7 -= carry7 & kTop39Bits;

	carry4 = h4 + (1 << 25); h5 += carry4 >> 26; h4 -= carry4 & kTop38Bits;
	carry8 = h8 + (1 << 25); h9 += carry8 >> 26; h8 -= carry8 & kTop38Bits;

	carry9 = h9 + (1 << 24); h0 += (carry9 >> 25) * 19; h9 -= carry9 & kTop39Bits;

	carry0 = h0 + (1 << 25); h1 += carry0 >> 26; h0 -= carry0 & kTop38Bits;

	h[0] = (int32_t)h0;
	h[1] = (int32_t)h1;
	h[2] = (int32_t)h2;
	h[3] = (int32_t)h3;
	h[4] = (int32_t)h4;
	h[5] = (int32_t)h5;
	h[6] = (int32_t)h6;
	h[7] = (int32_t)h7;
	h[8] = (int32_t)h8;
	h[9] = (int32_t)h9;
}
static void fe_invert(fe out, const fe z)
{
	fe t0;
	fe t1;
	fe t2;
	fe t3;
	int i;

	/*
	 * Compute z ** -1 = z ** (2 ** 255 - 19 - 2) with the exponent as
	 * 2 ** 255 - 21 = (2 ** 5) * (2 ** 250 - 1) + 11.
	 */

	 /* t0 = z ** 2 */
	fe_sq(t0, z);

	/* t1 = t0 ** (2 ** 2) = z ** 8 */
	fe_sq(t1, t0);
	fe_sq(t1, t1);

	/* t1 = z * t1 = z ** 9 */
	fe_mul(t1, z, t1);
	/* t0 = t0 * t1 = z ** 11 -- stash t0 away for the end. */
	fe_mul(t0, t0, t1);

	/* t2 = t0 ** 2 = z ** 22 */
	fe_sq(t2, t0);

	/* t1 = t1 * t2 = z ** (2 ** 5 - 1) */
	fe_mul(t1, t1, t2);

	/* t2 = t1 ** (2 ** 5) = z ** ((2 ** 5) * (2 ** 5 - 1)) */
	fe_sq(t2, t1);
	for (i = 1; i < 5; ++i) {
		fe_sq(t2, t2);
	}

	/* t1 = t1 * t2 = z ** ((2 ** 5 + 1) * (2 ** 5 - 1)) = z ** (2 ** 10 - 1) */
	fe_mul(t1, t2, t1);

	/* Continuing similarly... */

	/* t2 = z ** (2 ** 20 - 1) */
	fe_sq(t2, t1);
	for (i = 1; i < 10; ++i) {
		fe_sq(t2, t2);
	}
	fe_mul(t2, t2, t1);

	/* t2 = z ** (2 ** 40 - 1) */
	fe_sq(t3, t2);
	for (i = 1; i < 20; ++i) {
		fe_sq(t3, t3);
	}
	fe_mul(t2, t3, t2);

	/* t2 = z ** (2 ** 10) * (2 ** 40 - 1) */
	for (i = 0; i < 10; ++i) {
		fe_sq(t2, t2);
	}
	/* t1 = z ** (2 ** 50 - 1) */
	fe_mul(t1, t2, t1);

	/* t2 = z ** (2 ** 100 - 1) */
	fe_sq(t2, t1);
	for (i = 1; i < 50; ++i) {
		fe_sq(t2, t2);
	}
	fe_mul(t2, t2, t1);

	/* t2 = z ** (2 ** 200 - 1) */
	fe_sq(t3, t2);
	for (i = 1; i < 100; ++i) {
		fe_sq(t3, t3);
	}
	fe_mul(t2, t3, t2);

	/* t2 = z ** ((2 ** 50) * (2 ** 200 - 1) */
	fe_sq(t2, t2);
	for (i = 1; i < 50; ++i) {
		fe_sq(t2, t2);
	}

	/* t1 = z ** (2 ** 250 - 1) */
	fe_mul(t1, t2, t1);

	/* t1 = z ** ((2 ** 5) * (2 ** 250 - 1)) */
	fe_sq(t1, t1);
	for (i = 1; i < 5; ++i) {
		fe_sq(t1, t1);
	}

	/* Recall t0 = z ** 11; out = z ** (2 ** 255 - 21) */
	fe_mul(out, t1, t0);
}

static void fe_mul121666(fe h, fe f)
{
	int32_t f0 = f[0];
	int32_t f1 = f[1];
	int32_t f2 = f[2];
	int32_t f3 = f[3];
	int32_t f4 = f[4];
	int32_t f5 = f[5];
	int32_t f6 = f[6];
	int32_t f7 = f[7];
	int32_t f8 = f[8];
	int32_t f9 = f[9];
	int64_t h0 = f0 * (int64_t)121666;
	int64_t h1 = f1 * (int64_t)121666;
	int64_t h2 = f2 * (int64_t)121666;
	int64_t h3 = f3 * (int64_t)121666;
	int64_t h4 = f4 * (int64_t)121666;
	int64_t h5 = f5 * (int64_t)121666;
	int64_t h6 = f6 * (int64_t)121666;
	int64_t h7 = f7 * (int64_t)121666;
	int64_t h8 = f8 * (int64_t)121666;
	int64_t h9 = f9 * (int64_t)121666;
	int64_t carry0;
	int64_t carry1;
	int64_t carry2;
	int64_t carry3;
	int64_t carry4;
	int64_t carry5;
	int64_t carry6;
	int64_t carry7;
	int64_t carry8;
	int64_t carry9;

	carry9 = h9 + (1 << 24); h0 += (carry9 >> 25) * 19; h9 -= carry9 & kTop39Bits;
	carry1 = h1 + (1 << 24); h2 += carry1 >> 25; h1 -= carry1 & kTop39Bits;
	carry3 = h3 + (1 << 24); h4 += carry3 >> 25; h3 -= carry3 & kTop39Bits;
	carry5 = h5 + (1 << 24); h6 += carry5 >> 25; h5 -= carry5 & kTop39Bits;
	carry7 = h7 + (1 << 24); h8 += carry7 >> 25; h7 -= carry7 & kTop39Bits;

	carry0 = h0 + (1 << 25); h1 += carry0 >> 26; h0 -= carry0 & kTop38Bits;
	carry2 = h2 + (1 << 25); h3 += carry2 >> 26; h2 -= carry2 & kTop38Bits;
	carry4 = h4 + (1 << 25); h5 += carry4 >> 26; h4 -= carry4 & kTop38Bits;
	carry6 = h6 + (1 << 25); h7 += carry6 >> 26; h6 -= carry6 & kTop38Bits;
	carry8 = h8 + (1 << 25); h9 += carry8 >> 26; h8 -= carry8 & kTop38Bits;

	h[0] = (int32_t)h0;
	h[1] = (int32_t)h1;
	h[2] = (int32_t)h2;
	h[3] = (int32_t)h3;
	h[4] = (int32_t)h4;
	h[5] = (int32_t)h5;
	h[6] = (int32_t)h6;
	h[7] = (int32_t)h7;
	h[8] = (int32_t)h8;
	h[9] = (int32_t)h9;
}
static void x25519(uint8_t out[32],
	const uint8_t scalar[32],
	const uint8_t point[32]) {
	fe x1, x2, z2, x3, z3, tmp0, tmp1;
	uint8_t e[32];
	unsigned swap = 0;
	int pos;

	memcpy(e, scalar, 32);
	e[0] &= 248;
	e[31] &= 127;
	e[31] |= 64;
	fe_frombytes(x1, point);
	fe_1(x2);
	fe_0(z2);
	fe_copy(x3, x1);
	fe_1(z3);

	for (pos = 254; pos >= 0; --pos) {
		unsigned b = 1 & (e[pos / 8] >> (pos & 7));
		swap ^= b;
		fe_cswap(x2, x3, swap);
		fe_cswap(z2, z3, swap);
		swap = b;
		fe_sub(tmp0, x3, z3);
		fe_sub(tmp1, x2, z2);
		fe_add(x2, x2, z2);
		fe_add(z2, x3, z3);
		fe_mul(z3, tmp0, x2);
		fe_mul(z2, z2, tmp1);
		fe_sq(tmp0, tmp1);
		fe_sq(tmp1, x2);
		fe_add(x3, z3, z2);
		fe_sub(z2, z3, z2);
		fe_mul(x2, tmp1, tmp0);
		fe_sub(tmp1, tmp1, tmp0);
		fe_sq(z2, z2);
		fe_mul121666(z3, tmp1);
		fe_sq(x3, x3);
		fe_add(tmp0, tmp0, z3);
		fe_mul(z3, x1, z2);
		fe_mul(z2, tmp1, tmp0);
	}
	//fe_cswap(x2, x3, swap);
	//fe_cswap(z2, z3, swap);

	fe_invert(z2, z2);
	fe_mul(x2, x2, z2);
	fe_tobytes(out, x2);
}

#define OUT_LEN 128
#define KEY_SIZE 32
#define MD_FUNC EVP_sha256()
#define DERIVE_INFO "crystalIKey"

static int deriveSecrets(unsigned char* out, 
	size_t outLen, 
	unsigned char secret[KEY_SIZE], 
	const char* salt, int salt_len, std::string info)
{
	const EVP_MD* md = MD_FUNC;
	EVP_PKEY_CTX* pctx = EVP_PKEY_CTX_new_id(EVP_PKEY_HKDF, NULL);

	size_t  i, ret;
	ret = EVP_PKEY_derive_init(pctx) <= 0
		|| EVP_PKEY_CTX_hkdf_mode(pctx, EVP_PKEY_HKDEF_MODE_EXTRACT_AND_EXPAND) <= 0
		|| EVP_PKEY_CTX_set_hkdf_md(pctx, md) <= 0
		|| EVP_PKEY_CTX_set1_hkdf_key(pctx, secret, KEY_SIZE) <= 0
		|| EVP_PKEY_CTX_set1_hkdf_salt(pctx, salt, salt_len) <= 0
		|| EVP_PKEY_CTX_add1_hkdf_info(pctx, info.c_str(), 11) <= 0
		|| EVP_PKEY_derive(pctx, out, &outLen) <= 0;

	EVP_PKEY_CTX_free(pctx);
	return ret == 0 ? 0 : -1;
}

static std::string getKeyId(std::string &channelKey) {
	unsigned char key_data[KEY_SIZE + 1];
	memset(key_data, 0, KEY_SIZE + 1);
	int retsize = Comm::DecodeBase64((const unsigned char*)channelKey.c_str(), (unsigned char*)key_data, channelKey.size());
	//channelKeyId (channelKey  sha256 sub16)
	std::string channelKeyId = sha256Hex((unsigned char*)key_data, KEY_SIZE).substr(0, 16);
	return channelKeyId;
}

static int encryptData(std::string channelKey, 
	char* &out, int &outLen, 
	char* buff, int buffLen) {

	if (buff == nullptr || buffLen == 0) {
		INFO("buffbuff == nullptr || buffLen == 0");
		return -1;
	}

	unsigned char key_data[KEY_SIZE+1];
	memset(key_data, 0, KEY_SIZE+1);
	int retsize = Comm::DecodeBase64((const unsigned char*)channelKey.c_str(), (unsigned char*)key_data, channelKey.size());

	//keyId (channelKey  sha256 8byte)	
	unsigned char keyId[SHA256_DIGEST_LENGTH + 1];
	memset(keyId, 0, SHA256_DIGEST_LENGTH + 1);
	sha256((unsigned char*)key_data, KEY_SIZE, keyId);

	srand(time(0));
	int32_t randToken = rand();
	if (!strutil::little_endianess()) {
		uint8_t* p = (uint8_t*)&randToken;
		p[0] = randToken & 0xFF;
		p[1] = randToken >> 8;
	}
    //std::string randToken = "1234";

	//data: token + buff
	int dataLen = 4 + buffLen;
	//need padding
	int remain = 16 - dataLen % 16;

	//channelKey + token + buff
	char* section = (char*)malloc(KEY_SIZE + dataLen + remain  +1);
	memset(section, 0, sizeof(section));
	//remain padding
	int i=0;
	for (; i < remain; i++)
	{
		section[KEY_SIZE + dataLen + i] = remain;
		//printf("str[len+i]= %d\n",str[len+i]);
	}
	section[KEY_SIZE + dataLen + i] = '\0';

	//32 + 4+len
	memcpy(section, key_data, KEY_SIZE);
	memcpy(section + KEY_SIZE, &randToken, 4);
	memcpy(section + KEY_SIZE + 4, buff, buffLen);

	//INFO("buff=" + strutil::Hex2Str(section+KEY_SIZE, dataLen));

	//msgKey = MD5(channelKey + token + buff)
	char msgKey[MD5_DIGEST_LENGTH + 1] = { 0 };
	MD5((unsigned char*)section, KEY_SIZE + dataLen , (unsigned char*)msgKey);
	//std::string msgKey = strutil::MD5DigestToBase16(sig);//32byte

	//channelKey(seed 32byte)+msgKey(salt 16 byte)+"sdkChannelKey"  64byte
	unsigned char aes_key[65];
	memset(aes_key, 0, 65);
	int code = deriveSecrets(aes_key, 64, key_data, msgKey, MD5_DIGEST_LENGTH, DERIVE_INFO);
	if (code < 0) {//fail
		INFO("aes_key error");
		free(section);
		return -1;
	}

	//DEBUG("AesKey=" + strutil::Hex2Str((char*)aes_key, 32) + ",iv=" + strutil::Hex2Str((char*)aes_key+32, 16));
	DEBUG("msgKey=" + strutil::Hex2Str( msgKey, 16) + ",channelKeyId=" + strutil::Hex2Str((char*)keyId, 8) );

	//section encrypt
	AES_KEY key;
	if (AES_set_encrypt_key(aes_key, 256, &key)<0) {
		INFO("aes set key error");
		free(section);
		return -1;
	}
	//channelKeyId (8byte) , msgKey(16byte),  encryptedData
	int offset = 8 + MD5_DIGEST_LENGTH;
	//encrypt data must be %16 = 0
	dataLen = dataLen + remain;
	outLen = offset + dataLen;
	out = (char*)malloc(outLen +1);
	memset(out, 0, outLen+1);

	//INFO("offset=" + std::to_string(offset) + ",outLen=" + std::to_string(outLen),+",dataLen="+ std::to_string(dataLen));

	//cp channelKeyId,msgKey
	memcpy(out, keyId, 8);
	memcpy(out+8, msgKey, MD5_DIGEST_LENGTH);

	AES_cbc_encrypt((unsigned char*)section + KEY_SIZE, (unsigned char*)out + offset, dataLen, &key, aes_key+32, AES_ENCRYPT);
	//INFO("encrypt=" + strutil::Hex2Str(out, outLen));

	free(section);
	return 0;
}

static int decryptData(std::string channelKey,
	char*& out, int& outLen, 
	const char* buff, int buffLen) {

	if (buff == nullptr || buffLen == 0) {
		return -1;
	}
	DEBUG("channelKey=", channelKey);

	unsigned char key_data[KEY_SIZE + 1];
	memset(key_data, 0, KEY_SIZE + 1);
	int retsize = Comm::DecodeBase64((const unsigned char*)channelKey.c_str(), (unsigned char*)key_data, channelKey.size());

	//channelKeyId (8byte) , msgKey(16byte),  encryptedData
	unsigned char aes_key[64];
	memset(aes_key, 0, 64);

	//channel-key(seed 32byte)+msg-key(salt 16byte)+"crystalIKey"
	int code = deriveSecrets(aes_key, 64, key_data, buff+8,16, DERIVE_INFO);
	if (code < 0) {//fail
		INFO("aes_key error");
		return -1;
	}

	std::string msgKey = strutil::Hex2Str(buff + 8, 16);
	DEBUG("msgKey=" + msgKey + ",channelKeyId=" + strutil::Hex2Str((char*)buff, 8));
	//INFO("aesKey=" + strutil::Hex2Str((char*)aes_key, 32) + ",cid=" + strutil::Hex2Str((char*)aes_key + 32, 16));

	AES_KEY key;
	if (AES_set_decrypt_key(aes_key, 256, &key) < 0) {//bits:32*8
		INFO("aes set key error");
		return -1;
	}

	int offset = 24;
	outLen = buffLen - offset;
	out = (char*)malloc(outLen+1);
	memset(out, '\0', outLen+1);

	//INFO("buff=" + strutil::Hex2Str((char*)buff + offset, buffLen - offset));

	AES_cbc_encrypt((const unsigned char*)buff+ offset, (unsigned char*)out, outLen, &key, aes_key + 32, AES_DECRYPT);

	//INFO("out=" + strutil::Hex2Str((char*)out, outLen));

	//de padding
	int remain = out[outLen-1]; 
	outLen = outLen - remain;
	out[outLen]= '\0';//no need

	//check msgKey 与 SHA-256( channelKey + DectyptedData(token + buff) ) 
	char* section = (char*)malloc(KEY_SIZE + outLen + 1);
	memset(section, 0, sizeof(section));

	memcpy(section, key_data, KEY_SIZE);
	memcpy(section + KEY_SIZE, out, outLen);
	char msgKeyMD5[MD5_DIGEST_LENGTH + 1] = { 0 };
	MD5((unsigned char*)section, KEY_SIZE + outLen, (unsigned char*)msgKeyMD5);

	free(section);
	if (strutil::Hex2Str(msgKeyMD5, 16).compare(msgKey) != 0) {
		return -1;
	}

	//INFO("remain=" + std::to_string(remain));
	//INFO("out=" + strutil::Hex2Str((char*)out, outLen));
	return 0;
}


