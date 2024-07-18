#include <cstdio>
#include <iostream>
#include <stdbool.h>
#include <string>

using namespace std;

typedef struct {
  unsigned long bit_num;       /* 统计的码元个数 */
  unsigned long bit_error_num; /* 错误的码元个数 */
  double ber;                  /* 误码率 */
} ber_test_result;

/*
 * @ description:   该函数假设发射源Transmitter一直循环发送transmit_seq
 * @ result:        结构体指针，最后的计算结果会保存到该结构体中
 * @ start_postion: 起始位置
 * @ transmit_seq:  发射序列
 * @ demod_seq:     解调所得序列
 **/
bool bit_error_rate(ber_test_result *result, unsigned long start_postion,
                    string transmit_seq, string demod_seq) {
  if (!transmit_seq.size() || !demod_seq.size() ||
      (demod_seq.size() < transmit_seq.size())) {
    /* 对发射序列长度、解调序列产长度做检查。若不满足要求，可以打印相关信息 */

    /* coding here... */

    return false;
  }

  result->bit_num = demod_seq.size();
  result->bit_error_num = 0;

  /* 根据start_postion删去字节 */
  demod_seq.erase(0, start_postion);

  /* 在demod_seq中找到transmit_seq出现的位置，匹配区间长度默认取40bits。如果transmit_seq，长度小于512，则取30bits
   */
  int match_range;

  if (transmit_seq.size() > 512) {
    match_range = 40;
  } else {
    match_range = 30;
  }

  /* 开始匹配，如果在 4 * transmit_seq 个循环内还未匹配成功，则返回错误 */
  string::size_type n = 0;
  n = demod_seq.substr(0, 4 * transmit_seq.size())
          .find(transmit_seq.substr(0, match_range));
  if (n == string::npos) {
    /* 在demod_seq中无法匹配到transmit_seq */

    /* coding here */

    return false;
  }
  if (n > 0) {
    demod_seq.erase(0, n);
  }

  /* 统计错误个数 */
  char *transmit_seq_char = transmit_seq.data();
  char *demod_seq_char = demod_seq.data();

  for (int i = 0, j = 0; i < demod_seq.size(); i++) {
    /* 若相同，按位异或后结果为0 */
    result->bit_error_num += transmit_seq_char[j] ^ demod_seq_char[i];
    j = (j == transmit_seq.size() - 1) ? 0 : (j + 1);
  }

  /* 计算误码率 */
  result->ber = (double)result->bit_error_num / (double)result->bit_num;
  return true;
}

/* 测试程序 */
int main(int argc, char *argv[]) {
  ber_test_result test;

  /* 读取数据 */
  int check_len = 100e3;
  FILE *fp_demod =
      fopen("./BPSK_DATALIST_TEST_2024-07-16 "
            "16.02.13USRPData_lxfx_F-0.00061MHz_BPSK_SR32.012939kBd_bit.txt",
            "rb");
  char *demod_bit = (char *)malloc(sizeof(char) * check_len);
  fread(demod_bit, sizeof(char), check_len, fp_demod);

  FILE *fp_trans = fopen("./input_data_512.txt", "rb");
  char *trans_bit = (char *)malloc(sizeof(char) * 512);
  fread(trans_bit, sizeof(char), 512, fp_trans);

  fclose(fp_demod);
  fclose(fp_trans);

  string ttt = trans_bit;
  string ddd = demod_bit;

  bit_error_rate(&test, 3, ttt, ddd);

  cout << "BER: " << test.ber << "/" << test.bit_num << " = " << test.ber
       << endl;
  return 0;
}
