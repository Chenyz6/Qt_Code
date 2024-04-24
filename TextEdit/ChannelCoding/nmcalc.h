#pragma once
/*!
* ͨ����������࣬�������ɸ����������������
*/
#include <vector>
#include <assert.h>
#include <functional>
namespace NM_CALC {
	struct tag_NM_State
	{
		std::vector<unsigned short> vec_buf;
		std::vector<unsigned short> vec_bz;
		int swim;
		bool finished;
	};
	struct tag_FOR_State
	{
		std::vector<unsigned long long> vec_buf;
		std::vector<unsigned long long> vec_bz;
		int swim;
		bool finished;
	};
	//����for�㷨,m�飬ÿ��for��ģΪn
	int fnm(const unsigned long long n[/*m*/], int m, std::vector<std::vector <unsigned long long> > & vec_output, tag_FOR_State * state, int limit = 0);
	//Ԥ����vec_output�����㷨
	int fnm_next(const unsigned long long n[/*m*/], int m, std::vector<std::vector <unsigned long long> > & vec_output, tag_FOR_State * state, int limit = 0);
	unsigned long long fnm_total(const int n[/*m*/], int m);

	//���������㷨
	int pnm(int n, int m, std::vector<std::vector <unsigned short> > & vec_output, tag_NM_State * state, int limit = 0);
	//Ԥ����vec_output�����㷨
	int pnm_next(int n, int m, std::vector<std::vector <unsigned short> > & vec_output, tag_NM_State * state, int limit = 0);
	//��ģ
	unsigned long long pnm_total(int n, int m);


	//��������㷨
	int cnm(int n, int m, std::vector<std::vector <unsigned short> > & vec_output, tag_NM_State * state, int limit = 0);
	//Ԥ����vec_output����㷨
	int cnm_next(int n, int m, std::vector<std::vector <unsigned short> > & vec_output, tag_NM_State * state, int limit = 0);
	//��ģ
	unsigned long long cnm_total(int n, int m);

	//���鴮����������࣬��ѡ��ÿ����õĲ���

	/*! �������������
	* ������ָ�����������ÿ����ԣ�P,C���͹�ģ
	*/
	template <class T>
	class nmserial
	{
	public:
		enum enum_function {
			func_PNM = 0,
			func_CNM = 1
		};

	public:
		std::vector<std::vector< T > > vec_groups;
		std::vector<int> group_m;
		std::vector<enum_function> group_type;//0:PNM, 1:CNM, 2: FNM
	public:
		nmserial()
			:batch_deal(65536), group_deal(4096),
			serial_count(0), totaltask(1), m_totalout(0),
			nFNm(0)
		{}
		void set_batch_deal(int d) { batch_deal = d; }		//����ÿ��������෵�ض���
		void set_group_deal(int d) { group_deal = d; }		//����ÿ����������У����ڼ�����ٽ����
		int  get_batch_deal() const { return batch_deal; }
		int  get_group_deal() const { return group_deal; }
		//ȡ���ܵļ����ģ
		unsigned long long total_task() const { return totaltask; }
		//����ж���Ԫ�أ���ÿ��m�ĺ�
		int total_out() const { return m_totalout; }
	private:
		std::vector<std::function<unsigned long long(const int n, int m)> > vec_nmfunc_total;
		std::vector<std::function<int(int n, int m, std::vector<std::vector <unsigned short> > & vec_output, tag_NM_State * state, int limit)> > vec_nmfunc_initial;
		std::vector<std::function<int(int n, int m, std::vector<std::vector <unsigned short> > & vec_output, tag_NM_State * state, int limit)> > vec_nmfunc_next;
		int batch_deal;
		int group_deal;
		int serial_count;
		std::vector<int> vec_chars_n;
		std::vector<unsigned long long> vec_chars_pnm;
		std::vector<int> vec_char_out_group;
		std::vector<int> vec_char_out_offset;
		unsigned long long totaltask;
		int m_totalout;
		//fnm
		tag_FOR_State g_state;
		std::vector<std::vector < unsigned long long> > vec_fnm;
		int nFNm;
		//����
		std::vector<std::vector<std::vector <unsigned short> > > vec_states_pnm;
		std::vector<tag_NM_State> vec_states;
		std::vector<unsigned long long> vec_states_gp;
		std::vector<int> vec_states_size;
	public:
		/*!
		* \brief serial_nm_begin ��ʼ��һ������
		* \param vec_results ���ڴ���������ľ���
		* \return �ܹ�ģ
		*/
		unsigned long long serial_nm_begin(
			std::vector<std::vector<T> > & vec_results);
		/*!
		* \brief serial_nm_next �����һ��������
		* \param vec_results ���ڴ���������ľ���
		* \return  ������Ч�Ľ������
		*/
		int serial_nm_next(
			std::vector<std::vector<T> > & vec_results);

	};


	template <class T>
	unsigned long long nmserial<T>::serial_nm_begin(
		std::vector<std::vector<T> > & vec_results)
	{
		assert(this->serial_count == 0);
		assert(group_m.size() == vec_groups.size() && group_type.size() == group_m.size());
		assert(group_m.size()>0);
		//����
		this->serial_count = this->vec_groups.size();
		for (int i = 0; i<this->serial_count; ++i)
		{
			const int n = this->vec_groups[i].size();
			const int m = this->group_m[i];
			assert(m <= n);
			switch (group_type[i])
			{
			case func_PNM:
				vec_nmfunc_total.push_back(pnm_total);
				vec_nmfunc_initial.push_back(pnm);
				vec_nmfunc_next.push_back(pnm_next);
				break;
			case func_CNM:
				vec_nmfunc_total.push_back(cnm_total);
				vec_nmfunc_initial.push_back(cnm);
				vec_nmfunc_next.push_back(cnm_next);
				break;
			default:
				assert(false);
			}

			this->vec_chars_n.push_back(n);
			this->vec_chars_pnm.push_back(vec_nmfunc_total[i](n, m));
			this->totaltask *= this->vec_chars_pnm[i];
			for (int j = 0; j < m; j++)
			{
				this->vec_char_out_group.push_back(i);
				this->vec_char_out_offset.push_back(j);
			}
			this->m_totalout += m;

		}

		//��ʼ���д�������
		const int * pFNs = this->vec_chars_n.data();
		const int * pFMs = this->group_m.data();
		const unsigned long long * pPNMs = this->vec_chars_pnm.data();
		//fnm
		fnm(pPNMs, this->serial_count, this->vec_fnm, &this->g_state, this->batch_deal);
		this->nFNm = this->vec_fnm.size();

		//����
		for (int i = 0; i<this->serial_count; ++i)
		{
			this->vec_states_pnm.push_back(std::vector<std::vector <unsigned short> >());
			this->vec_states.push_back(tag_NM_State());
			this->vec_states_gp.push_back(0);
			vec_nmfunc_initial[i](pFNs[i], pFMs[i], this->vec_states_pnm[i], &this->vec_states[i], this->group_deal);
			this->vec_states_size.push_back(this->vec_states_pnm[i].size());
		}
		//���ս��
		if (this->nFNm)
		{
			for (int group = 0; group < this->nFNm; ++group)
			{
				std::vector<T> vec_result;
				vec_result.resize(this->m_totalout);
				//����˳����дvec_result
				bool bad = false;
				for (int ci = 0; ci<this->m_totalout && bad == false; ++ci)
				{
					//�ڼ���,�� vec_states_pnm�ĵڼ���
					const int serial_group = this->vec_char_out_group[ci];
					//�жϱ����ǲ�����Ҫ������һ������
					const unsigned long long npnmCt = this->vec_fnm[group][serial_group];
					const unsigned long long npnmGp = npnmCt / this->group_deal;
					//���Ҫ���ʵ�ǰ״̬��֮ǰ����ϣ�������PNM
					if (npnmGp < this->vec_states_gp[serial_group])
					{
						this->vec_states_pnm[serial_group] = (std::vector<std::vector <unsigned short> >());
						this->vec_states[serial_group] = tag_NM_State();
						this->vec_states_gp[serial_group] = 0;
						vec_nmfunc_initial[serial_group](pFNs[serial_group], pFMs[serial_group],
							this->vec_states_pnm[serial_group], &this->vec_states[serial_group], this->group_deal);
						this->vec_states_size[serial_group] = this->vec_states_pnm[serial_group].size();
					}//end if (npnmGp < vec_states_gp[serial_group])
					 //׷�Ͻ���
					while (npnmGp > this->vec_states_gp[serial_group] && bad == false)
					{
						++this->vec_states_gp[serial_group];
						this->vec_states_size[serial_group] = vec_nmfunc_next[serial_group](
							pFNs[serial_group],
							pFMs[serial_group],
							this->vec_states_pnm[serial_group],
							&this->vec_states[serial_group], this->group_deal);
						if (this->vec_states_size[serial_group] == 0)
							bad = true;
					}//end while (npnmGp > vec_states_gp[serial_group]&&bad==false)
					if (bad)
						continue;
					//���㱾�������������±�
					const int curPNMIdx = npnmCt % this->group_deal;
					if (curPNMIdx >= this->vec_states_size[serial_group])
					{
						bad = true;
						continue;
					}
					//ȡ������λ��
					const int sfidx = this->vec_states_pnm[serial_group][curPNMIdx][this->vec_char_out_offset[ci]];
					vec_result[ci] = this->vec_groups[serial_group][sfidx];
				}//end for (int ci=0;ci<total_out && bad==false;++ci)
				if (bad)
					continue;
				//������һ��PNM
				vec_results.push_back(std::move(vec_result));
			}//end for (int group = 0; group < nFNm; ++group)
		}//end while (nFNm)
		return this->totaltask;
	}

	template <class T>
	int nmserial<T>::serial_nm_next(
		std::vector<std::vector<T> > & vec_results)
	{
		if (this->serial_count == 0)
			return 0;
		//��ʼ���д�������
		const int * pFNs = this->vec_chars_n.data();
		const int * pFMs = this->group_m.data();
		const unsigned long long * pPNMs = this->vec_chars_pnm.data();
		//���ս��
		//��һ��
		this->nFNm = fnm_next(pPNMs, this->serial_count, this->vec_fnm, &this->g_state, this->batch_deal);
		if (this->nFNm)
		{
			for (int group = 0; group < this->nFNm; ++group)
			{
				std::vector<T> vec_result;
				vec_result.resize(this->m_totalout);
				//����˳����дvec_result
				bool bad = false;
				for (int ci = 0; ci<this->m_totalout && bad == false; ++ci)
				{
					//�ڼ���,�� vec_states_pnm�ĵڼ���
					const int serial_group = this->vec_char_out_group[ci];
					//�жϱ����ǲ�����Ҫ������һ������
					const unsigned long long npnmCt = this->vec_fnm[group][serial_group];
					const unsigned long long npnmGp = npnmCt / this->group_deal;
					//���Ҫ���ʵ�ǰ״̬��֮ǰ����ϣ�������PNM
					if (npnmGp < this->vec_states_gp[serial_group])
					{
						this->vec_states_pnm[serial_group] = (std::vector<std::vector <unsigned short> >());
						this->vec_states[serial_group] = tag_NM_State();
						this->vec_states_gp[serial_group] = 0;
						vec_nmfunc_initial[serial_group](pFNs[serial_group], pFMs[serial_group],
							this->vec_states_pnm[serial_group], &this->vec_states[serial_group], this->group_deal);
						this->vec_states_size[serial_group] = this->vec_states_pnm[serial_group].size();
					}//end if (npnmGp < vec_states_gp[serial_group])
					 //׷�Ͻ���
					while (npnmGp > this->vec_states_gp[serial_group] && bad == false)
					{
						++this->vec_states_gp[serial_group];
						this->vec_states_size[serial_group] = vec_nmfunc_next[serial_group](
							pFNs[serial_group],
							pFMs[serial_group],
							this->vec_states_pnm[serial_group],
							&this->vec_states[serial_group], this->group_deal);
						if (this->vec_states_size[serial_group] == 0)
							bad = true;
					}//end while (npnmGp > vec_states_gp[serial_group]&&bad==false)
					if (bad)
						continue;
					//���㱾�������������±�
					const int curPNMIdx = npnmCt % this->group_deal;
					if (curPNMIdx >= this->vec_states_size[serial_group])
					{
						bad = true;
						continue;
					}
					//ȡ������λ��
					const int sfidx = this->vec_states_pnm[serial_group][curPNMIdx][this->vec_char_out_offset[ci]];
					vec_result[ci] = this->vec_groups[serial_group][sfidx];
				}//end for (int ci=0;ci<total_out && bad==false;++ci)
				if (bad)
					continue;
				//������һ��PNM
				vec_results[group] = std::move(vec_result);
			}//end for (int group = 0; group < nFNm; ++group)
		}//end if (nFNm)
		return this->nFNm;
	}
}
