#ifndef LDPC_ENCODE_H
#define LDPC_ENCODE_H
#include "rand.h"
#include "alloc.h"
#include "intio.h"
#include "open.h"
#include "mod2sparse.h"
#include "mod2dense.h"
#include "mod2convert.h"
#include "rcode.h"
#include "distrib.h"
#include "dec.h"
namespace LDPCODEC {
	int ldpc_encode
	(
		ldpcodec * codec,
		const char sblk[/*N-M*/],
		char cblk[/*N*/],
		char chks[/*M*/],
		bool check
	);
	int ldpc_decode
	(
		decfg * cfg,
		const char *bsc_data/*[n]*/,
		const double * awn_data/*[n]*/,
		char* data/*[N-M]*/
	);
}
#endif // LDPC_ENCODE_H
