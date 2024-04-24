#ifndef RC_BCH_H
#define RC_BCH_H
#include "rc_codec_base.h"
#include <vector>
namespace RC_DICTCODEC {
	class rc_codec_bch : public rc_codec_base<char> {
	public:
		explicit rc_codec_bch(size_t n, size_t k, const size_t gindex[], size_t len);
		~rc_codec_bch() override;
		void set_initial_status(const char status[/*n-k*/]);
		void set_gx_oppo(bool oppo) { m_bgxop = oppo; }
	public:
		//所有译码器必须实现下列函数
		/*!
		* \brief encode 编码器。输入为数据，k符号，输出为编码，n符号。
		* \param data  数据
		* \param code  编码。
		* \return 自定义。
		* \attention data和code的指针地址可能为同一个地址，所以实现者请酌情考虑。
		*/
		int encode(const char data[/*k*/], char code[/*n*/]) override;
		/*!
		* \brief encode 计算校验。输入为编码，n符号，输出为校验后的编码，n符号。
		* \param data  编码
		* \param code  编码校验结果，可能包含信息部分。
		* \return 自定义。
		* \attention code和check的指针可能为同一个内存，请注意设计。
		*/
		int chcode(const char code[/*n*/], char check[/*n*/]) override;
		/*!
		* \brief hash   从校验结果计算错误图案的唯一字符串。
		* \param check  校验后的结果，可能包含信息部分。
		* \return 错误图案字符串
		*/
		std::string hash(const char check[/*n*/]) const override;
		char mod_m_add(const char & a, const char & b) const override { return (a ^ b) ? 1 : 0; }

		char mod_m_cross(const char & a, const char & b) const override { return (a & b) ? 1 : 0; }
	protected:
		std::vector<size_t> _g;
		std::vector<char> initial_status;
		std::vector<char> _rawbuf;
		bool m_bgxop;
	};

}

#endif // RC_BCH_H
