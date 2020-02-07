
/*
songlei@qihoo.net
2007/01/07
*/
// 2010-05.17 Ìí¼ÓÇ©Ãû¿â
// 2010.05.17 ÐÏ³¬ÆÀÉóÍ¨¹ý
#ifndef _SIGNLIB_INC_
#define _SIGNLIB_INC_

#ifdef __cplusplus
extern "C" {
#endif

#ifdef _MSC_VER
#pragma warning(disable : 4101) // disable 'unreferenced local variable'
#endif

#ifndef _ubyte_define_
#define _ubyte_define_
typedef unsigned char ubyte;
#endif

#ifndef X_MALLOC
#define X_MALLOC(x)	malloc(x)
#endif

#ifndef X_FREE
#define X_FREE(x)	free(x)
#endif

#define SIGN_DATA_LEN	32

typedef struct _sign_data_t
{
	int size;
	ubyte* data;
	int dlen;
	ubyte* key;
	ubyte* sign;
	int slen; // slen >= 128
}sign_data_t;

#ifndef _SYS

#include <Windows.h>

int get_pubkey_len();
int get_prvkey_len();

int gen_signkey(ubyte* pubkey, int pubkey_len, ubyte* prvkey, int prvkey_len);

int calc_sign(sign_data_t* sd);

BOOL CheckFileSign(LPCTSTR lpFileName, const ubyte* key);
BOOL Check360Sign(LPCTSTR lpFileName);

#endif

int check_sign(const sign_data_t* sd);

#ifdef __cplusplus
};
#endif

#endif /* _SIGNLIB_INC_ */