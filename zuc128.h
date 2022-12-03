#ifndef __ZUC128__
#define __ZUC128__
/* ----------------------- */ 
typedef unsigned char u8;
typedef unsigned int u32;
/* ----------------------- */ 


/* ----------------------- */ 
/* c = a + b mod 2^31 - 1 */
u32 AddM(u32 a, u32 b);

/* LFSR with initialization mode */
#define MulByPow2(x, k) ((((x) << k) | ((x) >> (31 - k))) & 0x7FFFFFFF)
void LFSRWithInitialisationMode(u32 u);
/* LFSR with work mode */
void LFSRWithWorkMode();
/* BitReorganization */
void BitReorganization();

#define ROT(a, k) (((a) << k) | ((a) >> (32 - k)))

/* L1 */
u32 L1(u32 X);

/* L2 */
u32 L2(u32 X);

#define MAKEU32(a, b, c, d) (((u32)(a) << 24) | ((u32)(b) << 16) | ((u32)(c) << 8) | ((u32)(d)))
/* F */
u32 F();

#define MAKEU31(a, b, c) (((u32)(a) << 23) | ((u32)(b) << 8) | (u32)(c))

/* initialize */
void Initialization(u8* k, u8* iv);

void GenerateKeystream(u32* pKeystream, int KeystreamLen);
#endif // __ZUC128__

