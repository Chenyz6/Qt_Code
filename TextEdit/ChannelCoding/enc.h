#pragma once
#include "mod2dense.h"
#include "rcode.h"
/* ENC.H - Interface to encoding procedures. */

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

namespace LDPCODEC {
	void sparse_encode(ldpcodec * codec, const char *, char *);
	void dense_encode(ldpcodec * codec, const char *, char *, mod2dense *, mod2dense *);
	void mixed_encode(ldpcodec * codec, const char *, char *, mod2dense *, mod2dense *);
}
