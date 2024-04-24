#ifndef CLSM_VITERBI_H_INCLUDED
#define CLSM_VITERBI_H_INCLUDED
/*!这个头文件定义了有限状态机维特比译码器和编码器的函数
* 使用方法是：
* 1、new_lsm_codec 创建编译码器;
* 2、init_status 初始化状态机
* 3、encode 编码（直接出结果），decode、decode_soft译码，pop_data弹出译码结果
* 4、curr_best_hamming 返回当前译码器误码率（L长度下，错误的比特）
* 5、reset_status 重置，delete_lsm_codec 删除
* 但是，不是所有情况都需要1～2,对常见的卷积码，已经有一个方便的8进制电路图生成器，在conv_lsm_codec.h里。
*/
namespace LSMVIT {
	//下列函数只有在非标准线性多项式等特殊情况下，供自定义状态机使用
	typedef  int(*cb_status) (int, int, int*, int *, void *);
	void * new_lsm_codec(const int reg_status, const int data_status, const int code_bitlen, const int L);
	void delete_lsm_codec(void * codec);
	int init_status(void * codec, cb_status cb, void * ud);
	//下列函数是通用函数
	void reset_status(void *codec);
	int encode(void * codec, const int inputArray[], int nLen, int outputArray[], int InitialStatus);
	int decode(void * codec, const int CodeArray[], int nSymbolLen, int bFinished/*, int InitialStatus*/);
	int decode_soft(void * codec, const int CodeArray[], int nBitLen, int v0, int v1, int vOmit, int bFinished/*, int InitialStatus*/);
	int pop_data(void * codec, int DataArray[], int nLen);
	long long curr_best_hamming(void * codec);
	long long worst_hamming(void * item);
	int get_initial_status(void * item);
}
#endif // CLSM_VITERBI_H_INCLUDED
