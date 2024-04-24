#ifndef CONV_LSM_MAKER_H_INCLUDED
#define CONV_LSM_MAKER_H_INCLUDED
namespace LSMVIT {
	void * new_conv_codec(int n, int k, int m, const int * poctPins/*k*n*/, const int * pfbPins/*k*k*/, int L);
}
#endif

