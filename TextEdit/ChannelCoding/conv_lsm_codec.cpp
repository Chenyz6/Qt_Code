#include <stdlib.h>
#include <memory.h>
#include "conv_lsm_codec.h"
#include "clsm_viterbi.h"
/** \brief 本文件实现了一般卷积码的状态机回调函数*/
namespace LSMVIT {
	void * new_conv_maker(int n, int k, int m, const int * poctPins/*k*n*/, const int * pfbPins/*k*k*/);
	void delete_conv_maker(void * maker);
	int cal_conv_status(int reg, int data, int *pNextReg, int * pOutput, void * maker);


	typedef struct {
		int n;
		int k;
		int m;
		int * OutPin/*[n][k][m+1]*/;
		int * FeedBackPin/*[k][k][m+1]*/;
	} conv_lsmaker;
	/** \brief 根据抽头初始化本类对象
	*
	* \param octPins[k][n] int 抽头，分别是第k组寄存器对第n个输出管脚的影响
	* \param fbPins[k][k] int 反馈抽头，分别是第k组寄存器对第k个输入管脚的影响
	* \return void
	*
	*/
	void * new_conv_maker(int n, int k, int m, const int * poctPins/*k*n*/, const int * pfbPins/*k*k*/)
	{
		int i, j, v;
		conv_lsmaker * item = (conv_lsmaker *)malloc(sizeof(conv_lsmaker));
		item->n = n;
		item->k = k;
		item->m = m;
		item->OutPin = (int *)malloc(sizeof(int)*n*k*(m + 1));
		if (pfbPins)
			item->FeedBackPin = (int *)malloc(sizeof(int)*k*k*(m + 1));
		else
			item->FeedBackPin = 0;
		for (i = 0; i<k; i++)
		{
			for (j = 0; j<n; j++)
			{
				int noctp = poctPins[i*n + j];
				int cpin = 0, ct = 0;
				while (noctp)
				{
					cpin += (noctp % 10) << (ct * 3);
					noctp /= 10;
					ct++;
				}//end while
				for (v = 0; v < m + 1; v++)
					item->OutPin[j*(k*(m + 1)) + i * (m + 1) + v] = (cpin >> (m - v)) & 0x01;
			}// end j
		}// end i
		if (pfbPins)
		{
			for (i = 0; i<k; i++)
			{
				for (j = 0; j<k; j++)
				{
					int noctp = pfbPins[i*k + j];
					int cpin = 0, ct = 0;
					while (noctp)
					{
						cpin += (noctp % 10) << (ct * 3);
						noctp /= 10;
						ct++;
					}//end while
					for (v = 0; v < m + 1; v++)
						item->FeedBackPin[j*k*(m + 1) + i*(m + 1) + v] = (cpin >> (m - v)) & 0x01;
				}// end j
			}// end i
		}

		return item;
	}
	void delete_conv_maker(void * maker)
	{
		conv_lsmaker * pmaker = 0;
		if (maker == 0)
			return;
		pmaker = (conv_lsmaker *)maker;

		free(pmaker->OutPin);
		if (pmaker->FeedBackPin)
			free(pmaker->FeedBackPin);
		free(pmaker);
	}
	/** \brief 回调函数，会递交给状态机
	*/
	int cal_conv_status(int reg, int data, int *pNextReg, int * pOutput, void * maker)
	{
		int i, j, v, n, k, m;
		conv_lsmaker * pmaker = 0;
		int * nOutput = 0;
		int * nReg = 0;
		int newreg = 0, out = 0;
		if (pNextReg == 0 || pOutput == 0 || maker == 0) return 0;
		pmaker = (conv_lsmaker *)maker;
		n = pmaker->n;
		k = pmaker->k;
		m = pmaker->m;
		nOutput = (int *)malloc(sizeof(int)*n);
		nReg = (int *)malloc(sizeof(int)*k*(m + 1));


		for (i = 0; i< k; i++)
		{
			nReg[i*(m + 1) + 0] = (data >> (k - i - 1)) & 0x01;
			for (j = 0; j<m; j++)
				nReg[i*(m + 1) + j + 1] = (reg >> ((k - i - 1)*m + (m - 1 - j))) & 0x01;
		}// end i
		if (pmaker->FeedBackPin)
		{
			//反馈
			for (i = 0; i<k; i++)
			{
				int cc = 0;
				for (j = 0; j<k; j++)
				{
					for (v = 0; v <m + 1; v++)
						cc += pmaker->FeedBackPin[i*(m + 1)*k + j*(m + 1) + v] * nReg[j*(m + 1) + v];
				}// end j
				nReg[i*(m + 1) + 0] += cc;
				nReg[i*(m + 1) + 0] %= 2;
			}//end i

		}
		//输出

		for (i = 0; i<n; i++)
		{
			int cc = 0;
			for (j = 0; j<k; j++)
			{
				for (v = 0; v <m + 1; v++)
					cc += pmaker->OutPin[i*k*(m + 1) + j*(m + 1) + v] * nReg[j*(m + 1) + v];
			}// end j
			nOutput[i] = cc % 2;
		}//end i
		 //下一状态
		newreg = 0, out = 0;
		for (i = 0; i<n; i++)
		{
			out <<= 1;
			out += nOutput[i];
		}
		for (j = 0; j<k; j++)
		{
			for (v = 0; v < m; v++)
			{
				newreg <<= 1;
				newreg += nReg[j*(m + 1) + v];
			}
		}
		*pNextReg = newreg;
		*pOutput = out;
		free(nOutput);
		free(nReg);
		return -1;
	} // end function cb_status
	void * new_conv_codec(int n, int k, int m, const int * poctPins/*k*n*/, const int * pfbPins/*k*k*/, int L)
	{
		const int nRegStatus = (1 << (m*k)), nInStatus = 1 << k;
		void * codec = new_lsm_codec(nRegStatus, nInStatus, n, L);
		void * maker = new_conv_maker(n, k, m, poctPins, pfbPins);
		init_status(codec, cal_conv_status, maker);
		delete_conv_maker(maker);
		return codec;
	}

}
