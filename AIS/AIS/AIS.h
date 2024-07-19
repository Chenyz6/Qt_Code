#pragma once

#include "basicFunc.h"
#include <cstdint>
#include <cstring>
#include <iostream>
#include <vector>

using namespace std;

class AIS {
public:
  AIS();
  ~AIS();

  bool data_input(string file_name, uint32_t data_type,
                  uint32_t data_len); // 指定文件数据长度
  bool data_input(string file_name, uint32_t data_typ); // 读取整个文件
  void demodulation(uint32_t sample_freq);
  string decode();
  void phase_corrlation();

  // 稀有变量访问函数
  char *get_demod_bit();

private:
  // 解码相关函数
  void decode_initial();                    // 解码初始化
  bool decode_search();                     // 寻找同步头
  bool decode_unpackage();                  // 解包
  void decode_processing(vector<int> flag); //
  string decode_crc_cheak(string str);      // CRC校验
  void decode_nazi();

  bool decode_fcs(string str);
  string decode_fcs_div(string seqone, string genpoly, int k);
  string decode_ascii6(string str);

  string bin;
  SearchRet searchRet;
  vector<Record> allInfo;

  // 信号IQ数据
  short *m_iq_data;      // 信号数据
  int32_t m_iq_data_len; // 信号数据点数(一个iq为两点)

  // 协议参数
  double m_symbol_rate; // 符号速率
  uint16_t m_modu_type; // 调制类型

  // 解调星座图与比特流(数字01)
  char *m_demod_bit;
  uint32_t m_demod_bit_len;

  long *m_demod_constellation;
  uint32_t *m_demod_constellation_len;

  // 解码比特流(字符01)
  char *m_decode_bit;

  // 相位相关结果
  double *m_phase_corr_result;

  // 报文同步头(字符01)
  char *m_sync_head_bit;

private:
  bool FCS_decode(string str);
  string FCS_div(string seqone, string genpoly, int k);
  string ascii6(string str);
  void re_message1(string str);
  void re_message2(string str);
  void re_message3(string str);
  void re_message4(string str);
  void re_message5(string str);
  void re_message6(string str);
  void re_message7(string str);
  void re_message8(string str);
  void re_message9(string str);
  void re_message10(string str);
  void re_message11(string str);
  void re_message12(string str);
  void re_message13(string str);
  void re_message14(string str);
  void re_message15(string str);
  void re_message16(string str);
  void re_message17(string str);
  void re_message18(string str);
  void re_message19(string str);
  void re_message20(string str);
  void re_message21(string str);
  void re_message22(string str);
};
