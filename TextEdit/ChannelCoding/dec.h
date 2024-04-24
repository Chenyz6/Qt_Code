#pragma once
#include "mod2sparse.h"
/* DEC.H - Interface to decoding procedures. */

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
#include "rcode.h"
namespace LDPCODEC {

	/* DECODING METHOD, ITS PARAMETERS, AND OTHER VARIABLES.  The global variables
	declared here are located in dec.c. */

	typedef enum
	{
		Enum_block, Enum_bit, Prprp
	} decoding_method;

	typedef enum { BSC, AWGN, AWLN } channel_type;

	struct decfg {
		ldpcodec * codec = nullptr;

		decoding_method dec_method = Prprp;  /* Decoding method to use */

		int table = 0;	/* Trace option, 2 for a table of decoding details */
		int block_no = 0;	/* Number of current block, from zero */

		int max_iter = 32;	/* Maximum number of iteratons of decoding to do */
		const char *gen_file = nullptr;	/* Generator file for Enum_block and Enum_bit */

		channel_type channel = BSC;	/* Type of channel */

		double error_prob = 0;	/* Error probability for BSC */
		double std_dev = 0;		/* Noise standard deviation for AWGN */
		double lwidth = 0;		/* Width of noise distribution for AWLN */

								//memory
		char *	dblk = nullptr;		//用于存储译码后比特，会自动分配。
		double *lratio = nullptr;
		char *	pchk = nullptr;
		double *bitpr = nullptr;

		~decfg()
		{
			printf("free memory.");
			if (dblk) free(dblk);
			if (lratio) free(lratio);
			if (pchk) free(pchk);
			if (bitpr) free(bitpr);
		}


	};



	/* PROCEDURES RELATING TO DECODING METHODS. */

	void enum_decode_setup(decfg * cfg);
	unsigned enum_decode(decfg * cfg, double *, char *, double *, int);

	void prprp_decode_setup(decfg * cfg);
	unsigned prprp_decode
	(decfg * cfg, mod2sparse *, double *, char *, char *, double *);

	void initprp(decfg *, mod2sparse *, double *, char *, double *);
	void iterprp(decfg * cfg, mod2sparse *, double *, char *, double *);

}
