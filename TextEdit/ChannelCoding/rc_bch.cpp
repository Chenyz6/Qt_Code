#include "rc_bch.h"
#include "rc_dict.h"
#include <cassert>
namespace RC_DICTCODEC {
	rc_codec_bch::rc_codec_bch(size_t n, size_t k, const size_t gindex[], size_t len)
		:rc_codec_base<char>(n, k, 2)
		, m_bgxop(true)
	{
		for (size_t i = 0; i<len; ++i)
			_g.push_back(gindex[i]);
		for (size_t i = 0; i<_n - _k; ++i)
			initial_status.push_back(0);
        size_t nproc = 6;
		for (size_t i = 0; i<n*nproc; ++i)
			_rawbuf.push_back(0);
	}
	rc_codec_bch::~rc_codec_bch()
	{

	}
	void rc_codec_bch::set_initial_status(const char status[/*n-k*/])
	{
		initial_status.clear();
		for (size_t i = 0; i<_n - _k; ++i)
			initial_status.push_back(status[i]);
	}
	//所有译码器必须实现下列函数
	/*!
	* \brief encode 编码器。输入为数据，k符号，输出为编码，n符号。
	* \param data  数据
	* \param code  编码。
	* \return 自定义。
	* \attention data和code的指针地址可能为同一个地址，所以实现者请酌情考虑。
	*/
	int rc_codec_bch::encode(const char data[/*k*/], char code[/*n*/])
	{
		const size_t polym = _n - _k;
        char * _buf = _rawbuf.data() + 0*_n;
		for (size_t i = 0; i<_k; ++i)
		{
			_buf[i] = data[i];
			code[i] = data[i];
		}
		for (size_t i = 0; i<polym; ++i)
			_buf[i + _k] = initial_status[i];

		const size_t pln = _g.size();
		const size_t * pli = _g.data();
		for (size_t i = 0; i<_k; ++i)
		{
			if (_buf[i])
			{
				for (size_t j = 0; j<pln; ++j)
				{
					const size_t p = m_bgxop ? (i + (polym - pli[j])) : (i + pli[j]);
					assert(p<_n);
					_buf[p] = _buf[p] ? 0 : 1;
				}
			}
		}
		for (size_t i = 0; i<polym; ++i)
			code[i + _k] = _buf[i + _k];

		//		printf ("Code:");
		//		for (int i = 0;i<_n;++i)
		//			printf ("%d",code[i]);
		//		printf ("\n");
		return 0;
	}
	/*!
	* \brief encode 计算校验。输入为编码，n符号，输出为校验后的编码，n符号。
	* \param data  编码
	* \param code  编码校验结果，可能包含信息部分。
	* \return 自定义。
	* \attention code和check的指针可能为同一个内存，请注意设计。
	*/
	int rc_codec_bch::chcode(const char code[/*n*/], char check[/*n*/])
	{
        char * _buf = _rawbuf.data() + 0*_n;
		const size_t polym = _n - _k;
		for (size_t i = 0; i<_n; ++i)
		{
			_buf[i] = code[i];
			check[i] = code[i];
		}
		//		printf ("Check:");
		//		for (int i = 0;i<_n;++i)
		//			printf ("%d",code[i]);
		//		printf ("\n");
		const size_t pln = _g.size();
		const size_t * pli = _g.data();
		for (size_t i = 0; i<_k; ++i)
		{
			if (_buf[i])
			{
				for (size_t j = 0; j<pln; ++j)
				{
					const size_t p = m_bgxop ? (i + (polym - pli[j])) : (i + pli[j]);
					assert(p<_n);
					_buf[p] = _buf[p] ? 0 : 1;
				}
			}
		}
		for (size_t i = 0; i<polym; ++i)
			check[i + _k] = _buf[i + _k];

		return 0;
	}
	/*!
	* \brief hash   从校验结果计算错误图案的唯一字符串。
	* \param check  校验后的结果，可能包含信息部分。
	* \return 错误图案字符串
	*/
	std::string rc_codec_bch::hash(const char check[/*n*/]) const
	{
		unsigned char mask[] = "0123456789ABCDEFG";
		int c = 0;
		std::string res;
		for (size_t i = 0; i<_n - _k; ++i)
		{
			if (i && (i % 4 == 0))
				res.push_back(mask[c & 0x0F]);
			c <<= 1;
			c += check[_k + i];

		}
		res.push_back(mask[c & 0x0F]);
		return res;
	}

}
