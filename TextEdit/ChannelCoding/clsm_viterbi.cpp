/** \brief ����ļ�ʵ��������״̬��(LSM)ά�ر������� by goldenhawking
*/
#include <stdlib.h>
#include <memory.h>
#include "clsm_viterbi.h"
namespace LSMVIT {
	//�洢�ӵ�ǰ״̬����һ״̬����ת
	typedef  struct {
		int * nNextStatus;	//��һ״̬
		int * nOutput;		//��ת���
	} tag_statusjmp_next;
	//�洢��ǰ״̬�����ĸ���״̬����ת
	typedef  struct {
		int * nLastInput;	//��һ�εĴ���
		int * nLastStatus;	//��һ״̬
		int * nLastOutput;	//��ת���
	} tag_statusjmp_last;
	//����·��������double_buffer����
	typedef  struct {
		long long nCurrentHamming[2];	//��ǰȫ·������
		int nInitialStatus[2];          //��·������ʼ��̬
		long long *nHamming[2];			//������ת�ĺ���
		int *nInChain[2];				//������ת�����루������
										//int nOutChain[2][L];			//������ת�����
										//int nRegChain[2][L];			//����״̬
	} tag_path;


	/** \brief ����״̬����������
	*   ����״̬��������״̬�����û�����Ĵ�������״̬����ת����ת�Ĺ�����
	*   ��ת�����У���������ת������ܵ��ŵ���Ⱦ�����������������������
	*    ͨ��ά�ر��㷨�������������һ�����Ĵ���
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
		int nClock;					//ʱ��
		int * vec_decoded_data;		//�洢�������Ļ���
		int vec_decoded_data_msz;
		int vec_decoded_data_sz;
		long long   nBestHamming;			//��������ǰ�����ź���
		long long   nWorstHamming;          //��������ʷ�������Ѻ���
		int * nBestStatus;			//��������·��������
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

			//��ʼ����ת��
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

		//��ʼ����ת��
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

	/** \brief ����������������������Ѿ�����ʼ��(m_nClock >=0)���������
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
				//�����û��ĺ���
				succeed = cb(i, j, &nNextReg, &nOutput, ud);
				if (succeed != 0)
				{
					item->table_nextjmp[i].nNextStatus[j] = nNextReg;
					item->table_nextjmp[i].nOutput[j] = nOutput;
					//������ʼ��
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


	/** \brief ���б��룬�����û������Ĵ���ָ�������Ϣ������״̬��
	* ��ת�����������
	* \param inputArray[] int �������롣����Ϊ [0 ~ data_status-1] ֮�������ֵ
	* \param nLen int ��Ч������
	* \param outputArray[] int �����������飬����ҲҪ������nLen��
	* \param InitialStatus int �������ĳ�̬��
	* \return int ���������ڵ�״̬���Ժܳ������ݣ����Էֶα��롣
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
			//���
			outputArray[i] = codec->table_nextjmp[nCurrStatus].nOutput[nInput];
			nCurrStatus = codec->table_nextjmp[nCurrStatus].nNextStatus[nInput];
		}
		return nCurrStatus;
	}

	/** \brief ά�ر������㷨�����ŵ�����·���䣬����״̬ʹ�ô����о�
	*
	* \param CodeArray[] int �����������ı�������У�ÿ��Ԫ��ȡֵ [0~2^code_bitlen-1]
	* \param nSymbolLen int ���г���
	* \param bFinished int ��ձ�־���Ƿ��LԼ�������ڵ�����Ԫ��ȫ�������true һ���������һ������
	* \param InitialStatus int �����̬��֪�����Ը������������Ա������ʼ�׶εľ�������
	* \return int Ŀǰ���������ж��ٸ��о����Զ�ȡ��(���� pop_data)
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
			��ÿ�����룬codec->nCLock ++, �Ը����Ĵ���״̬������ lastjmp ��
			�Ӷ�֪��ÿ����״̬����Щ�ϵ�״̬������ͬʱ���㺺�����롣
			*/
			codec->nBestHamming = 0x7ffffffffffffffe;
			codec->nBestStatus_sz = 0;
			//��ÿ��״̬��ͳ������·��
			for (i = 0; i<codec->reg_status; i++)
			{
				//���ڼ�¼����·������ʱ����
				nBestLastStatus = 0, nBestInput = 0, /*nBestOutput = 0, */nBestHamm = 0x7ffffffffffffffe,
					nBestDHamm = 0;
				//ĳ��״̬�������¼���״̬ת����
				for (j = 0; j<codec->data_status; j++)
				{
					//��ÿ�ֿ��ܵ�ת�����ܣ�������ת�����ɵ������ʵ���յ������ֵ���
					/*
					Ϊ�˴������룬�涨����ľ�������ÿ���ʾΪ		m...mm xx...x, ��code_bitlenλ�Ƿ��ţ���code_bitlenλΪ���룬
					�����ʾ��Ӧ����λ�Ƿ���ɾ�����أ�1Ϊ�ǣ�0Ϊ��
					*/
					unsigned int code = CodeArray[cd] % (1 << (unsigned int)codec->code_bitlen);
					unsigned int mask = (~((unsigned int)(CodeArray[cd] >> codec->code_bitlen))) % (1 << (unsigned int)codec->code_bitlen);
					int nErr = (code ^ codec->table_lastjmp[i].nLastOutput[j]);
					nErr &= mask;
					nHamm = 0;
					for (yy = 0; yy <= codec->code_bitlen / 4; yy++)
					{
						//���ñ�����λ�����أ�ͳ�Ʊ���ת���ĺ�������
						nHamm += bitweight[nErr & 0x0f];
						nErr >>= 4;
					}
					//ȫ·���ɺ�������
					nPathHamm = codec->best_path[codec->table_lastjmp[i].nLastStatus[j]].nCurrentHamming[codec->nClock % 2];
					//�¾���
					nPathHamm += nHamm;
					if (nBestHamm > nPathHamm)
					{
						//�����ת������,��ˢ�¼�¼
						nBestHamm = nPathHamm;
						nBestLastStatus = codec->table_lastjmp[i].nLastStatus[j];
						nBestInput = codec->table_lastjmp[i].nLastInput[j];
						/*nBestOutput  = codec->table_lastjmp[i].nLastOutput[j];*/
						nBestDHamm = nHamm;
					}
				} // end for j
				  //��ʼˢ�¾���·��,˫���棬���ҵ�������ת������Ϊ�µ�·��
				mirror = (codec->nClock + 1) % 2;
				raw = codec->nClock % 2;
				//���ȣ���ȫ�ɵ�·����ת������
				codec->best_path[nBestLastStatus].nHamming[raw][0] = nBestDHamm;
				codec->best_path[nBestLastStatus].nInChain[raw][0] = nBestInput;
				//codec->best_path[nBestLastStatus].nOutChain [raw][0] = nBestOutput;
				//��·��ȫ������һ��״̬���ڳ��ռ�
				for (j = 1; j<codec->L; j++)
				{
					codec->best_path[i].nHamming[mirror][j] = codec->best_path[nBestLastStatus].nHamming[raw][j - 1];
					codec->best_path[i].nInChain[mirror][j] = codec->best_path[nBestLastStatus].nInChain[raw][j - 1];
					//codec->best_path[i].nOutChain[mirror][j] = codec->best_path[nBestLastStatus].nOutChain[raw][j-1];
					//codec->best_path[i].nRegChain[mirror][j] = codec->best_path[nBestLastStatus].nRegChain[raw][j-1];
				}
				//���º�������·����ĩ (L-1)�ĺ��������ܳ��˷�Χ����ȥ,ͬʱ���µ���Ϊ�ײ�����
				codec->best_path[i].nCurrentHamming[mirror] = codec->best_path[nBestLastStatus].nCurrentHamming[raw];
				codec->best_path[i].nCurrentHamming[mirror] -= codec->best_path[nBestLastStatus].nHamming[raw][codec->L - 1];
				codec->best_path[i].nCurrentHamming[mirror] += nBestDHamm;
				//��̬����
				codec->best_path[i].nInitialStatus[mirror] = codec->best_path[nBestLastStatus].nInitialStatus[raw];
				//������ǰ�ˣ��£��Ľڵ�
				codec->best_path[i].nHamming[mirror][0] = 0;
				codec->best_path[i].nInChain[mirror][0] = 0;
				//codec->best_path[i].nOutChain[mirror][0] = 0;
				//codec->best_path[i].nRegChain[mirror][0] = i;
				//ˢ������·����¼
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
			//�ƽ� clock
			codec->nClock++;
			//�������
			if (codec->nClock >= codec->L - 1 || bFinished != 0)
			{
				if (cd + 1 < nSymbolLen && codec->nClock < codec->L - 1)
					continue;
				raw = codec->nClock % 2;
				nBests = codec->nBestStatus_sz;//���ŵ� n ��·��
				for (pos = codec->L - 1; pos >0; pos--)
				{
					//ͳ�Ƹ�����ת�Ĵ����о�
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

	/** \brief ά�ر��������о��㷨�����ŵ�����·���䣬����״̬ʹ�����о���
	*
	* \param CodeArray[] int �����������ı�������У�ÿ��Ԫ��ȡֵ v0~v1, vOmit Ϊ��ױ��
	* \param nBitLen int ���еı��س���.
	* \param bFinished int ��ձ�־���Ƿ��LԼ�������ڵ�����Ԫ��ȫ�������true һ���������һ������
	* \param InitialStatus int �����̬��֪�����Ը������������Ա������ʼ�׶εľ�������
	* \return int Ŀǰ���������ж��ٸ��о����Զ�ȡ��(���� pop_data)
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
			��ÿ�����룬codec->nCLock ++, �Ը����Ĵ���״̬������ lastjmp ��
			�Ӷ�֪��ÿ����״̬����Щ�ϵ�״̬������ͬʱ���㺺�����롣
			*/
			codec->nBestHamming = 0x7ffffffffffffffe;
			codec->nBestStatus_sz = 0;
			//��ÿ��״̬��ͳ������·��
			for (i = 0; i<codec->reg_status; i++)
			{
				//���ڼ�¼����·������ʱ����
				nBestLastStatus = 0, nBestInput = 0, /*nBestOutput = 0, */nBestHamm = 0x7ffffffffffffffe,
					nBestDHamm = 0;
				//ĳ��״̬�������¼���״̬ת����
				for (j = 0; j<codec->data_status; j++)
				{
					//��ÿ�ֿ��ܵ�ת�����ܣ�������ת�����ɵ������ʵ���յ������ֵ���
					/*
					Ϊ�˴������룬�涨����ľ����ȡ v_omitʱΪ����
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
					//ȫ·���ɺ�������
					nPathHamm = codec->best_path[codec->table_lastjmp[i].nLastStatus[j]].nCurrentHamming[codec->nClock % 2];
					//�¾���
					nPathHamm += nHamm;
					if (nBestHamm > nPathHamm)
					{
						//�����ת������,��ˢ�¼�¼
						nBestHamm = nPathHamm;
						nBestLastStatus = codec->table_lastjmp[i].nLastStatus[j];
						nBestInput = codec->table_lastjmp[i].nLastInput[j];
						/*nBestOutput  = codec->table_lastjmp[i].nLastOutput[j];*/
						nBestDHamm = nHamm;
					}
				} // end for j
				  //��ʼˢ�¾���·��,˫���棬���ҵ�������ת������Ϊ�µ�·��
				mirror = (codec->nClock + 1) % 2;
				raw = codec->nClock % 2;
				//���ȣ���ȫ�ɵ�·����ת������
				codec->best_path[nBestLastStatus].nHamming[raw][0] = nBestDHamm;
				codec->best_path[nBestLastStatus].nInChain[raw][0] = nBestInput;
				//codec->best_path[nBestLastStatus].nOutChain [raw][0] = nBestOutput;
				//��·��ȫ������һ��״̬���ڳ��ռ�
				for (j = 1; j<codec->L; j++)
				{
					codec->best_path[i].nHamming[mirror][j] = codec->best_path[nBestLastStatus].nHamming[raw][j - 1];
					codec->best_path[i].nInChain[mirror][j] = codec->best_path[nBestLastStatus].nInChain[raw][j - 1];
					//codec->best_path[i].nOutChain[mirror][j] = codec->best_path[nBestLastStatus].nOutChain[raw][j-1];
					//codec->best_path[i].nRegChain[mirror][j] = codec->best_path[nBestLastStatus].nRegChain[raw][j-1];
				}
				//���º�������·����ĩ (L-1)�ĺ��������ܳ��˷�Χ����ȥ,ͬʱ���µ���Ϊ�ײ�����
				codec->best_path[i].nCurrentHamming[mirror] = codec->best_path[nBestLastStatus].nCurrentHamming[raw];
				codec->best_path[i].nCurrentHamming[mirror] -= codec->best_path[nBestLastStatus].nHamming[raw][codec->L - 1];
				codec->best_path[i].nCurrentHamming[mirror] += nBestDHamm;
				//��̬����
				codec->best_path[i].nInitialStatus[mirror] = codec->best_path[nBestLastStatus].nInitialStatus[raw];
				//������ǰ�ˣ��£��Ľڵ�
				codec->best_path[i].nHamming[mirror][0] = 0;
				codec->best_path[i].nInChain[mirror][0] = 0;
				//codec->best_path[i].nOutChain[mirror][0] = 0;
				//codec->best_path[i].nRegChain[mirror][0] = i;
				//ˢ������·����¼
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
			//�ƽ� clock
			codec->nClock++;
			//�������
			if (codec->nClock >= codec->L - 1 || bFinished != 0)
			{
				if (cd + 1 < nBitLen && codec->nClock < codec->L - 1)
					continue;
				raw = codec->nClock % 2;
				nBests = codec->nBestStatus_sz;//���ŵ� n ��·��
				for (pos = codec->L - 1; pos >0; pos--)
				{
					//ͳ�Ƹ�����ת�Ĵ����о�
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
