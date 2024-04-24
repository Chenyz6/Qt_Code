//#include "rc_bch.h"
//#include "rc_rs.h"
//#include "rc_dict.h"
//using namespace  RC_DICTCODEC;
//int demo_bch();
//int demo_gf();
////int demo_rs_15();
////int demo_rs_15_dec();
//int demo_rs_255();
//int demo_rc()
//{
//	demo_bch();
//	demo_gf();
//	//demo_rs_15();
//	//demo_rs_15_dec();
//	demo_rs_255();
//	return 0;
//}
//
//int demo_bch()
//{
//	
//	using std::vector;
//	using std::string;
//	size_t pins[7] = { 10,8,5,4,2,1,0 };
//	rc_codec_bch codec(15, 5, pins, 7);
//	rc_dict<char> dict;
//	printf("---------------------\nBCH:\n");
//	size_t s = dict.init(&codec);
//	printf("Dict size = %u\n", s);
//
//	char code[15] = { 1,1,0,0,1,0,0,0,0,0,0,0,0,0,0 };
//
//	codec.encode(code, code);
//
//	printf("raw code=");
//	for (int i = 0; i<15; ++i)
//		printf("%d", code[i]);
//	printf("\n");
//
//	code[3] = 1 - code[3];
//	code[7] = 1 - code[7];
//
//	printf("err code=");
//	for (int i = 0; i<15; ++i)
//		printf("%d", code[i]);
//	printf("\n");
//
//	char check[15] = { 0 };
//	codec.chcode(code, check);
//	printf("chk code=");
//	for (int i = 0; i<15; ++i)
//		printf("%d", check[i]);
//	printf("\n");
//
//	string errk = codec.hash(check);
//	printf("err mask=");
//	puts(errk.c_str());
//
//	vector<rc_dict<char>::err_mask > err = dict.err_find(errk);
//
//	printf("errSize=%d\n", err.size());
//	for (size_t i = 0; i<err.size(); ++i)
//	{
//		printf("Pos = %d, Add = %d\n", err[i].index, err[i].err);
//		if (err[i].index >= 0 && err[i].index<15)
//			code[err[i].index] = codec.mod_m_add(code[err[i].index], err[i].err);
//	}
//	printf("ok  code=");
//	for (int i = 0; i<15; ++i)
//		printf("%d", code[i]);
//	printf("\n");
//
//	codec.chcode(code, check);
//	printf("chk code=");
//	for (int i = 0; i<15; ++i)
//		printf("%d", check[i]);
//	printf("\n");
//	return 0;
//}
//int demo_gf()
//{
//	
//	//g是域的元素多项试子
//	const int mv = 4;
//	char g[mv + 1] = { 1,0,0,1,1 };
//
//	Galois_domain GFm(g, mv);
//
//	puts(GFm.printGF().c_str());
//
//	printf("a + a2 = a%d\n", GFm.I(GFm.mod_m_add(GFm(1), GFm(2))));
//	printf("a * a2 = a%d\n", GFm.I(GFm.mod_m_cross(GFm(1), GFm(2))));
//	printf("a / a2 = a%d\n", GFm.I(GFm.mod_m_div(GFm(1), GFm(2))));
//	printf("a ^ 37= a%d\n", GFm.I(GFm.mod_m_pow(GFm(1), 37)));
//
//	return 0;
//}
//int demo_rs_15()
//{
//	
//	using std::vector;
//	using std::string;
//	//g是域的元素多项试子
//	const int mv = 4;
//	char g[mv + 1] = { 1,0,0,1,1 };
//	Galois_domain A(g, mv);
//	//G是GF(2^m)上的生成式
//	std::vector<unsigned short> Gx = A.geneGx(A(1), 2);
//	for (int i = 0; i < 15; i++)
//		printf("A(%d) = %d\n", i, A(i));
//	const int n = (1 << mv) - 1;
//	const int nl = (Gx.size() - 1);
//	const int k = n - nl;
//
//	printf("---------------------\n(%d,%d)RS Dict Decoding:\n", n, k);
//	printf("a2 + a5 = a%d\n", A.I(A.mod_m_add(A(2), A(5))));
//	printf("a2 * a5 = a%d\n", A.I(A.mod_m_cross(A(2), A(5))));
//	printf("a2 / a5 = a%d\n", A.I(A.mod_m_div(A(2), A(5))));
//	printf("a2 ^ 120= a%d\n", A.I(A.mod_m_pow(A(2), 120)));
//
//	printf("Gx=");
//	for (size_t i = 0; i<Gx.size(); ++i)
//		printf("%1X", Gx[i]);
//	printf("\n");
//
//	//int G[4] = {1,0,A(4),A(3)};
//	rc_codec_rs codec(n, k, Gx.data(), Gx.size(), A);
//	rc_dict<unsigned short> dict(1024 * 1024);
//	size_t s = dict.init(&codec);
//	printf("Dict size = %d\n", s);
//
//	unsigned short code[n] = { 0 };
//	for (int i = 0; i<n; ++i)
//		code[i] = (i<k) ? rand() % (n + 1) : 0;
//
//	codec.encode(code, code);
//
//	printf("raw code=");
//	for (int i = 0; i<n; ++i)
//		printf("%1X", code[i]);
//	printf("\n");
//
//	code[3] ^= 0x05;
//
//	printf("err code=");
//	for (int i = 0; i<n; ++i)
//		printf("%1X", code[i]);
//	printf("\n");
//
//	unsigned short check[n] = { 0 };
//	codec.chcode(code, check);
//	printf("chk code=");
//	for (int i = 0; i<n; ++i)
//		printf("%1X", check[i]);
//	printf("\n");
//
//	string errk = codec.hash(check);
//	printf("err mask=");
//	puts(errk.c_str());
//
//	vector<rc_dict<unsigned short>::err_mask > err = dict.err_find(errk);
//
//	printf("errSize=%d\n", err.size());
//	for (size_t i = 0; i<err.size(); ++i)
//	{
//		printf("Pos = %d, Add = %X\n", err[i].index, err[i].err);
//		if (err[i].index >= 0 && err[i].index<7)
//			code[err[i].index] = codec.mod_m_add(code[err[i].index], err[i].err);
//	}
//	printf("ok  code=");
//	for (size_t i = 0; i<n; ++i)
//		printf("%1X", code[i]);
//	printf("\n");
//
//	codec.chcode(code, check);
//	printf("chk code=");
//	for (size_t i = 0; i<n; ++i)
//		printf("%1X", check[i]);
//	printf("\n");
//
//	system("pause");
//	return 0;
//}
//
//
//int demo_rs_255()
//{
//	
//	using std::vector;
//	using std::string;
//	//g是域的元素多项试子
//	char g[] = { 1,0,0,0,1,1,1,0,1 };
//	Galois_domain A(g, 8);
//	//G是GF(2^m)上的生成式
//	const int t = 63;
//	std::vector<unsigned short> Gx = A.geneGx(A(77), 2 * t);
//
//	const int n = 255;
//	const int lm = (Gx.size() - 1);
//	const int k = n - lm;
//	printf("---------------------\n(%d,%d)RS iter decoding:\n", n, k);
//	printf("a2 + a5 = a%d\n", A.I(A.mod_m_add(A(2), A(5))));
//	printf("a2 * a5 = a%d\n", A.I(A.mod_m_cross(A(2), A(5))));
//	printf("a2 / a5 = a%d\n", A.I(A.mod_m_div(A(2), A(5))));
//	printf("a2 ^ 120= a%d\n", A.I(A.mod_m_pow(A(2), 120)));
//	printf("Gx=");
//	for (size_t i = 0; i<Gx.size(); ++i)
//		printf("%02X", Gx[i]);
//	printf("\n");
//
//	rc_codec_rs codec(n, k, Gx.data(), Gx.size(), A);
//	unsigned short rawcode[n] = { 0 };
//	unsigned short code[n] = { 0 };
//	for (int i = 0; i<n; ++i)
//		rawcode[i] = (i<k) ? rand() % 256 : 0;
//
//	codec.encode(rawcode, rawcode);
//
//	printf("raw code=\n");
//	for (int i = 0; i<n; ++i)
//		printf("%02X", code[i] = rawcode[i]);
//	printf("\n");
//
//	for (int i = 0; i<t; ++i)
//		code[rand() % n] ^= rand() % (n + 1);
//
//	printf("err code=\n");
//	for (int i = 0; i<n; ++i)
//		printf("%02X", code[i]);
//	printf("\n");
//
//	unsigned short check[n] = { 0 };
//	codec.chcode(code, check);
//	printf("chk code=\n");
//	for (int i = 0; i<n; ++i)
//		printf("%02X", check[i]);
//	printf("\n");
//
//	unsigned short decoded[n] = { 0 };
//	codec.decode_rs(code, decoded);
//
//	printf("decoded=\n");
//	for (size_t i = 0; i<n; ++i)
//		printf("%02X", decoded[i]);
//	printf("\n");
//
//	printf("rawcode[i] ^ decoded[i]=\n");
//	for (size_t i = 0; i<n; ++i)
//		printf("%02X", rawcode[i] ^ decoded[i]);
//	printf("\n");
//
//	printf("code[i] ^ decoded[i]=\n");
//	for (size_t i = 0; i<n; ++i)
//		printf("%02X", code[i] ^ decoded[i]);
//	printf("\n");
//
//	codec.chcode(decoded, check);
//	printf("chk code=\n");
//	for (size_t i = 0; i<n; ++i)
//		printf("%02X", check[i]);
//	printf("\n");
//
//	system("pause");
//	return 0;
//}
//
//
//int main()
//{
//	
//	using std::vector;
//	using std::string;
//	//g是域的元素多项试子
//	char g[] = { 1,1,0,0,1 };
//	Galois_domain A(g, 4);
//	//G是GF(2^m)上的生成式
//	std::vector<unsigned short> Gx = A.geneGx(A(1), 6);
//	for (int i = 0; i < 15; i++)
//		printf("A(%d) = %d\n", i, A(i));
//	const int n = 15;
//	const int lm = (Gx.size() - 1);
//	const int k = n - lm;
//	printf("---------------------\n(%d,%d)RS iter decoding:\n", n, k);
//	printf("a2 + a5 = a%d\n", A.I(A.mod_m_add(A(2), A(5))));
//	printf("a2 * a5 = a%d\n", A.I(A.mod_m_cross(A(2), A(5))));
//	printf("a2 / a5 = a%d\n", A.I(A.mod_m_div(A(2), A(5))));
//	printf("a2 ^ 120= a%d\n", A.I(A.mod_m_pow(A(2), 120)));
//	printf("Gx=");
//	for (size_t i = 0; i<Gx.size(); ++i)
//		printf("%1X", Gx[i]);
//	printf("\n");
//
//	rc_codec_rs codec(n, k, Gx.data(), Gx.size(), A);
//	unsigned short code[n] = { 9,9,15,4,2,1,8,6,8,0,0,0,0,0,0 };
//	//unsigned short code[n] = {8,6,8,1,2,4,15,9,9,0,0,0,0,0,0};
//
//
//	codec.encode(code, code);
//
//	printf("raw code=");
//	for (int i = 0; i<n; ++i)
//		printf("%1X", code[i]);
//	printf("\n");
//
//	code[0] = 3;
//	code[5] ^= 0x02;
//	code[6] ^= 0x0f;
//	printf("err code=");
//	for (int i = 0; i<n; ++i)
//		printf("%1X", code[i]);
//	printf("\n");
//
//	unsigned short decoded[n] = { 0 };
//	codec.decode_rs(code, decoded);
//
//	printf("ok  code=");
//	for (size_t i = 0; i<n; ++i)
//		printf("%1X", decoded[i]);
//	printf("\n");
//	printf("err Mask=");
//	for (size_t i = 0; i<n; ++i)
//		printf("%1X", code[i] ^ decoded[i]);
//	printf("\n");
//
//	unsigned short check[n] = { 0 };
//
//	codec.chcode(decoded, check);
//	printf("chk code=");
//	for (size_t i = 0; i<n; ++i)
//		printf("%1X", check[i]);
//	printf("\n");
//
//	system("pause");
//	return 0;
//}
////
////int main()
////{
////	demo_bch();
////	//demo_rs_15();
////
////	system("pause");
////	return 0;
////}