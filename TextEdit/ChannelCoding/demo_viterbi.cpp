//#include "clsm_viterbi.h"
//#include "conv_lsm_codec.h"
//#include <cstdlib>
//#include <cstdio>
//#include <memory.h>
//using namespace LSMVIT;
//int demo_216();
//int demo_324_soft();
////int demo_frame();
//int demo_21_nolinear();
//
//int demo_conv()
//{
//	demo_216();
//	demo_324_soft();
//	//demo_frame();
//	demo_21_nolinear();
//	return 0;
//}
//int demo_324_soft()
//{
//	/*
//	3 , 2 , 4 反馈码，下面是输出抽头
//	0-[]-[]-[]-[]-+    1-[]-[]-[]-[]-+
//	1  0  1  1  1      0  0  0  0  0
//	1  0  0  1  1      1  1  1  1  1
//	0  0  0  0  0      1  1  0  0  1
//	反馈抽头：
//	0-[]-[]-[]-[]-+    1-[]-[]-[]-[]-+
//	0  1  1  1  1      0  0  0  0  0
//	0  0  0  0  0      0  1  1  1  1
//	*/
//	int pins324[2][3] = { { 27,23,0 },{ 0,37,31 } };
//	int feedbackpin[2][2] = { { 17, 0 },{ 0,17 } };
//	void * codec = new_conv_codec(3, 2, 4, (int *)pins324, (int *)feedbackpin, 64);
//	printf("---------------\n324 feedback soft viterbi dec\n");
//	int inital_status = 0;
//
//
//	int data[1024], code[1024], code_soft[1024][3], recv[1024];
//	//200Kb data, 300KbCode
//	for (int i = 0; i<100; ++i)
//	{
//		for (int j = 0; j<1024; ++j)
//			data[j] = i * 1717 % 4;
//		inital_status = encode(codec, data, 1024, code, inital_status);
//
//		for (int j = 0; j<1024; ++j)
//		{
//			code_soft[j][0] = (code[j] >> 2) ? 128 : -128;
//			code_soft[j][1] = ((code[j] >> 1)) & 0x01 ? 128 : -128;
//			code_soft[j][2] = (code[j] >> 2) ? 128 : -128;
//		}
//
//		for (int i = 0; i<10; ++i)
//			code_soft[rand() % 1024][rand() % 3] = rand() % 256 - 128;
//		decode_soft(codec, (int *)code_soft, 1024 * 3, -128, 128, 0, false);
//		int e = curr_best_hamming(codec);
//
//		int poped = pop_data(codec, recv, 1024);
//		printf("group %03d Ham=%10d, poped %d symbols\n", i + 1, e, poped);
//
//
//	}
//
//	delete_lsm_codec(codec);
//	printf("\n");
//	return 0;
//}
//
//
//int demo_216()
//{
//	int pins[] = { 133,171 };
//	void * codec = new_conv_codec(2, 1, 6, pins, 0, 64);
//	printf("---------------\n216 hard viterbi dec\n");
//	int inital_status = 0;
//
//	int data[1024], code[1024], recv[1024];
//	//100Kb data, 200KbCode
//	for (int i = 0; i<100; ++i)
//	{
//		for (int j = 0; j<1024; ++j)
//			data[j] = i * 1717 % 2;
//		inital_status = encode(codec, data, 1024, code, inital_status);
//		for (int i = 0; i<5; ++i)
//			code[rand() % 1024] ^= rand() % 4;
//		decode(codec, code, 1024, false);
//		unsigned long long e = curr_best_hamming(codec);
//		int poped = pop_data(codec, recv, 1024);
//		printf("group %03d Ham=%7d, poped %d symbols\n", i + 1, e, poped);
//	}
//
//
//	unsigned long long w = worst_hamming(codec);
//	int st = get_initial_status(codec);
//	delete_lsm_codec(codec);
//	printf("\nInitial Status = %d, worst Hamming = %lld\n", w, st);
//	return 0;
//}
//
//
//int main()
//{
//	static const int err_weight[16] = { 0,1,1,2,1,2,2,3,1,2,2,3,2,3,3,4 };
//	int pins[] = { 133,171 };
//	const int L = 63;
//	void * codec = new_conv_codec(2, 1, 6, pins, 0, 64);
//	printf("---------------\n216 hard viterbi dec frame\n");
//	int data[1024 + L], code[1024 + L], errcode[1024 + L], recv[1024 + L];
//	//100Kb data, 200KbCode
//	for (int i = 0; i<100; ++i)
//	{
//		for (int j = 0; j<1024 + L; ++j)
//			data[j] = rand() % 2;
//
//		printf("initial code:\n");
//		for (int j = 0; j < 1024 + L; ++j)
//			printf("%d", data[j]);
//		printf("\n");
//
//		encode(codec, data, 1024 + L, code, 0);
//
//		printf("encoded:\n");
//		for (int j = 0; j < 1024 + L; ++j)
//			printf("%d", code[j]);
//		printf("\n");
//
//		memcpy(errcode, code, sizeof(int)*(1024 + L));
//		for (int i = 0; i<10; ++i)
//			errcode[rand() % 10] ^= rand() % 4;
//
//		printf("erro code:\n");
//		for (int j = 0; j < 1024 + L; ++j)
//			printf("%d", errcode[j]);
//		printf("\n");
//
//		reset_status(codec);
//		decode(codec, errcode, 1024 + L, false);
//
//		printf("decode code:\n");
//		for (int j = 0; j < 1024 + L; ++j)
//			printf("%d", errcode[j]);
//		printf("\n");
//
//		int e = curr_best_hamming(codec);
//		int poped = pop_data(codec, recv, 1024);
//
//		printf("reciving code:\n");
//		for (int j = 0; j < 1024 + L; ++j)
//			printf("%d", recv[j]);
//		printf("\n");
//
//		int eb = 0;
//		for (int i = 0; i<poped; ++i)
//			eb += err_weight[data[i] ^ recv[i]];
//
//		//printf("group %03d Ham=%7d, errBits = %4d poped %d symbols\n", i + 1, e, eb, poped);
//	}
//
//	delete_lsm_codec(codec);
//	printf("\n");
//
//	system("pause");
//	return 0;
//}
//
//int cb_status_noliner(int state, int input, int* next_state, int * out, void * /*nb*/)
//{
//
//	int ret = -1;
//	static const int bit_weight[16] = { 0,1,1,2,1,2,2,3,1,2,2,3,2,3,3,4 };
//
//	*next_state = ((state) * 997 + input) % 64;
//
//	*out = (input << 1) + (bit_weight[state & 0x0F] | bit_weight[(state >> 4) & 0x0F]) % 2;
//
//	printf("%d,%d,%d,%d\n", state, input, *next_state, *out);
//
//	return ret;
//}
//
//int demo_21_nolinear()
//{
//	static const int err_weight[16] = { 0,1,1,2,1,2,2,3,1,2,2,3,2,3,3,4 };
//	const int L = 128;
//	void * codec = new_lsm_codec(64, 2, 2, L);
//	init_status(codec, cb_status_noliner, nullptr);
//	printf("---------------\n216 hard viterbi non-linear frame\n");
//	int data[1024 + L], code[1024 + L], errcode[1024 + L], recv[1024 + L];
//	//100Kb data, 200KbCode
//	for (int i = 0; i<100; ++i)
//	{
//		for (int j = 0; j<1024 + L; ++j)
//			data[j] = rand() % 2;
//		encode(codec, data, 1024 + L, code, 0);
//		for (int j = 0; j<1024 + L; ++j)
//		{
//			//printf ("%d",code[j]);
//		}
//		//printf ("\n");
//		memcpy(errcode, code, sizeof(int)*(1024 + L));
//		for (int i = 0; i<10; ++i)
//			errcode[rand() % (1024 + L)] ^= rand() % 4;
//		reset_status(codec);
//		decode(codec, errcode, 1024 + L, false);
//		int e = curr_best_hamming(codec);
//		int poped = pop_data(codec, recv, 1024);
//		int eb = 0;
//		for (int i = 0; i<poped; ++i)
//			eb += err_weight[data[i] ^ recv[i]];
//
//		printf("group %03d Ham=%7d, errBits = %4d poped %d symbols\n", i + 1, e, eb, poped);
//	}
//
//	delete_lsm_codec(codec);
//	printf("\n");
//	return 0;
//}
//
//
////int main()
////{
////	demo_216();
////
////	system("pause");
////	return 0;
////}