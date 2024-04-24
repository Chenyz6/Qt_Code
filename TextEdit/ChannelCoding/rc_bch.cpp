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
	//��������������ʵ�����к���
	/*!
	* \brief encode ������������Ϊ���ݣ�k���ţ����Ϊ���룬n���š�
	* \param data  ����
	* \param code  ���롣
	* \return �Զ��塣
	* \attention data��code��ָ���ַ����Ϊͬһ����ַ������ʵ���������鿼�ǡ�
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
	* \brief encode ����У�顣����Ϊ���룬n���ţ����ΪУ���ı��룬n���š�
	* \param data  ����
	* \param code  ����У���������ܰ�����Ϣ���֡�
	* \return �Զ��塣
	* \attention code��check��ָ�����Ϊͬһ���ڴ棬��ע����ơ�
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
	* \brief hash   ��У�����������ͼ����Ψһ�ַ�����
	* \param check  У���Ľ�������ܰ�����Ϣ���֡�
	* \return ����ͼ���ַ���
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
