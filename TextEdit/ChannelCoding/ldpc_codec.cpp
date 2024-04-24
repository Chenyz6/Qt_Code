#include "ldpc_codec.h"
#include "enc.h"
#include "dec.h"
#include "check.h"
#include <cassert>
#include <cmath>
namespace LDPCODEC {



	int ldpc_encode
	(
		ldpcodec * codec,
		const char sblk[/*N-M*/],
		char cblk[/*N*/],
		char chks[/*M*/],
		bool check
	)
	{
		int i = 0, n = 0;

		int & M = codec->M;
		int & N = codec->N;
		mod2sparse * &H = codec->H;
		mod2dense * &u = codec->u;
		mod2dense * &v = codec->v;
		char &type = codec->type;
		/* Allocate needed space. */

		if (!u)
		{
			codec->free_uv();
			if (type == 'd')
			{
				u = mod2dense_allocate(N - M, 1);
				v = mod2dense_allocate(M, 1);
			}

			if (type == 'm')
			{
				u = mod2dense_allocate(M, 1);
				v = mod2dense_allocate(M, 1);
			}
		}
		/* Compute encoded block. */

		switch (type)
		{
		case 's':
		{ sparse_encode(codec, sblk, cblk);
		break;
		}
		case 'd':
		{ dense_encode(codec, sblk, cblk, u, v);
		break;
		}
		case 'm':
		{ mixed_encode(codec, sblk, cblk, u, v);
		break;
		}
		}

		/* Check that encoded block is a code word. */
		if (check)
		{
			mod2sparse_mulvec(H, cblk, chks);

			for (i = 0; i<M; i++)
			{
				if (chks[i] == 1)
				{
					fprintf(stderr, "Output block %d is not a code word!  (Fails check %d)\n", n, i);
					assert(false);
				}
			}
		}


		return 0;
	}



	int ldpc_decode
	(
		decfg * cfg,
		const char *bsc_data/*[n]*/,
		const double * awn_data/*[n]*/,
		char * data/*[N-M]*/
	)
	{
		ldpcodec * codec = cfg->codec;
		assert(codec);
		int & M = codec->M;
		int & N = codec->N;
		mod2sparse * &H = codec->H;

		char *	&dblk = cfg->dblk;
		double *&lratio = cfg->lratio;
		char *	&pchk = cfg->pchk;
		double *&bitpr = cfg->bitpr;

		unsigned iters;		/* Unsigned because can be huge for enum */
		double chngd;	/* Double because can be fraction if lratio==1*/

		int valid;

		int i;

		assert(N>M);
		/* Allocate space for data from channel. */
		switch (cfg->channel)
		{
		case BSC:
		{ assert(bsc_data);
		break;
		}
		case AWGN: case AWLN:
		{ assert(awn_data);
		break;
		}
		default:
		{ assert(false);
		}
		}

		/* Allocate other space. */
		if (!dblk)
		{
			dblk = (char *)chk_alloc(N, sizeof *dblk);
			lratio = (double *)chk_alloc(N, sizeof *lratio);
			pchk = (char *)chk_alloc(M, sizeof *pchk);
			bitpr = (double *)chk_alloc(N, sizeof *bitpr);

		}

		++cfg->block_no;
		/* Do the setup for the decoding method. */

		switch (cfg->dec_method)
		{
		case Prprp:
		{ prprp_decode_setup(cfg);
		break;
		}
		case Enum_block: case Enum_bit:
		{ enum_decode_setup(cfg);
		break;
		}
		default: assert(false);
		}

		/* Find likelihood ratio for each bit. */

		switch (cfg->channel)
		{
		case BSC:
		{ for (i = 0; i<N; i++)
		{
			lratio[i] = bsc_data[i] == 1 ? (1 - cfg->error_prob) / cfg->error_prob
				: cfg->error_prob / (1 - cfg->error_prob);
		}
		break;
		}
		case AWGN:
		{ for (i = 0; i<N; i++)
		{
			lratio[i] = exp(2 * awn_data[i] / (cfg->std_dev*cfg->std_dev));
		}
		break;
		}
		case AWLN:
		{ for (i = 0; i<N; i++)
		{
			double e, d1, d0;
			e = exp(-(awn_data[i] - 1) / cfg->lwidth);
			d1 = 1 / ((1 + e)*(1 + 1 / e));
			e = exp(-(awn_data[i] + 1) / cfg->lwidth);
			d0 = 1 / ((1 + e)*(1 + 1 / e));
			lratio[i] = d1 / d0;
		}
		break;
		}
		default: assert(false);
		}

		/* Try to decode using the specified method. */

		switch (cfg->dec_method)
		{
		case Prprp:
		{ iters = prprp_decode(cfg, H, lratio, dblk, pchk, bitpr);
		break;
		}
		case Enum_block: case Enum_bit:
		{ iters = enum_decode(cfg, lratio, dblk, bitpr, cfg->dec_method == Enum_block);
		break;
		}
		default: assert(false);
		}

		/* See if it worked, and how many bits were changed. */

		int errb = check(H, dblk, pchk);
		valid = errb == 0;

		chngd = changed(lratio, dblk, N);

		//de-inter data
		for (int j = M; j < N; j++)
		{
			data[j - M] = dblk[codec->cols[j]];
		}

		/* Print summary table entry. */

		if (cfg->table == 1)
		{
			printf("%7d %10f    %d(%d)  %8.1f\n",
				cfg->block_no, (double)iters, valid, errb, (double)chngd);
			/* iters is printed as a double to avoid problems if it's >= 2^31 */
			fflush(stdout);
		}
		return errb;
	}


}
