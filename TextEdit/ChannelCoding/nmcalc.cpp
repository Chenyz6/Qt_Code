#include "nmcalc.h"
#include <memory.h>
namespace NM_CALC {
	/*!
	* \brief fnm 产生m组for循环等效的下标组合，初始化状态机
	* \param n	每组中for 的规模。 等效  for (int j=0;j<n[i];++j)，其中 i 范围 [0~m-1]
	* \param m 组数
	* \param vec_output 用于存储输出的向量
	* \param state 状态机
	* \param limit 一次最多返回多少结果
	* \return 实际返回的结果数
	*/
	int fnm(const unsigned long long n[/*m*/], int m, std::vector<std::vector <unsigned long long> > & vec_output, tag_FOR_State * state, int limit/* = 0*/)
	{
		std::vector<unsigned long long> & vec_buf = state->vec_buf,
			&vec_bz = state->vec_bz;
		int &swim = state->swim;
		bool &finished = state->finished;

		if (vec_bz.size() == 0)
		{
			for (int i = 0; i<m; ++i)    vec_buf.push_back(0);
			vec_bz.push_back(0);
			swim = 0;
			finished = false;
		}

		if (finished == true)
			return 0;
		int group = 0;
		do
		{
			unsigned long long ch = 0;
			if (swim<m)
			{
				vec_buf[swim] = ch;
				++swim;
			}
			if (swim == m)
			{
				vec_output.push_back(vec_buf);
				++group;
				bool hit = false;
				do
				{
					--swim;
					if (swim >= 0)
					{
						unsigned long long nextv = vec_buf[swim];
						do
						{
							++nextv;
							if (nextv >= n[swim])
								break;
							hit = true;
						} while (hit == false);
						if (hit == true)
						{
							vec_buf[swim] = nextv;
							++swim;
						}
					}
					else
						finished = true;
				} while (finished == false && hit == false);
				if (group >= limit && limit>0)
					break;
			}
		} while (finished == false);
		return group;
	}


	/*!
	* \brief fnm_next 继续产生m组for循环等效的下标组合
	* \param n	每组中for 的规模。 等效  for (int j=0;j<n[i];++j)，其中 i 范围 [0~m-1]
	* \param m 组数
	* \param vec_output 用于存储输出的向量
	* \param state 状态机
	* \param limit 一次最多返回多少结果
	* \return 实际返回的结果数
	*/
	int fnm_next(const unsigned long long n[/*m*/], int m, std::vector<std::vector <unsigned long long> > & vec_output, tag_FOR_State * state, int limit/* = 0*/)
	{
		std::vector<unsigned long long> & vec_buf = state->vec_buf,
			&vec_bz = state->vec_bz;
		int &swim = state->swim;
		bool &finished = state->finished;

		if (vec_bz.size() == 0)
		{
			for (int i = 0; i<m; ++i)    vec_buf.push_back(0);
			vec_bz.push_back(0);
			swim = 0;
			finished = false;
		}
		if (finished == true)
			return 0;
		int group = 0;
		do
		{
			unsigned long long ch = 0;
			if (swim<m)
			{
				vec_buf[swim] = ch;
				++swim;
			}
			if (swim == m)
			{
				memcpy(vec_output[group].data(), vec_buf.data(), m * sizeof(unsigned long long));
				++group;
				bool hit = false;
				do
				{
					--swim;
					if (swim >= 0)
					{
						unsigned long long nextv = vec_buf[swim];
						do
						{
							++nextv;
							if (nextv >= n[swim])
								break;
							hit = true;
						} while (hit == false);
						if (hit == true)
						{
							vec_buf[swim] = nextv;
							++swim;
						}
					}
					else
						finished = true;
				} while (finished == false && hit == false);
				if (group >= limit && limit>0)
					break;
			}
		} while (finished == false);
		return group;
	}
	unsigned long long fnm_total(const int n[/*m*/], int m)
	{
		unsigned long long totalTestsCur = 1;
		for (int j = 0; j<m; ++j)
			totalTestsCur *= n[j];
		return totalTestsCur;

	}
	/*!
	\brief pnm 快速算法，使用带有记忆效应的 tag_NM_State 记录穷尽进度很好的避免了重新计算的耗时
	\fn pnm
	\param n				N，集合数
	\param m				M, 子集
	\param vec_output		存储结果的集合,本集合会自动增长
	\param state			状态存储
	\param limit			本次最多样本数
	\return int			本次给出的样本数
	*/
	int pnm(int n, int m, std::vector<std::vector <unsigned short> > & vec_output, tag_NM_State * state, int limit/* = 0*/)
	{
		std::vector<unsigned short> & vec_buf = state->vec_buf,
			&vec_bz = state->vec_bz;
		int &swim = state->swim;
		bool &finished = state->finished;

		if (vec_bz.size() == 0)
		{
			for (int i = 0; i<m; ++i)    vec_buf.push_back(0);
			for (int i = 0; i<n; ++i)    vec_bz.push_back(0);
			swim = 0;
			finished = false;
		}
		if (finished == true)
			return 0;
		int group = 0;
		do
		{
			int ch = 0;
			if (swim<m)
			{
				while (vec_bz[ch])
					++ch;
				vec_buf[swim] = ch;
				vec_bz[ch] = 1;
				++swim;
			}
			if (swim == m)
			{
				vec_output.push_back(vec_buf);
				++group;
				bool hit = false;
				do
				{
					if (swim<m && swim >= 0) vec_bz[vec_buf[swim]] = 0;
					--swim;
					if (swim >= 0)
					{
						int nextv = vec_buf[swim];
						do
						{
							++nextv;
							if (nextv >= n)
								break;
							if (vec_bz[nextv] == 0)
								hit = true;
						} while (hit == false);
						if (hit == true)
						{
							vec_bz[vec_buf[swim]] = 0;
							vec_buf[swim] = nextv;
							vec_bz[nextv] = 1;
							++swim;
						}
					}
					else
						finished = true;
				} while (finished == false && hit == false);
				if (group >= limit && limit>0)
					break;
			}
		} while (finished == false);
		return group;
	}
	/*!
	\brief pnm_next 使用带有记忆效应的 tag_NM_State 记录穷尽进度很好的避免了重新计算的耗时
	\fn pnm_next
	\param n				N，集合数
	\param m				M, 子集
	\param vec_output		存储结果的集合,本集合必须已经存在！！
	\param state			状态存储
	\param limit			本次最多样本数
	\return int			本次给出的样本数
	*/
	int pnm_next(int n, int m, std::vector<std::vector <unsigned short> > & vec_output, tag_NM_State * state, int limit/* = 0*/)
	{
		std::vector<unsigned short> & vec_buf = state->vec_buf,
			&vec_bz = state->vec_bz;
		int &swim = state->swim;
		bool &finished = state->finished;

		if (vec_bz.size() == 0)
		{
			for (int i = 0; i<m; ++i)    vec_buf.push_back(0);
			for (int i = 0; i<n; ++i)    vec_bz.push_back(0);
			swim = 0;
			finished = false;
		}
		if (finished == true)
			return 0;
		int group = 0;
		do
		{
			int ch = 0;
			if (swim<m)
			{
				while (vec_bz[ch])
					++ch;
				vec_buf[swim] = ch;
				vec_bz[ch] = 1;
				++swim;
			}
			if (swim == m)
			{
				memcpy(vec_output[group].data(), vec_buf.data(), m * sizeof(unsigned short));
				++group;
				bool hit = false;
				do
				{
					if (swim<m && swim >= 0) vec_bz[vec_buf[swim]] = 0;
					--swim;
					if (swim >= 0)
					{
						int nextv = vec_buf[swim];
						do
						{
							++nextv;
							if (nextv >= n)
								break;
							if (vec_bz[nextv] == 0)
								hit = true;
						} while (hit == false);
						if (hit == true)
						{
							vec_bz[vec_buf[swim]] = 0;
							vec_buf[swim] = nextv;
							vec_bz[nextv] = 1;
							++swim;
						}
					}
					else
						finished = true;
				} while (finished == false && hit == false);
				if (group >= limit && limit>0)
					break;
			}
		} while (finished == false);
		return group;
	}

	unsigned long long pnm_total(int n, int m)
	{
		unsigned long long totalTestsCur = 1;
		for (int j = 0; j<m; ++j)
			totalTestsCur *= (n - j);
		return totalTestsCur;
	}

	/*!
	\brief cnm 快速算法，使用带有记忆效应的 tag_NM_State 记录穷尽进度很好的避免了重新计算的耗时
	\fn cnm
	\param n				N，集合数
	\param m				M, 子集
	\param vec_output		存储结果的集合,本集合会自动增长
	\param state			状态存储
	\param limit			本次最多样本数
	\return int			本次给出的样本数
	*/
	int cnm(int n, int m, std::vector<std::vector <unsigned short> > & vec_output, tag_NM_State * state, int limit)
	{
		std::vector<unsigned short> & vec_buf = state->vec_buf,
			&vec_bz = state->vec_bz;
		int &swim = state->swim;
		bool &finished = state->finished;

		if (vec_bz.size() == 0)
		{
			for (int i = 0; i<m; ++i)    vec_buf.push_back(0);
			for (int i = 0; i<n; ++i)    vec_bz.push_back(0);
			swim = 0;
			finished = false;
		}
		if (finished == true)
			return 0;
		int group = 0;
		do
		{
			int ch = 0;
			if (swim>0 && swim < m)
				ch = vec_buf[swim - 1];
			if (swim<m)
			{
				bool hit = false;
				while (ch < n && !hit)
				{
					if (vec_bz[ch] == 0) hit = true;
					else
						++ch;
				}
				if (hit)
				{
					vec_buf[swim] = ch;
					vec_bz[ch] = 1;
					++swim;
				}
				else
					while (!hit && finished == false)
					{
						//回溯
						if (swim<m && swim >= 0) vec_bz[vec_buf[swim]] = 0;
						--swim;
						if (swim >= 0)
						{
							int nextv = vec_buf[swim];
							do
							{
								++nextv;
								if (nextv >= n)
									break;
								if (vec_bz[nextv] == 0)
									hit = true;
							} while (hit == false);
							if (hit == true)
							{
								vec_bz[vec_buf[swim]] = 0;
								vec_buf[swim] = nextv;
								vec_bz[nextv] = 1;
								++swim;
							}
						}
						else
							finished = true;

					}


			}
			if (swim == m && finished == false)
			{
				vec_output.push_back(vec_buf);
				if (group >= limit && limit>0)
					break;
				++group;

				bool hit = false;
				do
				{
					if (swim<m && swim >= 0) vec_bz[vec_buf[swim]] = 0;
					--swim;
					if (swim >= 0)
					{
						int nextv = vec_buf[swim];
						do
						{
							++nextv;
							if (nextv >= n)
								break;
							if (vec_bz[nextv] == 0)
								hit = true;
						} while (hit == false);
						if (hit == true)
						{
							vec_bz[vec_buf[swim]] = 0;
							vec_buf[swim] = nextv;
							vec_bz[nextv] = 1;
							++swim;
						}
					}
					else
						finished = true;
				} while (finished == false && hit == false);
			}
		} while (finished == false);
		return group;
	}
	/*!
	\brief cnm 快速算法，使用带有记忆效应的 tag_NM_State 记录穷尽进度很好的避免了重新计算的耗时
	\fn cnm_next
	\param n				N，集合数
	\param m				M, 子集
	\param vec_output		存储结果的集合,本集合必须已经存在！！
	\param state			状态存储
	\param limit			本次最多样本数
	\return int			本次给出的样本数
	*/
	int cnm_next(int n, int m, std::vector<std::vector <unsigned short> > & vec_output, tag_NM_State * state, int limit)
	{
		std::vector<unsigned short> & vec_buf = state->vec_buf,
			&vec_bz = state->vec_bz;
		int &swim = state->swim;
		bool &finished = state->finished;

		if (vec_bz.size() == 0)
		{
			for (int i = 0; i<m; ++i)    vec_buf.push_back(0);
			for (int i = 0; i<n; ++i)    vec_bz.push_back(0);
			swim = 0;
			finished = false;
		}
		if (finished == true)
			return 0;
		int group = 0;
		do
		{
			int ch = 0;
			if (swim>0 && swim < m)
				ch = vec_buf[swim - 1];
			if (swim<m)
			{
				bool hit = false;
				while (ch < n && !hit)
				{
					if (vec_bz[ch] == 0) hit = true;
					else
						++ch;
				}
				if (hit)
				{
					vec_buf[swim] = ch;
					vec_bz[ch] = 1;
					++swim;
				}
				else
					while (!hit && finished == false)
					{
						//回溯
						if (swim<m && swim >= 0) vec_bz[vec_buf[swim]] = 0;
						--swim;
						if (swim >= 0)
						{
							int nextv = vec_buf[swim];
							do
							{
								++nextv;
								if (nextv >= n)
									break;
								if (vec_bz[nextv] == 0)
									hit = true;
							} while (hit == false);
							if (hit == true)
							{
								vec_bz[vec_buf[swim]] = 0;
								vec_buf[swim] = nextv;
								vec_bz[nextv] = 1;
								++swim;
							}
						}
						else
							finished = true;

					}


			}
			if (swim == m && finished == false)
			{
				memcpy(vec_output[group].data(), vec_buf.data(), m * sizeof(unsigned short));
				if (group >= limit && limit>0)
					break;
				++group;

				bool hit = false;
				do
				{
					if (swim<m && swim >= 0) vec_bz[vec_buf[swim]] = 0;
					--swim;
					if (swim >= 0)
					{
						int nextv = vec_buf[swim];
						do
						{
							++nextv;
							if (nextv >= n)
								break;
							if (vec_bz[nextv] == 0)
								hit = true;
						} while (hit == false);
						if (hit == true)
						{
							vec_bz[vec_buf[swim]] = 0;
							vec_buf[swim] = nextv;
							vec_bz[nextv] = 1;
							++swim;
						}
					}
					else
						finished = true;
				} while (finished == false && hit == false);
			}
		} while (finished == false);
		return group;
	}
	unsigned long long cnm_total(int n, int m)
	{
		unsigned long long totalTestsCur = 1;
		for (int j = 0; j<m; ++j)
			totalTestsCur *= (n - j);
		for (int j = 0; j<m; ++j)
			totalTestsCur /= (j + 1);

		return totalTestsCur;

	}
}
