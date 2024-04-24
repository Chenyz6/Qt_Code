#include "ldpc_gene.h"

/* MAKE-LDPC.C - Make a Low Density Parity Check code's parity check matrix. */

/* Copyright (c) 1995-2012 by Radford M. Neal and Peter Junteng Liu.
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
#include <string.h>
#include <math.h>
#include <cassert>
namespace LDPCODEC {
	/* METHODS FOR CONSTRUCTING CODES. */


	int *column_partition(distrib *, int);

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
	)
	{
		make_method method;
		int no4cycle;
		distrib *d=NULL;
		FILE *f;

		fprintf(stderr, "Start Make LDPC HMatrix...\n");

		/* Look at initial arguments. */
		codec->M = nM;
		codec->N = nN;

		/* Look at the arguments specifying the method for producing the code. */
		no4cycle = 0;

		if (strcmp(str_method, "evencol") == 0 || strcmp(str_method, "evenboth") == 0)
		{
			method = strcmp(str_method, "evencol") == 0 ? Evencol : Evenboth;
			assert(str_distrib);
			d = distrib_create(str_distrib);
			assert(d);
			if (str_cycle)
			{
				if (strcmp(str_cycle, "no4cycle") == 0)
					no4cycle = 1;
			}
		}
		else
		{
			assert(false);
		}

		/* Check for some problems. */

		if (distrib_max(d)>codec->M)
		{
			fprintf(stderr,
				"At least one checks per bit (%d) is greater than total checks (%d)\n",
				distrib_max(d), codec->M);
			assert(false);
		}

		if (distrib_max(d) == codec->M && codec->N>1 && no4cycle)
		{
			fprintf(stderr,
				"Can't eliminate cycles of length four with this many checks per bit\n");
			assert(false);
		}

		/* Make the parity check matrix. */

		make_ldpc(codec, seed, method, d, no4cycle);

		/* Write out the parity check matrix. */

		f = open_file_std(file, "wb");
		if (f == NULL)
		{
			fprintf(stderr, "Can't create parity check file: %s\n", file);
			assert(false);
		}

		intio_write(f, ('P' << 8) + 0x80);

		if (ferror(f) || !mod2sparse_write(f, codec->H) || fclose(f) != 0)
		{
			fprintf(stderr, "Error writing to parity check file %s\n", file);
			assert(false);
		}
		fprintf(stderr, "Make LDPC HMatrix OK!\n");
		distrib_free(d);
		return 0;
	}

	int write_ldpc_file
	(
		ldpcodec * codec,
		const char * file
	)
	{
		FILE *f;
		/* Write out the parity check matrix. */

		f = open_file_std(file, "wb");
		if (f == NULL)
		{
			fprintf(stderr, "Can't create parity check file: %s\n", file);
			assert(false);
		}

		intio_write(f, ('P' << 8) + 0x80);

		if (ferror(f) || !mod2sparse_write(f, codec->H) || fclose(f) != 0)
		{
			fprintf(stderr, "Error writing to parity check file %s\n", file);
			assert(false);
		}
		fprintf(stderr, "Make LDPC HMatrix OK!\n");
		return 0;
	}

	/* CREATE A SPARSE PARITY-CHECK MATRIX.  Of size M by N, stored in H. */

	void make_ldpc
	(
		ldpcodec * codec,
		int seed,			/* Random number seed */
		make_method method,	/* How to make it */
		distrib *d,			/* Distribution list specified */
		int no4cycle		/* Eliminate cycles of length four? */
	)
	{
		mod2entry *e, *f, *g, *h;
		int added, uneven, elim4, all_even, n_full, left;
		int i, j, k, t, z, cb_N;
		int *part = nullptr, *u = nullptr;

		rand_seed(10 * seed + 1);
		codec->free_H();
		codec->H = mod2sparse_allocate(codec->M, codec->N);
		part = column_partition(d, codec->N);

		/* Create the initial version of the parity check matrix. */

		switch (method)
		{
		case Evencol:
		{
			z = 0;
			left = part[z];

			for (j = 0; j<codec->N; j++)
			{
				while (left == 0)
				{
					z += 1;
					if (z>distrib_size(d))
					{
						abort();
					}
					left = part[z];
				}
				for (k = 0; k<distrib_num(d, z); k++)
				{
					do
					{
						i = rand_int(codec->M);
					} while (mod2sparse_find(codec->H, i, j));
					mod2sparse_insert(codec->H, i, j);
				}
				left -= 1;
			}

			break;
		}

		case Evenboth:
		{
			cb_N = 0;
			for (z = 0; z<distrib_size(d); z++)
			{
				cb_N += distrib_num(d, z) * part[z];
			}

			u = (int *)chk_alloc(cb_N, sizeof *u);

			for (k = cb_N - 1; k >= 0; k--)
			{
				u[k] = k%codec->M;
			}

			uneven = 0;
			t = 0;
			z = 0;
			left = part[z];

			for (j = 0; j<codec->N; j++)
			{
				while (left == 0)
				{
					z += 1;
					if (z>distrib_size(d))
					{
						abort();
					}
					left = part[z];
				}

				for (k = 0; k<distrib_num(d, z); k++)
				{
					for (i = t; i<cb_N && mod2sparse_find(codec->H, u[i], j); i++);

					if (i == cb_N)
					{
						uneven += 1;
						do
						{
							i = rand_int(codec->M);
						} while (mod2sparse_find(codec->H, i, j));
						mod2sparse_insert(codec->H, i, j);
					}
					else
					{
						do
						{
							i = t + rand_int(cb_N - t);
						} while (mod2sparse_find(codec->H, u[i], j));
						mod2sparse_insert(codec->H, u[i], j);
						u[i] = u[t];
						t += 1;
					}
				}

				left -= 1;
			}

			if (uneven>0)
			{
				fprintf(stderr, "Had to place %d checks in rows unevenly\n", uneven);
			}

			break;
		}

		default: abort();
		}

		/* Add extra bits to avoid rows with less than two checks. */

		added = 0;

		for (i = 0; i<codec->M; i++)
		{
			e = mod2sparse_first_in_row(codec->H, i);
			if (mod2sparse_at_end(e))
			{
				j = rand_int(codec->N);
				e = mod2sparse_insert(codec->H, i, j);
				added += 1;
			}
			e = mod2sparse_first_in_row(codec->H, i);
			if (mod2sparse_at_end(mod2sparse_next_in_row(e)) && codec->N>1)
			{
				do
				{
					j = rand_int(codec->N);
				} while (j == mod2sparse_col(e));
				mod2sparse_insert(codec->H, i, j);
				added += 1;
			}
		}

		if (added>0)
		{
			fprintf(stderr,
				"Added %d extra bit-checks to make row counts at least two\n",
				added);
		}

		/* Add extra bits to try to avoid problems with even column counts. */

		n_full = 0;
		all_even = 1;
		for (z = 0; z<distrib_size(d); z++)
		{
			if (distrib_num(d, z) == codec->M)
			{
				n_full += part[z];
			}
			if (distrib_num(d, z) % 2 == 1)
			{
				all_even = 0;
			}
		}

		if (all_even && codec->N - n_full>1 && added<2)
		{
			int a;
			for (a = 0; added + a<2; a++)
			{
				do
				{
					i = rand_int(codec->M);
					j = rand_int(codec->N);
				} while (mod2sparse_find(codec->H, i, j));
				mod2sparse_insert(codec->H, i, j);
			}
			fprintf(stderr,
				"Added %d extra bit-checks to try to avoid problems from even column counts\n",
				a);
		}

		/* Eliminate cycles of length four, if asked, and if possible. */

		if (no4cycle)
		{
			elim4 = 0;

			for (t = 0; t<10; t++)
			{
				k = 0;
				for (j = 0; j<codec->N; j++)
				{
					for (e = mod2sparse_first_in_col(codec->H, j);
						!mod2sparse_at_end(e);
						e = mod2sparse_next_in_col(e))
					{
						for (f = mod2sparse_first_in_row(codec->H, mod2sparse_row(e));
							!mod2sparse_at_end(f);
							f = mod2sparse_next_in_row(f))
						{
							if (f == e) continue;
							for (g = mod2sparse_first_in_col(codec->H, mod2sparse_col(f));
								!mod2sparse_at_end(g);
								g = mod2sparse_next_in_col(g))
							{
								if (g == f) continue;
								for (h = mod2sparse_first_in_row(codec->H, mod2sparse_row(g));
									!mod2sparse_at_end(h);
									h = mod2sparse_next_in_row(h))
								{
									if (mod2sparse_col(h) == j)
									{
										do
										{
											i = rand_int(codec->M);
										} while (mod2sparse_find(codec->H, i, j));
										mod2sparse_delete(codec->H, e);
										mod2sparse_insert(codec->H, i, j);
										elim4 += 1;
										k += 1;
										goto nextj;
									}
								}
							}
						}
					}
				nextj:;
				}
				if (k == 0) break;
			}

			if (elim4>0)
			{
				fprintf(stderr,
					"Eliminated %d cycles of length four by moving checks within column\n",
					elim4);
			}

			if (t == 10)
			{
				fprintf(stderr,
					"Couldn't eliminate all cycles of length four in 10 passes\n");
			}
		}
		//Add by D
		if (part)
			free(part);
		if (u)
			free(u);
	}


	/* PARTITION THE COLUMNS ACCORDING TO THE SPECIFIED PROPORTIONS.  It
	may not be possible to do this exactly.  Returns a pointer to an
	array of integers containing the numbers of columns corresponding
	to the entries in the distribution passed. */

	int *column_partition
	(distrib *d,		/* List of proportions and number of check-bits */
		int n			/* Total number of columns to partition */
	)
	{
		double *trunc;
		int *part;
		int cur, used;
		int i, j;

		trunc = (double *)chk_alloc(distrib_size(d), sizeof(double));
		part = (int *)chk_alloc(distrib_size(d), sizeof(int));

		used = 0;
		for (i = 0; i<distrib_size(d); i++)
		{
			cur = floor(distrib_prop(d, i)*n);
			part[i] = cur;
			trunc[i] = distrib_prop(d, i)*n - cur;
			used += cur;
		}

		if (used>n)
		{
			abort();
		}

		while (used<n)
		{
			cur = 0;
			for (j = 1; j<distrib_size(d); j++)
			{
				if (trunc[j]>trunc[cur])
				{
					cur = j;
				}
			}
			part[cur] += 1;
			used += 1;
			trunc[cur] = -1;
		}

		free(trunc);
		return part;
	}



	int make_gen_file
	(
		ldpcodec * codec,
		const char *pchk_file,
		const char *gen_file,
		const char* meth /*= "dense"*/,
		const char *str_strategy /*="minprod"*/,
		int abandon_number /*=0*/,
		int abandon_when,/*=0*/
		const char *other_gen_file /*=0*/
	)
	{
		fprintf(stderr, "Start Make LDPC GMatrix...\n");
		mod2sparse_strategy strategy = Mod2sparse_minprod;
		make_method method;
		FILE *f;

		if (strcmp(meth, "sparse") == 0)
		{
			method = Sparse;
			strategy = Mod2sparse_minprod;
			if (str_strategy)
			{
				if (strcmp(str_strategy, "first") == 0)        strategy = Mod2sparse_first;
				else if (strcmp(str_strategy, "mincol") == 0)  strategy = Mod2sparse_mincol;
				else if (strcmp(str_strategy, "minprod") == 0) strategy = Mod2sparse_minprod;
				else
				{
					assert(false);
				}
			}
		}
		else if (strcmp(meth, "dense") == 0)
		{
			method = Dense;
		}
		else if (strcmp(meth, "mixed") == 0)
		{
			method = Mixed;
		}
		else
		{
			assert(false);
		}

		/* Read parity check matrix. */

		read_pchk(codec, pchk_file);
		int & M = codec->M;
		int & N = codec->N;
		int * & cols = codec->cols;
		int * & rows = codec->rows;
		if (N <= M)
		{
			fprintf(stderr,
				"Can't encode if number of bits (%d) isn't greater than number of checks (%d)\n", N, M);
			assert(false);
		}

		/* Create generator matrix file. */

		f = open_file_std(gen_file, "wb");
		if (f == NULL)
		{
			fprintf(stderr, "Can't create generator matrix file: %s\n", gen_file);
			assert(false);
		}

		/* Allocate space for row and column permutations. */

		cols = (int *)chk_alloc(N, sizeof *cols);
		rows = (int *)chk_alloc(M, sizeof *rows);

		/* Create generator matrix with specified method. */

		switch (method)
		{
		case Sparse:
		{ make_sparse(codec, f, strategy, abandon_number, abandon_when);
		break;
		}
		case Dense: case Mixed:
		{ make_dense_mixed(codec, f, method, other_gen_file);
		break;
		}
		default: abort();
		}

		/* Check for error writing file. */

		if (ferror(f) || fclose(f) != 0)
		{
			fprintf(stderr, "Error writing to generator matrix file\n");
			assert(false);
		}
		fprintf(stderr, "Make LDPC GMatrix OK!\n");
		return 0;
	}


	/* MAKE DENSE OR MIXED REPRESENTATION OF GENERATOR MATRIX. */

	void make_dense_mixed
	(
		ldpcodec * codec,
		FILE *f,
		make_method method,
		const char *other_gen_file
	)
	{
		int & M = codec->M;
		int & N = codec->N;
		int * & cols = codec->cols;
		int * & rows = codec->rows;
		mod2sparse * &H = codec->H;
		mod2dense * &G = codec->G;


		mod2dense *DH = nullptr, *A = nullptr, *A2 = nullptr, *AI = nullptr, *B = nullptr;
		int i, j, c, c2, n;
		int *rows_inv = nullptr;

		DH = mod2dense_allocate(M, N);
		AI = mod2dense_allocate(M, M);
		B = mod2dense_allocate(M, N - M);
		codec->free_G();
		codec->G = mod2dense_allocate(M, N - M);

		mod2sparse_to_dense(H, DH);

		/* If another generator matrix was specified, invert using the set of
		columns it specifies. */

		if (other_gen_file)
		{
			read_gen(codec, other_gen_file, 1, 0);

			A = mod2dense_allocate(M, M);
			mod2dense_copycols(DH, A, cols);

			if (!mod2dense_invert(A, AI))
			{
				fprintf(stderr,
					"Couldn't invert sub-matrix with column order given in other file\n");
				assert(false);
			}

			mod2dense_copycols(DH, B, cols + M);
		}

		/* If no other generator matrix was specified, invert using whatever
		selection of rows/columns is needed to get a non-singular sub-matrix. */

		if (!other_gen_file)
		{
			A = mod2dense_allocate(M, N);
			A2 = mod2dense_allocate(M, N);

			n = mod2dense_invert_selected(DH, A2, rows, cols);
			mod2sparse_to_dense(H, DH);  /* DH was destroyed by invert_selected */

			if (n>0)
			{
				fprintf(stderr, "Note: Parity check matrix has %d redundant checks\n", n);
			}

			rows_inv = (int *)chk_alloc(M, sizeof *rows_inv);

			for (i = 0; i<M; i++)
			{
				rows_inv[rows[i]] = i;
			}

			mod2dense_copyrows(A2, A, rows);
			mod2dense_copycols(A, A2, cols);
			mod2dense_copycols(A2, AI, rows_inv);

			mod2dense_copycols(DH, B, cols + M);
		}

		/* Form final generator matrix. */

		if (method == Dense)
		{
			mod2dense_multiply(AI, B, G);
		}
		else if (method == Mixed)
		{
			codec->G = AI;
		}
		else
		{
			assert(false);
		}

		/* Compute and print number of 1s. */

		if (method == Dense)
		{
			c = 0;
			for (i = 0; i<M; i++)
			{
				for (j = 0; j<N - M; j++)
				{
					c += mod2dense_get(G, i, j);
				}
			}
			fprintf(stderr,
				"Number of 1s per check in Inv(A) X B is %.1f\n", (double)c / M);
		}

		if (method == Mixed)
		{
			c = 0;
			for (i = 0; i<M; i++)
			{
				for (j = 0; j<M; j++)
				{
					c += mod2dense_get(G, i, j);
				}
			}
			c2 = 0;
			for (i = M; i<N; i++)
			{
				c2 += mod2sparse_count_col(H, cols[i]);
			}
			fprintf(stderr,
				"Number of 1s per check in Inv(A) is %.1f, in B is %.1f, total is %.1f\n",
				(double)c / M, (double)c2 / M, (double)(c + c2) / M);
		}

		/* Write the represention of the generator matrix to the file. */

		intio_write(f, ('G' << 8) + 0x80);

		if (method == Dense)
		{
			fwrite("d", 1, 1, f);
		}
		if (method == Mixed)
		{
			fwrite("m", 1, 1, f);
		}

		intio_write(f, M);
		intio_write(f, N);

		for (i = 0; i<N; i++)
		{
			intio_write(f, cols[i]);
			//printf("%d ", cols[i]);
		}
		//printf("\n");

		mod2dense_write(f, G);

		if (DH) mod2dense_free(DH);
		if (A) mod2dense_free(A);
		if (A2) mod2dense_free(A2);
		if (AI) mod2dense_free(AI);
		if (B) mod2dense_free(B);
		if (rows_inv) free(rows_inv);

	}


	/* MAKE SPARSE REPRESENTATION OF GENERATOR MATRIX. */

	void make_sparse
	(
		ldpcodec * codec,
		FILE *f,
		mod2sparse_strategy strategy,
		int abandon_number,
		int abandon_when
	)
	{
		int n, cL, cU, cB;
		int i;
		int & M = codec->M;
		int & N = codec->N;
		int * & cols = codec->cols;
		int * & rows = codec->rows;
		mod2sparse * &H = codec->H;
		mod2sparse * &L = codec->L;
		mod2sparse * &U = codec->U;
		/* Find LU decomposition. */
		codec->free_LU();
		L = mod2sparse_allocate(M, M);
		U = mod2sparse_allocate(M, N);

		n = mod2sparse_decomp(H, M, L, U, rows, cols, strategy, abandon_number, abandon_when);

		if (n != 0 && abandon_number == 0)
		{
			fprintf(stderr, "Note: Parity check matrix has %d redundant checks\n", n);
		}
		if (n != 0 && abandon_number>0)
		{
			fprintf(stderr,
				"Note: Have %d dependent columns, but this could be due to abandonment.\n", n);
			fprintf(stderr,
				"      Try again with lower abandonment number.\n");
			assert(false);
		}

		/* Compute and print number of 1s. */

		cL = cU = cB = 0;

		for (i = 0; i<M; i++) cL += mod2sparse_count_row(L, i);
		for (i = 0; i<M; i++) cU += mod2sparse_count_row(U, i);
		for (i = M; i<N; i++) cB += mod2sparse_count_col(H, cols[i]);

		fprintf(stderr,
			"Number of 1s per check in L is %.1f, U is %.1f, B is %.1f, total is %.1f\n",
			(double)cU / M, (double)cL / M, (double)cB / M, (double)(cL + cU + cB) / M);

		/* Write it all to the generator matrix file. */

		intio_write(f, ('G' << 8) + 0x80);

		fwrite("s", 1, 1, f);

		intio_write(f, M);
		intio_write(f, N);

		for (i = 0; i<N; i++)
		{
			intio_write(f, cols[i]);
		}

		for (i = 0; i<M; i++)
		{
			intio_write(f, rows[i]);
		}

		mod2sparse_write(f, L);
		mod2sparse_write(f, U);
	}


	/* MAIN PROGRAM. */

	int write_alist
	(
		ldpcodec * codec,
		FILE * af,
		int trans /*=0*/,
		int nozeros /*=0*/
	)
	{
		int mxrw, mxcw;
		int *rw = nullptr, *cw = nullptr;
		int i, j, k;
		mod2entry *e;
		int last;
		int & M = codec->M;
		int & N = codec->N;
		mod2sparse * &H = codec->H;
		if (trans)
		{
			mod2sparse *HT;
			HT = H;
			H = mod2sparse_allocate(N, M);
			mod2sparse_transpose(HT, H);
			M = mod2sparse_rows(H);
			N = mod2sparse_cols(H);
		}

		if (af == NULL)
		{
			af = stdout;
		}

		fprintf(af, "%d %d\n", M, N);

		rw = (int *)chk_alloc(M, sizeof *rw);
		mxrw = 0;

		for (i = 0; i<M; i++)
		{
			rw[i] = mod2sparse_count_row(H, i);
			if (rw[i]>mxrw)
			{
				mxrw = rw[i];
			}
		}

		cw = (int *)chk_alloc(N, sizeof *cw);
		mxcw = 0;

		for (j = 0; j<N; j++)
		{
			cw[j] = mod2sparse_count_col(H, j);
			if (cw[j]>mxcw)
			{
				mxcw = cw[j];
			}
		}

		fprintf(af, "%d %d\n", mxrw, mxcw);

		for (i = 0; i<M; i++)
		{
			fprintf(af, "%d%c", rw[i], i == M - 1 ? '\n' : ' ');
		}

		for (j = 0; j<N; j++)
		{
			fprintf(af, "%d%c", cw[j], j == N - 1 ? '\n' : ' ');
		}

		for (i = 0; i<M; i++)
		{
			e = mod2sparse_first_in_row(H, i);
			last = 0;
			for (k = 0; !last; k++)
			{
				last = nozeros ? k == rw[i] - 1 : k == mxrw - 1;
				fprintf(af, "%d%c", mod2sparse_at_end(e) ? 0 : mod2sparse_col(e) + 1,
					last ? '\n' : ' ');
				if (!mod2sparse_at_end(e))
				{
					e = mod2sparse_next_in_row(e);
				}
			}
		}

		for (j = 0; j<N; j++)
		{
			e = mod2sparse_first_in_col(H, j);
			last = 0;
			for (k = 0; !last; k++)
			{
				last = nozeros ? k == cw[j] - 1 : k == mxcw - 1;
				fprintf(af, "%d%c", mod2sparse_at_end(e) ? 0 : mod2sparse_row(e) + 1,
					last ? '\n' : ' ');
				if (!mod2sparse_at_end(e))
				{
					e = mod2sparse_next_in_col(e);
				}
			}
		}

		if (trans)
		{
			mod2sparse *HT;
			HT = H;
			H = mod2sparse_allocate(N, M);
			mod2sparse_transpose(HT, H);
			M = mod2sparse_rows(H);
			N = mod2sparse_cols(H);
		}
		if (rw)
			free(rw);
		if (cw)
			free(cw);

		return 0;
	}

	int read_alist(
		ldpcodec * codec,
		FILE * af,
		int trans
	)
	{
		int mxrw, mxcw;
		int *rw = nullptr, *cw = nullptr;
		int i, j, k;
		int tot;
		int nxt;
		int & M = codec->M;
		int & N = codec->N;
		mod2sparse * &H = codec->H;

		if (fscanf_s(af, "%d", &M) != 1 || M<1
			|| fscanf_s(af, "%d", &N) != 1 || N<1
			|| fscanf_s(af, "%d", &mxrw) != 1 || mxrw<0 || mxrw>N
			|| fscanf_s(af, "%d", &mxcw) != 1 || mxcw<0 || mxcw>M)
		{
			fprintf(stderr, "Alist file doesn't have the right format\n");
			assert(false);
		}

		rw = (int *)chk_alloc(M, sizeof *rw);

		for (i = 0; i<M; i++)
		{
			if (fscanf_s(af, "%d", &rw[i]) != 1 || rw[i]<0 || rw[i]>N)
			{
				fprintf(stderr, "Alist file doesn't have the right format\n");
				assert(false);
			}
		}

		cw = (int *)chk_alloc(N, sizeof *cw);

		for (j = 0; j<N; j++)
		{
			if (fscanf_s(af, "%d", &cw[j]) != 1 || cw[j]<0 || cw[j]>M)
			{
				fprintf(stderr, "Alist file doesn't have the right format\n");
				assert(false);
			}
		}

		H = mod2sparse_allocate(M, N);

		do { if (fscanf_s(af, "%d", &nxt) != 1) nxt = -1; } while (nxt == 0);

		tot = 0;

		for (i = 0; i<M; i++)
		{
			for (k = 0; k<rw[i]; k++)
			{
				if (nxt <= 0 || nxt>N || mod2sparse_find(H, i, nxt - 1))
				{
					fprintf(stderr, "Alist file doesn't have the right format\n");
					assert(false);
				}
				mod2sparse_insert(H, i, nxt - 1);
				tot += 1;
				do { if (fscanf_s(af, "%d", &nxt) != 1) nxt = -1; } while (nxt == 0);
			}
		}

		for (j = 0; j<N; j++)
		{
			for (k = 0; k<cw[j]; k++)
			{
				if (nxt <= 0 || nxt>M || !mod2sparse_find(H, nxt - 1, j))
				{
					fprintf(stderr, "Alist file doesn't have the right format\n");
					assert(false);
				}
				tot -= 1;
				do { if (fscanf_s(af, "%d", &nxt) != 1) nxt = -1; } while (nxt == 0);
			}
		}

		if (tot != 0 || nxt != -1 || !feof(af))
		{
			fprintf(stderr, "Alist file doesn't have the right format\n");
			assert(false);
		}

		if (trans)
		{
			mod2sparse *HT;
			HT = H;
			H = mod2sparse_allocate(N, M);
			mod2sparse_transpose(HT, H);
		}
		if (rw)
			free(rw);
		if (cw)
			free(cw);
		return 0;
	}




}
