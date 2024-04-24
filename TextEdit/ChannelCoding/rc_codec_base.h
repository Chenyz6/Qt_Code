#ifndef RC_CODEC_BASE_H
#define RC_CODEC_BASE_H
#include <string>
namespace RC_DICTCODEC {
	/*!
	*  多进制循环码的基类
	*/
	template <class IntType>
	class rc_codec_base {
	public:
		explicit rc_codec_base(size_t n, size_t k, size_t m = 2) :_n(n), _k(k), _m(m) {}
		virtual ~rc_codec_base() {}
	public:
		size_t get_n() const { return  _n; }
		size_t get_k() const { return  _k; }
		size_t get_m() const { return  _m; }
	public:
		//所有译码器必须实现下列函数
		/*!
		* \brief encode 编码器。输入为数据，k符号，输出为编码，n符号。
		* \param data  数据
		* \param code  编码。
		* \return 自定义。
		* \attention data和code的指针地址可能为同一个地址，所以实现者请酌情考虑。
		*/
		virtual int encode(const IntType data[/*k*/], IntType code[/*n*/]) = 0;
		/*!
		* \brief encode 计算校验。输入为编码，n符号，输出为校验后的编码，n符号。
		* \param data  编码
		* \param code  编码校验结果，可能包含信息部分。
		* \return 自定义。
		* \attention code和check的指针可能为同一个内存，请注意设计。
		*/
		virtual int chcode(const IntType code[/*n*/], IntType check[/*n*/]) = 0;
		/*!
		* \brief hash   从校验结果计算错误图案的唯一字符串。
		* \param check  校验后的结果，可能包含信息部分。
		* \return 错误图案字符串
		*/
		virtual std::string hash(const IntType check[/*n*/]) const = 0;

		virtual IntType mod_m_add(const IntType & a, const IntType & b) const = 0;

		virtual IntType mod_m_cross(const IntType & a, const IntType & b) const = 0;
	protected:
		size_t _n;//码长（符号）
		size_t _k;//信息长度（符号）
		size_t _m;//域元素总数（包括0元素，如GF(2) 为2, GF(2^8）为256）
	};
}
#endif // RC_CODEC_BASE_H
