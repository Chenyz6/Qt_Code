#ifndef LDPC_DEF_H
#define LDPC_DEF_H
#include "rand.h"
#include "alloc.h"
#include "intio.h"
#include "open.h"
#include "mod2sparse.h"
#include "mod2dense.h"
#include "mod2convert.h"
#include "rcode.h"
#include "distrib.h"
namespace LDPCODEC {
	enum make_method
	{
		Evencol, 	/* Uniform number of bits per column, with number specified */
		Evenboth, 	/* Uniform (as possible) over both columns and rows */
		Sparse, Dense, Mixed //Gen
	};
	//1.Make LDPC H Mat
	int make_ldpc_file
	(
		ldpcodec * codec,
		const char * file,
		const int nM,
		const int nN,
		const int seed,
		const char * str_method,
		const char * str_distrib,
		const char * str_cycle
	);
	void make_ldpc(ldpcodec * codec, int, make_method, distrib *, int);
	int write_ldpc_file
	(
		ldpcodec * codec,
		const char * file
	);
	//2.Make LDPC G(x) for encoding
	int make_gen_file(ldpcodec * codec,
		const char *pchk_file,
		const char *gen_file,
		const char* meth = "dense",
		const char *str_strategy = "minprod",
		int abandon_number = 0,
		int abandon_when = 0,
		const char *other_gen_file = 0
	);
	void make_dense_mixed(ldpcodec * codec, FILE *, make_method = Dense, const char * p = nullptr);     /* Procs to make it */
	void make_sparse(ldpcodec * codec, FILE *, mod2sparse_strategy, int, int);

	//3. A-List and BIN pCHK converting
	int write_alist
	(
		ldpcodec * codec,
		FILE * af,
		int trans = 0,
		int nozeros = 0
	);
	int read_alist(
		ldpcodec * codec,
		FILE * af,
		int trans = 0
	);
	//4.File read
	void read_pchk(ldpcodec * codec, const char *);
	void read_gen(ldpcodec * codec, const char *, int, int);
}
#endif // LDPC_DEF_H
