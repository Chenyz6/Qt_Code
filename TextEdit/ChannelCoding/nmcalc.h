#pragma once
/*!
* 通用排列组合类，用于生成各种排列组合序数。
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
	//快速for算法,m组，每组for规模为n
	int fnm(const unsigned long long n[/*m*/], int m, std::vector<std::vector <unsigned long long> > & vec_output, tag_FOR_State * state, int limit = 0);
	//预分配vec_output排列算法
	int fnm_next(const unsigned long long n[/*m*/], int m, std::vector<std::vector <unsigned long long> > & vec_output, tag_FOR_State * state, int limit = 0);
	unsigned long long fnm_total(const int n[/*m*/], int m);

	//快速排列算法
	int pnm(int n, int m, std::vector<std::vector <unsigned short> > & vec_output, tag_NM_State * state, int limit = 0);
	//预分配vec_output排列算法
	int pnm_next(int n, int m, std::vector<std::vector <unsigned short> > & vec_output, tag_NM_State * state, int limit = 0);
	//规模
	unsigned long long pnm_total(int n, int m);


	//快速组合算法
	int cnm(int n, int m, std::vector<std::vector <unsigned short> > & vec_output, tag_NM_State * state, int limit = 0);
	//预分配vec_output组合算法
	int cnm_next(int n, int m, std::vector<std::vector <unsigned short> > & vec_output, tag_NM_State * state, int limit = 0);
	//规模
	unsigned long long cnm_total(int n, int m);

	//多组串联排列组合类，可选择每组采用的策略

	/*! 联合排列组合类
	* 可任意指定分组个数、每组策略（P,C）和规模
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
		void set_batch_deal(int d) { batch_deal = d; }		//设置每波数据最多返回多少
		void set_group_deal(int d) { group_deal = d; }		//设置每组排列组合中，窗口记忆多少结果。
		int  get_batch_deal() const { return batch_deal; }
		int  get_group_deal() const { return group_deal; }
		//取得总的计算规模
		unsigned long long total_task() const { return totaltask; }
		//输出有多少元素，即每组m的和
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
		//各组
		std::vector<std::vector<std::vector <unsigned short> > > vec_states_pnm;
		std::vector<tag_NM_State> vec_states;
		std::vector<unsigned long long> vec_states_gp;
		std::vector<int> vec_states_size;
	public:
		/*!
		* \brief serial_nm_begin 初始化一次任务
		* \param vec_results 用于存放任务结果的矩阵
		* \return 总规模
		*/
		unsigned long long serial_nm_begin(
			std::vector<std::vector<T> > & vec_results);
		/*!
		* \brief serial_nm_next 获得下一组结果集合
		* \param vec_results 用于存放任务结果的矩阵
		* \return  本次有效的结果个数
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
		//分组
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

		//开始进行串联排列
		const int * pFNs = this->vec_chars_n.data();
		const int * pFMs = this->group_m.data();
		const unsigned long long * pPNMs = this->vec_chars_pnm.data();
		//fnm
		fnm(pPNMs, this->serial_count, this->vec_fnm, &this->g_state, this->batch_deal);
		this->nFNm = this->vec_fnm.size();

		//各组
		for (int i = 0; i<this->serial_count; ++i)
		{
			this->vec_states_pnm.push_back(std::vector<std::vector <unsigned short> >());
			this->vec_states.push_back(tag_NM_State());
			this->vec_states_gp.push_back(0);
			vec_nmfunc_initial[i](pFNs[i], pFMs[i], this->vec_states_pnm[i], &this->vec_states[i], this->group_deal);
			this->vec_states_size.push_back(this->vec_states_pnm[i].size());
		}
		//最终结果
		if (this->nFNm)
		{
			for (int group = 0; group < this->nFNm; ++group)
			{
				std::vector<T> vec_result;
				vec_result.resize(this->m_totalout);
				//按照顺序填写vec_result
				bool bad = false;
				for (int ci = 0; ci<this->m_totalout && bad == false; ++ci)
				{
					//第几组,即 vec_states_pnm的第几行
					const int serial_group = this->vec_char_out_group[ci];
					//判断本组是不是需要进行下一轮生成
					const unsigned long long npnmCt = this->vec_fnm[group][serial_group];
					const unsigned long long npnmGp = npnmCt / this->group_deal;
					//如果要访问当前状态机之前的组合，则重新PNM
					if (npnmGp < this->vec_states_gp[serial_group])
					{
						this->vec_states_pnm[serial_group] = (std::vector<std::vector <unsigned short> >());
						this->vec_states[serial_group] = tag_NM_State();
						this->vec_states_gp[serial_group] = 0;
						vec_nmfunc_initial[serial_group](pFNs[serial_group], pFMs[serial_group],
							this->vec_states_pnm[serial_group], &this->vec_states[serial_group], this->group_deal);
						this->vec_states_size[serial_group] = this->vec_states_pnm[serial_group].size();
					}//end if (npnmGp < vec_states_gp[serial_group])
					 //追赶进度
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
					//计算本组排列组合里的下标
					const int curPNMIdx = npnmCt % this->group_deal;
					if (curPNMIdx >= this->vec_states_size[serial_group])
					{
						bad = true;
						continue;
					}
					//取得索引位置
					const int sfidx = this->vec_states_pnm[serial_group][curPNMIdx][this->vec_char_out_offset[ci]];
					vec_result[ci] = this->vec_groups[serial_group][sfidx];
				}//end for (int ci=0;ci<total_out && bad==false;++ci)
				if (bad)
					continue;
				//生成了一组PNM
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
		//开始进行串联排列
		const int * pFNs = this->vec_chars_n.data();
		const int * pFMs = this->group_m.data();
		const unsigned long long * pPNMs = this->vec_chars_pnm.data();
		//最终结果
		//下一组
		this->nFNm = fnm_next(pPNMs, this->serial_count, this->vec_fnm, &this->g_state, this->batch_deal);
		if (this->nFNm)
		{
			for (int group = 0; group < this->nFNm; ++group)
			{
				std::vector<T> vec_result;
				vec_result.resize(this->m_totalout);
				//按照顺序填写vec_result
				bool bad = false;
				for (int ci = 0; ci<this->m_totalout && bad == false; ++ci)
				{
					//第几组,即 vec_states_pnm的第几行
					const int serial_group = this->vec_char_out_group[ci];
					//判断本组是不是需要进行下一轮生成
					const unsigned long long npnmCt = this->vec_fnm[group][serial_group];
					const unsigned long long npnmGp = npnmCt / this->group_deal;
					//如果要访问当前状态机之前的组合，则重新PNM
					if (npnmGp < this->vec_states_gp[serial_group])
					{
						this->vec_states_pnm[serial_group] = (std::vector<std::vector <unsigned short> >());
						this->vec_states[serial_group] = tag_NM_State();
						this->vec_states_gp[serial_group] = 0;
						vec_nmfunc_initial[serial_group](pFNs[serial_group], pFMs[serial_group],
							this->vec_states_pnm[serial_group], &this->vec_states[serial_group], this->group_deal);
						this->vec_states_size[serial_group] = this->vec_states_pnm[serial_group].size();
					}//end if (npnmGp < vec_states_gp[serial_group])
					 //追赶进度
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
					//计算本组排列组合里的下标
					const int curPNMIdx = npnmCt % this->group_deal;
					if (curPNMIdx >= this->vec_states_size[serial_group])
					{
						bad = true;
						continue;
					}
					//取得索引位置
					const int sfidx = this->vec_states_pnm[serial_group][curPNMIdx][this->vec_char_out_offset[ci]];
					vec_result[ci] = this->vec_groups[serial_group][sfidx];
				}//end for (int ci=0;ci<total_out && bad==false;++ci)
				if (bad)
					continue;
				//生成了一组PNM
				vec_results[group] = std::move(vec_result);
			}//end for (int group = 0; group < nFNm; ++group)
		}//end if (nFNm)
		return this->nFNm;
	}
}
