#include "ldpc_gene.h"
#include "ldpc_codec.h"
#include <cassert>
void demo_ldpc_filegen();
void demo_ldpc_read();
void demo_ldpc_code();

int demo_ldpc()
{
	demo_ldpc_filegen();
	demo_ldpc_read();
	demo_ldpc_code();
	return 0;
}

void demo_ldpc_filegen()
{
	using namespace LDPCODEC;
	FILE * fp = fopen("ex-ldpc36-8192a.pchk", "r");
	ldpcodec codec;
	if (!fp)
	{
		printf("\n------LDPC Designing------\n");
		rand_initialize("randfile");

		make_ldpc_file(&codec, "ex-ldpc36-8192a.pchk", 8192, 16384, 2, "evenboth", "3", "no4cycle");
		make_gen_file(&codec, "ex-ldpc36-8192a.pchk", "ex-ldpc36-8192a.gen", "dense");

		//mod2sparse_print(stdout,codec.H);
		//mod2dense_print(stdout,codec.G);

		FILE * falist = fopen("ex-ldpc36-8192a.alist", "w");
		write_alist(&codec, falist);
		fclose(falist);
		falist = fopen("ex-ldpc36-8192a.alist", "r");
		read_alist(&codec, falist);
		fclose(falist);
	}
	else {
		fclose(fp);
	}

	printf("\n------finished------\n");
}

void demo_ldpc_read()
{
	using namespace LDPCODEC;
	//reading alist
	ldpcodec codec;
	FILE * falist = fopen("ex-ldpc36-8192a.alist", "r");
	read_alist(&codec, falist);
	fclose(falist);
	//convert to check
	write_ldpc_file(&codec, "ex-ldpc36-8192a.read.pchk");

}

void demo_ldpc_code()
{
	using namespace LDPCODEC;
	printf("\n------LDPC coding------\n");
	ldpcodec codec;
	read_pchk(&codec, "ex-ldpc36-8192a.read.pchk");
	assert(codec.N>codec.M);
	read_gen(&codec, "ex-ldpc36-8192a.gen", 0, 0);

	char* data = new char[codec.N - codec.M],
		*code = new char[codec.N],
		*recv = new char[codec.N],
		*check = new char[codec.M],
		*dedat = new char[codec.N - codec.M];

	decfg cfg;
	cfg.codec = &codec;
	cfg.table = 0;//No output
	cfg.channel = BSC;
	cfg.error_prob = 0.2;


	for (int i = 0; i<1000; ++i)
	{
		for (int j = 0; j<codec.N - codec.M; ++j)
			data[j] = rand() % 2;
		ldpc_encode(&codec, data, code, check, true);

		for (int j = 0; j<codec.N; ++j)
			recv[j] = code[j];
		for (int j = 0; j<500; ++j)
			recv[rand() % 10000] = 1 - recv[rand() % 10000];

		int errb1 = 0;
		for (int i = 0; i<codec.N; ++i)
		{
			errb1 += recv[i] == code[i] ? 0 : 1;
		}

		int ret = ldpc_decode(&cfg, recv, nullptr, dedat);

		int errb2 = 0;
		for (int i = 0; i<codec.N - codec.M; ++i)
		{
			errb2 += dedat[i] == data[i] ? 0 : 1;
		}

		printf("Frame %i ret %d, errbits %d->%d\n", i, ret, errb1, errb2);
	}
	delete[]data;
	delete[]code;
	delete[]recv;
	delete[]check;
	delete[]dedat;

	printf("\n------finished------\n");

}
//int main()
//{
//
//	system("pause");
//	return 0;
//}
