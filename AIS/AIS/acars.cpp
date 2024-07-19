#include "ACARS.h"
#include "DataFIRDF.h"
#include "DataRead_Write.h"
#include "SgnlPrcsDll.h"
#include "SignalDemodProbe.h"
#include "basicFunc.h"
#include "fdacoefs.h"
#include <cstdint>
#include <math.h>
#include <stdio.h>
#include <QDebug>

ACARS::ACARS() {
  m_acars_carrier_freq = 1.8e3;
  m_symbol_rate = 2.4e3;
  m_modu_type = SMT_MSK; // 其实是AM-MSK
}

ACARS::~ACARS() {
  // delete[] m_demod_bit;
  // delete[] m_demod_constellation_len;
  // delete[] m_decode_bit;
  // delete[] m_phase_corr_result;
  // delete[] m_sync_head_bit;
  // delete[] m_iq_data;
}

bool ACARS::data_input(string file_name, uint32_t data_type,
                       uint32_t data_len) {
  FILE *fp = fopen(file_name.data(), "rb");
  if (fp == nullptr) {
    fprintf(stderr, "文件打开错误！请检查文件地址！\n");
    return false;
  }

  m_iq_data_len = data_len;
  m_iq_data = new short[m_iq_data_len];
  if (m_iq_data == nullptr) {
    fprintf(stderr, "申请内存出现问题！\n");
    return false;
  }

  fread(m_iq_data, data_len, sizeof(short), fp);
  return true;
}

void ACARS::decode_msk2acars() {
  if (m_demod_bit_len < 0) {
    fprintf(stdout, "[Error]: demod_bit_len = 0;\n");
    return;
  }
  m_decode_bit = new char[m_demod_bit_len];
  char *bitbuff2 = new char[m_demod_bit_len];
  m_decode_bit[0] = '0';
  bitbuff2[0] = 0;

  // 解码得到的码元是数字01，可以直接进行异或
  for (int i = 1; i < m_demod_bit_len; i++) {
    bitbuff2[i] = (m_demod_bit[i] ^ 1) ^ bitbuff2[i - 1];
  }

  // decode处理的是符号01
  for (int i = 0; i < m_demod_bit_len; i++) {
    if (bitbuff2[i] == 1)
      m_decode_bit[i] = '1';
    else
      m_decode_bit[i] = '0';
  }

  bin = m_decode_bit;
  delete[] bitbuff2;
  // return TRUE;
}

void ACARS::demod_hilbertfilter(double *pData, int nDataLen) {
  double *m_pDataI = new double[lplen];
  int i;
  double dTemp;
  for (int k = 0; k < nDataLen; k++) {
    for (i = lplen - 1; i > 0; i--) {
      *(m_pDataI + i) = *(m_pDataI + i - 1);
    }

    m_pDataI[0] = *(pData + k);

    dTemp = 0.0;
    for (i = 0; i < lplen; i++) {
      dTemp += lpbuf[i] * m_pDataI[i];
    }

    *(pData + k) = dTemp;
  }
  delete[] m_pDataI;
}

void ACARS::demodulation(double sample_freq) {

  string sResult;
  double acars_singal_bandwidth = 4e3;
  double *AMdemod_data = new double[m_iq_data_len];
  double *AMdemod_dataI = new double[m_iq_data_len / 2];
  double *AMdemod_dataQ = new double[m_iq_data_len / 2];
  double *DataI = new double[m_iq_data_len / 2];
  double *DataQ = new double[m_iq_data_len / 2];
  short *ACARSdata = new short[m_iq_data_len];
  // 确定时间t
  double *t = new double[m_iq_data_len / 2];
  for (int i = 0; i < m_iq_data_len / 2; i++)
    t[i] = i * 1.0 / sample_freq;
  // 包络检波
  // 平方和开根号
  for (int i = 0; i < m_iq_data_len / 2; i++) {
    DataI[i] = sqrt(m_iq_data[2 * i] * m_iq_data[2 * i] +
                    m_iq_data[2 * i + 1] * m_iq_data[2 * i + 1]);
  }
  // 希尔伯特变换
  memcpy(DataQ, DataI, m_iq_data_len / 2);
  demod_hilbertfilter(DataQ, m_iq_data_len / 2);
  for (int i = 0; i < m_iq_data_len / 2; i++) {
    ACARSdata[2 * i] = DataI[i];
    ACARSdata[2 * i + 1] = DataQ[i];
  }

  // 信号下变频
  for (int i = 0; i < m_iq_data_len / 2; i++) {
    AMdemod_dataI[i] =
        ACARSdata[2 * i] * 1.0 * cos(2 * PI * (-m_acars_carrier_freq) * t[i]) -
        ACARSdata[2 * i + 1] * 1.0 *
            sin(2 * PI * (-m_acars_carrier_freq) * t[i]);
    AMdemod_dataQ[i] = cos(2 * PI * (-m_acars_carrier_freq) * t[i]) *
                       ACARSdata[2 * i + 1] * 1.0;
  }

  // 低通滤波
  short nLPFType = FIRLPFDT_Hamming;
  double dPasswidth = acars_singal_bandwidth / sample_freq;
  double dStopwidth =
      g_dllStopwidthFactor * acars_singal_bandwidth / sample_freq;
  double dPassRipple = 0.01;
  double dStopRipple = 60;
  CDataFIRDF *m_pFilter;
  m_pFilter = new CDataFIRDF;
  BOOL a = m_pFilter->IinitialLPFPara(nLPFType, dPasswidth, dStopwidth,
                                      dPassRipple, dStopRipple); // 滤波器初始化
  m_pFilter->Filter(AMdemod_dataI, AMdemod_dataQ, m_iq_data_len / 2);
  for (int i = 0; i < m_iq_data_len / 2; i++) {
    AMdemod_data[2 * i] = AMdemod_dataI[i];
    AMdemod_data[2 * i + 1] = AMdemod_dataQ[i];
  }

  /* MSK解调 */
  DWORD Fsize = m_iq_data_len;

  // double* dData = AMdemod_data;
  CSignalDemodProbe *stCSignalDemodProbe;
  stCSignalDemodProbe = new CSignalDemodProbe;
  DWORD dwBitNum = 0;
  stCSignalDemodProbe->InitDemodParam(sample_freq, m_symbol_rate, 0,
                                      m_modu_type, false);
  stCSignalDemodProbe->InputData(AMdemod_data, Fsize);
  m_demod_bit = stCSignalDemodProbe->GetSignalSymbol(dwBitNum);
  m_demod_bit_len = dwBitNum;

  delete[] ACARSdata;
  delete[] AMdemod_data;
  delete[] AMdemod_dataI;
  delete[] AMdemod_dataQ;
  delete[] t;
  delete m_pFilter;
  delete[] DataI;
  delete[] DataQ;
};

/*
 * 初始化
 **/
void ACARS::decode_initial() {

  searchRet.matchIndex = -1;
  searchRet.flag_bit_not = false;
  searchRet.len = 0;
  allInfo.clear();
}

bool ACARS::decode_search() {

  bool ret = false;
  int len = bin.length();
  int startIndex = searchRet.matchIndex + 1;
  string syncHead = "1101010101010100011010000110100010000000";
  string syncHead_ = bitNot(syncHead);
  for (int i = startIndex; i < len - 1930; i++) {
    string vague = bin.substr(i, 40);
    if (vague == syncHead) {
      searchRet.matchIndex = i;
      searchRet.flag_bit_not = false;
      searchRet.len = len - i;
      ret = true;
      break;
    }
    if (vague == syncHead_) {
      searchRet.matchIndex = i;
      searchRet.flag_bit_not = true;
      searchRet.len = len - i;
      ret = true;
      break;
    }
  }
  return ret;
}

bool ACARS::decode_unpackage() {

  string syncHead = "1101010101010100011010000110100010000000";
  string syncHead_ = bitNot(syncHead);
  allInfo.clear();
  Record oneRec;
  string y = bin.substr(searchRet.matchIndex + 40, 1896);
  if (y.length() < 1896)
    return false;
  if (searchRet.flag_bit_not)
    y = bitNot(y);
  int yf = 0; // 含头指示
  int jt = 0; // 头位置
  int kt;
  int num = 0;
  for (int j = 1; j <= y.length() - 39; j++) {
    string vague = y.substr(j - 1, 40);
    if (sum(Xor(vague, syncHead)) < 3) {
      yf = 1;
      jt = j;
      break;
    }
    if (sum(Xor(vague, syncHead_)) < 3) {
      yf = 1;
      jt = j;
      break;
    }
  }

  if (yf == 1)
    kt = jt - 1;
  else
    kt = 1896;
  int yend = 0;
  if (y.length() > 8) {
    for (int k = 1; k <= kt - 31; k++) {
      string t = y.substr(k - 1, 8) + y.substr(k - 1 + 24, 8);
      if (t == "1110100111111110" || t == "1100000111111110") {
        yend = k + 23;
        break;
      }
    }

    if (yend < 23 && yf == 1)
      yend = jt;
    if (yend < 23 && yf == 0)
      yend = 1865 + 23;
  }

  if (yend > 23) {
    num++;
    string z = y.substr(0, yend);
    string zc = z.substr(0, z.length() - 16);
    // string crc_compare=z.Right(16);
    string crc_compare = z.substr(z.length() - 16, 16);
    string crc_bcs = decode_crc_check(zc);
    if (crc_compare == crc_bcs) {
      oneRec.field = "CRC Check";
      oneRec.val = "OK";
    } else {
      oneRec.field = "CRC Check";
      oneRec.val = "Error";
    }
    allInfo.push_back(oneRec);

    string zb = z.substr(0, z.length() - 24);
    int kb = zb.length() / 8;
    vector<string> zu;
    for (int k = 1; k <= kb; k++) {
      string a = zb.substr((k - 1) * 8, 8);
      string tmp = fliplr(a.substr(0, 7));
      zu.push_back(tmp);
    }

    try {
      string M = binstr2ascii(zu.at(0), 7, "left-msb");
      int M_value = binstr2decnum(zu.at(0), "left-msb");
      if (M_value == 50)
        // oneRec.field.Format("M=A(%s)"),M);
        oneRec.field = "M=A(" + M + ")";
      else if (M_value > 63 && M_value < 94)
        // oneRec.field.Format("M=%d(%s)"),M_value-64,M);
        oneRec.field =
            "M=" + to_string(M_value - 64) + "(" + M + ")";
      else
        // oneRec.field.Format("M=2(%s)"),M);
        oneRec.field = "M=2(" + M + ")";
      oneRec.val = "";
      allInfo.push_back(oneRec);

      string ADDR = "";
      for (int i = 1; i <= 7; i++)
        ADDR += binstr2ascii(zu.at(i), 7, "left-msb");
      // oneRec.field.Format("ADDR=%s"),ADDR);
      oneRec.field = "ADDR=" + ADDR;
      allInfo.push_back(oneRec);

      string TA = binstr2ascii(zu.at(8), 7, "left-msb");
      // oneRec.field.Format("TA=%s"),TA);
      oneRec.field = "TA=" + TA;
      allInfo.push_back(oneRec);

      string ML = binstr2ascii(zu.at(9), 7, "left-msb") +
                  binstr2ascii(zu.at(10), 7, "left-msb");
      // oneRec.field.Format("ML=%s"),ML);
      oneRec.field = "ML=" + ML;
      allInfo.push_back(oneRec);

      string B = binstr2ascii(zu.at(11), 7, "left-msb");
      // oneRec.field.Format("B=%s"),B);
      oneRec.field = "B=" + B;
      allInfo.push_back(oneRec);

      if (kb > 22) {
        string FID = "";
        for (int i = 18; i <= 23; i++)
          FID += binstr2ascii(zu.at(i - 1), 7, "left-msb");
        string MSN = binstr2ascii(zu.at(13), 7, "left-msb") +
                     binstr2ascii(zu.at(14), 7, "left-msb") +
                     binstr2ascii(zu.at(15), 7, "left-msb") +
                     binstr2ascii(zu.at(16), 7, "left-msb");
        int MSN1 = binstr2decnum(zu.at(13), "left-msb");
        int MSN2 = binstr2decnum(zu.at(14), "left-msb");
        int MSN3 = binstr2decnum(zu.at(15), "left-msb");
        int MSN4 = binstr2decnum(zu.at(16), "left-msb");
        if (MSN1 > 47 && MSN2 > 47 && MSN3 > 47 && MSN4 > 47 && MSN1 < 123 &&
            MSN2 < 123 && MSN3 < 123 && MSN4 < 123) {
          // oneRec.field.Format("MSN=%s"),MSN);
          oneRec.field = "MSN=" + MSN;
          allInfo.push_back(oneRec);
          // oneRec.field.Format("FID=%s"),FID);
          oneRec.field = "FID=" + FID;
          allInfo.push_back(oneRec);
          string data = "";
          for (int i = 24; i <= zu.size(); i++)
            data += binstr2ascii(zu.at(i - 1), 7, "left-msb");
          oneRec.field = data;
          allInfo.push_back(oneRec);
        } else {
          string data = "";
          for (int i = 14; i <= zu.size(); i++)
            data += binstr2ascii(zu.at(i - 1), 7, "left-msb");
          oneRec.field = data;
          allInfo.push_back(oneRec);
        }
      }
    } catch (out_of_range &e) {
    }
  }
  return true;
}

string ACARS::decode_crc_check(string str) {

  int len = str.length();
  string reg('0', 16);
  str += reg;
  for (int i = 1; i <= len + 16; i++) {
    string tmp;
    string bb = reg;
    reg = "0" + reg.substr(0, 15);
    int reg1 = (_ttoi(str.substr(i - 1, 1).c_str()) +
                _ttoi(bb.substr(15, 1).c_str())) %
               2;
    // tmp.Format("%d"),reg1);
    tmp = to_string(reg1);
    // reg.SetAt(0,tmp.GetAt(0));
    reg[0] = tmp[0];
    int reg13 =
        (_ttoi(bb.substr(15, 1).c_str()) + _ttoi(bb.substr(11, 1).c_str())) % 2;
    // tmp.Format("%d"),reg13);
    tmp = to_string(reg13);
    // reg.SetAt(12,tmp.GetAt(0));
    reg[12] = tmp[0];
    int reg6 =
        (_ttoi(bb.substr(15, 1).c_str()) + _ttoi(bb.substr(4, 1).c_str())) % 2;
    // tmp.Format("%d"),reg6);
    tmp = to_string(reg6);
    // reg.SetAt(5,tmp.GetAt(0));
    reg[5] = tmp[0];
  }
  return fliplr(reg);
}

void ACARS::decode_processing(vector<int> flag) {
  bin = preProcessing(bin, flag);
}

void ACARS::decode() {
  string temp_a;
  string temp_b;
  string result;

  decode_msk2acars(); // 解MSK码元，得到ACARS码元

  bool f1, f2;
  int32_t N = allInfo.size(); // 计算数据大小

  while (true) {
    decode_initial();
    f1 = decode_search();
    f2 = decode_unpackage();
    bin = bin.substr(searchRet.matchIndex + 1);

    if (f1 & f2) {
      for (int i = 0; i < allInfo.size(); i++) {
        temp_a = allInfo[i].field;
        temp_b = temp_b + temp_a + "\n";
      }
    }

    if (searchRet.matchIndex == -1) {
      result = temp_b;
      qDebug() << result.data();
      return;
    }
  }
}
