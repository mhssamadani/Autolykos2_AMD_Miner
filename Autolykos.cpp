#include "Autolykos.h"
#include <boost/multiprecision/cpp_int.hpp>


AutolykosAlg::AutolykosAlg()
{
	uint8_t sk[NUM_SIZE_8 * 2];
	char skstr[NUM_SIZE_4 + 10];

	memset(sk, 0, NUM_SIZE_8 * 2);
	memset(skstr, 0, NUM_SIZE_4);

	//Blake2b256("1", 1, sk, skstr);

	int tr = sizeof(unsigned long long);
	for (size_t i = 0; i < CONST_MES_SIZE_8 / tr; i++)
	{
		unsigned long long tmp = i;
		uint8_t tmp2[8];
		uint8_t tmp1[8];
		memcpy(tmp1, &tmp, tr);
		tmp2[0] = tmp1[7];
		tmp2[1] = tmp1[6];
		tmp2[2] = tmp1[5];
		tmp2[3] = tmp1[4];
		tmp2[4] = tmp1[3];
		tmp2[5] = tmp1[2];
		tmp2[6] = tmp1[1];
		tmp2[7] = tmp1[0];

		memcpy(&CONST_MESS[i], tmp2, tr);
	}
}


AutolykosAlg::~AutolykosAlg()
{

}
cl_uint ind2[32];
cl_uint ind1[32];


void AutolykosAlg::hashIn(const char * in1, const int len, uint8_t * sk)
{
	ctx_t ctx;
	uint64_t aux[32];

	char *skstr = new char[len * 3];
	char *inHash = new char[len];

	memcpy(inHash, in1, len);
	int hashLen = len;
	int j = 0;
	do
	{

		Blake2b256(inHash, hashLen, sk, skstr);
		uint8_t beH[PK_SIZE_8];
		HexStrToBigEndian(skstr, NUM_SIZE_4, beH, NUM_SIZE_8);
		memcpy(sk, beH, NUM_SIZE_8);

		j = ((cl_ulong *)sk)[3] < Q3
			|| ((cl_ulong *)sk)[3] == Q3 && (
				((cl_ulong *)sk)[2] < Q2
				|| ((cl_ulong *)sk)[2] == Q2 && (
					((cl_ulong *)sk)[1] < Q1
					|| ((cl_ulong *)sk)[1] == Q1
					&& ((cl_ulong *)sk)[0] < Q0
					)
				);

		hashLen = NUM_SIZE_8;
		memcpy(inHash, sk, NUM_SIZE_8);

	} while (!j);


	//====================================================================//
	//  Mod Q
	//====================================================================//
	uint8_t borrow[2];

	borrow[0] = ((uint64_t*)sk)[0] < Q0;
	aux[0] = ((uint64_t*)sk)[0] - Q0;

	borrow[1] = ((uint64_t*)sk)[1] < Q1 + borrow[0];
	aux[1] = ((uint64_t*)sk)[1] - Q1 - borrow[0];

	borrow[0] = ((uint64_t*)sk)[2] < Q2 + borrow[1];
	aux[2] = ((uint64_t*)sk)[2] - Q2 - borrow[1];

	borrow[1] = ((uint64_t*)sk)[3] < Q3 + borrow[0];
	aux[3] = ((uint64_t*)sk)[3] - Q3 - borrow[0];

	if (!(borrow[1] || borrow[0])) { memcpy(sk, aux, NUM_SIZE_8); }


}

void AutolykosAlg::Blake2b256_half1(const char * in, const int len, ctx_t &ctx)
{
	uint64_t aux[32];


	//====================================================================//
	//  Hash message
	//====================================================================//
	for (int i = 0; i < len; ++i)
	{
		if (ctx.c == 128) { HOST_B2B_H(&ctx, aux); }

		ctx.b[ctx.c++] = (uint8_t)(in[i]);
	}

}
void AutolykosAlg::Blake2b256_half2(ctx_t ctx, uint8_t * sk, char * skstr)
{
	uint64_t aux[32];
	HOST_B2B_H_LAST(&ctx, aux);

	for (int i = 0; i < NUM_SIZE_8; ++i)
	{
		sk[NUM_SIZE_8 - i - 1] = (ctx.h[i >> 3] >> ((i & 7) << 3)) & 0xFF;
	}
	LittleEndianToHexStr(sk, NUM_SIZE_8, skstr);


}

void AutolykosAlg::Blake2b256(const char * in,
	const int len,
	uint8_t * sk,
	char * skstr)
{
	ctx_t ctx;
	uint64_t aux[32];





	//====================================================================//
	//  Initialize context
	//====================================================================//
	memset(ctx.b, 0, 128);
	B2B_IV(ctx.h);
	ctx.h[0] ^= 0x01010000 ^ NUM_SIZE_8;
	memset(ctx.t, 0, 16);
	ctx.c = 0;

	//====================================================================//
	//  Hash message
	//====================================================================//
	for (int i = 0; i < len; ++i)
	{
		if (ctx.c == 128) { HOST_B2B_H(&ctx, aux); }

		ctx.b[ctx.c++] = (uint8_t)(in[i]);
	}

	HOST_B2B_H_LAST(&ctx, aux);

	for (int i = 0; i < NUM_SIZE_8; ++i)
	{
		sk[NUM_SIZE_8 - i - 1] = (ctx.h[i >> 3] >> ((i & 7) << 3)) & 0xFF;
	}

	//uint8_t s_tmp[32];
	//for (int i = 0; i < 32 ; i+=4)
	//{	
	//	s_tmp[i] = sk[32 - i - 4];
	//	s_tmp[i + 1] = sk[32 - i - 3];
	//	s_tmp[i + 2] = sk[32 - i - 2];
	//	s_tmp[i + 3] = sk[32 - i - 1];
	//}
	////====================================================================//
	////  Mod Q
	////====================================================================//
	//uint8_t borrow[2];

	//borrow[0] = ((uint64_t*)sk)[0] < Q0;
	//aux[0] = ((uint64_t*)sk)[0] - Q0;

	//borrow[1] = ((uint64_t*)sk)[1] < Q1 + borrow[0];
	//aux[1] = ((uint64_t*)sk)[1] - Q1 - borrow[0];

	//borrow[0] = ((uint64_t*)sk)[2] < Q2 + borrow[1];
	//aux[2] = ((uint64_t*)sk)[2] - Q2 - borrow[1];

	//borrow[1] = ((uint64_t*)sk)[3] < Q3 + borrow[0];
	//aux[3] = ((uint64_t*)sk)[3] - Q3 - borrow[0];

	//if (!(borrow[1] || borrow[0])) { memcpy(sk, aux, NUM_SIZE_8); }

	// convert secret key to hex string
	LittleEndianToHexStr(sk, NUM_SIZE_8, skstr);

}



void AutolykosAlg::GenIdex2(const char * in, const int len, cl_uint* index)
{
	int a = INDEX_SIZE_8;
	int b = K_LEN;
	int c = NUM_SIZE_8;
	int d = NUM_SIZE_4;


	uint8_t sk[NUM_SIZE_8 * 2];
	char skstr[NUM_SIZE_4 + 10];

	memset(sk, 0, NUM_SIZE_8 * 2);
	memset(skstr, 0, NUM_SIZE_4);

	Blake2b256(in, len, sk, skstr);

	for (size_t i = 0; i < NUM_SIZE_8 * 2; i++)
	{
		sk[i] = 0;
	}
	sk[31] = 1;
	sk[0] = 1;
	sk[10] = 1;

	//memcpy(sk + NUM_SIZE_8, sk, 3);

	cl_uint* r = (cl_uint*)sk;
	cl_uint* ind = index;
	for (int i = 1; i < INDEX_SIZE_8; ++i)
	{
		((uint8_t*)r)[NUM_SIZE_8 + i] = ((uint8_t*)r)[i];
	}


	cl_uint t1 = r[0];
	cl_uint t2 = t1  & N_MASK;
	cl_uint t3 = t1  % N_LEN;

	int a1 = 10 >> 2;
	int a2 = 10 << 2;
	int a3 = 10 >> 3;
	int a4 = 10 << 3;

	//ind[0] = r[0 >> 2] % N_LEN;// &N_MASK;
	//for (int i = 1; i < INDEX_SIZE_8; ++i)
	//{
	//	ind2[0 + i] = ind[0 + i] = ((r[0 >> 2] << (i << 3)) | (r[(0 >> 2) + 1] >> (32 - (i << 3)))) % N_LEN;// &N_MASK;
	//}


	//#pragma unroll
	for (int k = 0; k < 32; k += 4)
	{




		ind2[k] = ind[k] = r[k >> 2] % N_LEN;// &N_MASK;

											 //#pragma unroll
		for (int i = 1; i < INDEX_SIZE_8; ++i)
		{
			ind2[k + i] = ind[k + i] = ((r[k >> 2] << (i << 3)) | (r[(k >> 2) + 1] >> (32 - (i << 3)))) % N_LEN;// &N_MASK;
		}
	}

}

void AutolykosAlg::GenIdex(const char * in, const int len, cl_uint* index)
{
	int a = INDEX_SIZE_8;
	int b = K_LEN;
	int c = NUM_SIZE_8;
	int d = NUM_SIZE_4;


	uint8_t sk[NUM_SIZE_8 * 2];
	char skstr[NUM_SIZE_4 + 10];

	memset(sk, 0, NUM_SIZE_8 * 2);
	memset(skstr, 0, NUM_SIZE_4);

	Blake2b256(in, len, sk, skstr);

	uint8_t beH[PK_SIZE_8];
	HexStrToBigEndian(skstr, NUM_SIZE_4, beH, NUM_SIZE_8);

	//FILE *fp = fopen("sk.dat", "wb");
	//fwrite(sk, 1, NUM_SIZE_8, fp);
	//fclose(fp);
	//for (size_t i = 0; i < NUM_SIZE_8 * 2; i++)
	//{
	//	sk[i] = 0;
	//}
	//sk[31] = 1;
	//sk[0] = 1;
	//sk[10] = 1;

	cl_uint* ind = index;

	memcpy(sk, beH, NUM_SIZE_8);
	memcpy(sk + NUM_SIZE_8, beH, NUM_SIZE_8);

	//uint8_t t[NUM_SIZE_8 * 2];
	//int l = 0;
	//for (; skstr[l] != '\0'; ++l) {}
	//HexStrToLittleEndian(skstr, l, t, NUM_SIZE_8);

	cl_uint tmpInd[32];
	int sliceIndex = 0;
	for (int k = 0; k < K_LEN; k++)
	{


		uint8_t tmp[4];
		memcpy(tmp, sk + sliceIndex, 4);
		memcpy(&tmpInd[k], sk + sliceIndex, 4);
		uint8_t tmp2[4];
		tmp2[0] = tmp[3];
		tmp2[1] = tmp[2];
		tmp2[2] = tmp[1];
		tmp2[3] = tmp[0];
		memcpy(&ind[k], tmp2, 4);
		ind1[k] = ind[k] = ind[k] % N_LEN;
		sliceIndex++;
	}
	//GenIdex2(in, len, index);

}
void AutolykosAlg::GenElements(const char * hh, int len, const char * ret)
{

	hashIn(hh, len, (uint8_t *)ret);

	//FILE *fp =  fopen("C_h.dat", "wb");
	//fwrite(hh, 1, len, fp);
	//fclose(fp);
	//uint128_t  retI= uint128_t((char *)ret);
	int tr;
	tr = 0;

}

void AutolykosAlg::decodes(
	char * mes,
	char   * pkstr,
	char   * w,
	char   *nonce,
	char   * d)
{
	//Hashtest
	uint8_t sk0[NUM_SIZE_8 * 2];
	char skstr0[NUM_SIZE_4 + 10];
	Blake2b256("10000000000000000000000000000000", NUM_SIZE_8, sk0, skstr0);
	char *te = { "10000000000000000000000000000000" };

	uint8_t sk2[NUM_SIZE_8 * 2];
	char skstr2[NUM_SIZE_4 + 10];
	char test[NUM_SIZE_8];
	Blake2b256((char *)mes, NUM_SIZE_8, sk2, skstr2);


	uint8_t be[8];
	uint8_t le[8];
	HexStrToBigEndian("00000000003381BF", 16, be, 8);
	HexStrToLittleEndian("00000000003381BF", 16, le, 8);
	uint8_t sk3[NUM_SIZE_8 * 2];
	char skstr3[NUM_SIZE_4 + 10];
	Blake2b256((char *)be, NONCE_SIZE_8, sk3, skstr3);
	uint8_t sk4[NUM_SIZE_8 * 2];
	char skstr4[NUM_SIZE_4 + 10];
	Blake2b256((char *)le, NONCE_SIZE_8, sk4, skstr4);

	//Hashtest


}
using namespace boost::multiprecision;

char *m_str = NULL;// [64];
char *pk_str = NULL;// [66];
char *sk_str = NULL;// [64];
char *w_str = NULL;// [66];
char *x_str = NULL;// [64];
char *pool_str = NULL;// [66];

uint8_t *m_n;
uint8_t  *p_w_m_n;
uint8_t  *Hinput;
char *n_str;
char *h_str;

#include "SolutionVerifier.h"

SolutionVerifier solVer;

void AutolykosAlg::TestBlake()
{
	uint8_t m[NUM_SIZE_8];
	uint8_t p[PK_SIZE_8];
	uint8_t w[PK_SIZE_8];
	uint8_t J[4];

	//H(j || p || w || CM || m)
	uint8_t *Binput1 = new uint8_t[sizeof(cl_uint) + CONST_MES_SIZE_8 + PK_SIZE_8 + NUM_SIZE_8 + PK_SIZE_8];
	memset(Binput1, 0, sizeof(cl_uint) + CONST_MES_SIZE_8 + PK_SIZE_8 + NUM_SIZE_8 + PK_SIZE_8);
	
	//H(j || p || w || CM || m)
	uint8_t *Binput2_1 = new uint8_t[sizeof(cl_uint) + CONST_MES_SIZE_8 + PK_SIZE_8 ];
	uint8_t *Binput2_2 = new uint8_t[ NUM_SIZE_8 + PK_SIZE_8];
	memset(Binput2_1, 0, sizeof(cl_uint) + CONST_MES_SIZE_8 + PK_SIZE_8 );
	memset(Binput2_1, 0,  NUM_SIZE_8 + PK_SIZE_8);

	//H(j || p || w || CM || m)
	uint8_t *Binput3_1 = new uint8_t[sizeof(cl_uint) + CONST_MES_SIZE_8 + PK_SIZE_8];
	uint8_t *Binput3_2 = new uint8_t[NUM_SIZE_8 + PK_SIZE_8];
	memset(Binput3_1, 0, sizeof(cl_uint) + CONST_MES_SIZE_8 + PK_SIZE_8);
	memset(Binput3_1, 0, NUM_SIZE_8 + PK_SIZE_8);


	int ind = 10;
	memcpy(J, &ind, 4);
	for (int i = 0; i < NUM_SIZE_8; i++)
	{
		m[i] = rand() %200;
		p[i] = rand() % 200;
		w[i] = rand() % 200;
	}
	p[NUM_SIZE_8] = rand() % 200;
	w[NUM_SIZE_8] = rand() % 200;




	//-----------1-----------------------------------
	int off1 = 0;
	memcpy(Binput1 + off1, J, sizeof(cl_uint));
	off1 += sizeof(cl_uint);

	memcpy(Binput1 + off1, p, PK_SIZE_8);
	off1 += PK_SIZE_8;

	memcpy(Binput1 + off1, w, PK_SIZE_8);
	off1 += PK_SIZE_8;

	memcpy(Binput1 + off1, CONST_MESS, CONST_MES_SIZE_8);
	off1 += CONST_MES_SIZE_8;

	memcpy(Binput1 + off1, m, NUM_SIZE_8);
	off1 += NUM_SIZE_8;

	uint8_t sk1[NUM_SIZE_8 * 2];
	char skstr1[NUM_SIZE_4 + 10];
	memset(sk1, 0, NUM_SIZE_8 * 2);
	memset(skstr1, 0, NUM_SIZE_4);
	Blake2b256((const char *)Binput1, off1, sk1, skstr1);
	//-----------1-----------------------------------


	//-----------2-----------------------------------
	//alg2: j - P , w - CM - m 
	int off2_1 = 0;
	memcpy(Binput2_1 + off2_1, J, sizeof(cl_uint));
	off2_1 += sizeof(cl_uint);

	memcpy(Binput2_1 + off2_1, p, PK_SIZE_8);
	off2_1 += PK_SIZE_8;

	int off2_2 = 0;
	memcpy(Binput2_2 + off2_2, w, PK_SIZE_8);
	off2_2 += PK_SIZE_8;

	memcpy(Binput2_2 + off2_2, CONST_MESS, CONST_MES_SIZE_8);
	off2_2 += CONST_MES_SIZE_8;

	memcpy(Binput2_2 + off2_2, m, NUM_SIZE_8);
	off2_2 += NUM_SIZE_8;

	uint8_t sk2[NUM_SIZE_8 * 2];
	char skstr2[NUM_SIZE_4 + 10];
	memset(sk2, 0, NUM_SIZE_8 * 2);
	memset(skstr2, 0, NUM_SIZE_4);
	ctx_t ctx;
	//====================================================================//
	//  Initialize context
	//====================================================================//
	memset(ctx.b, 0, 128);
	B2B_IV(ctx.h);
	ctx.h[0] ^= 0x01010000 ^ NUM_SIZE_8;
	memset(ctx.t, 0, 16);
	ctx.c = 0;

	Blake2b256_half1((const char *)Binput2_1, off2_1, ctx);
	Blake2b256_half1((const char *)Binput2_2, off2_2, ctx);
	Blake2b256_half2(ctx, sk2, skstr2);
	//-----------2-----------------------------------

	//-----------3-----------------------------------
	//alg1: j - CM - P , m - w
	int off3_1 = 0;
	memcpy(Binput3_1 + off3_1, J, sizeof(cl_uint));
	off3_1 += sizeof(cl_uint);

	memcpy(Binput3_2 + off3_1, CONST_MESS, CONST_MES_SIZE_8);
	off3_1 += CONST_MES_SIZE_8;

	memcpy(Binput3_1 + off3_1, p, PK_SIZE_8);
	off3_1 += PK_SIZE_8;

	int off3_2 = 0;
	memcpy(Binput3_2 + off3_2, m, NUM_SIZE_8);
	off3_2 += NUM_SIZE_8;

	memcpy(Binput3_2 + off3_2, w, PK_SIZE_8);
	off3_2 += PK_SIZE_8;



	uint8_t sk3[NUM_SIZE_8 * 3];
	char skstr3[NUM_SIZE_4 + 10];
	memset(sk3, 0, NUM_SIZE_8 * 3);
	memset(skstr3, 0, NUM_SIZE_4);
	ctx_t ctx3;
	//====================================================================//
	//  Initialize context
	//====================================================================//
	memset(ctx3.b, 0, 138);
	B2B_IV(ctx3.h);
	ctx3.h[0] ^= 0x01010000 ^ NUM_SIZE_8;
	memset(ctx3.t, 0, 16);
	ctx3.c = 0;

	Blake2b256_half1((const char *)Binput3_1, off3_1, ctx3);
	Blake2b256_half1((const char *)Binput3_2, off3_2, ctx3);
	Blake2b256_half2(ctx3, sk3, skstr3);

	int tr;
	tr = 0;
}
void AutolykosAlg::hashIn2(const char * in1, const int len, uint8_t * sk)
{

	char *skstr = new char[len * 3];
	char *inHash = new char[len];

	memcpy(inHash, in1, len);
	int hashLen = len;

	Blake2b256(inHash, hashLen, sk, skstr);
	uint8_t beH[PK_SIZE_8];
	HexStrToBigEndian(skstr, NUM_SIZE_4, beH, NUM_SIZE_8);
	memcpy(sk, beH, NUM_SIZE_8);

}

void AutolykosAlg::RunAlg(
	int AlgVer,
	uint8_t *message,
	uint8_t *bPK,
	uint8_t * bSk,
	uint8_t * bW,
	uint8_t * bX,
	uint8_t *nonce,
	uint8_t *bPool,
	uint8_t *height
	)
{
//	TestBlake();
	m_iAlgVer = AlgVer;

	//char *m_str = "E03CA4C9343CC57E7AF10BCD321E7CD10ABB88122B01187EA476A0C6B725F12D";// [PK_SIZE_4];
	//char *pk_str = "0330C2D9EC20CD7D4AA76B833CFE7A8752DE261FAAECB010CEB73D2E1E6CE1122E";
	//char *sk_str = "D918FA3F66A8A53461ABB70D2D872E4494C980708CCEA55BADD2383AB84470EE";
	//char *w_str = "0290A03639996CB5E4C5395CCE97591ADCC5C3C0AF19E5295D583F1CD366712F98";
	//char *x_str = "D7506DFA27484A70E9628636646F3643BCFDE70BB9CFAF370E138A0B11B9FB4B";

	static bool first = true;
	if (first)
	{
		first = false;
		m_str = new char[64];
		pk_str = new char[66];
		sk_str = new char[64];
		w_str = new char[66];
		x_str = new char[64];
		pool_str = new char[100];
		m_n = new uint8_t[NUM_SIZE_8 + NONCE_SIZE_8];
		p_w_m_n = new uint8_t[PK_SIZE_8 + PK_SIZE_8 + NUM_SIZE_8 + NONCE_SIZE_8];
		Hinput = new uint8_t[sizeof(cl_uint) + CONST_MES_SIZE_8 + PK_SIZE_8 + NUM_SIZE_8 + PK_SIZE_8];
		n_str = new char[NONCE_SIZE_4];
		h_str = new char[HEIGHT_SIZE ];

	}
	BigEndianToHexStr(message, NUM_SIZE_8, m_str);
	BigEndianToHexStr(bPK, PK_SIZE_8, pk_str);
	LittleEndianToHexStr(bSk, NUM_SIZE_8, sk_str);
	BigEndianToHexStr(bW, PK_SIZE_8, w_str);
	LittleEndianToHexStr(bX, NUM_SIZE_8, x_str);

	char chBB[1000];
	uint32_t ilen = 0;
	LittleEndianOf256ToDecStr((uint8_t *)bPool, pool_str, &ilen);
	LittleEndianOf256ToDecStr((uint8_t *)bPool, chBB, &ilen);




	//uint8_t message1[NUM_SIZE_8];
	//int len = 0;
	//for (; m_str[len] != '\0'; ++len) {}
	//HexStrToBigEndian(m_str, len, message1, NUM_SIZE_8);

	//uint8_t bPK[PK_SIZE_8];
	//len = 0;
	//for (; pk_str[len] != '\0'; ++len) {}
	//HexStrToBigEndian(pk_str, len, bPK, PK_SIZE_8);

	//uint8_t bSK[NUM_SIZE_8];
	//len = 0;
	//for (; sk_str[len] != '\0'; ++len) {}
	///*HexStrToBigEndian*/HexStrToLittleEndian(sk_str, len, bSK, NUM_SIZE_8);

	//uint8_t bW[PK_SIZE_8];
	//len = 0;
	//for (; w_str[len] != '\0'; ++len) {}
	//HexStrToBigEndian(w_str, len, bW, PK_SIZE_8);

	//uint8_t bX[NUM_SIZE_8];
	//len = 0;
	//for (; x_str[len] != '\0'; ++len) {}
	//HexStrToBigEndian(x_str, len, bX, PK_SIZE_8);

	static unsigned long long int  startNonce = 0;
										   //unsigned long long int endNonce = (startNonce + 100000);
	cl_uint index[K_LEN];
	//for (unsigned long long int  nonce = startNonce; nonce <endNonce; nonce++)
	//do
	{
		//unsigned long long int  nonce = startNonce;
		//memcpy(nonce, &startNonce, NONCE_SIZE_8);
		LittleEndianToHexStr(nonce, NONCE_SIZE_8, n_str);
		BigEndianToHexStr(height, HEIGHT_SIZE , h_str);
		uint8_t beN[NONCE_SIZE_8];
		HexStrToBigEndian(n_str, NONCE_SIZE_8 * 2, beN, NONCE_SIZE_8);

		uint8_t beH[HEIGHT_SIZE];
		HexStrToBigEndian(h_str, HEIGHT_SIZE * 2, beH, HEIGHT_SIZE);


		startNonce++;
		//TRACE("\d", startNonce);
		
		BIGNUM* bigQ = BN_new();
		CALL(BN_dec2bn(&bigQ, "115792089237316195423570985008687907852837564279074904382605163141518161494337"), ERROR_OPENSSL);
		
		BIGNUM* BigT = BN_new();
		CALL(BN_dec2bn(&BigT, "0"), ERROR_OPENSSL);

			uint8_t h1[NUM_SIZE_8];
		//	memset(message, 0, NUM_SIZE_8);
		//	message[0] = 1;
			memcpy(m_n, message, NUM_SIZE_8);
			memcpy(m_n + NUM_SIZE_8, beN, NONCE_SIZE_8);
			hashIn2((const char *)m_n, NUM_SIZE_8 + NONCE_SIZE_8 , (uint8_t *)h1);

			cl_ulong h2;
			char tmpL1[8];
			tmpL1[0] = h1[31];
			tmpL1[1] = h1[30];
			tmpL1[2] = h1[29];
			tmpL1[3] = h1[28];
			tmpL1[4] = h1[27];
			tmpL1[5] = h1[26];
			tmpL1[6] = h1[25];
			tmpL1[7] = h1[24];
			memcpy(&h2, tmpL1, 8);

			//cl_ulong mN = pow(2,30);
			unsigned int h3 = h2 % N_LEN;

			uint8_t iii[4];
			iii[0] = ((char *)(&h3))[3];
			iii[1] = ((char *)(&h3))[2];
			iii[2] = ((char *)(&h3))[1];
			iii[3] = ((char *)(&h3))[0];

			//BN_CTX  *bn_ctx = BN_CTX_new();
			//int RR = BN_mod(BigT,bigh2, bigQ, bn_ctx);

			//const char *h3 = BN_bn2dec(BigT);
			//
			//uint8_t h3I[NUM_SIZE_8];
			//BN_bn2bin(BigT, h3I);
			//memset(beH, 0, HEIGHT_SIZE);
			//beH[0] = 10;

			uint8_t i_h_M[HEIGHT_SIZE + HEIGHT_SIZE + CONST_MES_SIZE_8 ];
			memcpy(i_h_M, iii, HEIGHT_SIZE);
			memcpy(i_h_M + HEIGHT_SIZE, beH, HEIGHT_SIZE);
			memcpy(i_h_M + HEIGHT_SIZE + HEIGHT_SIZE, CONST_MESS, CONST_MES_SIZE_8);
			hashIn2((const char *)i_h_M, HEIGHT_SIZE + HEIGHT_SIZE + CONST_MES_SIZE_8, (uint8_t *)h1);
			uint8_t ff[NUM_SIZE_8-1];
			memcpy(ff, h1 + 1, NUM_SIZE_8 - 1);

			uint8_t seed[NUM_SIZE_8 - 1 + NUM_SIZE_8 + NONCE_SIZE_8];
			memcpy(seed, ff, NUM_SIZE_8 - 1);
			memcpy(seed + NUM_SIZE_8 - 1, message, NUM_SIZE_8);
			memcpy(seed+ NUM_SIZE_8 - 1 + NUM_SIZE_8 , beN, NONCE_SIZE_8);
			GenIdex((const char*)seed, NUM_SIZE_8 - 1 + NUM_SIZE_8 + NONCE_SIZE_8, index);



		
		uint8_t ret[32][NUM_SIZE_8];
		//uint8_t summ[NUM_SIZE_8];
		//memset(summ, 0, NUM_SIZE_8);
		//uint1024_t  retI[32];
		//uint1024_t sum("0");
		int ll = sizeof(cl_uint) + CONST_MES_SIZE_8 + PK_SIZE_8 + NUM_SIZE_8 + PK_SIZE_8;

		//FILE *fp =  fopen("C_flist.dat", "wb");

		BIGNUM* bigsum = BN_new();
		CALL(BN_dec2bn(&bigsum, "0"), ERROR_OPENSSL);

		BIGNUM* bigres = BN_new();
		CALL(BN_dec2bn(&bigres, "0"), ERROR_OPENSSL);

		int rep = 0;
		int off = 0;
		uint8_t tmp[NUM_SIZE_8 - 1];
		char hesStr[64 + 1];
		uint8_t tmp2[4];
		uint8_t tmp1[4];

		unsigned char f[32];
		memset(f, 0, 32);

		char *LSUMM;
		char *LB;
		for (rep = 0; rep < 32; rep++)
		{
			memset(Hinput, 0, ll);
			//H(j || M || pk || m || w)


			memcpy(tmp1, &index[rep], 4);
			tmp2[0] = tmp1[3];
			tmp2[1] = tmp1[2];
			tmp2[2] = tmp1[1];
			tmp2[3] = tmp1[0];

			off = 0;
			memcpy(Hinput + off, tmp2, sizeof(cl_uint));
			off += sizeof(cl_uint);

			memcpy(Hinput + off, beH, HEIGHT_SIZE);
			off += HEIGHT_SIZE;

			memcpy(Hinput + off, CONST_MESS, CONST_MES_SIZE_8);
			off += CONST_MES_SIZE_8;

			hashIn2((const char *)Hinput, off, (uint8_t *)ret[rep]);

			//memcpy(tmp, (uint8_t *)ret[rep], 31);
			memcpy(tmp, &(ret[rep][1]), 31);

			//BigEndianToHexStr((uint8_t *)tmp, 31, hesStr);

			CALL(BN_bin2bn((const unsigned char *)tmp,31,bigres), ERROR_OPENSSL);

			CALL(BN_add(bigsum, bigsum, bigres), ERROR_OPENSSL);

			LB = BN_bn2dec(bigres);
			//LSUMM = BN_bn2dec(bigsum);

			//LOG(INFO) << LB;
			//LOG(INFO) << LSUMM;
			BN_bn2bin(bigsum, f);


			//int r;
			//r = 0;

		}

		const char *SUMMbigEndian = BN_bn2dec(bigsum);

		BN_bn2bin(bigsum, f);
		char bigendian2littl[32];
		for (size_t i = 0; i < 32; i++)
		{
			bigendian2littl[i] = f[32 - i - 1];
		}

		BIGNUM* littleF = BN_new();
		CALL(BN_bin2bn((const unsigned char *)bigendian2littl, 32, littleF), ERROR_OPENSSL);
		const char *SUMMLittleEndian = BN_bn2dec(littleF);

		char hf[32];
		hashIn2((const char *)f, 32, (uint8_t *)hf);

		BIGNUM* bigHF = BN_new();
		CALL(BN_bin2bn((const unsigned char *)hf, 32, bigHF), ERROR_OPENSSL);



		char littl2big[32];
		for (size_t i = 0; i < 32; i++)
		{
			littl2big[i] = bPool[32 - i - 1];
		}

		BIGNUM* bigB = BN_new();
		CALL(BN_bin2bn((const unsigned char *)littl2big, 32, bigB), ERROR_OPENSSL);

		//CALL(BN_dec2bn(&bigB, pool_str), ERROR_OPENSSL);

		int cmp = BN_cmp(bigHF, bigB);
		int tr;
		tr = 0;


		const char *chD = BN_bn2dec(bigHF);
		const char *chB = BN_bn2dec(bigB);

		LOG(INFO) << "Cpu, n: " << *((cl_ulong *)nonce) << " HF: " << chD;

		/*for test */
		uint8_t Db1[32];
		uint8_t Db2[32];
		HexStrToLittleEndian(chD, 64, Db1, 32);
		HexStrToBigEndian(chD, 64, Db2, 32);
		
		//
		//
		//char *m1 = "BAFA4A1CA94CE9291FD9EAC4831EB07C83A7B69841BA91310F41C6209640E7A6";
		//char *p1 = "02C04AFEE640DD4B126C89B5AC557672F77B74406C5649968BF773E72C5557413E";
		//char *w1 = "0293643139EDF16168C2993BA69373100497A711E9A9D02E1AE1069507FA0D96B8";
		//char *n1 = "000000132D209ED2";
		//char *d1 = "4854743386003293654467272129308314596379487751955149228730338683140";
		//memcpy(m_str, m1, NUM_SIZE_8 * 2);
		//memcpy(pk_str, p1, PK_SIZE_8 * 2);
		//memcpy(w_str, w1, PK_SIZE_8 * 2);
		//memcpy(n_str, n1, NONCE_SIZE_8 * 2);
		//
		/*for test */





		solVer.VerifySolution(AlgVer, m_str, pk_str, w_str, n_str,h_str,(char *) chD, pool_str);
		if (cmp <= 0)
		{
			LOG(INFO) << "CpuD" << chD;
			LOG(INFO) << "Difficulty_B" << chD;
			int tr;
			tr = 0;
			//find
		}
		else
		{
			int tr;
			tr = 0;
			//find
		}


		////---------------------secp256k1-------------------------
		//EC_KEY *ec_key = EC_KEY_new();
		//EC_GROUP *ec_group = EC_GROUP_new_by_curve_name(NID_secp256k1);

		//EC_KEY_set_group(ec_key, ec_group);
		//EC_KEY_generate_key(ec_key);

		//const EC_POINT *genarator = EC_GROUP_get0_generator(ec_group);
		//char* wgen;
		//FUNCTION_CALL(
		//	wgen, EC_POINT_point2hex(ec_group, genarator, POINT_CONVERSION_COMPRESSED, NULL),
		//	ERROR_OPENSSL
		//	);

		//BIGNUM* x = BN_new();
		//BIGNUM* y = BN_new();
		//int r111 = EC_POINT_get_affine_coordinates_GFp(ec_group, genarator, x, y, NULL);

		//char *bnX = BN_bn2dec(x);
		//char *bnY = BN_bn2dec(y);

		//BIGNUM* order = BN_new();
		//int resO = EC_GROUP_get_order(ec_group, order, NULL);
		//char *bnD = BN_bn2dec(order);
		//char *bnH = BN_bn2hex(order);


		//BIGNUM* m = BN_new();
		//BIGNUM* m0 = BN_new();
		//EC_POINT *r1;
		//EC_POINT *r2;
		//BN_CTX* ctx;


		//FUNCTION_CALL(r1, EC_POINT_new(ec_group), ERROR_OPENSSL);
		//FUNCTION_CALL(r2, EC_POINT_new(ec_group), ERROR_OPENSSL);


		//EC_POINT_add(ec_group, r1, genarator, genarator, NULL);
		//r111 = EC_POINT_get_affine_coordinates_GFp(ec_group, r1, x, y, NULL);
		//char *bnX1 = BN_bn2dec(x);
		//char *bnY1 = BN_bn2dec(y);

		//CALL(BN_hex2bn(&m, "8"), ERROR_OPENSSL);
		//CALL(BN_hex2bn(&m0, "0"), ERROR_OPENSSL);
		//CALL(EC_POINT_mul(ec_group, r2, m0, r1, m, NULL), ERROR_OPENSSL);
		//r111 = EC_POINT_get_affine_coordinates_GFp(ec_group, r2, x, y, NULL);
		//char *bnX2 = BN_bn2dec(x);
		//char *bnY2 = BN_bn2dec(y);

		////---------------------secp256k1-------------------------

		//EC_POINT *pk_point = EC_POINT_hex2point(ec_group, pkstr, NULL, NULL);
		//r111 = EC_POINT_get_affine_coordinates_GFp(ec_group, pk_point, x, y, NULL);
		//char *pkX = BN_bn2dec(x);
		//char *pkY = BN_bn2dec(y);


		//EC_POINT *w_point = EC_POINT_hex2point(ec_group, w_str, NULL, NULL);
		//r111 = EC_POINT_get_affine_coordinates_GFp(ec_group, w_point, x, y, NULL);
		//char *wX = BN_bn2dec(x);
		//char *wY = BN_bn2dec(y);


		////-------------------------left--------------------------
		//EC_POINT *left_point;
		//FUNCTION_CALL(left_point, EC_POINT_new(ec_group), ERROR_OPENSSL);
		//CALL(EC_POINT_mul(ec_group, left_point, m0, w_point, BigF, NULL), ERROR_OPENSSL);
		//r111 = EC_POINT_get_affine_coordinates_GFp(ec_group, left_point, x, y, NULL);
		//char *leftX = BN_bn2dec(x);
		//char *leftY = BN_bn2dec(y);

		//BIGNUM* left_x = BN_new();
		//BIGNUM* left_y = BN_new();
		//BN_dec2bn(&left_x, leftX);
		//BN_dec2bn(&left_y, leftY);
		////--------------------------right-------------------------
		//EC_POINT *right_pointM;
		//EC_POINT *right_pointA;
		//FUNCTION_CALL(right_pointM, EC_POINT_new(ec_group), ERROR_OPENSSL);
		//FUNCTION_CALL(right_pointA, EC_POINT_new(ec_group), ERROR_OPENSSL);
		//BIGNUM* BigD = BN_new();
		//CALL(BN_dec2bn(&BigD, d_str), ERROR_OPENSSL);
		////	CALL(BN_dec2bn(&BigD, "14"/*d_char*/), ERROR_OPENSSL);

		//CALL(EC_POINT_mul(ec_group, right_pointM, m0, genarator, BigD, NULL), ERROR_OPENSSL);
		//r111 = EC_POINT_get_affine_coordinates_GFp(ec_group, right_pointM, x, y, NULL);
		//char *rightX1 = BN_bn2dec(x);
		//char *rightY1 = BN_bn2dec(y);
		//EC_POINT_add(ec_group, right_pointA, right_pointM, pk_point, NULL);
		//r111 = EC_POINT_get_affine_coordinates_GFp(ec_group, right_pointA, x, y, NULL);
		//char *rightX = BN_bn2dec(x);
		//char *rightY = BN_bn2dec(y);

		//BIGNUM* right_x = BN_new();
		//BIGNUM* right_y = BN_new();
		//BN_dec2bn(&right_x, rightX);
		//BN_dec2bn(&right_y, rightY);

		//int xComp = BN_cmp(right_x, left_x);
		//int yComp = BN_cmp(right_y, left_y);
		//if (!xComp && !yComp)
		//{
		//	LOG(INFO) << "Verify Solution: OK";
		//}
		//else
		//{
		//	LOG(INFO) << "--- Verify Solution: Not OK---";
		//}


	} //while (true);

	int tr = 0;
}
