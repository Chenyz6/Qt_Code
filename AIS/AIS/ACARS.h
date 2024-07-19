#pragma once

#include "basicFunc.h"
#include <cstdint>
#include <cstring>
#include <iostream>
#include <vector>

using namespace std;

class ACARS {
public:
  ACARS();
  ~ACARS();

  bool data_input(string file_name, uint32_t data_type,
                  uint32_t data_len); // 指定文件数据长度
  bool data_input(string file_name, uint32_t data_typ); // 读取整个文件
  void demodulation(double sample_freq);                // ACARS解调
  void decode();                                        // 报文解析
  void phase_corrlation();                              // 相位相关

  // 私有变量访问函数
  char *get_demod_bit();

private:
  // 解码相关函数
  void decode_initial();                    // 解码初始化
  bool decode_search();                     // 寻找同步头
  bool decode_unpackage();                  // 解包
  void decode_processing(vector<int> flag); //
  string decode_crc_check(string str);      // CRC校验
  void decode_msk2acars();

  void demod_hilbertfilter(double *pData, int nDataLen);

  string bin;
  SearchRet searchRet;
  vector<Record> allInfo;

  // 信号IQ数据
  short *m_iq_data;      // 信号数据
  int32_t m_iq_data_len; // 信号数据点数（iq为两点）

  // 协议参数
  double m_symbol_rate; // 符号速率
  uint16_t m_modu_type; // 调制类型

  // 解调星座图与比特流(数字01)
  char *m_demod_bit;
  int32_t m_demod_bit_len;

  long *m_demod_constellation;
  int32_t m_demod_constellation_len;

  // 解码比特流(字符01)
  char *m_decode_bit;

  // 相位相关结果
  double *m_phase_corr_result;

  // 报文同步头(字符01)
  char *m_sync_head_bit;

  double m_acars_carrier_freq; // acars调制副载波
};
