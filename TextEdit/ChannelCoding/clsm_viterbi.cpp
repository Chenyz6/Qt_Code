/** \brief 这个文件实现了有限状态机(LSM)维特比译码器 by goldenhawking
*/
#include <stdlib.h>
#include <memory.h>
#include "clsm_viterbi.h"
namespace LSMVIT {
	//存储从当前状态到下一状态的跳转
	typedef  struct {
		int * nNextStatus;	//下一状态
		int * nOutput;		//跳转输出
	} tag_statusjmp_next;
	//存储当前状态来自哪个旧状态的跳转
	typedef  struct {
		int * nLastInput;	//上一次的触发
		int * nLastStatus;	//上一状态
		int * nLastOutput;	//跳转输出
	} tag_statusjmp_last;
	//最优路径表，采用double_buffer技术
	typedef  struct {
		long long nCurrentHamming[2];	//当前全路径汉明
		int nInitialStatus[2];          //本路径的起始初态
		long long *nHamming[2];			//各个跳转的汉明
		int *nInChain[2];				//各个跳转的输入（触发）
										//int nOutChain[2][L];			//各个跳转的输出
										//int nRegChain[2][L];			//各步状态
	} tag_path;


	/** \brief 有限状态机编译码器
	*   有限状态机有若干状态，因用户输入的触发，在状态间跳转。跳转的过程中
	*   跳转过程中，产生了跳转输出。受到信道污染的输出被重新送入译码器，
	*    通过维特比算法，译码器会纠正一定量的错误。
	*/
	typedef struct
	{
		int reg_status;
		int data_status;
		int code_bitlen;
		int L;
		tag_statusjmp_next  * table_nextjmp/*reg_status items*/;
		tag_statusjmp_last  *  table_lastjmp/*reg_status items*/;
		tag_path  *  best_path/*reg_status items*/;
		int nClock;					//时钟
		int * vec_decoded_data;		//存储译码结果的缓存
		int vec_decoded_data_msz;
		int vec_decoded_data_sz;
		long long   nBestHamming;			//译码器当前的最优汉明
		long long   nWorstHamming;          //译码器历史的最差最佳汉明
		int * nBestStatus;			//所有最优路径的索引
		int nBestStatus_sz;
	} clsm_codec_arg;

	void * new_lsm_codec(const int reg_status, const int data_status, const int code_bitlen, const int L)
	{
		int i;
		clsm_codec_arg * newitem = (clsm_codec_arg *)malloc(sizeof(clsm_codec_arg));
		if (newitem != 0)
		{
			newitem->reg_status = reg_status;
			newitem->data_status = data_status;
			newitem->code_bitlen = code_bitlen;
			newitem->L = L;
			newitem->table_nextjmp = (tag_statusjmp_next *)malloc(reg_status * sizeof(tag_statusjmp_next));
			newitem->table_lastjmp = (tag_statusjmp_last *)malloc(reg_status * sizeof(tag_statusjmp_last));
			newitem->best_path = (tag_path *)malloc(reg_status * sizeof(tag_path));
			newitem->nClock = -1;
			newitem->nBestHamming = 0;
			newitem->nWorstHamming = -1;
			newitem->vec_decoded_data_msz = 65536;
			newitem->vec_decoded_data_sz = 0;
			newitem->vec_decoded_data = (int *)malloc(65536 * sizeof(int));
			newitem->nBestStatus_sz = 0;
			newitem->nBestStatus = (int *)malloc(reg_status * sizeof(int));

			//初始胡跳转表
			for (i = 0; i<reg_status; i++)
			{
				newitem->table_nextjmp[i].nNextStatus = (int *)malloc(sizeof(int)*data_status);
				newitem->table_nextjmp[i].nOutput = (int *)malloc(sizeof(int)*data_status);
				newitem->table_lastjmp[i].nLastInput = (int *)malloc(sizeof(int)*data_status);
				newitem->table_lastjmp[i].nLastOutput = (int *)malloc(sizeof(int)*data_status);
				newitem->table_lastjmp[i].nLastStatus = (int *)malloc(sizeof(int)*data_status);
				newitem->best_path[i].nInChain[0] = (int *)malloc(sizeof(int)*L);
				newitem->best_path[i].nInChain[1] = (int *)malloc(sizeof(int)*L);
				newitem->best_path[i].nHamming[0] = (long long *)malloc(sizeof(long long)*L);
				newitem->best_path[i].nHamming[1] = (long long *)malloc(sizeof(long long)*L);

				memset(newitem->table_nextjmp[i].nNextStatus, 0, sizeof(int) * data_status);
				memset(newitem->table_nextjmp[i].nOutput, 0, sizeof(int) * data_status);
				memset(newitem->table_lastjmp[i].nLastInput, 0, sizeof(int) * data_status);
				memset(newitem->table_lastjmp[i].nLastOutput, 0, sizeof(int) * data_status);
				memset(newitem->table_lastjmp[i].nLastStatus, 0, sizeof(int) * data_status);

			}

			reset_status(newitem);
		}

		return newitem;
	}
	void delete_lsm_codec(void * item)
	{
		int i;
		clsm_codec_arg * codec = (clsm_codec_arg *)item;
		if (item == 0)
			return;

		//初始胡跳转表
		for (i = 0; i<codec->reg_status; i++)
		{
			free(codec->table_nextjmp[i].nNextStatus);
			free(codec->table_nextjmp[i].nOutput);
			free(codec->table_lastjmp[i].nLastInput);
			free(codec->table_lastjmp[i].nLastOutput);
			free(codec->table_lastjmp[i].nLastStatus);
			free(codec->best_path[i].nInChain[0]);
			free(codec->best_path[i].nInChain[1]);
			free(codec->best_path[i].nHamming[0]);
			free(codec->best_path[i].nHamming[1]);
		}
		free(codec->table_nextjmp);
		free(codec->table_lastjmp);
		free(codec->best_path);
		free(codec->vec_decoded_data);
		free(codec->nBestStatus);
		free(codec);
	}

	long long worst_hamming(void * item)
	{
		clsm_codec_arg * codec = (clsm_codec_arg *)item;
		if (!item)
			return -1;
		return codec->nWorstHamming;
	}

	int get_initial_status(void * item)
	{
		clsm_codec_arg * codec = (clsm_codec_arg *)item;
		if (!item)
			return -1;
		return codec->best_path[codec->nBestStatus[0]].nInitialStatus[0];
	}

	/** \brief 重置译码器。如果译码器已经被初始化(m_nClock >=0)，则会掷零
	*/
	void reset_status(void *codec)
	{
		int i;
		clsm_codec_arg * item = (clsm_codec_arg *)codec;
		for (i = 0; i<item->reg_status; i++)
		{
			memset(item->best_path[i].nInChain[0], 0, sizeof(int) * item->L);
			memset(item->best_path[i].nInChain[1], 0, sizeof(int) * item->L);
			memset(item->best_path[i].nHamming[0], 0, sizeof(long long) * item->L);
			memset(item->best_path[i].nHamming[1], 0, sizeof(long long) * item->L);
			item->best_path[i].nCurrentHamming[0] = 0;
			item->best_path[i].nCurrentHamming[1] = 0;
			item->best_path[i].nInitialStatus[0] = i;
			item->best_path[i].nInitialStatus[1] = i;
		}
		if (item->nClock >= 0) item->nClock = 0;
		item->nBestHamming = 0;
		item->nWorstHamming = -1;
		item->nBestStatus_sz = 0;
		item->vec_decoded_data_sz = 0;
	}

	int init_status(void * codec, cb_status cb, void * ud)
	{
		int succeed = -1;
		int i, j, k;
		clsm_codec_arg * item = (clsm_codec_arg *)codec;
		for (i = 0; i<item->reg_status && succeed != 0; i++)
		{
			for (j = 0; j<item->data_status && succeed != 0; j++)
			{
				int nNextReg, nOutput;
				//调用用户的函数
				succeed = cb(i, j, &nNextReg, &nOutput, ud);
				if (succeed != 0)
				{
					item->table_nextjmp[i].nNextStatus[j] = nNextReg;
					item->table_nextjmp[i].nOutput[j] = nOutput;
					//反差表初始化
					for (k = 1; k<item->data_status; k++)
					{
						item->table_lastjmp[nNextReg].nLastInput[k - 1] = item->table_lastjmp[nNextReg].nLastInput[k];
						item->table_lastjmp[nNextReg].nLastOutput[k - 1] = item->table_lastjmp[nNextReg].nLastOutput[k];
						item->table_lastjmp[nNextReg].nLastStatus[k - 1] = item->table_lastjmp[nNextReg].nLastStatus[k];
					}
					item->table_lastjmp[nNextReg].nLastStatus[item->data_status - 1] = i;
					item->table_lastjmp[nNextReg].nLastInput[item->data_status - 1] = j;
					item->table_lastjmp[nNextReg].nLastOutput[item->data_status - 1] = nOutput;
				}// end if succeed
			}//next j
		}// next i
		if (succeed != 0)
			item->nClock = 0;
		else
			item->nClock = -1;
		reset_status(item);
		return succeed;
	}


	/** \brief 进行编码，利用用户所给的触发指令（输入信息），在状态间
	* 跳转，并产生输出
	* \param inputArray[] int 各个输入。输入为 [0 ~ data_status-1] 之间的任意值
	* \param nLen int 有效输入数
	* \param outputArray[] int 存放输出的数组，必须也要至少有nLen长
	* \param InitialStatus int 编码器的初态。
	* \return int 编码器现在的状态。对很长的数据，可以分段编码。
	*
	*/
	int encode(void * pcodec, const int inputArray[], int nLen, int outputArray[], int InitialStatus)
	{
		int i = 0;
		int nCurrStatus = 0;
		clsm_codec_arg * codec = (clsm_codec_arg *)pcodec;
		if (codec->nClock < 0) return -1;
		InitialStatus %= codec->reg_status;
		nCurrStatus = InitialStatus;
		for (i = 0; i<nLen; i++)
		{
			int nInput = inputArray[i] % codec->data_status;
			//查表
			outputArray[i] = codec->table_nextjmp[nCurrStatus].nOutput[nInput];
			nCurrStatus = codec->table_nextjmp[nCurrStatus].nNextStatus[nInput];
		}
		return nCurrStatus;
	}

	/** \brief 维特比译码算法，最优的所有路径间，各个状态使用大数判决
	*
	* \param CodeArray[] int 输入译码器的编码后序列，每个元素取值 [0~2^code_bitlen-1]
	* \param nSymbolLen int 序列长度
	* \param bFinished int 清空标志。是否把L约束长度内的所有元素全部输出，true 一般用于最后一批数据
	* \param InitialStatus int 如果初态已知，可以告诉译码器，以便提高起始阶段的纠错能力
	* \return int 目前，缓存中有多少个判决可以读取。(调用 pop_data)
	*
	*/
	int decode(void * pcodec, const  int CodeArray[], int nSymbolLen, int bFinished)
	{
		int cd = 0, i, j, yy, pos, mirror, raw;
		long long nHamm = 0;
		int nBestStatus = 0;
		int nMaxHit = 0;
		int *tmp;
		int nBests = 0;
		long long nPathHamm = 0;
		int nBestLastStatus = 0, nBestInput = 0; /*nBestOutput = 0, */
		long long nBestHamm = 0x7ffffffffffffffe;
		long long nBestDHamm = 0;
		int * nStatusCounter = 0;
		int bitweight[16] = { 0,1,1,2,1,2,2,3,1,2,2,3,2,3,3,4 };
		clsm_codec_arg * codec = (clsm_codec_arg *)pcodec;
		if (codec->nClock<0) return -1;
		nStatusCounter = (int *)malloc(sizeof(int)*codec->data_status);
		for (cd = 0; cd < nSymbolLen; cd++)
		{
			/*
			对每次输入，codec->nCLock ++, 对各个寄存器状态，查下 lastjmp 表，
			从而知道每个新状态由哪些老的状态而来，同时计算汉明距离。
			*/
			codec->nBestHamming = 0x7ffffffffffffffe;
			codec->nBestStatus_sz = 0;
			//对每个状态，统计最优路径
			for (i = 0; i<codec->reg_status; i++)
			{
				//用于记录最优路径的临时变量
				nBestLastStatus = 0, nBestInput = 0, /*nBestOutput = 0, */nBestHamm = 0x7ffffffffffffffe,
					nBestDHamm = 0;
				//某个状态是由以下几个状态转来的
				for (j = 0; j<codec->data_status; j++)
				{
					//对每种可能的转换可能，计算有转换生成的输出与实际收到的码字的误差。
					/*
					为了处理打孔码，规定输入的卷积码符号每组表示为		m...mm xx...x, 低code_bitlen位是符号，高code_bitlen位为掩码，
					掩码表示对应符号位是否是删除比特，1为是，0为否
					*/
					unsigned int code = CodeArray[cd] % (1 << (unsigned int)codec->code_bitlen);
					unsigned int mask = (~((unsigned int)(CodeArray[cd] >> codec->code_bitlen))) % (1 << (unsigned int)codec->code_bitlen);
					int nErr = (code ^ codec->table_lastjmp[i].nLastOutput[j]);
					nErr &= mask;
					nHamm = 0;
					for (yy = 0; yy <= codec->code_bitlen / 4; yy++)
					{
						//利用比特移位、称重，统计本次转换的汉明距离
						nHamm += bitweight[nErr & 0x0f];
						nErr >>= 4;
					}
					//全路径旧汉明距离
					nPathHamm = codec->best_path[codec->table_lastjmp[i].nLastStatus[j]].nCurrentHamming[codec->nClock % 2];
					//新距离
					nPathHamm += nHamm;
					if (nBestHamm > nPathHamm)
					{
						//如果本转换较优,则刷新纪录
						nBestHamm = nPathHamm;
						nBestLastStatus = codec->table_lastjmp[i].nLastStatus[j];
						nBestInput = codec->table_lastjmp[i].nLastInput[j];
						/*nBestOutput  = codec->table_lastjmp[i].nLastOutput[j];*/
						nBestDHamm = nHamm;
					}
				} // end for j
				  //开始刷新镜像路径,双缓存，用找到的最优转换来做为新的路径
				mirror = (codec->nClock + 1) % 2;
				raw = codec->nClock % 2;
				//首先，补全旧的路径的转换参数
				codec->best_path[nBestLastStatus].nHamming[raw][0] = nBestDHamm;
				codec->best_path[nBestLastStatus].nInChain[raw][0] = nBestInput;
				//codec->best_path[nBestLastStatus].nOutChain [raw][0] = nBestOutput;
				//旧路径全部后移一个状态，腾出空间
				for (j = 1; j<codec->L; j++)
				{
					codec->best_path[i].nHamming[mirror][j] = codec->best_path[nBestLastStatus].nHamming[raw][j - 1];
					codec->best_path[i].nInChain[mirror][j] = codec->best_path[nBestLastStatus].nInChain[raw][j - 1];
					//codec->best_path[i].nOutChain[mirror][j] = codec->best_path[nBestLastStatus].nOutChain[raw][j-1];
					//codec->best_path[i].nRegChain[mirror][j] = codec->best_path[nBestLastStatus].nRegChain[raw][j-1];
				}
				//更新汉明，旧路径最末 (L-1)的汉明由于跑出了范围，减去,同时，新的作为首部加入
				codec->best_path[i].nCurrentHamming[mirror] = codec->best_path[nBestLastStatus].nCurrentHamming[raw];
				codec->best_path[i].nCurrentHamming[mirror] -= codec->best_path[nBestLastStatus].nHamming[raw][codec->L - 1];
				codec->best_path[i].nCurrentHamming[mirror] += nBestDHamm;
				//初态传递
				codec->best_path[i].nInitialStatus[mirror] = codec->best_path[nBestLastStatus].nInitialStatus[raw];
				//更新最前端（新）的节点
				codec->best_path[i].nHamming[mirror][0] = 0;
				codec->best_path[i].nInChain[mirror][0] = 0;
				//codec->best_path[i].nOutChain[mirror][0] = 0;
				//codec->best_path[i].nRegChain[mirror][0] = i;
				//刷新最优路径纪录
				if (codec->nBestHamming > codec->best_path[i].nCurrentHamming[mirror])
				{
					codec->nBestHamming = codec->best_path[i].nCurrentHamming[mirror];
					codec->nBestStatus_sz = 0;
				}
				if (codec->nBestHamming == codec->best_path[i].nCurrentHamming[mirror])
					codec->nBestStatus[codec->nBestStatus_sz++] = i;
			}// end for i

			if (codec->nWorstHamming == -1 || codec->nWorstHamming < codec->nBestHamming)
				codec->nWorstHamming = codec->nBestHamming;
			//推进 clock
			codec->nClock++;
			//缓存输出
			if (codec->nClock >= codec->L - 1 || bFinished != 0)
			{
				if (cd + 1 < nSymbolLen && codec->nClock < codec->L - 1)
					continue;
				raw = codec->nClock % 2;
				nBests = codec->nBestStatus_sz;//最优的 n 条路径
				for (pos = codec->L - 1; pos >0; pos--)
				{
					//统计各个跳转的大数判决
					memset(nStatusCounter, 0, sizeof(int)*codec->data_status);
					for (i = 0; i< nBests; i++)
						nStatusCounter[codec->best_path[codec->nBestStatus[i]].nInChain[raw][pos]] ++;
					//					}

					nBestStatus = 0;
					nMaxHit = -1;
					for (i = 0; i<codec->data_status; i++)
					{
						if (nMaxHit < nStatusCounter[i])
						{
							nMaxHit = nStatusCounter[i];
							nBestStatus = i;
						}
					}// next i
					if (codec->vec_decoded_data_sz + 1 >= codec->vec_decoded_data_msz)
					{
						tmp = (int *)malloc((codec->vec_decoded_data_msz + 65536) * sizeof(int));
						memcpy(tmp, codec->vec_decoded_data, codec->vec_decoded_data_sz * sizeof(int));
						codec->vec_decoded_data_msz += 65536;
						free(codec->vec_decoded_data);
						codec->vec_decoded_data = tmp;
					}
					codec->vec_decoded_data[codec->vec_decoded_data_sz++] = nBestStatus;
					if (bFinished == 0 || cd + 1 < nSymbolLen)
						break;
				}//next Pos
			}//end if codec->nClock >= L - 1 || bFinished == true
		}// next cd
		if (bFinished != 0)
		{
			codec->nClock = 0;
			for (i = 0; i<codec->reg_status; i++)
				codec->best_path[i].nCurrentHamming[0] = codec->best_path[i].nCurrentHamming[1] = 0;
		}
		free(nStatusCounter);
		return (int)codec->vec_decoded_data_sz;
	}

	/** \brief 维特比译码软判决算法，最优的所有路径间，各个状态使用软判决。
	*
	* \param CodeArray[] int 输入译码器的编码后序列，每个元素取值 v0~v1, vOmit 为打孔标记
	* \param nBitLen int 序列的比特长度.
	* \param bFinished int 清空标志。是否把L约束长度内的所有元素全部输出，true 一般用于最后一批数据
	* \param InitialStatus int 如果初态已知，可以告诉译码器，以便提高起始阶段的纠错能力
	* \return int 目前，缓存中有多少个判决可以读取。(调用 pop_data)
	*
	*/
	int decode_soft(void * pcodec, const int CodeArray[], int nBitLen, int val_0, int val_1, int val_omit, int bFinished)
	{
		int cd = 0, i, j, pos, mirror, raw, ot;
		long long nHamm = 0;
		int nBestStatus = 0;
		int nMaxHit = 0;
		int *tmp;
		long long ev, ddd;
		int nBests;
		long long nPathHamm;
		int nBestLastStatus = 0, nBestInput = 0; /*nBestOutput = 0, */
		long long nBestHamm = 0x7ffffffffffffffe;
		long long nBestDHamm = 0;
		int * nStatusCounter = 0;
		int maxval = val_0<val_1 ? val_1 : val_0;
		int minval = val_0>val_1 ? val_1 : val_0;
		int nsig = val_0<val_1 ? 1 : -1;
		clsm_codec_arg * codec = (clsm_codec_arg *)pcodec;
		int nSymbolLen = nBitLen / codec->code_bitlen;
		if (codec->nClock<0) return -1;
		nStatusCounter = (int *)malloc(sizeof(int)*codec->data_status);
		for (cd = 0; cd < nSymbolLen; cd++)
		{
			/*
			对每次输入，codec->nCLock ++, 对各个寄存器状态，查下 lastjmp 表，
			从而知道每个新状态由哪些老的状态而来，同时计算汉明距离。
			*/
			codec->nBestHamming = 0x7ffffffffffffffe;
			codec->nBestStatus_sz = 0;
			//对每个状态，统计最优路径
			for (i = 0; i<codec->reg_status; i++)
			{
				//用于记录最优路径的临时变量
				nBestLastStatus = 0, nBestInput = 0, /*nBestOutput = 0, */nBestHamm = 0x7ffffffffffffffe,
					nBestDHamm = 0;
				//某个状态是由以下几个状态转来的
				for (j = 0; j<codec->data_status; j++)
				{
					//对每种可能的转换可能，计算有转换生成的输出与实际收到的码字的误差。
					/*
					为了处理打孔码，规定输入的卷积码取 v_omit时为忽略
					*/
					nHamm = 0;
					for (ot = 0; ot<codec->code_bitlen; ot++)
					{
						int code = CodeArray[cd*codec->code_bitlen + ot];
						if (code == val_omit)
							continue;
						if (code > maxval) code = maxval;
						if (code < minval) code = minval;
						ev = (codec->table_lastjmp[i].nLastOutput[j] >> (codec->code_bitlen - 1 - ot)) & 0x01;
						if (ev == 0)
						{
							ddd = nsig == 1 ? (code - val_0) : (val_0 - code);
							nHamm += ddd*ddd;
						}
						else
						{
							ddd = nsig == 0 ? (code - val_1) : (val_1 - code);
							nHamm += ddd*ddd;
						}
					}
					//全路径旧汉明距离
					nPathHamm = codec->best_path[codec->table_lastjmp[i].nLastStatus[j]].nCurrentHamming[codec->nClock % 2];
					//新距离
					nPathHamm += nHamm;
					if (nBestHamm > nPathHamm)
					{
						//如果本转换较优,则刷新纪录
						nBestHamm = nPathHamm;
						nBestLastStatus = codec->table_lastjmp[i].nLastStatus[j];
						nBestInput = codec->table_lastjmp[i].nLastInput[j];
						/*nBestOutput  = codec->table_lastjmp[i].nLastOutput[j];*/
						nBestDHamm = nHamm;
					}
				} // end for j
				  //开始刷新镜像路径,双缓存，用找到的最优转换来做为新的路径
				mirror = (codec->nClock + 1) % 2;
				raw = codec->nClock % 2;
				//首先，补全旧的路径的转换参数
				codec->best_path[nBestLastStatus].nHamming[raw][0] = nBestDHamm;
				codec->best_path[nBestLastStatus].nInChain[raw][0] = nBestInput;
				//codec->best_path[nBestLastStatus].nOutChain [raw][0] = nBestOutput;
				//旧路径全部后移一个状态，腾出空间
				for (j = 1; j<codec->L; j++)
				{
					codec->best_path[i].nHamming[mirror][j] = codec->best_path[nBestLastStatus].nHamming[raw][j - 1];
					codec->best_path[i].nInChain[mirror][j] = codec->best_path[nBestLastStatus].nInChain[raw][j - 1];
					//codec->best_path[i].nOutChain[mirror][j] = codec->best_path[nBestLastStatus].nOutChain[raw][j-1];
					//codec->best_path[i].nRegChain[mirror][j] = codec->best_path[nBestLastStatus].nRegChain[raw][j-1];
				}
				//更新汉明，旧路径最末 (L-1)的汉明由于跑出了范围，减去,同时，新的作为首部加入
				codec->best_path[i].nCurrentHamming[mirror] = codec->best_path[nBestLastStatus].nCurrentHamming[raw];
				codec->best_path[i].nCurrentHamming[mirror] -= codec->best_path[nBestLastStatus].nHamming[raw][codec->L - 1];
				codec->best_path[i].nCurrentHamming[mirror] += nBestDHamm;
				//初态传递
				codec->best_path[i].nInitialStatus[mirror] = codec->best_path[nBestLastStatus].nInitialStatus[raw];
				//更新最前端（新）的节点
				codec->best_path[i].nHamming[mirror][0] = 0;
				codec->best_path[i].nInChain[mirror][0] = 0;
				//codec->best_path[i].nOutChain[mirror][0] = 0;
				//codec->best_path[i].nRegChain[mirror][0] = i;
				//刷新最优路径纪录
				if (codec->nBestHamming > codec->best_path[i].nCurrentHamming[mirror])
				{
					codec->nBestHamming = codec->best_path[i].nCurrentHamming[mirror];
					codec->nBestStatus_sz = 0;
				}
				if (codec->nBestHamming == codec->best_path[i].nCurrentHamming[mirror])
					codec->nBestStatus[codec->nBestStatus_sz++] = i;
			}// end for i
			if (codec->nWorstHamming == -1 || codec->nWorstHamming < codec->nBestHamming)
				codec->nWorstHamming = codec->nBestHamming;
			//推进 clock
			codec->nClock++;
			//缓存输出
			if (codec->nClock >= codec->L - 1 || bFinished != 0)
			{
				if (cd + 1 < nBitLen && codec->nClock < codec->L - 1)
					continue;
				raw = codec->nClock % 2;
				nBests = codec->nBestStatus_sz;//最优的 n 条路径
				for (pos = codec->L - 1; pos >0; pos--)
				{
					//统计各个跳转的大数判决
					memset(nStatusCounter, 0, sizeof(int)*codec->data_status);
					for (i = 0; i< nBests; i++)
						nStatusCounter[codec->best_path[codec->nBestStatus[i]].nInChain[raw][pos]] ++;

					nBestStatus = 0;
					nMaxHit = 0;
					for (i = 0; i<codec->data_status; i++)
					{
						if (nMaxHit < nStatusCounter[i])
						{
							nMaxHit = nStatusCounter[i];
							nBestStatus = i;
						}
					}// next i
					if (codec->vec_decoded_data_sz + 1 >= codec->vec_decoded_data_msz)
					{
						tmp = (int *)malloc((codec->vec_decoded_data_msz + 65536) * sizeof(int));
						memcpy(tmp, codec->vec_decoded_data, codec->vec_decoded_data_sz * sizeof(int));
						codec->vec_decoded_data_msz += 65536;
						free(codec->vec_decoded_data);
						codec->vec_decoded_data = tmp;
					}
					codec->vec_decoded_data[codec->vec_decoded_data_sz++] = nBestStatus;
					if (bFinished == 0 || cd + 1 < nBitLen)
						break;
				}//next Pos
			}//end if codec->nClock >= L - 1 || bFinished == true
		}// next cd
		if (bFinished != 0)
		{
			codec->nClock = 0;
			for (i = 0; i<codec->reg_status; i++)
				codec->best_path[i].nCurrentHamming[0] = codec->best_path[i].nCurrentHamming[1] = 0;
		}
		free(nStatusCounter);
		return (int)codec->vec_decoded_data_sz;
	}

	int pop_data(void * pcodec, int DataArray[], int nLen)
	{
		int i;
		int * newalloc = 0;
		clsm_codec_arg * codec = (clsm_codec_arg *)pcodec;
		int cps = nLen<codec->vec_decoded_data_sz ? nLen : codec->vec_decoded_data_sz;

		memcpy(DataArray, codec->vec_decoded_data, cps * sizeof(int));
		for (i = 0; i<codec->vec_decoded_data_sz - cps; i++)
			codec->vec_decoded_data[i] = codec->vec_decoded_data[i + cps];
		codec->vec_decoded_data_sz -= cps;
		if (codec->vec_decoded_data_sz<4096 && codec->vec_decoded_data_msz >4096)
		{
			newalloc = (int *)malloc(4096 * sizeof(int));
			memcpy(newalloc, codec->vec_decoded_data, 4096 * sizeof(int));
			free(codec->vec_decoded_data);
			codec->vec_decoded_data = newalloc;
			codec->vec_decoded_data_msz = 4096;
		}
		return cps;
	}

	long long curr_best_hamming(void * pcodec)
	{
		clsm_codec_arg * codec = (clsm_codec_arg *)pcodec;
		return codec->nBestHamming;
	}


}
