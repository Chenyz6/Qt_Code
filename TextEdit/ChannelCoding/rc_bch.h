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
		//��������������ʵ�����к���
		/*!
		* \brief encode ������������Ϊ���ݣ�k���ţ����Ϊ���룬n���š�
		* \param data  ����
		* \param code  ���롣
		* \return �Զ��塣
		* \attention data��code��ָ���ַ����Ϊͬһ����ַ������ʵ���������鿼�ǡ�
		*/
		int encode(const char data[/*k*/], char code[/*n*/]) override;
		/*!
		* \brief encode ����У�顣����Ϊ���룬n���ţ����ΪУ���ı��룬n���š�
		* \param data  ����
		* \param code  ����У���������ܰ�����Ϣ���֡�
		* \return �Զ��塣
		* \attention code��check��ָ�����Ϊͬһ���ڴ棬��ע����ơ�
		*/
		int chcode(const char code[/*n*/], char check[/*n*/]) override;
		/*!
		* \brief hash   ��У�����������ͼ����Ψһ�ַ�����
		* \param check  У���Ľ�������ܰ�����Ϣ���֡�
		* \return ����ͼ���ַ���
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
