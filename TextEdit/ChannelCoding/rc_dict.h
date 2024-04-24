#ifndef RC_CODEC_HEADER_2015_02_11_H
#define RC_CODEC_H
/*!
* 使用错误图案法的通用多进制循环码编译码器
*/
#include <string>
#include <vector>
#include <unordered_map>
#include <cassert>
#include <memory.h>
//#include <omp.h>
#include "rc_codec_base.h"
#include "nmcalc.h"
namespace RC_DICTCODEC {

		template <class IntType>
		class rc_dict {
		public:
			struct err_mask{
				int index;
				IntType err;
			};
		public:
			explicit rc_dict(long long maxelements = 1024*1024):m_maxels(maxelements){}
			virtual ~rc_dict(){}
		public:
			size_t init(rc_codec_base<IntType> * encoder )
			{
				using namespace NM_CALC;
				const int N = encoder->get_n(), K = encoder->get_k(), M=encoder->get_m();
				IntType * zero_data = new IntType [K];
				IntType * zero_code = new IntType [N];
                const int procs = 6;
				memset(zero_data,0,sizeof(IntType) * K);
				memset(zero_code,0,sizeof(IntType) * N);
	
				m_dict.clear();
	
				encoder->encode(zero_data, zero_code);
	
				IntType * err_code_pool = new IntType [procs*N];
				if (m_dict.size()==0)
				{
					IntType * err_code = err_code_pool;
					memcpy(err_code,zero_code,sizeof(IntType) * N);
					encoder->chcode(err_code,err_code);
					std::string strKey = encoder->hash(err_code);
					assert (strKey.size());
					std::vector<err_mask> vec_mask;
					m_dict[strKey] = std::move(vec_mask);
					//puts(strKey.c_str());
				}
	
				const int CNM_N = N;
				int CNM_M = 1;
	
				bool bfinished = false;
				while( !bfinished)
				{
					long long expand_max = 1;
					for (int i=0;i<CNM_M;++i)
						expand_max *= (M-1);
					tag_NM_State state;
					std::vector<std::vector <unsigned short> > vec_output;
					int ret =  cnm(CNM_N, CNM_M, vec_output, &state, 256);
					while (ret>0 &&!bfinished)
					{
	#pragma omp parallel for
						for (long long i=0;i<static_cast<long long>(ret)  ;++i)
						{
							if (bfinished)
								continue;
							//printf ("%d :%d : %lld\n\n",i, CNM_M, expand_max);
                            IntType * err_code = &err_code_pool[0*N];
							for (long long j=0;j<expand_max  &&!bfinished;++j)
							{
								memcpy(err_code,zero_code,sizeof(IntType) * N);
								long long currv = j;
								std::vector<err_mask> vec_mask;
								for (size_t k = 0;k<static_cast<size_t>(CNM_M);++k)
								{
									const int v = currv % (M-1) + 1;
									const unsigned short idx = vec_output[i][k];
									//printf ("%d",idx);
									err_code[idx] = encoder->mod_m_add(err_code[idx],v);
									currv /= (M-1);
									err_mask e;
									e.err = v;
									e.index = idx;
									vec_mask.push_back(e);
								}
								encoder->chcode(err_code,err_code);
								std::string strKey = encoder->hash(err_code);
								if (strKey.size())
								{
	#pragma omp critical(m_dict)
									{
										//puts(strKey.c_str());
	
										if(m_dict.find(strKey)!=m_dict.end())
											bfinished = true;
										else if (m_dict.size()>=m_maxels)
											bfinished = true;
										else {
											m_dict[strKey] = std::move(vec_mask);
										}
										if (m_dict.size()%1000==0)
											printf ("Dict Size = %u\n",m_dict.size());
									}
	
								}
	
							}
						}
						ret =  cnm_next(CNM_N, CNM_M, vec_output, &state, 256);
	
					}
					++CNM_M;
					if (CNM_M>=CNM_N-1)
						bfinished = true;
				}
	
	
				delete [] zero_code;
				delete [] zero_data;
				delete [] err_code_pool;
				return m_dict.size();
			}
	
			std::vector<err_mask> err_find(const std::string  & strKey )
			{
				std::vector<err_mask> res;
	
				if (strKey.size())
				{
					if(m_dict.find(strKey)!=m_dict.end())
						res = m_dict[strKey];
					else {
						err_mask m;
						m.err = -1;
						m.index = -1;
						res.push_back(m);
					}
				}
				return res;
			}
	
		protected:
			long long m_maxels;
			std::unordered_map<std::string,  std::vector<err_mask> > m_dict;
		};
	

}
#endif // RC_CODEC_H
