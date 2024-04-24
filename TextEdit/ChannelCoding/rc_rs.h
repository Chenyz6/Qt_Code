#pragma once
#ifndef RC_RS_H
#define RC_RS_H
#include "rc_codec_base.h"
#include <vector>
#include <string>
namespace RC_DICTCODEC {

	/*!
	* \brief The Galois_domain class ʵ��٤�������ϵļ���
	*/
	class Galois_domain {
	public:
		/*!
		* \brief Galois_domain ��ʼ��٤������
		* \param g   ������ʽ
		* \param m   ��������GF(2^m))
		* \param b07oppo ����ʽȡ��
		*/
		explicit Galois_domain(const char g[/*m+1*/], size_t m, bool b07oppo = false);
		size_t get_startA() const { return startOrder; }
		~Galois_domain() {}
	public:
		void set_gx_oppo(bool oppo) { m_bgxop = oppo; }
		size_t m() const { return _cm; }
		/*!
		* \brief operator () ȡ Alpha ^ p
		* \param p ���� 0Ԫ�ص�����-1
		* \return  ��Ԫ�صĶ�������ʽ
		*/
		unsigned short operator () (int p) const
		{
			if (p<0)
				return 0;
			return Galois_elems[p % (_m - 1)];
		}
		/*!
		* \brief I ��ѯ����
		* \param p ��Ԫ�صĶ�������ʽ
		* \return  ������0Ԫ�ص�����-1
		*/
		int I(size_t p) const
		{
			if (p >= Galois_index.size())
				return -1;
			return Galois_index[p];
		}
		//����Ԫ�ر��±���������ȡֵ����Ԫ�صĶ�����
		const std::vector<unsigned short> & Alpha_to() const { return Galois_elems; }
		//�����������±�����Ԫ�صĶ����ƣ�ȡֵ������
		const std::vector<short> & IndexOf() const { return  Galois_index; }
	public:
		bool fill_galois_elems();	//���٤������
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
		* \brief geneGx �������ڱ���������ϵĶ���ʽ
		* \param startA ��ʼ��,�����������ʽAlpha_to
		* \param Nz ���˸���������Ϊż��������/2���ھ������
		* \return ����ʽ
		*/
		std::vector<unsigned short> geneGx(unsigned short startA, size_t Nz);

		/*!
		* \brief printGF ������Ԫ��
		* \return
		*/
		std::string printGF();
	protected:
		size_t _m;
		size_t _cm;
		size_t startOrder;
		//���޻�Ԫ��
		std::vector<char> _g;
		std::vector<unsigned short> Galois_elems;
		std::vector<short> Galois_index;
		bool m_bgxop;

	};

	/*!
	* \brief The rc_codec_rs class RS���������
	*/
	class rc_codec_rs : public rc_codec_base<unsigned short> {
	public:
		explicit rc_codec_rs(size_t n, size_t k, unsigned short G[], size_t len, const Galois_domain & domain, bool byteOppo = true);
		~rc_codec_rs() override;
		void set_initial_status(unsigned short status[/*n-k*/]);
		void set_Gx_oppo(bool oppo) { m_bGxop = oppo; }
		const std::vector<unsigned short> & get_Gx()const { return _G; }
	public:
		//��������������ʵ�����к���
		/*!
		* \brief encode ������������Ϊ���ݣ�k���ţ����Ϊ���룬n���š�
		* \param data  ����
		* \param code  ���롣
		* \return �Զ��塣
		* \attention data��code��ָ���ַ����Ϊͬһ����ַ������ʵ���������鿼�ǡ�
		*/
		int encode(const unsigned short data[/*k*/], unsigned short code[/*n*/]) override;
		/*!
		* \brief encode ����У�顣����Ϊ���룬n���ţ����ΪУ���ı��룬n���š�
		* \param data  ����
		* \param code  ����У���������ܰ�����Ϣ���֡�
		* \return �Զ��塣
		* \attention code��check��ָ�����Ϊͬһ���ڴ棬��ע����ơ�
		*/
		int chcode(const unsigned short code[/*n*/], unsigned short check[/*n*/]) override;
		/*!
		* \brief hash   ��У�����������ͼ����Ψһ�ַ�����
		* \param check  У���Ľ�������ܰ�����Ϣ���֡�
		* \return ����ͼ���ַ���
		*/
		std::string hash(const unsigned short check[/*n*/]) const override;
		//��������
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
