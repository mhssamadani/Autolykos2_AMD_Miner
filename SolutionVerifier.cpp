#include "SolutionVerifier.h"
#include <boost/multiprecision/cpp_int.hpp>


SolutionVerifier::SolutionVerifier()
{
	uint8_t sk[NUM_SIZE_8 * 2];
	char skstr[NUM_SIZE_4+10];

	memset(sk,0, NUM_SIZE_8 * 2);
	memset(skstr,0, NUM_SIZE_4);

	//Blake2b256("1", 1, sk, skstr);

	int tr = sizeof(unsigned long long);
	for (size_t i = 0; i < CONST_MES_SIZE_8/ tr; i++)
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


SolutionVerifier::~SolutionVerifier()
{

}


void SolutionVerifier::hashIn(const char * in1, const int len, uint8_t * sk)
{

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
	uint64_t aux[32];

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


void SolutionVerifier::hashIn2(const char * in1, const int len, uint8_t * sk)
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

void SolutionVerifier::Blake2b256(const char * in,
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
void SolutionVerifier::GenIdex2(const char * in, const int len , cl_uint* index )
{
	int a = INDEX_SIZE_8;
	int b = K_LEN;
	int c = NUM_SIZE_8;
	int d = NUM_SIZE_4;


	uint8_t sk[NUM_SIZE_8*2];
	char skstr[NUM_SIZE_4+10];

	memset(sk,0, NUM_SIZE_8*2);
	memset(skstr,0, NUM_SIZE_4);

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




		ind2[k ]  = ind[k] = r[k >> 2] % N_LEN;// &N_MASK;

//#pragma unroll
		for (int i = 1; i < INDEX_SIZE_8; ++i)
		{
			ind2[k + i] = ind[k + i] = ((r[k >> 2] << (i << 3)) | (r[(k >> 2) + 1] >> (32 - (i << 3)))) % N_LEN;// &N_MASK;
		}
	}

}

void SolutionVerifier::GenIdex(const char * in, const int len, cl_uint* index)
{
	int a = INDEX_SIZE_8;
	int b = K_LEN;
	int c = NUM_SIZE_8;
	int d = NUM_SIZE_4;


	uint8_t sk[NUM_SIZE_8 * 2];
	char skstr[NUM_SIZE_4 + 10];

	memset(sk,0, NUM_SIZE_8 * 2);
	memset(skstr,0, NUM_SIZE_4);

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

	memcpy(sk , beH, NUM_SIZE_8);
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
void SolutionVerifier::GenElements(const char * hh,int len, const char * ret)
{

	hashIn2(hh, len,(uint8_t *) ret);

	//FILE *fp =  fopen("C_h.dat", "wb");
	//fwrite(hh, 1, len, fp);
	//fclose(fp);
	//uint128_t  retI= uint128_t((char *)ret);
	int tr;
	tr = 0;

}

void SolutionVerifier::decodes(
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
void SolutionVerifier::setSolution(
		const uint8_t * mes,
		char  * m_str,
		const uint8_t * w,
		char  * w_str,
		const uint8_t * nonce,
		char  *nonce_str
)
{


	//HexStrToBigEndian(
	//	newreq->GetTokenStart(MesPos), newreq->GetTokenLen(MesPos),
	//	info->mes, NUM_SIZE_8
	//	);
	BigEndianToHexStr(mes, PK_SIZE_8, m_str);


    BigEndianToHexStr(w, PK_SIZE_8, w_str);
    //pos += PK_SIZE_4;


    LittleEndianToHexStr(nonce, NONCE_SIZE_8, nonce_str);
    //pos += NONCE_SIZE_4;

}
using namespace boost::multiprecision;
void SolutionVerifier::VerifySolution(
		int AlgVer,
		char * mes_str,
		 char  * pk_str,
		 char  *  w_str,
		 char  *  n_str,
		char  *  h_str,
	char * d_str,
	char *pool_str
	)
{
	m_iAlgVer = AlgVer;
	//char d_str[69];// *d_str = NULL;
	int len;
	uint8_t message[NUM_SIZE_8];
	HexStrToBigEndian(mes_str, NUM_SIZE_8 * 2, message, NUM_SIZE_8);


	uint8_t beN[NONCE_SIZE_8];
	HexStrToBigEndian(n_str, NONCE_SIZE_8*2, beN, NONCE_SIZE_8);

	uint8_t beH[NONCE_SIZE_8];
	HexStrToBigEndian(h_str, NONCE_SIZE_8 * 2, beH, NONCE_SIZE_8);

	uint8_t bePK[PK_SIZE_8];
	HexStrToBigEndian(pk_str, PK_SIZE_8*2, bePK, PK_SIZE_8);

	uint8_t beW[PK_SIZE_8];
	HexStrToBigEndian(w_str, PK_SIZE_8 * 2, beW, PK_SIZE_8);

//	char *d_char = new char[dLen];
//	memset(d_char,0, dLen );
//	memcpy(d_char, d, dLen);
//	decodes((char *)message, pk, w, nonce, d_char);


	cl_uint index[K_LEN];

	//-----------Gen Index
	uint8_t n_h_M_m[NONCE_SIZE_8  + HEIGHT_SIZE + CONST_MES_SIZE_8 + NUM_SIZE_8  ];
	memcpy(n_h_M_m, beN, NONCE_SIZE_8);
	memcpy(n_h_M_m + NONCE_SIZE_8, beH, HEIGHT_SIZE);
	memcpy(n_h_M_m + NONCE_SIZE_8 + HEIGHT_SIZE, CONST_MESS, CONST_MES_SIZE_8);
	memcpy(n_h_M_m+ NONCE_SIZE_8+ HEIGHT_SIZE+ CONST_MES_SIZE_8, message, NUM_SIZE_8);
	GenIdex((const char*)n_h_M_m, NONCE_SIZE_8 + HEIGHT_SIZE + CONST_MES_SIZE_8 + NUM_SIZE_8, index);


	uint8_t ret[32][NUM_SIZE_8];
	uint8_t *Hinput = new uint8_t[sizeof(cl_uint) + CONST_MES_SIZE_8 + PK_SIZE_8 + NUM_SIZE_8 + PK_SIZE_8];
	int ll = sizeof(cl_uint) + CONST_MES_SIZE_8 + PK_SIZE_8 + NUM_SIZE_8 + PK_SIZE_8;

	//FILE *fp =  fopen("C_flist.dat", "wb");

	BIGNUM* bigsum = BN_new();
	CALL(BN_dec2bn(&bigsum, "0"), ERROR_OPENSSL);

	BIGNUM* bigres = BN_new();
	CALL(BN_dec2bn(&bigres, "0"), ERROR_OPENSSL);


	for (int rep = 0; rep < 32; rep++)
	{
		memset(Hinput,0, ll);
		//H(j || M || pk || m || w)

		uint8_t tmp2[4];
		uint8_t tmp1[4];
		memcpy(tmp1, &index[rep], 4);
		tmp2[0] = tmp1[3];
		tmp2[1] = tmp1[2];
		tmp2[2] = tmp1[1];
		tmp2[3] = tmp1[0];

		int off = 0;
		memcpy(Hinput+ off, tmp2, sizeof(cl_uint));
		off += sizeof(cl_uint);

		//toBigInt(hash(Bytes.concat(indexBytes, heightBytes, M)).drop(1))

		{
			memcpy(Hinput + off, beH, HEIGHT_SIZE);
			off += HEIGHT_SIZE;

			memcpy(Hinput + off, CONST_MESS, CONST_MES_SIZE_8);
			off += CONST_MES_SIZE_8;

		}
		hashIn2((const char *)Hinput,off, (uint8_t *)ret[rep]);

		uint8_t tmp[NUM_SIZE_8-1];
		//memcpy(tmp, (uint8_t *)ret[rep], 31);
		memcpy(tmp, &(ret[rep][1]), 31);

		char hesStr[64+1];
		BigEndianToHexStr((uint8_t *)tmp, 31, hesStr);

		CALL(BN_hex2bn(&bigres, hesStr), ERROR_OPENSSL);

		CALL(BN_add(bigsum, bigsum, bigres), ERROR_OPENSSL);

	}
	//fclose(fp);


//	BIGNUM* bigQ = BN_new();
//	CALL(BN_dec2bn(&bigQ, "115792089237316195423570985008687907852837564279074904382605163141518161494337"), ERROR_OPENSSL);

	const char *SUMM = BN_bn2dec(bigsum);
	unsigned char f[32];
	memset(f, 0, 32);
	BN_bn2bin(bigsum,f);
	char hf[32];
	hashIn2((const char *)f, 32, (uint8_t *)hf);

	char HFStr[64 + 1];
	BigEndianToHexStr((uint8_t *)hf, 32, HFStr);

	BIGNUM* bigHF = BN_new();
	CALL(BN_dec2bn(&bigHF, "0"), ERROR_OPENSSL);
	CALL(BN_hex2bn(&bigHF, HFStr), ERROR_OPENSSL);

	BIGNUM* bigB = BN_new();
	CALL(BN_dec2bn(&bigB, pool_str), ERROR_OPENSSL);
	
	int cmp = BN_cmp(bigHF, bigB);
	int tr; 
	tr = 0;

}
