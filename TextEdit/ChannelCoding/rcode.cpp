/* RCODE.C - Procedures to read parity check and generator matrices. */

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

#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <cassert>
#include "alloc.h"
#include "intio.h"
#include "open.h"
#include "mod2sparse.h"
#include "mod2dense.h"
#include "mod2convert.h"
#include "rcode.h"
namespace LDPCODEC {

	/* VARIABLES DECLARED IN RCODE.H.  These global variables are set to
	representations of the parity check and generator matrices by read_pchk
	and read_gen. */


	/* READ PARITY CHECK MATRIX.  Sets the H, M, and N global variables.  If an
	error is encountered, a message is displayed on standard error, and the
	program is terminated. */

	void read_pchk
	(
		ldpcodec * codec,
		const char *pchk_file
	)
	{
		FILE *f;

		f = open_file_std(pchk_file, "rb");
		if (f == NULL)
		{
			fprintf(stderr, "Can't open parity check file: %s\n", pchk_file);
			assert(false);
		}

		if (intio_read(f) != ('P' << 8) + 0x80)
		{
			fprintf(stderr, "File %s doesn't contain a parity check matrix\n", pchk_file);
			assert(false);
		}

		codec->free_H();
		codec->H = mod2sparse_read(f);

		if (codec->H == 0)
		{
			fprintf(stderr, "Error reading parity check matrix from %s\n", pchk_file);
			assert(false);
		}

		codec->M = mod2sparse_rows(codec->H);
		codec->N = mod2sparse_cols(codec->H);

		fclose(f);
	}


	/* READ GENERATOR MATRIX.  The parity check matrix must have already been
	read, unless the last argument is set to 1.  The generator matrix must be
	compatible with the parity check matrix, if it has been read.  If the
	second argument is 1, only the column ordering (the last N-M of which are
	the indexes of the message bits) is read, into the 'cols' global variable.
	Otherwise, everything is read, into the global variables appropriate
	to the representation.  The 'type' global variable is set to a letter
	indicating which represention is used.

	If an error is encountered, a message is displayed on standard error,
	and the program is terminated. */

	void read_gen
	(
		ldpcodec * codec,
		const char * gen_file,	/* Name of generator matrix file */
		int cols_only,	/* Read only column ordering? */
		int no_pchk_file	/* No parity check file used? */
	)
	{
		int M2, N2;
		FILE *f;
		int i;

		f = open_file_std(gen_file, "rb");
		if (f == NULL)
		{
			fprintf(stderr, "Can't open generator matrix file: %s\n", gen_file);
			assert(false);
		}

		if (intio_read(f) != ('G' << 8) + 0x80)
		{
			fprintf(stderr, "File %s doesn't contain a generator matrix\n", gen_file);
			assert(false);
		}

		if (fread(&codec->type, 1, 1, f) != 1) goto error;

		M2 = intio_read(f);
		N2 = intio_read(f);

		if (feof(f) || ferror(f)) goto error;

		if (no_pchk_file)
		{
			codec->M = M2;
			codec->N = N2;
		}
		else
		{
			if (M2 != codec->M || N2 != codec->N)
			{
				fprintf(stderr,
					"Generator matrix and parity-check matrix are incompatible\n");
				assert(false);
			}
		}

		codec->free_rowcol();
		codec->cols = (int *)chk_alloc(codec->N, sizeof *codec->cols);
		codec->rows = (int *)chk_alloc(codec->M, sizeof *codec->rows);

		for (i = 0; i<codec->N; i++)
		{
			codec->cols[i] = intio_read(f);
			if (feof(f) || ferror(f)) goto error;
		}

		if (!cols_only)
		{
			switch (codec->type)
			{
			case 's':
			{
				for (i = 0; i<codec->M; i++)
				{
					codec->rows[i] = intio_read(f);
					if (feof(f) || ferror(f)) goto error;
				}
				codec->free_LU();

				if ((codec->L = mod2sparse_read(f)) == 0) goto error;
				if ((codec->U = mod2sparse_read(f)) == 0) goto error;

				if (mod2sparse_rows(codec->L) != codec->M || mod2sparse_cols(codec->L) != codec->M) goto garbled;
				if (mod2sparse_rows(codec->U) != codec->M || mod2sparse_cols(codec->U)<codec->M) goto garbled;

				break;
			}

			case 'd':
			{
				codec->free_G();
				if ((codec->G = mod2dense_read(f)) == 0) goto error;

				if (mod2dense_rows(codec->G) != codec->M || mod2dense_cols(codec->G) != codec->N - codec->M) goto garbled;

				break;
			}

			case 'm':
			{
				codec->free_G();
				if ((codec->G = mod2dense_read(f)) == 0) goto error;

				if (mod2dense_rows(codec->G) != codec->M || mod2dense_cols(codec->G) != codec->M) goto garbled;

				break;
			}

			default:
			{ fprintf(stderr,
				"Unknown type of generator matrix in file %s\n", gen_file);
			assert(false);
			}
			}
		}

		fclose(f);

		return;

	error:
		fprintf(stderr, "Error reading generator matrix from file %s\n", gen_file);
		assert(false);

	garbled:
		fprintf(stderr, "Garbled generator matrix in file %s\n", gen_file);
		assert(false);
	}
}
