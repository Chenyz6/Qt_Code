#pragma once
#ifndef RC_RS_H
#define RC_RS_H
#include "rc_codec_base.h"
#include <vector>
#include <string>
namespace RC_DICTCODEC {

	/*!
	* \brief The Galois_domain class 实现伽罗瓦域上的计算
	*/
	class Galois_domain {
	public:
		/*!
		* \brief Galois_domain 初始化伽罗瓦域
		* \param g   域生成式
		* \param m   域幂数（GF(2^m))
		* \param b07oppo 生成式取反
		*/
		explicit Galois_domain(const char g[/*m+1*/], size_t m, bool b07oppo = false);
		size_t get_startA() const { return startOrder; }
		~Galois_domain() {}
	public:
		void set_gx_oppo(bool oppo) { m_bgxop = oppo; }
		size_t m() const { return _cm; }
		/*!
		* \brief operator () 取 Alpha ^ p
		* \param p 幂数 0元素的幂是-1
		* \return  域元素的二进制形式
		*/
		unsigned short operator () (int p) const
		{
			if (p<0)
				return 0;
			return Galois_elems[p % (_m - 1)];
		}
		/*!
		* \brief I 查询幂数
		* \param p 域元素的二进制形式
		* \return  幂数，0元素的幂是-1
		*/
		int I(size_t p) const
		{
			if (p >= Galois_index.size())
				return -1;
			return Galois_index[p];
		}
		//返回元素表，下标是幂数，取值是域元素的二进制
		const std::vector<unsigned short> & Alpha_to() const { return Galois_elems; }
		//返回幂数表，下标是域元素的二进制，取值是幂数
		const std::vector<short> & IndexOf() const { return  Galois_index; }
	public:
		bool fill_galois_elems();	//填充伽罗瓦域
	public:
		unsigned short mod_m_add(const unsigned short & a, const unsigned short & b) const { return a ^ b; }
		unsigned short mod_m_cross(const unsigned short & a, const unsigned short & b) const {
			if (a == 0 || b == 0)
				return 0;
			const size_t v = (Galois_index[a] + Galois_index[b]) % (_m - 1);
			return Galois_elems[v];
		}
		unsigned short mod_m_minus(const unsigned short & a, const unsigned short & b) const { return a ^ b; }
		unsigned short mod_m_div(const unsigned short & a, const unsigned short & b) const {
			if (a == 0 || b == 0)
				return 0;
			int v = Galois_index[a] - Galois_index[b];
			if (v>0)
				v %= (_m - 1);
			else
			{
				v = -v;
				v %= (_m - 1);
				v = int(_m - 1) - v;
			}
			return Galois_elems[static_cast<size_t>(v)];
		}
		unsigned short mod_m_pow(const unsigned short & a, const int & b) const {
			if (a == 0)
				return 0;
			if (b == 0)
				return 1;
			int v = Galois_index[a] * b;
			v %= (_m - 1);
			return Galois_elems[static_cast<size_t>(v)];
		}
	public:
		/*!
		* \brief geneGx 生成用于编译码的域上的多项式
		* \param startA 初始根,输入二进制形式Alpha_to
		* \param Nz 连乘个数，必须为偶数，个数/2等于纠错个数
		* \return 多项式
		*/
		std::vector<unsigned short> geneGx(unsigned short startA, size_t Nz);

		/*!
		* \brief printGF 输出域的元素
		* \return
		*/
		std::string printGF();
	protected:
		size_t _m;
		size_t _cm;
		size_t startOrder;
		//加罗华元素
		std::vector<char> _g;
		std::vector<unsigned short> Galois_elems;
		std::vector<short> Galois_index;
		bool m_bgxop;

	};

	/*!
	* \brief The rc_codec_rs class RS码编译码器
	*/
	class rc_codec_rs : public rc_codec_base<unsigned short> {
	public:
		explicit rc_codec_rs(size_t n, size_t k, unsigned short G[], size_t len, const Galois_domain & domain, bool byteOppo = true);
		~rc_codec_rs() override;
		void set_initial_status(unsigned short status[/*n-k*/]);
		void set_Gx_oppo(bool oppo) { m_bGxop = oppo; }
		const std::vector<unsigned short> & get_Gx()const { return _G; }
	public:
		//所有译码器必须实现下列函数
		/*!
		* \brief encode 编码器。输入为数据，k符号，输出为编码，n符号。
		* \param data  数据
		* \param code  编码。
		* \return 自定义。
		* \attention data和code的指针地址可能为同一个地址，所以实现者请酌情考虑。
		*/
		int encode(const unsigned short data[/*k*/], unsigned short code[/*n*/]) override;
		/*!
		* \brief encode 计算校验。输入为编码，n符号，输出为校验后的编码，n符号。
		* \param data  编码
		* \param code  编码校验结果，可能包含信息部分。
		* \return 自定义。
		* \attention code和check的指针可能为同一个内存，请注意设计。
		*/
		int chcode(const unsigned short code[/*n*/], unsigned short check[/*n*/]) override;
		/*!
		* \brief hash   从校验结果计算错误图案的唯一字符串。
		* \param check  校验后的结果，可能包含信息部分。
		* \return 错误图案字符串
		*/
		std::string hash(const unsigned short check[/*n*/]) const override;
		//迭代译码
		int decode_rs(const unsigned short code[/*n*/], unsigned short correct[/*n*/]);
		unsigned short mod_m_add(const unsigned short & a, const unsigned short & b) const override { return _d.mod_m_add(a, b); }
		unsigned short mod_m_cross(const unsigned short & a, const unsigned short & b) const override { return _d.mod_m_cross(a, b); }

	protected:
		const Galois_domain & _d;
		std::vector<unsigned short> _G;
		std::vector<unsigned short> initial_status;
		std::vector<unsigned short> _rawbuf;
		bool m_bGxop;
		size_t _cm;
		size_t _t;
	private:
		std::vector<int> buf_elp;
		std::vector<int> buf_d;
		std::vector<int> buf_l;
		std::vector<int> buf_u_lu;
		std::vector<int> buf_s;
		std::vector<int> buf_root;
		std::vector<int> buf_loc;
		std::vector<int> buf_z;
		std::vector<int> buf_err;
		std::vector<int> buf_reg;
		std::vector<int> buf_recd;
	};
}
#endif // RC_RS_H
