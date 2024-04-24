#pragma once
#include "mod2sparse.h"
#include "mod2dense.h"
/* RCODE.H - Parity chk and gen matrix storage, and procedures to read them.*/

/* Copyright (c) 1995-2012 by Radford M. Neal.
*
* Permission is granted for anyone to copy, use, modify, and distribute
* these programs and accompanying documents for any purpose, provided
* this copyright notice is retained and prominently displayed, and note
* is made of any changes made to these programs.  These programs and
* documents are distributed without any warranty, express or implied.
* As the programs were written for research purposes only, they have not
* been tested to the degree that would be advisable in any important
* application.  All use of these programs is entirely at the user's own
* risk.
*/
#include <cstdlib>
namespace LDPCODEC {

	/* VARIABLES HOLDING DATA READ.  These are declared for real in rcode.c. */

	struct ldpcodec {
		mod2sparse *H = nullptr;	/* Parity check matrix */
		int M = 0;					/* Number of rows in parity check matrix */
		int N = 0;					/* Number of columns in parity check matrix */
		char type = 0;				/* Type of generator matrix representation */
		int *cols = nullptr;		/* Ordering of columns in generator matrix */
		mod2sparse *L = nullptr;	/* Sparse LU decomposition, if type=='s' */
		mod2sparse *U = nullptr;	/* Sparse LU decomposition, if type=='s' */
		int *rows = nullptr;		/* Ordering of rows in generator matrix (type 's') */
		mod2dense *G = nullptr;		/* Dense or mixed representation of generator matrix,
									if type=='d' or type=='m' */

		mod2dense * u = nullptr;
		mod2dense * v = nullptr;
		ldpcodec() {}
		~ldpcodec() {
			printf("free memory.");
			free_H();
			free_G();
			free_uv();
			free_LU();
			free_rowcol();
		}
		void free_H() {
			if (H)	mod2sparse_free(H);
			H = nullptr;
		}
		void free_LU()
		{
			if (L)	mod2sparse_free(L);
			L = nullptr;
			if (U)	mod2sparse_free(U);
			U = nullptr;
		}
		void free_uv()
		{
			if (u)	mod2dense_free(u);
			u = nullptr;
			if (v)	mod2dense_free(v);
			v = nullptr;
		}
		void free_G()
		{
			if (G)	mod2dense_free(G);
			G = nullptr;
		}
		void free_rowcol()
		{
			if (cols)	free(cols);
			cols = nullptr;
			if (rows)	free(rows);
			rows = nullptr;
		}
	};

	/* PROCEDURES FOR READING DATA. */

	void read_pchk(ldpcodec * codec, const char *);
	void read_gen(ldpcodec * codec, const char *, int, int);
}
