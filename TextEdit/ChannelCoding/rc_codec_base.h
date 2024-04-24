#ifndef RC_CODEC_BASE_H
#define RC_CODEC_BASE_H
#include <string>
namespace RC_DICTCODEC {
	/*!
	*  �����ѭ����Ļ���
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
		//��������������ʵ�����к���
		/*!
		* \brief encode ������������Ϊ���ݣ�k���ţ����Ϊ���룬n���š�
		* \param data  ����
		* \param code  ���롣
		* \return �Զ��塣
		* \attention data��code��ָ���ַ����Ϊͬһ����ַ������ʵ���������鿼�ǡ�
		*/
		virtual int encode(const IntType data[/*k*/], IntType code[/*n*/]) = 0;
		/*!
		* \brief encode ����У�顣����Ϊ���룬n���ţ����ΪУ���ı��룬n���š�
		* \param data  ����
		* \param code  ����У���������ܰ�����Ϣ���֡�
		* \return �Զ��塣
		* \attention code��check��ָ�����Ϊͬһ���ڴ棬��ע����ơ�
		*/
		virtual int chcode(const IntType code[/*n*/], IntType check[/*n*/]) = 0;
		/*!
		* \brief hash   ��У�����������ͼ����Ψһ�ַ�����
		* \param check  У���Ľ�������ܰ�����Ϣ���֡�
		* \return ����ͼ���ַ���
		*/
		virtual std::string hash(const IntType check[/*n*/]) const = 0;

		virtual IntType mod_m_add(const IntType & a, const IntType & b) const = 0;

		virtual IntType mod_m_cross(const IntType & a, const IntType & b) const = 0;
	protected:
		size_t _n;//�볤�����ţ�
		size_t _k;//��Ϣ���ȣ����ţ�
		size_t _m;//��Ԫ������������0Ԫ�أ���GF(2) Ϊ2, GF(2^8��Ϊ256��
	};
}
#endif // RC_CODEC_BASE_H
