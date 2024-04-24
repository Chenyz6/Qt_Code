#ifndef CLSM_VITERBI_H_INCLUDED
#define CLSM_VITERBI_H_INCLUDED
/*!���ͷ�ļ�����������״̬��ά�ر��������ͱ������ĺ���
* ʹ�÷����ǣ�
* 1��new_lsm_codec ������������;
* 2��init_status ��ʼ��״̬��
* 3��encode ���루ֱ�ӳ��������decode��decode_soft���룬pop_data����������
* 4��curr_best_hamming ���ص�ǰ�����������ʣ�L�����£�����ı��أ�
* 5��reset_status ���ã�delete_lsm_codec ɾ��
* ���ǣ����������������Ҫ1��2,�Գ����ľ���룬�Ѿ���һ�������8���Ƶ�·ͼ����������conv_lsm_codec.h�
*/
namespace LSMVIT {
	//���к���ֻ���ڷǱ�׼���Զ���ʽ����������£����Զ���״̬��ʹ��
	typedef  int(*cb_status) (int, int, int*, int *, void *);
	void * new_lsm_codec(const int reg_status, const int data_status, const int code_bitlen, const int L);
	void delete_lsm_codec(void * codec);
	int init_status(void * codec, cb_status cb, void * ud);
	//���к�����ͨ�ú���
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
