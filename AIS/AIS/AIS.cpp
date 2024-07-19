

#include "AIS.h"
#include "DataFIRDF.h"
#include "DataRead_Write.h"
#include "SgnlPrcsDll.h"
#include "SignalDemodProbe.h"
// #include "SignalParamProbe.h"
#include "basicFunc.h"
#include "fdacoefs.h"
#include <cstdint>
#include <cstdio>
#include <math.h>
// #include <ostream>
#include <stdio.h>
#include <QCoreApplication>
#include <QString>
#include <QMap>
#include <QDebug>

AIS::AIS() {
  m_symbol_rate = 9.6e3;  // AIS符号速率
  m_modu_type = SMT_GMSK; // AIS调制方式
}

AIS::~AIS() {}

bool AIS::data_input(string file_name, uint32_t data_type, uint32_t data_len) {
  FILE *fp = fopen(file_name.data(), "rb");
  if (fp == nullptr) {
    fprintf(stderr, "文件打开错误！请检查文件地址！");
    return false;
  }

  m_iq_data_len = data_len;
  m_iq_data = new short[m_iq_data_len];
  if (m_iq_data == nullptr) {
    fprintf(stderr, "无法开辟堆内存！");
    return false;
  }

  fread(m_iq_data, data_len, sizeof(short), fp);
  return true;
}
struct LocationData {
    QString mmsi;
    qreal longitude;
    qreal latitude;
};

void parseAndSave(const QString &message) {
    QMap<QString, LocationData> locationMap;
    QStringList lines = message.split('\n');

    for (const QString &line : lines) {
        int colonPos = line.indexOf(':');
        if (colonPos != -1) {
            QString key = line.left(colonPos).trimmed();
            QString value = line.mid(colonPos + 1).trimmed();

            if (key == "用户识别码") {
                locationMap[value].mmsi = value;
            } else if (key == "经度") {
                int degreePos = value.indexOf("度");
                if (degreePos != -1) {
                    locationMap[value.left(3)].longitude = value.mid(3, degreePos - 3).toDouble();
                }
            } else if (key == "纬度") {
                int degreePos = value.indexOf("度");
                if (degreePos != -1) {
                    locationMap[value.left(3)].latitude = value.mid(3, degreePos - 3).toDouble();
                }
            }
        }
    }

    for (auto it = locationMap.begin(); it != locationMap.end(); ++it) {
        qDebug() << "MMSI:" << it->mmsi << ", Longitude:" << it->longitude << ", Latitude:" << it->latitude;
    }
}
void AIS::decode_nazi() {
  // 开辟内存空间
  m_decode_bit = new char[m_demod_bit_len];
  m_decode_bit[0] = '0';

  for (int i = 1; i < m_demod_bit_len; i++) {
    if (m_demod_bit[i] == m_demod_bit[i - 1]) {
      m_decode_bit[i] = '1';
    } else {
      m_decode_bit[i] = '0';
    }
  }

  // printf("Nazi解码后输出\n");
  // for (int i = 0; i < m_demod_bit_len; i++) {
  //   printf("%c", m_decode_bit[i]);
  // }
  // printf("\n");
}

void AIS::decode_processing(vector<int> flag) {
  bin = preProcessing(bin, flag);
}

string AIS::decode() {

  string temp_a;
  string temp_b;
  string sResult;

  // printf("解调后输出2：\n");
  // for (int i = 0; i < m_demod_bit_len; i++) {
  //   printf("%d", m_demod_bit[i]);
  // }
  // printf("\n");

  decode_nazi();
  bin = m_decode_bit;

  // for (int i = 0; i < m_demod_bit_len; i ++) {
  //     printf("%d", m_demod_bit[i]);
  // }

  bool f1, f2; ////第一次搜索结果判断
  // f1 = p.search();
  // printf("%d ",f1);					////第一次搜索结果为1
  // f2 = p.unpackage();
  // printf("%d ", f2);				////第二次搜索解包结果为1
  // 均为搜索一次解包一次
  int N = allInfo.size(); ////计算所有数据大小
  // int cc = 0;

  while (true) {
    // cc++;
    // cout << cc << endl;
    // p.searchRet.matchIndex = p.searchRet.matchIndex - 1;
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
      // sResult = CW2A(temp_b.GetBuffer());
      sResult = temp_b;
      // delete p;
      return sResult;
    }
  }
}

void AIS::demodulation(uint32_t sample_freq) {
  double *temp_iq = new double[m_iq_data_len];
  for (int i = 0; i < m_iq_data_len; i++) {
    temp_iq[i] = m_iq_data[i] * 1.0;
  }

  CSignalDemodProbe *stCSignalDemodProbe;
  stCSignalDemodProbe = new CSignalDemodProbe;
  DWORD dwBitNum = 0;
  stCSignalDemodProbe->InitDemodParam(sample_freq, m_symbol_rate, 0,
                                      m_modu_type, false);
  stCSignalDemodProbe->InputData(temp_iq, m_iq_data_len);
  m_demod_bit = stCSignalDemodProbe->GetSignalSymbol(dwBitNum);
  m_demod_bit_len = dwBitNum;

  // printf("解调后输出：\n");
  // for (int i = 0; i < m_demod_bit_len; i++) {
  //   printf("%d", m_demod_bit[i]);
  // }
  // printf("\n");

  delete[] temp_iq;
}

void AIS::decode_initial() {
  searchRet.matchIndex = -1;
  searchRet.flag_bit_not = false;
  searchRet.len = 0;
  allInfo.clear();
}

bool AIS::decode_search() {
  bool ret = false;
  string preamble = "010101010101010101010101";
  string start_tag = "01111110";
  string head = "01" + start_tag;
  int startIndex = searchRet.matchIndex + 1;
  for (int i = startIndex; i < bin.length() - 9; i++) {
    string vague = bin.substr(i, 10);
    if (head == vague) {
      searchRet.matchIndex = i;
      searchRet.flag_bit_not = false;
      searchRet.len = bin.length() - i;
      ret = true;
      break;
    }
    if (vague == bitNot(head)) {
      searchRet.matchIndex = i;
      searchRet.flag_bit_not = true;
      searchRet.len = bin.length() - i;
      ret = true;
      break;
    }
  }

  return ret;
}

bool AIS::decode_fcs(string code) {

  bool is_wrong;
  int k = code.length() - 16;
  string genpoly = "10001000000100001";
  string seqone(k, '0');
  seqone = "1111111111111111" + seqone;
  string polyone = decode_fcs_div(seqone, genpoly, k);
  string polytwo = "";
  string tmp = code.substr(k, 16);
  for (int i = 1; i <= 16; i++) {
    int aa = (_ttoi(tmp.substr(i - 1, 1).c_str()) +
              _ttoi(polyone.substr(i - 1, 1).c_str()) + 1) %
             2;
    string aa_tmp;
    // aa_tmp.Format("%d",aa);
    aa_tmp = to_string(aa);
    polytwo += aa_tmp;
  }
  string rece_code = code.substr(0, k) + polytwo;
  string spoly = decode_fcs_div(rece_code, genpoly, k);
  if (sum(spoly) == 0)
    is_wrong = false;
  else
    is_wrong = true;
  return is_wrong;
}

string AIS::decode_fcs_div(string seqone, string genpoly, int k) {
  string residue = "";
  int len = genpoly.length() - 1;
  for (int i = 1; i <= k; i++) {
    if (seqone.substr(i - 1, 1) != "0") {
      for (int j = i; j <= i + len; j++) {
        int tmp = (_ttoi(seqone.substr(j - 1, 1).c_str()) +
                   _ttoi(genpoly.substr(j - i, 1).c_str())) %
                  2;
        if (tmp == 0)
          // seqone.SetAt(j-1,'0');
          seqone[j - 1] = '0';
        else
          // seqone.SetAt(j-1,'1');
          seqone[j - 1] = '1';
      }
    }
    //	else
    //		i++;
  }

  for (int i = 1; i <= len; i++)
    residue += seqone.substr(k + i - 1, 1);
  return residue;
}

string AIS::decode_ascii6(string str) {}

bool AIS::decode_unpackage() {

  allInfo.clear();
  Record oneRec;
  oneRec.val = "";
  int Index = searchRet.matchIndex;
  if (searchRet.matchIndex < 0)
    Index = 0;
  string mes = bin.substr(Index);
  int numberofslot = 0;
  int totalslot = 0;
  int tag = 1;
  string preamble = "010101010101010101010101";
  string end_tag = "01111110";
  string start_tag = "01" + end_tag;
  if (searchRet.flag_bit_not)
    mes = bitNot(mes);

  // find start tag
  bool flag = false;
  int sl = tag;
  if (mes.substr(sl - 1, 10) == start_tag)
    flag = true;
  else
    return false;

  // find end tag
  flag = false;
  int el;
  for (el = sl + 8; el <= mes.length() - 8 + 1; el++) {
    if (mes.substr(el - 1, 8) == end_tag) {
      flag = true;
      break;
    }
  }
  if (!flag)
    return false;

  // extract msg data
  string msg_data = mes.substr(sl + 10 - 1, el - sl - 10);

  // destuffing
  int ii = 1;
  while (true) {
    int myx = msg_data.length();
    if (ii <= myx - 6 + 1) {
      if (msg_data.substr(ii - 1, 6) == "111110") {
        msg_data = msg_data.substr(0, ii + 5 - 1) + msg_data.substr(ii + 5);
        ii += 5;
      } else
        ii++;
    } else
      break;
  }

  // FCS校验
  if (msg_data.length() < 16)
    return false;
  bool is_wrong = decode_fcs(msg_data);
  totalslot++;
  if (!is_wrong) {
    oneRec.field = "FCS检验结果：数据传输无误";
    allInfo.push_back(oneRec);
    msg_data = byteReverse(msg_data);

    int mesT = binstr2decnum(msg_data.substr(0, 6), "left-msb");
    string mg = msg_data.substr(6, msg_data.length() - 16 - 6);
    if (mesT == 1) {
      re_message1(mg);
      numberofslot++;
    }
    if (mesT == 2) {
      re_message2(mg);
      numberofslot++;
    }
    if (mesT == 3) {
      re_message3(mg);
      numberofslot++;
    }
    if (mesT == 4) {
      re_message4(mg);
      numberofslot++;
    }
    if (mesT == 5) {
      re_message5(mg);
      numberofslot++;
    }
    if (mesT == 6) {
      re_message6(mg);
      numberofslot++;
    }
    if (mesT == 7) {
      re_message7(mg);
      numberofslot++;
    }
    if (mesT == 8) {
      re_message8(mg);
      numberofslot++;
    }
    if (mesT == 9) {
      re_message9(mg);
      numberofslot++;
    }
    if (mesT == 10) {
      re_message10(mg);
      numberofslot++;
    }
    if (mesT == 11) {
      re_message11(mg);
      numberofslot++;
    }
    if (mesT == 12) {
      re_message12(mg);
      numberofslot++;
    }
    if (mesT == 13) {
      re_message13(mg);
      numberofslot++;
    }
    if (mesT == 14) {
      re_message13(mg);
      numberofslot++;
    }
    if (mesT == 15) {
      re_message15(mg);
      numberofslot++;
    }
    if (mesT == 16) {
      re_message16(mg);
      numberofslot++;
    }
    if (mesT == 17) {
      re_message17(mg);
      numberofslot++;
    }
    if (mesT == 18) {
      re_message18(mg);
      numberofslot++;
    }
    if (mesT == 19) {
      re_message19(mg);
      numberofslot++;
    }
    if (mesT == 20) {
      re_message20(mg);
      numberofslot++;
    }
    if (mesT == 21) {
      re_message21(mg);
      numberofslot++;
    }
    if (mesT == 22) {
      re_message22(mg);
      numberofslot++;
    }
  }
  return true;
}

void AIS::re_message1(string mes) {
  Record oneRec;
  oneRec.field = "**********所恢复的信息如下**********";
  oneRec.val = "";
  allInfo.push_back(oneRec);
  oneRec.field = "消息识别码：消息1(船位报告)";
  allInfo.push_back(oneRec);

  int tag = 1;
  string ps = binstr2decstr(mes.substr(tag - 1, 2), "left-msb");
  oneRec.field = "转发次数：" + ps;
  allInfo.push_back(oneRec);
  tag += 2;

  // MMSI的判别，需要后续修改完整
  ps = binstr2decstr(mes.substr(tag - 1, 30), "left-msb");
  if (ps.length() > 9)
    ps += "[错误，超过9位]";
  while (ps.length() < 9)
    ps = "0" + ps;
  oneRec.field = "用户识别码：" + ps;
  //-----------------add here---------------


  allInfo.push_back(oneRec);
  tag += 30;

  //
  double p = binstr2decnum(mes.substr(tag - 1, 4), "left-msb");
  if (p == 0)
    ps = "在航";
  else if (1 == p)
    ps = "锚泊";
  else if (2 == p)
    ps = "失控";
  else if (3 == p)
    ps = "操纵受限";
  else if (4 == p)
    ps = "吃水受限";
  else if (5 == p)
    ps = "靠泊";
  else if (6 == p)
    ps = "搁浅";
  else if (7 == p)
    ps = "从事捕鱼";
  else if (8 == p)
    ps = "靠船帆提供动力";
  else if (15 == p)
    ps = "未定义";
  else
    ps = "保留";
  oneRec.field = "航行状态：" + ps;
  allInfo.push_back(oneRec);
  tag += 4;

  // 需要修改(按国际标准修改)
  if (mes.substr(tag - 1, 1) == "0") {
    ps = "右转";
    p = binstr2decnum(mes.substr(tag, 7), "left-msb");
  } else {
    ps = "左转";
    p = binstr2decnum(bitNot(mes.substr(tag, 7)), "left-msb") + 1;
  }
  string ps2;
  // ps2.Format("%d",p);
  ps2 = to_string(p);
  if (p == 128)
    ps2 = "无法获得";
  oneRec.field = "转向率(AIS)：" + ps + ps2 + "度/分钟";
  allInfo.push_back(oneRec);
  // ps2.Format("%f",(p*1.0/4.733)*(p*1.0/4.733));
  ps2 = to_string(float((p * 1.0 / 4.733) * (p * 1.0 / 4.733)));
  oneRec.field = "转向率(sensor)：" + ps + ps2 + "度/分钟";
  allInfo.push_back(oneRec);
  tag += 8;

  //
  p = binstr2decnum(mes.substr(tag - 1, 10), "left-msb");
  if (1023 == p)
    ps = "不可用";
  else
    // ps.Format("%f",p*1.0/10);
    ps = to_string(float(p * 1.0 / 10));
  if (1022 == p)
    oneRec.field = "对地航速：" + ps + "kn或以上";
  else
    oneRec.field = "对地航速：" + ps + "kn";
  allInfo.push_back(oneRec);
  tag += 10;

  if (mes.substr(tag - 1, 1) == "1")
    ps = "高(<10m，DGNSS接收机的差分模式)";
  else
    ps = "低(>10m，GNSS接收机或其他电子定位系统的自主模式)";
  oneRec.field = "航位精确度：" + ps;
  allInfo.push_back(oneRec);
  tag += 1;

  if (mes.substr(tag - 1, 1) == "0") {
    ps = "东经";
    p = binstr2decnum(mes.substr(tag, 27), "left-msb") * 1.0 / 10000 / 60;
  } else {
    ps = "西经";
    p = (1 + binstr2decnum(bitNot(mes.substr(tag, 27)), "left-msb")) * 1.0 /
        10000 / 60;
  }
  // ps2.Format("%f度",p);
  ps2 = to_string(float(p)) + "度";
  if (ps == "东经" && p == 181)
    ps2 = "不可用";
  if ((ps == "东经" && p > 181) || (ps == "西经" && p > 180))
    ps2 += "[错误，度数超过180度]";
  oneRec.field = "经度：" + ps + ps2;
  allInfo.push_back(oneRec);
  tag += 28;

  if (mes.substr(tag - 1, 1) == "0") {
    ps = "北纬";
    p = binstr2decnum(mes.substr(tag, 26), "left-msb") * 1.0 / 10000 / 60;
  } else {
    ps = "南纬";
    p = (binstr2decnum(bitNot(mes.substr(tag, 26)), "left-msb") + 1) * 1.0 /
        10000 / 60;
  }
  // ps2.Format("%f度",p);
  ps2 = to_string(float(p)) + "度";
  if (ps == "北纬" && p == 91)
    ps2 = "不可用";
  if ((ps == "北纬" && p > 91) || (ps == "南纬" && p > 90))
    ps2 += "[错误，度数超过90度]";
  oneRec.field = "纬度：" + ps + ps2;
  allInfo.push_back(oneRec);
  tag += 27;

  p = binstr2decnum(mes.substr(tag - 1, 12), "left-msb");
  if (3600 == p)
    ps = "不可用";
  else
    // ps.Format("%f度",p*1.0/10);
    ps = to_string(float(p * 1.0 / 10)) + "度";
  if (p > 3600)
    ps += "[错误，度数超过359度]";
  oneRec.field = "对地航向：" + ps;
  allInfo.push_back(oneRec);
  tag += 12;

  p = binstr2decnum(mes.substr(tag - 1, 9), "left-msb");
  if (511 == p)
    ps = "不可用";
  else
    // ps.Format("%d度",p);
    ps2 = to_string(int(p)) + "度";
  if (p > 359)
    ps += "[错误，度数超过359度]";
  oneRec.field = "真航向：" + ps;
  allInfo.push_back(oneRec);
  tag += 9;

  p = binstr2decnum(mes.substr(tag - 1, 6), "left-msb");
  if (60 == p)
    ps = "不可用";
  if (62 == p)
    ps = "电子定位系统以估算模式(船位推算法)运行";
  if (61 == p)
    ps = "电子定位系统以人工输入方法运行";
  if (63 == p)
    ps = "定位系统未运行";
  if (p < 60)
    // ps.Format("报告产生时的UTC为%d秒",p);
    ps = "报告产生时的UTC为" + to_string(int(p)) + "秒";
  oneRec.field = "时间标记：" + ps;
  allInfo.push_back(oneRec);
  tag += 6;

  p = binstr2decnum(mes.substr(tag - 1, 4), "left-msb");
  if (0 == p)
    ps = "未作地区性使用";
  else
    ps = "作地区性使用";
  oneRec.field = "地区性应用：" + ps;
  allInfo.push_back(oneRec);
  tag += 4;

  tag++;

  if (mes.substr(tag - 1, 1) == "0")
    ps = "RAIM未使用";
  else
    ps = "RAIM使用";
  oneRec.field = "接收机自主完整性监测(RAIM)标志：" + ps;
  allInfo.push_back(oneRec);
  tag++;

  oneRec.field = "通信状态：SOTDMA";
  allInfo.push_back(oneRec);

  p = binstr2decnum(mes.substr(tag - 1, 2), "left-msb");
  if (0 == p)
    ps = "直接获取UTC";
  if (1 == p)
    ps = "间接获取UTC";
  if (2 == p)
    ps = "台站直接同步于基地台";
  if (3 == p)
    ps = "台站同步于另一个接收台数量最多的台站或同步于另一个直接与基地台同"
         "步的移动台站";
  oneRec.field = "同步状态：" + ps;
  allInfo.push_back(oneRec);
  tag += 2;

  p = binstr2decnum(mes.substr(tag - 1, 3), "left-msb");
  if (0 == p)
    ps = "该时隙最后一次传输";
  else
    // ps.Format("在时隙改变前保留该时隙%d帧",p);
    ps = "在时隙改变前保留该时隙" + to_string(int(p)) + "帧";
  oneRec.field = "时隙超时：" + ps;
  allInfo.push_back(oneRec);
  tag += 3;

  int p_tmp = p;
  p = binstr2decnum(mes.substr(tag - 1, 14), "left-msb");
  if (0 == p_tmp)
    // ps.Format("时隙偏移量(下一帧该时隙的偏移量)：%d",p);
    ps = "时隙偏移量(下一帧该时隙的偏移量)：" + to_string(int(p));
  if (1 == p_tmp) {
    int p1 = binstr2decnum(mes.substr(tag - 1, 5), "left-msb");
    int p2 = binstr2decnum(mes.substr(tag + 4, 7), "left-msb");
    // ps.Format("UTC小时和分钟：%d时%d分",p1,p2);
    ps = "UTC小时和分钟：" + to_string(p1) + "时" + to_string(p2) +
         "分";
    if (p1 > 23)
      ps += "[错误，小时超过23]";
    if (p2 > 59)
      ps += "[错误，分钟超过59]";
  }
  if (3 == p_tmp || 5 == p_tmp || 7 == p_tmp) {
    // ps.Format("接收到的台站数量：%d",p);
    ps = "接收到的台站数量：" + to_string(p);
    if (p > 16383)
      ps += "[错误，超过16383]";
  }
  if (2 == p_tmp || 4 == p_tmp || 6 == p_tmp) {
    // ps.Format("用于该发射的时隙号：%d",p);
    ps = "用于该发射的时隙号：" + to_string(p);
    if (p > 2249)
      ps += "[错误，超过2249]";
  }
  oneRec.field = ps;
  allInfo.push_back(oneRec);
  tag += 14;
}

void AIS::re_message2(string data) {
  Record oneRec;
  oneRec.val = "";
  oneRec.field = "**********所恢复的信息如下**********";
  data = "000010" + data;

  string identify = data.substr(0, 6);
  int data_type = binstr2decnum(identify, "left-msb");
  string display;
  // display.Format("消息类型为：消息%d(船位报告)",data_type);
  display = "消息类型为：消息" + to_string(data_type) + "(船位报告)";
  oneRec.field = display;
  allInfo.push_back(oneRec);

  // 转发指示符2bit
  string transmit = data.substr(6, 2);
  int data_transmit = binstr2decnum(transmit, "left-msb");
  // display.Format("转发次数为：%d",data_transmit);
  display = "转发次数为：" + to_string(data_transmit);
  if (data_transmit == 3)
    display = "转发指示：不再转发";
  oneRec.field = display;
  allInfo.push_back(oneRec);

  // 消息源识别码30bit
  string classify = data.substr(8, 30);
  int data_classify = binstr2decnum(classify, "left-msb");
  if (data_classify >= 10000000000) {
    data_classify /= 10;
    // display.Format("消息源识别码：%d",data_classify);
    display = "消息源识别码：" + to_string(data_classify);
  } else if (data_classify < 1000000000) {
    int num;
    for (int i = 8; i >= 1; i--) {
      if (floor(data_classify / (pow(10.0, i))) > 0) {
        num = 9 - i - 1;
        break;
      }
    }
    // display.Format("%d",data_classify);
    display = to_string(data_classify);
    for (int j = 1; j <= num; j++)
      display = "0" + display;
    display = "消息源识别码：" + display;
  } else
    // display.Format("消息源识别码：%d",data_classify);
    display = "消息源识别码：" + to_string(data_classify);
  oneRec.field = display;
  allInfo.push_back(oneRec);

  // 航行状态4bit
  int boatstate = binstr2decnum(data.substr(38, 4), "left-msb");
  if (0 == boatstate)
    display = "航行状态：在航(主机推动)";
  else if (1 == boatstate)
    display = "航行状态：锚泊";
  else if (2 == boatstate)
    display = "航行状态：失控";
  else if (3 == boatstate)
    display = "航行状态：操作受限";
  else if (4 == boatstate)
    display = "航行状态：吃水受限";
  else if (5 == boatstate)
    display = "航行状态：靠泊";
  else if (6 == boatstate)
    display = "航行状态：搁浅";
  else if (7 == boatstate)
    display = "航行状态：从事捕鱼";
  else if (8 == boatstate)
    display = "航行状态：靠船帆提供动力";
  else if (9 == boatstate)
    display = "航行状态提示：为将来船舶运输DG, HS, "
              "MP或IMO规定的有毒或污染C类货物的航行状态修正所保留";
  else if (10 == boatstate)
    display = "航行状态提示：为将来船舶运输DG, HS, "
              "MP或IMO规定的有毒或污染八类货物的航行状态修正所保留";
  else if (15 == boatstate)
    display = "航行状态提示：未定义，默认";
  else
    display = "航行状态提示：未定义，为将来使用保留";
  oneRec.field = display;
  allInfo.push_back(oneRec);

  // 转向率8bit
  string turn = data.substr(42, 8);
  if (turn.substr(0, 1) == "0") {
    int data_turn = binstr2decnum(turn.substr(1, 7), "left-msb");
    // display.Format("转向率为：每分钟右转%d度",data_turn);
    display = "转向率为：每分钟右转" + to_string(data_turn) + "度";
  } else {
    int data_turn =
        binstr2decnum(bitNot(turn.substr(1, 7)), "left-msb") + 1;
    // display.Format("转向率为：每分钟左转%d度",data_turn);
    display = "转向率为：每分钟左转" + to_string(data_turn) + "度";
  }
  oneRec.field = display;
  allInfo.push_back(oneRec);

  // 对地航速10bit
  string rate = data.substr(50, 10);
  int data_rate = binstr2decnum(rate, "left-msb");
  if (1023 == data_rate)
    display = "对地航速错误，不可用";
  else
    // display.Format("对地航速为：%fkn",data_rate*1.0/10);
    display =
        "对地航速为：" + to_string(float(data_rate * 1.0 / 10)) + "kn";
  oneRec.field = display;
  allInfo.push_back(oneRec);

  // 航位精确度1bit
  if (data.substr(60, 1) == "1")
    display = "航位精确度：高";
  else
    display = "航位精确度：低";
  oneRec.field = display;
  allInfo.push_back(oneRec);

  // 经度28bit
  string longitude = data.substr(61, 28);
  if (longitude.substr(0, 1) == "0") {
    int data_longitude = binstr2decnum(longitude.substr(1), "left-msb");
    // display.Format("经度为：东经%f度",data_longitude*1.0/600000);
    display = "经度为：东经" +
              to_string(float(data_longitude * 1.0 / 600000)) + "度";

  } else {
    int data_longitude =
        1 + binstr2decnum(bitNot(longitude.substr(1)), "left-msb");
    // display.Format("经度为：西经%f度",data_longitude*1.0/600000);
    display = "经度为：西经" +
              to_string(float(data_longitude * 1.0 / 600000)) + "度";
  }
  oneRec.field = display;
  allInfo.push_back(oneRec);

  // 纬度27bit
  string latitude = data.substr(89, 27);
  if (latitude.substr(0, 1) == "0") {
    int data_latitude = binstr2decnum(latitude.substr(1), "left-msb");
    // display.Format("纬度为：北纬%f度",data_latitude*1.0/600000);
    display = "纬度为：北纬" +
              to_string(float(data_latitude * 1.0 / 600000)) + "度";
  } else {
    int data_latitude =
        binstr2decnum(bitNot(latitude.substr(1)), "left-msb") + 1;
    // display.Format("纬度为：南纬%f度",data_latitude*1.0/600000);
    display = "纬度为：南纬" +
              to_string(float(data_latitude * 1.0 / 600000)) + "度";
  }
  oneRec.field = display;
  allInfo.push_back(oneRec);

  // 对地航向12bit
  string course = data.substr(116, 12);
  int data_course = binstr2decnum(course, "left-msb");
  if (3600 == data_course)
    display = "对地航向系统默认,此项指标不可用";
  if (data_course < 3600)
    // display.Format("对地航向为：%f度",data_course*1.0/10);
    display = "对地航向为：" + to_string(float(data_course * 1.0 / 10)) +
              "度";
  if (data_course > 3600)
    display = "对地航向显示错误";
  oneRec.field = display;
  allInfo.push_back(oneRec);

  // 真航向9bit
  string realcourse = data.substr(128, 9);
  int data_realcourse = binstr2decnum(realcourse, "left-msb");
  if (511 == data_realcourse)
    display = "真航向系统默认,此项指标不可用";
  else if (data_realcourse < 360)
    // display.Format("真航向为：%d度",data_realcourse);
    display = "真航向为：" + to_string(data_realcourse) + "度";
  else
    display = "真航向显示错误";
  oneRec.field = display;
  allInfo.push_back(oneRec);

  // 时间标记6bit
  string timemark = data.substr(137, 6);
  int data_timemark = binstr2decnum(timemark, "left-msb");
  // display.Format("报告产生时的UTC为%d秒",data_timemark);
  display = "报告产生时的UTC为：" + to_string(data_timemark) + "秒";
  if (63 == data_timemark)
    display += "定位系统未运行";
  else if (62 == data_timemark)
    display += "电子定位系统以估算模式(船位推算法)运行";
  else if (61 == data_timemark)
    display += "定位系统以人工输人方法运行";
  else if (60 == data_timemark)
    display += "时间标记系统默认,此项指标不可用";
  else
    display += "";
  oneRec.field = display;
  allInfo.push_back(oneRec);

  // 此项为地区性应用所保留4bit
  string protect = data.substr(143, 4);
  int data_protect = binstr2decnum(protect, "left-msb");
  if (0 == data_protect)
    display = "地区性使用指标系统默认,此项指标不可用";
  else
    display = "地区性使用指标(尚待查明)";
  oneRec.field = display;
  allInfo.push_back(oneRec);

  // 备用位1bit
  oneRec.field = "备用位默认为0";
  allInfo.push_back(oneRec);

  // RAIM标志1bit
  if (data.substr(148, 1) == "1")
    display = "RAIM(接收机自主完整性监测)使用";
  else
    display = "RAIM(接收机自主完整性监测)未使用";
  oneRec.field = display;
  allInfo.push_back(oneRec);

  // 通信状态19bit
  string comstate = data.substr(149, 19);
  int data_comstate = binstr2decnum(comstate.substr(0, 2), "left-msb");
  oneRec.field = "通信状态：处于SOTDMA通信状态";
  allInfo.push_back(oneRec);
  if (0 == data_comstate)
    display = "同步状态：直接获取UTC";
  if (1 == data_comstate)
    display = "同步状态：间接获取UTC";
  if (2 == data_comstate)
    display = "同步状态：台站同步于基地台";
  if (3 == data_comstate)
    display = "同步状态：台站同步于另一个接收台数量最多的台站";
  oneRec.field = display;
  allInfo.push_back(oneRec);

  data_comstate = binstr2decnum(comstate.substr(2, 3), "left-msb");
  if (0 == data_comstate)
    display = "该时隙最后一次传输";
  else
    // display.Format("时隙超时：在时隙改变前将保留该时隙%d帧",data_comstate);
    display = "时隙超时：在时隙改变前将保留该时隙" +
              to_string(data_comstate) + "帧";
  oneRec.field = display;
  allInfo.push_back(oneRec);

  if (0 == data_comstate)
    // display.Format("时隙偏移量为：%d",binstr2decnum(comstate.substr(5,14),"left-msb"));
    display = "时隙偏移量为：" +
              to_string(binstr2decnum(comstate.substr(5, 14), "left-msb"));
  if (1 == data_comstate)
    // display.Format("UTC小时和分钟为：%d时%d分",binstr2decnum(comstate.substr(5,5),"left-msb"),binstr2decnum(comstate.substr(10,7),"left-msb"));
    display =
        "UTC小时和分钟为：" +
        to_string(int(binstr2decnum(comstate.substr(5, 5), "left-msb"))) +
        "时" +
        to_string(int(binstr2decnum(comstate.substr(10, 7), "left-msb"))) +
        "分";
  if (2 == data_comstate || 4 == data_comstate || 6 == data_comstate)
    // display.Format("时隙号为：%d",binstr2decnum(comstate.substr(5,14),"left-msb"));
    display =
        "时隙号为：" +
        to_string(int(binstr2decnum(comstate.substr(5, 14), "left-msb")));
  if (3 == data_comstate || 5 == data_comstate || 7 == data_comstate)
    // display.Format("接收到的台站数量为：%d",binstr2decnum(comstate.substr(5,14),"left-msb"));
    display =
        "接收到的台站数量为：" +
        to_string(int(binstr2decnum(comstate.substr(5, 14), "left-msb")));
  oneRec.field = display;
  allInfo.push_back(oneRec);
}
void AIS::re_message3(string mes) {
  Record oneRec;
  oneRec.val = "";
  oneRec.field = "**********所恢复的信息如下**********";
  allInfo.push_back(oneRec);
  oneRec.field = "消息识别码：消息3(船位报告)";
  allInfo.push_back(oneRec);

  int tag = 1;
  int p = binstr2decnum(mes.substr(tag - 1, 2), "left-msb");
  string ps;
  // ps.Format("%d",p);
  ps = to_string(p);
  oneRec.field = "转发次数：" + ps;
  ;
  allInfo.push_back(oneRec);
  tag += 2;

  //%%%%%%%%MMSI的判别，需要后续修改完整%%%%%%%%%%%%%%%%%%%
  p = binstr2decnum(mes.substr(tag - 1, 30), "left-msb");
  // ps.Format("%d",p);
  ps = to_string(p);
  if (ps.length() > 9)
    ps += "[错误，超过9位]";
  while (ps.length() < 9)
    ps = "0" + ps;
  oneRec.field = "用户识别码：" + ps;
  allInfo.push_back(oneRec);
  tag += 30;

  //
  p = binstr2decnum(mes.substr(tag - 1, 4), "left-msb");
  if (0 == p)
    ps = "在航";
  else if (1 == p)
    ps = "锚泊";
  else if (2 == p)
    ps = "失控";
  else if (3 == p)
    ps = "操纵受限";
  else if (4 == p)
    ps = "吃水受限";
  else if (5 == p)
    ps = "靠泊";
  else if (6 == p)
    ps = "搁浅";
  else if (7 == p)
    ps = "从事捕鱼";
  else if (8 == p)
    ps = "靠船帆提供动力";
  else if (15 == p)
    ps = "未定义";
  else
    ps = "保留";
  oneRec.field = "航行状态：" + ps;
  allInfo.push_back(oneRec);
  tag += 4;

  // 需要修改(按国际标准修改)
  if (mes.substr(tag - 1, 1) == "0") {
    ps = "右转";
    p = binstr2decnum(mes.substr(tag, 7), "left-msb");
  } else {
    ps = "左转";
    p = binstr2decnum(bitNot(mes.substr(tag, 7)), "left-msb") + 1;
  }
  string ps2;
  // ps2.Format("%d",p);
  ps2 = to_string(p);
  if (128 == p)
    ps2 = "无法获得";
  oneRec.field = "转向率(AIS)：" + ps + ps2 + "度/分钟\n";
  allInfo.push_back(oneRec);
  // ps2.Format("%f",pow(p*1.0/4.733,2));
  ps2 = to_string(float(pow(p * 1.0 / 4.733, 2)));
  oneRec.field = "转向率(sensor)：" + ps + ps2 + "度/分钟";
  allInfo.push_back(oneRec);
  tag += 8;

  //
  p = binstr2decnum(mes.substr(tag - 1, 10), "left-msb");
  if (1023 == p)
    ps = "不可用";
  else
    // ps.Format("%f",p*1.0/10);
    ps = to_string(float(p * 1.0 / 10));
  if (1022 == p)
    oneRec.field = "对地航速：" + ps + "kn或以上";
  else
    oneRec.field = "对地航速：" + ps + "kn";
  allInfo.push_back(oneRec);
  tag += 10;

  //
  if (mes.substr(tag - 1, 1) == "1")
    ps = "高(<10m，DGNSS接收机的差分模式)";
  else
    ps = "低(>10m，GNSS接收机或其他电子定位系统的自主模式)";
  oneRec.field = "航位精确度：" + ps;
  allInfo.push_back(oneRec);
  tag++;

  //
  double p1;
  if (mes.substr(tag - 1, 1) == "0") {
    ps = "东经";
    p1 = binstr2decnum(mes.substr(tag, 27), "left-msb") / 10000 / 60;
  } else {
    ps = "西经";
    p1 = (binstr2decnum(bitNot(mes.substr(tag, 27)), "left-msb") + 1) /
         10000 / 60;
  }
  // ps2.Format("%f度",p1);
  ps2 = to_string(float(p1)) + "度";
  if (mes.substr(tag - 1, 1) == "0" && p1 == 181)
    ps2 = "不可用";
  if ((mes.substr(tag - 1, 1) == "0" && p1 > 181) ||
      (mes.substr(tag - 1, 1) == "1" && p1 > 180))
    ps += "[错误，度数超过180度]";
  oneRec.field = "经度：" + ps + ps2;
  allInfo.push_back(oneRec);
  tag += 28;

  //
  if (mes.substr(tag - 1, 1) == "0") {
    ps = "北纬";
    p1 = binstr2decnum(mes.substr(tag, 26), "left-msb") / 10000 / 60;
  } else {
    ps = "南纬";
    p1 = (binstr2decnum(bitNot(mes.substr(tag, 26)), "left-msb") + 1) /
         10000 / 60;
  }
  // ps2.Format("%f度",p1);
  ps2 = to_string(float(p1)) + "度";
  if (mes.substr(tag - 1, 1) == "0" && p1 == 91)
    ps2 = "不可用";
  if ((mes.substr(tag - 1, 1) == "0" && p1 > 91) ||
      (mes.substr(tag - 1, 1) == "1" && p1 > 90))
    ps2 += "[错误，度数超过90度]";
  oneRec.field = "纬度：" + ps + ps2;
  allInfo.push_back(oneRec);
  tag += 27;

  //
  p = binstr2decnum(mes.substr(tag, 12), "left-msb");
  if (3600 == p)
    ps = "不可用";
  else
    // ps.Format("%f度",p*1.0/10);
    ps = to_string(float(p * 1.0 / 10)) + "度";
  if (p > 3600)
    ps += "[错误，度数超过359度]";
  oneRec.field = "对地航向：" + ps;
  allInfo.push_back(oneRec);
  tag += 12;

  //
  p = binstr2decnum(mes.substr(tag - 1, 9), "left-msb");
  if (511 == p)
    ps = "不可用";
  else
    // ps.Format("%d度",p);
    ps = to_string(int(p)) + "度";
  if (p > 359)
    ps += "[错误，度数超过359度]";
  oneRec.field = "真航向：" + ps;
  allInfo.push_back(oneRec);
  tag += 9;

  //
  p = binstr2decnum(mes.substr(tag - 1, 6), "left-msb");
  if (60 == p)
    ps = "不可用";
  if (62 == p)
    ps = "电子定位系统以估算模式(船位推算法)运行";
  if (61 == p)
    ps = "电子定位系统以人工输入方法运行";
  if (63 == p)
    ps = "定位系统未运行";
  if (p < 60)
    // ps.Format("报告产生时的UTC为%d秒",p);
    ps = "报告产生时的UTC为" + to_string(int(p)) + "秒";
  oneRec.field = "时间标记：" + ps;
  allInfo.push_back(oneRec);
  tag += 6;

  //
  p = binstr2decnum(mes.substr(tag - 1, 4), "left-msb");
  if (0 == p)
    ps = "未作地区性使用";
  else
    ps = "作地区性使用";
  oneRec.field = "地区性应用：" + ps;
  allInfo.push_back(oneRec);
  tag += 4;

  //
  tag++;
  if (mes.substr(tag - 1, 1) == "0")
    ps = "RAIM未使用";
  else
    ps = "RAIM使用";
  oneRec.field = "接收机自主完整性监测(RAIM)标志：" + ps;
  allInfo.push_back(oneRec);
  tag++;

  //
  oneRec.field = "通信状态：ITDMA";
  allInfo.push_back(oneRec);
  //%%%%%%%%%%%%%已按国际标准Edition 1.5修改%%%%%%%%%%%%%%
  p = binstr2decnum(mes.substr(tag - 1, 2), "left-msb");
  if (0 == p)
    ps = "直接获取UTC";
  if (1 == p)
    ps = "间接获取UTC";
  if (2 == p)
    ps = "台站直接同步于基地台";
  if (3 == p)
    ps = "台站同步于另一个接收台数量最多的台站或同步于另一个直接与基地台同"
         "步的移动台站";
  oneRec.field = "同步状态：" + ps;
  allInfo.push_back(oneRec);
  tag += 2;

  p = binstr2decnum(mes.substr(tag - 1, 13), "left-msb");
  if (0 == p)
    // ps.Format("%d(不再进行进一步的传输)",p);
    ps = to_string(int(p)) + "(不再进行进一步的传输)";
  else
    // ps.Format("%d",p);
    ps = to_string(int(p));
  oneRec.field = "时隙增量：" + ps;
  allInfo.push_back(oneRec);
  tag += 13;

  p = binstr2decnum(mes.substr(tag - 1, 3), "left-msb");
  if (p > 4)
    ps = "[错误，超出连续时隙数范围]";
  else
    // ps.Format("%d",p+1);
    ps = to_string(int(p + 1));
  oneRec.field = "应分配的连续时隙数：" + ps;
  allInfo.push_back(oneRec);
  tag += 3;

  if (mes.substr(tag - 1, 1) == "1")
    ps = "是";
  else
    ps = "否";
  oneRec.field = "时隙分配是否多保留一帧：" + ps;
  allInfo.push_back(oneRec);
}
void AIS::re_message4(string data) {
  Record oneRec;
  oneRec.val = "";
  oneRec.field = "**********所恢复的信息如下**********";
  allInfo.push_back(oneRec);
  data = "000100" + data;

  string identify = data.substr(0, 6);
  int data_type = binstr2decnum(identify, "left-msb");
  string display;
  // display.Format("消息类型为：消息%d(基地台报告)",data_type);
  display = "消息类型为：消息" + to_string(data_type) + "(基地台报告)";
  oneRec.field = display;
  allInfo.push_back(oneRec);

  // 转发指示符2bit
  string transmit = data.substr(6, 2);
  int data_transmit = binstr2decnum(transmit, "left-msb");
  // display.Format("转发次数为：%d",data_transmit);
  display = "转发次数为：" + to_string(data_transmit);
  if (3 == data_transmit)
    display = "不再转发";
  oneRec.field = display;
  allInfo.push_back(oneRec);

  // 用户识别码30bit
  string classify = data.substr(8, 30);
  int data_classify = binstr2decnum(classify, "left-msb");
  if (data_classify >= 10000000000) {
    data_classify = floor(data_classify * 1.0 / 10);
    // display.Format("用户识别码(MMSI)：%d",data_classify);
    display = "用户识别码(MMSI)：" + to_string(data_classify);
  } else if (data_classify < 1000000000) {
    int num = 0;
    for (int i = 8; i <= 1; i--) {
      if (floor(data_classify * 1.0 / pow(10.0, i)) > 0) {
        num = 9 - i - 1;
        break;
      }
    }
    // display.Format("%d",data_classify);
    display = to_string(data_classify);
    for (int j = 1; j <= num; j++)
      display = "0" + display;
    display = "用户识别码(MMSI)：" + display;
  } else
    // display.Format("用户识别码(MMSI)：%d",data_classify);
    display = "用户识别码(MMSI)：" + to_string(data_classify);
  oneRec.field = display;
  allInfo.push_back(oneRec);

  // UTC年份
  string year = data.substr(38, 14);
  int data_year = binstr2decnum(year, "left-msb");
  if (0 == data_year)
    display = "无UTC年份";
  else
    // display.Format("UTC年份为：%d年",data_year);
    display = "UTC年份为：" + to_string(data_year) + "年";
  oneRec.field = display;
  allInfo.push_back(oneRec);

  // UTC月份
  string month = data.substr(52, 4);
  int data_month = binstr2decnum(month, "left-msb");
  if (0 == data_month)
    display = "无UTC月份";
  else if (data_month > 0 && data_month < 13)
    // display.Format("UTC月份为：%d月",data_month);
    display = "UTC月份为：" + to_string(data_month) + "月";
  else
    display = "UTC月份显示错误";
  oneRec.field = display;
  allInfo.push_back(oneRec);

  // UTC日期
  string day = data.substr(56, 5);
  int data_day = binstr2decnum(day, "left-msb");
  if (0 == data_day)
    display = "无UTC日期";
  else
    // display.Format("UTC日期为：%d日",data_day);
    display = "UTC日期为：" + to_string(data_day) + "日";
  oneRec.field = display;
  allInfo.push_back(oneRec);

  // UTC小时
  string hour = data.substr(61, 5);
  int data_hour = binstr2decnum(hour, "left-msb");
  if (24 == data_hour)
    display = "无UTC时刻";
  else if (data_hour >= 0 && data_hour <= 23)
    // display.Format("UTC时刻为：%d时",data_hour);
    display = "UTC时刻为：" + to_string(data_hour) + "时";
  else
    display = "UTC时刻显示错误";
  oneRec.field = display;
  allInfo.push_back(oneRec);

  // UTC分钟
  string minutes = data.substr(66, 6);
  int data_minutes = binstr2decnum(minutes, "left-msb");
  if (60 == data_minutes)
    display = "无UTC分钟";
  else if (data_minutes >= 0 && data_minutes <= 59)
    // display.Format("UTC分钟为：%d分",data_minutes);
    display = "UTC分钟为：" + to_string(data_minutes) + "分";
  else
    display = "UTC分钟显示错误";
  oneRec.field = display;
  allInfo.push_back(oneRec);

  // UTC秒
  string second = data.substr(72, 6);
  int data_second = binstr2decnum(second, "left-msb");
  if (60 == data_second)
    display = "无UTC秒时";
  else if (data_second >= 0 && data_second <= 59)
    // display.Format("UTC秒时为：%d秒",data_second);
    display = "UTC秒时为：" + to_string(data_second) + "秒";
  else
    display = "UTC秒时显示错误";
  oneRec.field = display;
  allInfo.push_back(oneRec);

  // 航位精确度1bit
  if (data.substr(78, 1) == "1")
    display = "航位精确度：高";
  else
    display = "航位精确度：低";
  oneRec.field = display;
  allInfo.push_back(oneRec);

  // 经度28bit
  string longitude = data.substr(79, 28);
  if (longitude.substr(0, 1) == "0") {
    int data_longitude = binstr2decnum(longitude.substr(1, 27), "left-msb");
    // display.Format("经度为：东经%f度",data_longitude*1.0/600000);
    display = "经度为：东经" +
              to_string(float(data_longitude * 1.0 / 600000)) + "度";
  } else {
    int data_longitude =
        binstr2decnum(bitNot(longitude.substr(1, 27)), "left-msb") + 1;
    // display.Format("经度为：西经%f度",data_longitude*1.0/600000);
    display = "经度为：西经" +
              to_string(float(data_longitude * 1.0 / 600000)) + "度";
  }
  oneRec.field = display;
  allInfo.push_back(oneRec);

  // 纬度27bit
  string latitude = data.substr(107, 27);
  if (latitude.substr(0, 1) == "0") {
    int data_latitude = binstr2decnum(latitude.substr(1, 26), "left-msb");
    // display.Format("纬度为：北纬%f度",data_latitude*1.0/600000);
    display = "纬度为：北纬" +
              to_string(float(data_latitude * 1.0 / 600000)) + "度";
  } else {
    int data_latitude =
        binstr2decnum(bitNot(latitude.substr(1, 26)), "left-msb") + 1;
    // display.Format("纬度为：南纬%f度",data_latitude*1.0/600000);
    display = "纬度为：南纬" +
              to_string(float(data_latitude * 1.0 / 600000)) + "度";
  }
  oneRec.field = display;
  allInfo.push_back(oneRec);

  // 电子定位装备的类型
  string diff = data.substr(134, 4);
  int data_diff = binstr2decnum(diff, "left-msb");
  if (0 == data_diff)
    display = "定位装备类型：预设";
  else if (1 == data_diff)
    display = "定位装备类型：GPS";
  else if (2 == data_diff)
    display = "定位装备类型：GLONASS";
  else if (3 == data_diff)
    display = "定位装备类型：结合GPS/GLONASS";
  else if (4 == data_diff)
    display = "定位装备类型：罗兰";
  else if (5 == data_diff)
    display = "定位装备类型：Chayak";
  else if (6 == data_diff)
    display = "定位装备类型：组合导航系统";
  else if (7 == data_diff)
    display = "定位装备类型：观测";
  else
    display = "定位装备类型：未被定义使用";
  oneRec.field = display;
  allInfo.push_back(oneRec);

  // 备用码
  oneRec.field = "备用码应设为0";
  allInfo.push_back(oneRec);

  // RAIM标志
  if (data.substr(148, 1) == "1")
    display = "RAIM(接收机自主完整性监测)使用";
  else
    display = "RAIM(接收机自主完整性监测)未使用";
  oneRec.field = display;
  allInfo.push_back(oneRec);

  // 通信状态19bit
  string comstate = data.substr(149, 19);
  oneRec.field = "通信状态：处于SOTDMA通信状态";
  allInfo.push_back(oneRec);

  int data_comstate = binstr2decnum(comstate.substr(0, 2), "left-msb");
  if (0 == data_comstate)
    display = "同步状态：直接获取UTC";
  else if (1 == data_comstate)
    display = "同步状态：间接获取UTC";
  else if (2 == data_comstate)
    display = "同步状态：台站同步于基地台";
  else
    display = "同步状态：台站同步于另一个接收台数量最多的台站";
  oneRec.field = display;
  allInfo.push_back(oneRec);

  data_comstate = binstr2decnum(comstate.substr(2, 3), "left-msb");
  if (0 == data_comstate)
    display = "该时隙最后一次传输";
  else
    // display.Format("时隙超时：在时隙改变前将保留该时隙%d帧",data_comstate);
    display = "时隙超时：在时隙改变前将保留该时隙" +
              to_string(data_comstate) + "帧";
  oneRec.field = display;
  allInfo.push_back(oneRec);

  if (0 == data_comstate)
    // display.Format("时隙偏移量为：%d",binstr2decnum(comstate.substr(5,14),"left-msb"));
    display =
        "时隙偏移量为：" +
        to_string(int(binstr2decnum(comstate.substr(5, 14), "left-msb")));
  else if (1 == data_comstate)
    // display.Format("UTC小时和分钟为：%d时%d分",binstr2decnum(comstate.substr(14,5),"left-msb"),binstr2decnum(comstate.substr(5,9),"left-msb"));
    display =
        "UTC小时和分钟为：" +
        to_string(int(binstr2decnum(comstate.substr(14, 5), "left-msb"))) +
        "时" +
        to_string(int(binstr2decnum(comstate.substr(5, 9), "left-msb"))) +
        "分";
  else if (2 == data_comstate || 4 == data_comstate || 6 == data_comstate)
    // display.Format("时隙号为：%d",binstr2decnum(comstate.substr(5,14),"left-msb"));
    display =
        "时隙号为：" +
        to_string(int(binstr2decnum(comstate.substr(5, 14), "left-msb")));
  else
    // display.Format("接收到的台站数量为：%d",binstr2decnum(comstate.substr(5,14),"left-msb"));
    display =
        "接收到的台站数量为：" +
        to_string(int(binstr2decnum(comstate.substr(5, 14), "left-msb")));
  oneRec.field = display;
  allInfo.push_back(oneRec);
}
void AIS::re_message5(string mes) {
  Record oneRec;
  oneRec.val = "";
  oneRec.field = "**********所恢复的信息如下**********";
  allInfo.push_back(oneRec);
  oneRec.field = "消息识别码：消息5(船舶静态数据和与航程有关的数据)";
  allInfo.push_back(oneRec);

  CHAR asc2[64] = {'@', 'A', 'B', 'C', 'D', 'E', 'F',  'G', 'H', 'I', 'J',
                   'K', 'L', 'M', 'N', 'O', 'P', 'Q',  'R', 'S', 'T', 'U',
                   'V', 'W', 'X', 'Y', 'Z', '[', '\\', ']', '^', '_', ' ',
                   '!', '"', '#', '$', '%', '&', '\'', '(', ')', '*', '+',
                   ',', '-', '.', '/', '0', '1', '2',  '3', '4', '5', '6',
                   '7', '8', '9', ':', ';', '<', '=',  '>', '?'};
  int tag = 1;
  int p = binstr2decnum(mes.substr(tag - 1, 2), "left-msb");
  string ps;
  // ps.Format("%d",p);
  ps = to_string(p);
  oneRec.field = "转发次数：" + ps;
  allInfo.push_back(oneRec);
  tag += 2;

  //%%%%%%%%MMSI的判别，需要后续修改完整%%%%%%%%%%%%%%%%%%%
  p = binstr2decnum(mes.substr(tag - 1, 30), "left-msb");
  // ps.Format("%d",p);
  ps = to_string(p);
  if (ps.length() > 9)
    ps += "[错误，超过9位]";
  while (ps.length() < 9)
    ps = "0" + ps;
  oneRec.field = "用户识别码：" + ps;
  allInfo.push_back(oneRec);
  tag += 30;

  //
  p = binstr2decnum(mes.substr(tag - 1, 2), "left-msb");
  if (0 == p)
    ps = "台站遵循AIS版本0";
  else
    // ps.Format("台站遵循未来的AIS版本%d",p);
    ps = "台站遵循未来的AIS版本" + to_string(p);
  oneRec.field = "AIS版本：" + ps;
  allInfo.push_back(oneRec);
  tag += 2;

  //
  p = binstr2decnum(mes.substr(tag - 1, 30), "left-msb");
  if (0 == p)
    ps = "不可用";
  else
    // ps.Format("%d",p);
    ps = to_string(p);
  if (p > 999999999)
    ps += "[错误，超过999999999]";
  oneRec.field = "IMO号码：" + ps;
  allInfo.push_back(oneRec);
  tag += 30;

  //
  ps = "0000000";
  p = binstr2decnum(mes.substr(tag - 1, 6), "left-msb");
  // ps.SetAt(0,asc2[p]);
  ps[0] = asc2[p];
  tag += 6;
  p = binstr2decnum(mes.substr(tag - 1, 6), "left-msb");
  // ps.SetAt(1,asc2[p]);
  ps[1] = asc2[p];
  tag += 6;
  p = binstr2decnum(mes.substr(tag - 1, 6), "left-msb");
  // ps.SetAt(2,asc2[p]);
  ps[2] = asc2[p];
  tag += 6;
  p = binstr2decnum(mes.substr(tag - 1, 6), "left-msb");
  // ps.SetAt(3,asc2[p]);
  ps[3] = asc2[p];
  tag += 6;
  p = binstr2decnum(mes.substr(tag - 1, 6), "left-msb");
  // ps.SetAt(4,asc2[p]);
  ps[4] = asc2[p];
  tag += 6;
  p = binstr2decnum(mes.substr(tag - 1, 6), "left-msb");
  // ps.SetAt(5,asc2[p]);
  ps[5] = asc2[p];
  tag += 6;
  p = binstr2decnum(mes.substr(tag - 1, 6), "left-msb");
  // ps.SetAt(6,asc2[p]);
  ps[6] = asc2[p];
  tag += 6;
  if ("@@@@@@@" == ps)
    ps = "不可用";
  oneRec.field = "呼号：" + ps;
  allInfo.push_back(oneRec);

  p = binstr2decnum(mes.substr(tag - 1, 6), "left-msb");
  string ps1;
  // ps1.Format("%c",asc2[p]);
  ps1 = to_string(char(asc2[p]));
  tag += 6;
  ps = ps1;
  for (int i = 1; i <= 19; i++) {
    p = binstr2decnum(mes.substr(tag - 1, 6), "left-msb");
    // ps1.Format("%c",asc2[p]);
    ps1 = to_string(char(asc2[p]));
    ps += ps1;
    tag += 6;
  }
  if ("@@@@@@@@@@@@@@@@@@@@" == ps)
    ps = "不可用";
  oneRec.field = "名称：" + ps;
  allInfo.push_back(oneRec);

  p = binstr2decnum(mes.substr(tag - 1, 8), "left-msb");
  if (0 == p)
    ps = "不可用或非船舶";
  else if (50 == p)
    ps = "引航船";
  else if (51 == p)
    ps = "搜救船";
  else if (52 == p)
    ps = "拖轮";
  else if (53 == p)
    ps = "港口供应船";
  else if (54 == p)
    ps = "载有防污染装置和设备的船舶";
  else if (55 == p)
    ps = "执法船";
  else if (56 == p)
    ps = "备用-分配给当地船舶使用";
  else if (57 == p)
    ps = "备用-分配给当地船舶使用";
  else if (58 == p)
    ps = "医疗船";
  else if (59 == p)
    ps = "符合《无线电规则》第18号决议(Mob-83)的船舶";
  else if (30 == p)
    ps = "船舶(从事捕鱼)";
  else if (31 == p)
    ps = "船舶(从事拖带作业)";
  else if (32 == p)
    ps = "船舶(从事拖带作业，且拖带长度超过200m，或宽度超过25m)";
  else if (33 == p)
    ps = "船舶(从事疏浚或水下作业)";
  else if (34 == p)
    ps = "船舶(从事潜水作业)";
  else if (35 == p)
    ps = "船舶(参与军事行动)";
  else if (36 == p)
    ps = "船舶(驶帆航行)";
  else if (37 == p)
    ps = "船舶(娱乐船)";
  else if (38 == p)
    ps = "船舶(为将来使用保留)";
  else if (39 == p)
    ps = "船舶(为将来使用保留)";
  vector<string> second;
  second.push_back("(这一类型的所有船舶)");
  second.push_back("(载有危险品，有害物质或海上污染物，属IMO规定的A类有害或"
                   "污染物质的船舶)");
  second.push_back("(载有危险品，有害物质或海上污染物，属IMO规定的B类有害或"
                   "污染物质的船舶)");
  second.push_back("(载有危险品，有害物质或海上污染物，属IMO规定的C类有害或"
                   "污染物质的船舶)");
  second.push_back("(载有危险品，有害物质或海上污染物，属IMO规定的D类有害或"
                   "污染物质的船舶)");
  second.push_back("(为将来使用保留)");
  second.push_back("(为将来使用保留)");
  second.push_back("(为将来使用保留)");
  second.push_back("(为将来使用保留)");
  second.push_back("(无附加信息)");
  int firstno = floor(p * 1.0 / 10);
  int secondno = p - firstno * 10;
  if (1 == firstno)
    ps = "为将来使用保留";
  else if (2 == firstno)
    ps = "地效应船";
  else if (4 == firstno)
    ps = "高速船";
  else if (6 == firstno)
    ps = "客船";
  else if (7 == firstno)
    ps = "货船";
  else if (8 == firstno)
    ps = "油轮";
  else if (9 == firstno)
    ps = "其他类型的船舶";
  if (firstno < 10 && firstno != 0 && firstno != 3 && firstno != 5)
    ps += second.at(secondno);
  if (p >= 100 && p < 200)
    ps = "为地区性使用保留";
  if (p >= 200)
    ps = "为将来使用保留";
  if (p > 0 && p < 10)
    ps = "标准中无定义";
  oneRec.field = "船舶及载货类型：" + ps;
  allInfo.push_back(oneRec);
  tag += 8;

  ps = "";
  p = binstr2decnum(mes.substr(tag - 1, 9), "left-msb");
  // ps1.Format("A-%dm；",p);
  ps1 = "A" + to_string(p) + "m；";
  ps += ps1;
  tag += 9;
  p = binstr2decnum(mes.substr(tag - 1, 9), "left-msb");
  // ps1.Format("B-%dm；",p);
  ps1 = "B" + to_string(p) + "m；";
  ps += ps1;
  tag += 9;
  p = binstr2decnum(mes.substr(tag - 1, 6), "left-msb");
  if (63 == p)
    // ps1.Format("C-%dm或以上；",p);
    ps1 = "C" + to_string(p) + "m或以上；";
  else
    // ps1.Format("C-%dm；",p);
    ps1 = "C" + to_string(p) + "m；";
  ps += ps1;
  tag += 6;
  p = binstr2decnum(mes.substr(tag - 1, 6), "left-msb");
  if (63 == p)
    // ps1.Format("D-%dm或以上；",p);
    ps1 = "D" + to_string(p) + "m或以上；";
  else
    // ps1.Format("D-%dm；",p);
    ps1 = "D" + to_string(p) + "m；";
  ps += ps1;
  tag += 6;
  oneRec.field = "尺度和位置参照：" + ps;
  allInfo.push_back(oneRec);

  //%%%%%%%%%%%%%已按国际标准Edition 1.5修改%%%%%%%%%%%%%%
  p = binstr2decnum(mes.substr(tag - 1, 4), "left-msb");
  if (0 == p)
    ps = "未定义";
  else if (1 == p)
    ps = "GPS";
  else if (2 == p)
    ps = "GLONASS";
  else if (3 == p)
    ps = "结合GPS/GLONASS";
  else if (4 == p)
    ps = "罗兰C";
  else if (5 == p)
    ps = "Chayka";
  else if (6 == p)
    ps = "结合导航系统";
  else if (7 == p)
    ps = "观测";
  else if (8 == p)
    ps = "伽利略";
  else
    ps = "[错误]";
  oneRec.field = "电子定位装置的类型：" + ps;
  allInfo.push_back(oneRec);
  tag += 4;

  //%%%%%%%%%%%%%已按国际标准Edition 1.5修改%%%%%%%%%%%%%%
  ps = "";
  p = binstr2decnum(mes.substr(tag - 1, 4), "left-msb");
  // ps1.Format("%d月",p);
  ps1 = to_string(p) + "月";
  if (0 == p)
    ps1 = "月份不可用";
  if (p > 12)
    ps1 += "[错误，月份超过12]";
  ps += ps1;
  tag += 4;
  p = binstr2decnum(mes.substr(tag - 1, 5), "left-msb");
  // ps1.Format("%d日",p);
  ps1 = to_string(p) + "日";
  if (0 == p)
    ps1 = "日期不可用";
  ps += ps1;
  tag += 5;
  p = binstr2decnum(mes.substr(tag - 1, 5), "left-msb");
  // ps1.Format("%d时",p);
  ps1 = to_string(p) + "时";
  if (24 == p)
    ps1 = "小时不可用";
  if (p > 24)
    ps1 += "[错误，小时超过23]";
  ps += ps1;
  tag += 5;
  p = binstr2decnum(mes.substr(tag - 1, 6), "left-msb");
  // ps1.Format("%d分",p);
  ps1 = to_string(p) + "分";
  if (60 == p)
    ps1 = "分钟不可用";
  if (p > 60)
    ps1 += "[错误，分钟超过59]";
  ps += ps1;
  tag += 6;
  oneRec.field = "预计到达时间：" + ps;
  allInfo.push_back(oneRec);

  //
  p = binstr2decnum(mes.substr(tag - 1, 8), "left-msb");
  if (0 == p)
    ps = "不可用";
  else if (255 == p)
    ps = "25.5m或更深";
  else
    // ps.Format("%fm",p*1.0/10);
    ps1 = to_string(p * 1.0 / 10) + "m";
  oneRec.field = "当前最深静态吃水量：" + ps;
  allInfo.push_back(oneRec);
  tag += 8;

  p = binstr2decnum(mes.substr(tag - 1, 6), "left-msb");
  // ps1.Format("%c",asc2[p]);
  ps1 = to_string(char(asc2[p]));
  tag += 6;
  ps = ps1;
  for (int i = 1; i <= 19; i++) {
    p = binstr2decnum(mes.substr(tag - 1, 6), "left-msb");
    // ps1.Format("%c",asc2[p]);
    ps1 = to_string(char(asc2[p]));
    ps += ps1;
    tag += 6;
  }
  if ("@@@@@@@@@@@@@@@@@@@@" == ps)
    ps = "不可用";
  oneRec.field = "目的地：" + ps;
  allInfo.push_back(oneRec);

  if (mes.substr(tag - 1, 1) == "1")
    ps = "不可用";
  else
    ps = "可用";
  oneRec.field = "数据终端设备：" + ps;
  allInfo.push_back(oneRec);
  tag++;

  ps = mes.substr(tag - 1, 1);
  if ("1" == ps)
    ps += "[错误，应设为0]";
  oneRec.field = "备用码：" + ps;
  allInfo.push_back(oneRec);
}
void AIS::re_message6(string data) {
  Record oneRec;
  oneRec.val = "";
  oneRec.field = "**********所恢复的信息如下**********";
  allInfo.push_back(oneRec);
  data = "000110" + data;

  string identify = data.substr(0, 6);
  int data_type = binstr2decnum(identify, "left-msb");
  string display;
  // display.Format("消息类型为：消息%d(二进制编址信息)",data_type);
  display =
      "消息类型为：消息" + to_string(data_type) + "(二进制编址信息)";
  oneRec.field = display;
  allInfo.push_back(oneRec);

  // 转发指示符2bit
  string transmit = data.substr(6, 2);
  int data_transmit = binstr2decnum(transmit, "left-msb");
  // display.Format("转发次数为：%d",data_transmit);
  display = "转发次数为：" + to_string(data_transmit);
  if (3 == data_transmit)
    display = "不再转发";
  oneRec.field = display;
  allInfo.push_back(oneRec);

  // 用户识别码30bit
  string classify = data.substr(8, 30);
  int data_classify = binstr2decnum(classify, "left-msb");
  if (data_classify >= 10000000000) {
    data_classify = floor(data_classify * 1.0 / 10);
    // display.Format("用户识别码(MMSI)：%d",data_classify);
    display = "用户识别码(MMSI)：" + to_string(data_classify);
  } else if (data_classify < 1000000000) {
    int num = 0;
    for (int i = 8; i <= 1; i--) {
      if (floor(data_classify * 1.0 / pow(10.0, i)) > 0) {
        num = 9 - i - 1;
        break;
      }
    }
    // display.Format("%d",data_classify);
    display = to_string(data_classify);
    for (int j = 1; j <= num; j++)
      display = "0" + display;
    display = "用户识别码(MMSI)：" + display;
  } else
    // display.Format("用户识别码(MMSI)：%d",data_classify);
    display = "用户识别码(MMSI)：" + to_string(data_classify);
  oneRec.field = display;
  allInfo.push_back(oneRec);

  // 序列号码
  string series = data.substr(38, 2);
  int data_series = binstr2decnum(series, "left-msb");
  // display.Format("序列号为：%d",data_series);
  display = "序列号为：" + to_string(data_series);
  oneRec.field = display;
  allInfo.push_back(oneRec);

  // 目标台识别码
  classify = data.substr(40, 30);
  data_classify = binstr2decnum(classify, "left-msb");
  if (data_classify >= 10000000000) {
    data_classify = floor(data_classify * 1.0 / 10);
    // display.Format("目标台识别码：%d",data_classify);
    display = "目标台识别码：" + to_string(data_classify);
  } else if (data_classify < 1000000000) {
    int num = 0;
    for (int i = 8; i >= 1; i--) {
      if (floor(data_classify * 1.0 / pow(10.0, i)) > 0) {
        num = 9 - i - 1;
        break;
      }
    }
    // display.Format("%d",data_classify);
    display = to_string(data_classify);
    for (int j = 1; j <= num; j++)
      display = "0" + display;
    display = "目标台识别码：" + display;
  } else
    // display.Format("%d",data_classify);
    display = to_string(data_classify);
  oneRec.field = display;
  allInfo.push_back(oneRec);

  // 重发标志
  if (data.substr(70, 1) == "1")
    display = "重发状态：重发";
  else
    display = "重发状态：未重发";
  oneRec.field = display;
  allInfo.push_back(oneRec);

  // 备用码
  oneRec.field = "备用码：不用应设为0";
  allInfo.push_back(oneRec);

  // 应用识别码
  string app = data.substr(71, 16);
  display = binstr2hexstr(app, "left-msb");
  display = "应用识别码为：" + display;
  oneRec.field = display;
  allInfo.push_back(oneRec);
  // display.Format("指定区域码为：%d",binstr2decnum(app.substr(0,10),"left-msb"));
  display = "指定区域码为：" +
            to_string(int(binstr2decnum(app.substr(0, 10), "left-msb")));
  oneRec.field = display;
  allInfo.push_back(oneRec);
  // display.Format("功能识别码为：%d",binstr2decnum(app.substr(10,6),"left-msb"));
  display = "功能识别码为：" +
            to_string(int(binstr2decnum(app.substr(10, 6), "left-msb")));
  oneRec.field = display;
  allInfo.push_back(oneRec);

  // 应用数据
  oneRec.field = "应用数据意义尚未得知，暂不显示";
  allInfo.push_back(oneRec);
}
void AIS::re_message7(string mes) {
  Record oneRec;
  oneRec.val = "";
  oneRec.field = "**********所恢复的信息如下**********";
  allInfo.push_back(oneRec);
  oneRec.field = "消息识别码：消息7(二进制确认)";
  allInfo.push_back(oneRec);

  int tag = 1;
  int p = binstr2decnum(mes.substr(tag - 1, 2), "left-msb");
  string ps;
  // ps.Format("%d",p);
  ps = to_string(p);
  oneRec.field = "转发次数：" + ps;
  ;
  allInfo.push_back(oneRec);
  tag += 2;

  //%%%%%%%%MMSI的判别，需要后续修改完整%%%%%%%%%%%%%%%%%%%
  p = binstr2decnum(mes.substr(tag - 1, 30), "left-msb");
  // ps.Format("%d",p);
  ps = to_string(p);
  if (ps.length() > 9)
    ps += "[错误，超过9位]";
  while (ps.length() < 9)
    ps = "0" + ps;
  oneRec.field = "用户识别码：" + ps;
  allInfo.push_back(oneRec);
  tag += 30;

  tag += 2; // 备用码，应设为0

  p = binstr2decnum(mes.substr(tag - 1, 30), "left-msb");
  // ps.Format("%d",p);
  ps = to_string(p);
  if (ps.length() > 9)
    ps += "[错误，超过9位]";
  while (ps.length() < 9)
    ps = "0" + ps;
  oneRec.field = "目的台识别码ID1：" + ps;
  allInfo.push_back(oneRec);
  tag += 30;

  p = binstr2decnum(mes.substr(tag - 1, 2), "left-msb");
  // ps.Format("%d",p);
  ps = to_string(p);
  oneRec.field = "ID1的序列号：" + ps;
  allInfo.push_back(oneRec);
  tag += 2;

  if (mes.length() > tag - 1) {
    p = binstr2decnum(mes.substr(tag - 1, 30), "left-msb");
    // ps.Format("%d",p);
    ps = to_string(p);
    if (ps.length() > 9)
      ps += "[错误，超过9位]";
    while (ps.length() < 9)
      ps = "0" + ps;
    oneRec.field = "目的台识别码ID2：" + ps;
    allInfo.push_back(oneRec);
    tag += 30;

    p = binstr2decnum(mes.substr(tag - 1, 2), "left-msb");
    // ps.Format("%d",p);
    ps = to_string(p);
    oneRec.field = "ID2的序列号：" + ps;
    allInfo.push_back(oneRec);
    tag += 2;
  }

  if (mes.length() > tag - 1) {
    p = binstr2decnum(mes.substr(tag - 1, 30), "left-msb");
    // ps.Format("%d",p);
    ps = to_string(p);
    if (ps.length() > 9)
      ps += "[错误，超过9位]";
    while (ps.length() < 9)
      ps = "0" + ps;
    oneRec.field = "目的台识别码ID3：" + ps;
    allInfo.push_back(oneRec);
    tag += 30;

    p = binstr2decnum(mes.substr(tag - 1, 2), "left-msb");
    // ps.Format("%d",p);
    ps = to_string(p);
    oneRec.field = "ID3的序列号：" + ps;
    allInfo.push_back(oneRec);
    tag += 2;
  }

  if (mes.length() > tag - 1) {
    p = binstr2decnum(mes.substr(tag - 1, 30), "left-msb");
    // ps.Format("%d",p);
    ps = to_string(p);
    if (ps.length() > 9)
      ps += "[错误，超过9位]";
    while (ps.length() < 9)
      ps = "0" + ps;
    oneRec.field = "目的台识别码ID4：" + ps;
    allInfo.push_back(oneRec);
    tag += 30;

    p = binstr2decnum(mes.substr(tag - 1, 2), "left-msb");
    // ps.Format("%d",p);
    ps = to_string(p);
    oneRec.field = "ID4的序列号：" + ps;
    allInfo.push_back(oneRec);
    tag += 2;
  }
}
void AIS::re_message8(string data) {
  Record oneRec;
  oneRec.val = "";
  oneRec.field = "**********所恢复的信息如下**********";
  allInfo.push_back(oneRec);
  data = "001000" + data;

  string identify = data.substr(0, 6);
  int data_type = binstr2decnum(identify, "left-msb");
  string display;
  // display.Format("消息类型为：消息%d(广播二进制信息)",data_type);
  display =
      "消息类型为：消息" + to_string(data_type) + "(广播二进制信息)";
  oneRec.field = display;
  allInfo.push_back(oneRec);

  // 转发指示符2bit
  string transmit = data.substr(6, 2);
  int data_transmit = binstr2decnum(transmit, "left-msb");
  // display.Format("转发次数为：%d",data_transmit);
  display = "转发次数为：" + to_string(data_transmit);
  if (3 == data_transmit)
    display = "不再转发";
  oneRec.field = display;
  allInfo.push_back(oneRec);

  // 用户识别码30bit
  string classify = data.substr(8, 30);
  int data_classify = binstr2decnum(classify, "left-msb");
  if (data_classify >= 10000000000) {
    data_classify = floor(data_classify * 1.0 / 10);
    // display.Format("消息源识别码：%d",data_classify);
    display = "消息源识别码：" + to_string(data_classify);
  } else if (data_classify < 1000000000) {
    int num = 0;
    for (int i = 8; i <= 1; i--) {
      if (floor(data_classify * 1.0 / pow(10.0, i)) > 0) {
        num = 9 - i - 1;
        break;
      }
    }
    // display.Format("%d",data_classify);
    display = to_string(data_classify);
    for (int j = 1; j <= num; j++)
      display = "0" + display;
    display = "消息源识别码：" + display;
  } else
    // display.Format("消息源识别码：%d",data_classify);
    display = "消息源识别码：" + to_string(data_classify);
  oneRec.field = display;
  allInfo.push_back(oneRec);

  // 备用码
  oneRec.field = "备用码：不用应设为0";
  allInfo.push_back(oneRec);

  // 应用识别码
  string app = data.substr(41, 16);
  display = binstr2hexstr(app, "left-msb");
  display = "应用识别码为：" + display;
  oneRec.field = display;
  allInfo.push_back(oneRec);
  // display.Format("指定区域码为：%d",binstr2decnum(app.substr(0,10),"left-msb"));
  display = "指定区域码为：" +
            to_string(int(binstr2decnum(app.substr(0, 10), "left-msb")));
  oneRec.field = display;
  allInfo.push_back(oneRec);
  // display.Format("功能识别码为：%d",binstr2decnum(app.substr(10,6),"left-msb"));
  display = "功能识别码为：" +
            to_string(int(binstr2decnum(app.substr(10, 6), "left-msb")));
  oneRec.field = display;
  allInfo.push_back(oneRec);

  // 应用数据
  oneRec.field = "应用数据意义尚未得知，暂不显示";
  allInfo.push_back(oneRec);
}
void AIS::re_message9(string mes) {
  Record oneRec;
  oneRec.val = "";
  oneRec.field = "**********所恢复的信息如下**********";
  allInfo.push_back(oneRec);
  oneRec.field = "消息识别码：消息9(标准搜救飞机位置报告)";
  allInfo.push_back(oneRec);

  int tag = 1;
  int p = binstr2decnum(mes.substr(tag - 1, 2), "left-msb");
  string ps;
  // ps.Format("%d",p);
  ps = to_string(p);
  oneRec.field = "转发次数：" + ps;
  ;
  allInfo.push_back(oneRec);
  tag += 2;

  //%%%%%%%%MMSI的判别，需要后续修改完整%%%%%%%%%%%%%%%%%%%
  p = binstr2decnum(mes.substr(tag - 1, 30), "left-msb");
  // ps.Format("%d",p);
  ps = to_string(p);
  if (ps.length() > 9)
    ps += "[错误，超过9位]";
  while (ps.length() < 9)
    ps = "0" + ps;
  oneRec.field = "用户识别码：" + ps;
  allInfo.push_back(oneRec);
  tag += 30;

  //
  p = binstr2decnum(mes.substr(tag - 1, 12), "left-msb");
  // ps.Format("%dm",p);
  ps = to_string(p) + "m";
  if (4095 == p)
    ps = "不可用";
  if (4094 == p)
    ps += "或以上";
  oneRec.field = "海拔高度：" + ps;
  allInfo.push_back(oneRec);
  tag += 12;

  //
  p = binstr2decnum(mes.substr(tag - 1, 10), "left-msb");
  if (1023 == p)
    ps = "不可用";
  else
    // ps.Format("%d",p);
    ps = to_string(p);
  if (1022 == p)
    oneRec.field = "对地航速：" + ps + "kn或以上";
  else
    oneRec.field = "对地航速：" + ps + "kn";
  allInfo.push_back(oneRec);
  tag += 10;

  //
  if (mes.substr(tag - 1, 1) == "1")
    ps = "高(<10m，DGNSS接收机的差分模式)";
  else
    ps = "低(>10m，GNSS接收机或其他电子定位系统的自主模式)";
  oneRec.field = "航位精确度：" + ps;
  allInfo.push_back(oneRec);
  tag++;

  //
  double p1;
  string ps2;
  if (mes.substr(tag - 1, 1) == "0") {
    ps = "东经";
    p1 = binstr2decnum(mes.substr(tag, 27), "left-msb") / 10000 / 60;
  } else {
    ps = "西经";
    p1 = (binstr2decnum(bitNot(mes.substr(tag, 27)), "left-msb") + 1) /
         10000 / 60;
  }
  // ps2.Format("%f度",p1);
  ps2 = to_string(float(p1)) + "度";
  if (mes.substr(tag - 1, 1) == "0" && p1 == 181)
    ps2 = "不可用";
  if ((mes.substr(tag - 1, 1) == "0" && p1 > 181) ||
      (mes.substr(tag - 1, 1) == "1" && p1 > 180))
    ps += "[错误，度数超过180度]";
  oneRec.field = "经度：" + ps + ps2;
  allInfo.push_back(oneRec);
  tag += 28;

  //
  if (mes.substr(tag - 1, 1) == "0") {
    ps = "北纬";
    p1 = binstr2decnum(mes.substr(tag, 26), "left-msb") / 10000 / 60;
  } else {
    ps = "南纬";
    p1 = (binstr2decnum(bitNot(mes.substr(tag, 26)), "left-msb") + 1) /
         10000 / 60;
  }
  // ps2.Format("%f度",p1);
  ps2 = to_string(float(p1)) + "度";
  if (mes.substr(tag - 1, 1) == "0" && p1 == 91)
    ps2 = "不可用";
  if ((mes.substr(tag - 1, 1) == "0" && p1 > 91) ||
      (mes.substr(tag - 1, 1) == "1" && p1 > 90))
    ps2 += "[错误，度数超过90度]";
  oneRec.field = "纬度：" + ps + ps2;
  allInfo.push_back(oneRec);
  tag += 27;

  //
  p = binstr2decnum(mes.substr(tag, 12), "left-msb");
  if (3600 == p)
    ps = "不可用";
  else
    // ps.Format("%f度",p*1.0/10);
    ps = to_string(float(p * 1.0 / 10)) + "度";
  if (p > 3600)
    ps += "[错误，度数超过359度]";
  oneRec.field = "对地航向：" + ps;
  allInfo.push_back(oneRec);
  tag += 12;

  //
  p = binstr2decnum(mes.substr(tag - 1, 9), "left-msb");
  if (511 == p)
    ps = "不可用";
  else
    // ps.Format("%d度",p);
    ps = to_string(p) + "度";
  if (p > 359)
    ps += "[错误，度数超过359度]";
  oneRec.field = "真航向：" + ps;
  allInfo.push_back(oneRec);
  tag += 9;

  //
  p = binstr2decnum(mes.substr(tag - 1, 6), "left-msb");
  if (60 == p)
    ps = "不可用";
  if (62 == p)
    ps = "电子定位系统以估算模式(船位推算法)运行";
  if (61 == p)
    ps = "电子定位系统以人工输入方法运行";
  if (63 == p)
    ps = "定位系统未运行";
  if (p < 60)
    // ps.Format("报告产生时的UTC为%d秒",p);
    ps = "报告产生时的UTC为" + to_string(p) + "秒";
  oneRec.field = "时间标记：" + ps;
  allInfo.push_back(oneRec);
  tag += 6;

  //
  p = binstr2decnum(mes.substr(tag - 1, 8), "left-msb");
  if (0 == p)
    ps = "未作地区性使用";
  else
    ps = "作地区性使用";
  oneRec.field = "地区性应用：" + ps;
  allInfo.push_back(oneRec);
  tag += 8;

  //
  if (mes.substr(tag - 1, 1) == "0")
    ps = "可用";
  else
    ps = "不可用";
  oneRec.field = "数据终端：" + ps;
  allInfo.push_back(oneRec);
  tag++;

  //
  p = binstr2decnum(mes.substr(tag - 1, 5), "left-msb");
  // ps.Format("%d",p);
  ps = to_string(p);
  if (p != 0)
    ps += "[错误，应为0(5 bit)]";
  else
    ps += "(5 bit)";
  oneRec.field = "备用位：" + ps;
  allInfo.push_back(oneRec);
  tag += 5;

  //
  if (mes.substr(tag - 1, 1) == "0")
    ps = "RAIM未使用";
  else
    ps = "RAIM使用";
  oneRec.field = "接收机自主完整性监测(RAIM)标志：" + ps;
  allInfo.push_back(oneRec);
  tag++;

  //
  oneRec.field = "通信状态：SOTDMA";
  allInfo.push_back(oneRec);
  //%%%%%%%%%%%%%已按国际标准Edition 1.5修改%%%%%%%%%%%%%%
  p = binstr2decnum(mes.substr(tag - 1, 2), "left-msb");
  if (0 == p)
    ps = "直接获取UTC";
  if (1 == p)
    ps = "间接获取UTC";
  if (2 == p)
    ps = "台站直接同步于基地台";
  if (3 == p)
    ps = "台站同步于另一个接收台数量最多的台站或同步于另一个直接与基地台同"
         "步的移动台站";
  oneRec.field = "同步状态：" + ps;
  allInfo.push_back(oneRec);
  tag += 2;

  p = binstr2decnum(mes.substr(tag - 1, 3), "left-msb");
  if (0 == p)
    ps = "该时隙最后一次传输";
  else
    // ps.Format("在时隙改变前保留该时隙%d帧",p);
    ps = "在时隙改变前保留该时隙" + to_string(p) + "帧";
  oneRec.field = "时隙超时：" + ps;
  allInfo.push_back(oneRec);
  tag += 3;

  int tmp = p;
  p = binstr2decnum(mes.substr(tag - 1, 14), "left-msb");
  if (0 == tmp)
    // ps.Format("时隙偏移量(下一帧该时隙的偏移量)：%d",p);
    ps = "时隙偏移量(下一帧该时隙的偏移量)：" + to_string(p);
  if (1 == tmp) {
    ps = "UTC小时和分钟：";
    ps2 = binstr2decstr(mes.substr(tag - 1, 5), "left-msb");
    ps += ps2 + "时";
    ps2 = binstr2decstr(mes.substr(tag + 4, 7), "left-msb");
    ps += ps2 + "分";
    if (binstr2decnum(mes.substr(tag - 1, 5), "left-msb") > 23)
      ps += "[错误，小时超过23]";
    if (binstr2decnum(mes.substr(tag + 4, 7), "left-msb") > 59)
      ps += "[错误，分钟超过59]";
  }
  if (3 == tmp || 5 == tmp || 7 == tmp) {
    // ps.Format("接收到的台站数量：%d",p);
    ps = "接收到的台站数量：" + to_string(p);
    if (p > 16383)
      ps += "[错误，超过16383]";
  }
  if (2 == tmp || 4 == tmp || 6 == tmp) {
    // ps.Format("用于该发射的时隙号：%d",p);
    ps = "用于该发射的时隙号：" + to_string(p);
    if (p > 2249)
      ps += "[错误，超过2249]";
  }
  oneRec.field = ps;
  allInfo.push_back(oneRec);
}
void AIS::re_message10(string data) {
  Record oneRec;
  oneRec.val = "";
  oneRec.field = "**********所恢复的信息如下**********";
  allInfo.push_back(oneRec);
  data = "001010" + data;

  string identify = data.substr(0, 6);
  int data_type = binstr2decnum(identify, "left-msb");
  string display;
  // display.Format("消息类型为：消息%d(UTC时间和日期询问)",data_type);
  display =
      "消息类型为：消息" + to_string(data_type) + "(UTC时间和日期询问)";
  oneRec.field = display;
  allInfo.push_back(oneRec);

  // 转发指示符2bit
  string transmit = data.substr(6, 2);
  int data_transmit = binstr2decnum(transmit, "left-msb");
  // display.Format("转发次数为：%d",data_transmit);
  display = "转发次数为：" + to_string(data_transmit);
  if (3 == data_transmit)
    display = "不再转发";
  oneRec.field = display;
  allInfo.push_back(oneRec);

  // 用户识别码30bit
  string classify = data.substr(8, 30);
  int data_classify = binstr2decnum(classify, "left-msb");
  if (data_classify >= 10000000000) {
    data_classify = floor(data_classify * 1.0 / 10);
    // display.Format("消息源识别码：%d",data_classify);
    display = "消息源识别码：" + to_string(data_classify);
  } else if (data_classify < 1000000000) {
    int num = 0;
    for (int i = 8; i <= 1; i--) {
      if (floor(data_classify * 1.0 / pow(10.0, i)) > 0) {
        num = 9 - i - 1;
        break;
      }
    }
    // display.Format("%d",data_classify);
    display = to_string(data_classify);
    for (int j = 1; j <= num; j++)
      display = "0" + display;
    display = "消息源识别码：" + display;
  } else
    // display.Format("消息源识别码：%d",data_classify);
    display = "消息源识别码：" + to_string(data_classify);
  oneRec.field = display;
  allInfo.push_back(oneRec);

  // 备用码
  oneRec.field = "备用码：不用应设为0";
  allInfo.push_back(oneRec);

  // 目标台识别码
  classify = data.substr(40, 30);
  data_classify = binstr2decnum(classify, "left-msb");
  if (data_classify >= 10000000000) {
    data_classify = floor(data_classify * 1.0 / 10);
    // display.Format("目标台识别码：%d",data_classify);
    display = "目标台识别码：" + to_string(data_classify);
  } else if (data_classify < 1000000000) {
    int num = 0;
    for (int i = 8; i <= 1; i--) {
      if (floor(data_classify * 1.0 / pow(10.0, i)) > 0) {
        num = 9 - i - 1;
        break;
      }
    }
    // display.Format("%d",data_classikfy);
    display = to_string(data_classify);
    for (int j = 1; j <= num; j++)
      display = "0" + display;
    display = "目标台识别码：" + display;
  } else
    // display.Format("目标台识别码：%d",data_classify);
    display = "目标台识别码：" + to_string(data_classify);
  oneRec.field = display;
  allInfo.push_back(oneRec);

  oneRec.field = "备用码：不用应设为0";
  allInfo.push_back(oneRec);
}
void AIS::re_message11(string mes) {
  Record oneRec;
  oneRec.val = "";
  oneRec.field = "**********所恢复的信息如下**********";
  allInfo.push_back(oneRec);
  oneRec.field = "消息识别码：消息11(移动台发出的UTC和日期回应)";
  allInfo.push_back(oneRec);

  int tag = 1;
  int p = binstr2decnum(mes.substr(tag - 1, 2), "left-msb");
  string ps;
  // ps.Format("%d",p);
  ps = to_string(p);
  oneRec.field = "转发次数：" + ps;
  ;
  allInfo.push_back(oneRec);
  tag += 2;

  //%%%%%%%%MMSI的判别，需要后续修改完整%%%%%%%%%%%%%%%%%%%
  p = binstr2decnum(mes.substr(tag - 1, 30), "left-msb");
  // ps.Format("%d",p);
  ps = to_string(p);
  if (ps.length() > 9)
    ps += "[错误，超过9位]";
  while (ps.length() < 9)
    ps = "0" + ps;
  oneRec.field = "用户识别码：" + ps;
  allInfo.push_back(oneRec);
  tag += 30;

  // UTC时间
  p = binstr2decnum(mes.substr(tag - 1, 14), "left-msb");
  // ps.Format("%d年",p);
  ps = to_string(p) + "年";
  if (0 == p)
    ps = "无UTC年份";
  if (p > 9999)
    ps += "[错误，年份超过9999]";
  oneRec.field = "UTC年份：" + ps;
  allInfo.push_back(oneRec);
  tag += 14;

  p = binstr2decnum(mes.substr(tag - 1, 4), "left-msb");
  // ps.Format("%d月",p);
  ps = to_string(p) + "月";
  if (0 == p)
    ps = "无UTC月份";
  if (p > 12)
    ps += "[错误，月份超过12]";
  oneRec.field = "UTC月份：" + ps;
  allInfo.push_back(oneRec);
  tag += 4;

  p = binstr2decnum(mes.substr(tag - 1, 5), "left-msb");
  // ps.Format("%d日",p);
  ps = to_string(p) + "日";
  if (0 == p)
    ps = "无UTC日期";
  oneRec.field = "UTC日期：" + ps;
  allInfo.push_back(oneRec);
  tag += 5;

  p = binstr2decnum(mes.substr(tag - 1, 5), "left-msb");
  // ps.Format("%d时",p);
  ps = to_string(p) + "时";
  if (0 == p)
    ps = "无UTC小时";
  if (p > 23)
    ps += "[错误，小时超过23]";
  oneRec.field = "UTC小时：" + ps;
  allInfo.push_back(oneRec);
  tag += 5;

  p = binstr2decnum(mes.substr(tag - 1, 6), "left-msb");
  // ps.Format("%d分",p);
  ps = to_string(p) + "分";
  if (0 == p)
    ps = "无UTC分钟";
  if (p > 60)
    ps += "[错误，分钟超过59]";
  oneRec.field = "UTC分钟：" + ps;
  allInfo.push_back(oneRec);
  tag += 6;

  p = binstr2decnum(mes.substr(tag - 1, 6), "left-msb");
  // ps.Format("%d秒",p);
  ps = to_string(p) + "秒";
  if (0 == p)
    ps = "无UTC秒";
  if (p > 60)
    ps += "[错误，秒超过59]";
  oneRec.field = "UTC秒：" + ps;
  allInfo.push_back(oneRec);
  tag += 6;

  //
  if (mes.substr(tag - 1, 1) == "1")
    ps = "高(<10m，DGNSS接收机的差分模式)";
  else
    ps = "低(>10m，GNSS接收机或其他电子定位系统的自主模式)";
  oneRec.field = "航位精确度：" + ps;
  allInfo.push_back(oneRec);
  tag++;

  //
  double p1;
  string ps2;
  if (mes.substr(tag - 1, 1) == "0") {
    ps = "东经";
    p1 = binstr2decnum(mes.substr(tag, 27), "left-msb") / 10000 / 60;
  } else {
    ps = "西经";
    p1 = (binstr2decnum(bitNot(mes.substr(tag, 27)), "left-msb") + 1) /
         10000 / 60;
  }
  // ps2.Format("%f度",p1);
  ps2 = to_string(float(p1)) + "度";
  if (mes.substr(tag - 1, 1) == "0" && p1 == 181)
    ps2 = "不可用";
  if ((mes.substr(tag - 1, 1) == "0" && p1 > 181) ||
      (mes.substr(tag - 1, 1) == "1" && p1 > 180))
    ps += "[错误，度数超过180度]";
  oneRec.field = "经度：" + ps + ps2;
  allInfo.push_back(oneRec);
  tag += 28;

  //
  if (mes.substr(tag - 1, 1) == "0") {
    ps = "北纬";
    p1 = binstr2decnum(mes.substr(tag, 26), "left-msb") / 10000 / 60;
  } else {
    ps = "南纬";
    p1 = (binstr2decnum(bitNot(mes.substr(tag, 26)), "left-msb") + 1) /
         10000 / 60;
  }
  // ps2.Format("%f度",p1);
  ps2 = to_string(float(p1)) + "度";
  if (mes.substr(tag - 1, 1) == "0" && p1 == 91)
    ps2 = "不可用";
  if ((mes.substr(tag - 1, 1) == "0" && p1 > 91) ||
      (mes.substr(tag - 1, 1) == "1" && p1 > 90))
    ps2 += "[错误，度数超过90度]";
  oneRec.field = "纬度：" + ps + ps2;
  allInfo.push_back(oneRec);
  tag += 27;

  //
  p = binstr2decnum(mes.substr(tag, 12), "left-msb");
  if (3600 == p)
    ps = "不可用";
  else
    // ps.Format("%f度",p*1.0/10);
    ps2 = to_string(float(p * 1.0 / 10)) + "度";
  if (p > 3600)
    ps += "[错误，度数超过359度]";
  oneRec.field = "对地航向：" + ps;
  allInfo.push_back(oneRec);
  tag += 12;

  //
  p = binstr2decnum(mes.substr(tag - 1, 9), "left-msb");
  if (511 == p)
    ps = "不可用";
  else
    // ps.Format("%d度",p);
    ps = to_string(p) + "度";
  if (p > 359)
    ps += "[错误，度数超过359度]";
  oneRec.field = "真航向：" + ps;
  allInfo.push_back(oneRec);
  tag += 9;

  //
  p = binstr2decnum(mes.substr(tag - 1, 4), "left-msb");
  if (0 == p)
    ps = "未定义";
  else if (1 == p)
    ps = "GPS";
  else if (2 == p)
    ps = "GLONASS";
  else if (3 == p)
    ps = "结合GPS/GLONASS";
  else if (4 == p)
    ps = "罗兰C";
  else if (5 == p)
    ps = "Chayka";
  else if (6 == p)
    ps = "结合导航系统";
  else if (7 == p)
    ps = "观测";
  else if (8 == p)
    ps = "伽利略";
  else
    ps = "[错误]";
  oneRec.field = "电子定位装置的类型：" + ps;
  allInfo.push_back(oneRec);
  tag += 4;

  p = binstr2decnum(mes.substr(tag - 1, 4), "left-msb");
  if (0 == p)
    ps = "0(10 bit)";
  else
    // ps.Format("%d[错误，备用码必须为0(10 bit)]",p);
    ps = to_string(int(p)) + "[错误，备用码必须为0(10 bit)]";
  oneRec.field = "备用码：" + ps;
  allInfo.push_back(oneRec);
  tag += 10;

  //
  if (mes.substr(tag - 1, 1) == "0")
    ps = "RAIM未使用";
  else
    ps = "RAIM使用";
  oneRec.field = "接收机自主完整性监测(RAIM)标志：" + ps;
  allInfo.push_back(oneRec);
  tag++;

  //
  oneRec.field = "通信状态：SOTDMA";
  allInfo.push_back(oneRec);
  //%%%%%%%%%%%%%已按国际标准Edition 1.5修改%%%%%%%%%%%%%%
  p = binstr2decnum(mes.substr(tag - 1, 2), "left-msb");
  if (0 == p)
    ps = "直接获取UTC";
  if (1 == p)
    ps = "间接获取UTC";
  if (2 == p)
    ps = "台站直接同步于基地台";
  if (3 == p)
    ps = "台站同步于另一个接收台数量最多的台站或同步于另一个直接与基地台同"
         "步的移动台站";
  oneRec.field = "同步状态：" + ps;
  allInfo.push_back(oneRec);
  tag += 2;

  //
  p = binstr2decnum(mes.substr(tag - 1, 3), "left-msb");
  if (0 == p)
    ps = "该时隙最后一次传输";
  else
    // ps.Format("在时隙改变前保留该时隙%d帧",p);
    ps = "在时隙改变前保留该时隙" + to_string(int(p)) + "帧";
  oneRec.field = "时隙超时：" + ps;
  allInfo.push_back(oneRec);
  tag += 3;

  int tmp = p;
  p = binstr2decnum(mes.substr(tag - 1, 14), "left-msb");
  if (0 == tmp)
    // ps.Format("时隙偏移量(下一帧该时隙的偏移量)：%d",p);
    ps = "时隙偏移量(下一帧该时隙的偏移量)：" + to_string(int(p));
  if (1 == tmp) {
    ps = "UTC小时和分钟：";
    ps2 = binstr2decstr(mes.substr(tag - 1, 5), "left-msb");
    ps += ps2 + "时";
    ps2 = binstr2decstr(mes.substr(tag + 4, 7), "left-msb");
    ps += ps2 + "分";
    if (binstr2decnum(mes.substr(tag - 1, 5), "left-msb") > 23)
      ps += "[错误，小时超过23]";
    if (binstr2decnum(mes.substr(tag + 4, 7), "left-msb") > 59)
      ps += "[错误，分钟超过59]";
  }
  if (3 == tmp || 5 == tmp || 7 == tmp) {
    // ps.Format("接收到的台站数量：%d",p);
    ps = "接收到的台站数量：" + to_string(int(p));
    if (p > 16383)
      ps += "[错误，超过16383]";
  }
  if (2 == tmp || 4 == tmp || 6 == tmp) {
    // ps.Format("用于该发射的时隙号：%d",p);
    ps = "用于该发射的时隙号：" + to_string(int(p));
    if (p > 2249)
      ps += "[错误，超过2249]";
  }
  oneRec.field = ps;
  allInfo.push_back(oneRec);
}
void AIS::re_message12(string data) {
  Record oneRec;
  oneRec.val = "";
  oneRec.field = "**********所恢复的信息如下**********";
  allInfo.push_back(oneRec);
  data = "001100" + data;

  string identify = data.substr(0, 6);
  int data_type = binstr2decnum(identify, "left-msb");
  string display;
  // display.Format("消息类型为：消息%d(编址安全信息)",data_type);
  display =
      "消息类型为：消息" + to_string(data_type) + "(编址安全信息)";
  oneRec.field = display;
  allInfo.push_back(oneRec);

  // 转发指示符2bit
  string transmit = data.substr(6, 2);
  int data_transmit = binstr2decnum(transmit, "left-msb");
  // display.Format("转发次数为：%d",data_transmit);
  display = "转发次数为：" + to_string(data_transmit);
  if (3 == data_transmit)
    display = "不再转发";
  oneRec.field = display;
  allInfo.push_back(oneRec);

  // 消息源识别码30bit
  string classify = data.substr(8, 30);
  int data_classify = binstr2decnum(classify, "left-msb");
  if (data_classify >= 10000000000) {
    data_classify = floor(data_classify * 1.0 / 10);
    // display.Format("消息源识别码：%d",data_classify);
    display = "消息源识别码：" + to_string(data_classify);
  } else if (data_classify < 1000000000) {
    int num = 0;
    for (int i = 8; i <= 1; i--) {
      if (floor(data_classify * 1.0 / pow(10.0, i)) > 0) {
        num = 9 - i - 1;
        break;
      }
    }
    // display.Format("%d",data_classify);
    display = to_string(data_classify);
    for (int j = 1; j <= num; j++)
      display = "0" + display;
    display = "消息源识别码：" + display;
  } else
    // display.Format("消息源识别码：%d",data_classify);
    display = "消息源识别码：" + to_string(data_classify);
  oneRec.field = display;
  allInfo.push_back(oneRec);

  // 序列号码
  string series = data.substr(38, 2);
  int data_series = binstr2decnum("%d", series);
  // display.Format("序列号为：%d",data_series);
  display = "序列号为：" + to_string(data_series);
  oneRec.field = display;
  allInfo.push_back(oneRec);

  // 目标台识别码
  classify = data.substr(40, 30);
  data_classify = binstr2decnum(classify, "left-msb");
  if (data_classify >= 10000000000) {
    data_classify = floor(data_classify * 1.0 / 10);
    // display.Format("目标台识别码：%d",data_classify);
    display = "目标台识别码：" + to_string(data_classify);
  } else if (data_classify < 1000000000) {
    int num = 0;
    for (int i = 8; i <= 1; i--) {
      if (floor(data_classify * 1.0 / pow(10.0, i)) > 0) {
        num = 9 - i - 1;
        break;
      }
    }
    // display.Format("%d",data_classify);
    display = to_string(data_classify);
    for (int j = 1; j <= num; j++)
      display = "0" + display;
    display = "目标台识别码：" + display;
  } else
    // display.Format("目标台识别码：%d",data_classify);
    display = "目标台识别码：" + to_string(data_classify);
  oneRec.field = display;
  allInfo.push_back(oneRec);

  // 重发标志
  if (data.substr(70, 1) == "1")
    display = "重发指令：重发";
  else
    display = "重发指令：不重发";
  oneRec.field = display;
  allInfo.push_back(oneRec);

  oneRec.field = "备用码：不用应设为0";
  allInfo.push_back(oneRec);
}
void AIS::re_message13(string mes) {
  Record oneRec;
  oneRec.val = "";
  oneRec.field = "**********所恢复的信息如下**********";
  allInfo.push_back(oneRec);
  oneRec.field = "消息识别码：消息13(安全确认)";
  allInfo.push_back(oneRec);

  int tag = 1;
  int p = binstr2decnum(mes.substr(tag - 1, 2), "left-msb");
  string ps;
  // ps.Format("%d",p);
  ps = to_string(p);
  oneRec.field = "转发次数：" + ps;
  ;
  allInfo.push_back(oneRec);
  tag += 2;

  //%%%%%%%%MMSI的判别，需要后续修改完整%%%%%%%%%%%%%%%%%%%
  p = binstr2decnum(mes.substr(tag - 1, 30), "left-msb");
  // ps.Format("%d",p);
  ps = to_string(p);
  if (ps.length() > 9)
    ps += "[错误，超过9位]";
  while (ps.length() < 9)
    ps = "0" + ps;
  oneRec.field = "消息源识别码：" + ps;
  allInfo.push_back(oneRec);
  tag += 30;

  tag += 2; // 备用码，应设为0

  p = binstr2decnum(mes.substr(tag - 1, 30), "left-msb");
  // ps.Format("%d",p);
  ps = to_string(p);
  if (ps.length() > 9)
    ps += "[错误，超过9位]";
  while (ps.length() < 9)
    ps = "0" + ps;
  oneRec.field = "目的台识别码ID1：" + ps;
  allInfo.push_back(oneRec);
  tag += 30;

  p = binstr2decnum(mes.substr(tag - 1, 2), "left-msb");
  // ps.Format("%d",p);
  ps = to_string(p);
  oneRec.field = "ID1的序列号：" + ps;
  allInfo.push_back(oneRec);
  tag += 2;

  if (mes.length() > tag - 1) {
    p = binstr2decnum(mes.substr(tag - 1, 30), "left-msb");
    // ps.Format("%d",p);
    ps = to_string(p);
    if (ps.length() > 9)
      ps += "[错误，超过9位]";
    while (ps.length() < 9)
      ps = "0" + ps;
    oneRec.field = "目的台识别码ID2：" + ps;
    allInfo.push_back(oneRec);
    tag += 30;

    p = binstr2decnum(mes.substr(tag - 1, 2), "left-msb");
    // ps.Format("%d",p);
    ps = to_string(p);
    oneRec.field = "ID2的序列号：" + ps;
    allInfo.push_back(oneRec);
    tag += 2;
  }

  if (mes.length() > tag - 1) {
    p = binstr2decnum(mes.substr(tag - 1, 30), "left-msb");
    // ps.Format("%d",p);
    ps = to_string(p);
    if (ps.length() > 9)
      ps += "[错误，超过9位]";
    while (ps.length() < 9)
      ps = "0" + ps;
    oneRec.field = "目的台识别码ID3：" + ps;
    allInfo.push_back(oneRec);
    tag += 30;

    p = binstr2decnum(mes.substr(tag - 1, 2), "left-msb");
    // ps.Format("%d",p);
    ps = to_string(p);
    oneRec.field = "ID3的序列号：" + ps;
    allInfo.push_back(oneRec);
    tag += 2;
  }

  if (mes.length() > tag - 1) {
    p = binstr2decnum(mes.substr(tag - 1, 30), "left-msb");
    // ps.Format("%d",p);
    ps = to_string(p);
    if (ps.length() > 9)
      ps += "[错误，超过9位]";
    while (ps.length() < 9)
      ps = "0" + ps;
    oneRec.field = "目的台识别码ID4：" + ps;
    allInfo.push_back(oneRec);
    tag += 30;

    p = binstr2decnum(mes.substr(tag - 1, 2), "left-msb");
    // ps.Format("%d",p);
    ps = to_string(p);
    oneRec.field = "ID4的序列号：" + ps;
    allInfo.push_back(oneRec);
    tag += 2;
  }
}
void AIS::re_message14(string data) {
  Record oneRec;
  oneRec.val = "";
  oneRec.field = "**********所恢复的信息如下**********";
  allInfo.push_back(oneRec);
  data = "001110" + data;

  string identify = data.substr(0, 6);
  int data_type = binstr2decnum(identify, "left-msb");
  string display;
  // display.Format("消息类型为：消息%d(安全广播信息)",data_type);
  display =
      "消息类型为：消息" + to_string(data_type) + "(安全广播信息)";
  oneRec.field = display;
  allInfo.push_back(oneRec);

  // 转发指示符2bit
  string transmit = data.substr(6, 2);
  int data_transmit = binstr2decnum(transmit, "left-msb");
  // display.Format("转发次数为：%d",data_transmit);
  display = "转发次数为：" + to_string(data_transmit);
  if (3 == data_transmit)
    display = "不再转发";
  oneRec.field = display;
  allInfo.push_back(oneRec);

  // 消息源识别码30bit
  string classify = data.substr(8, 30);
  int data_classify = binstr2decnum(classify, "left-msb");
  if (data_classify >= 10000000000) {
    data_classify = floor(data_classify * 1.0 / 10);
    // display.Format("消息源识别码：%d",data_classify);
    display = "消息源识别码：" + to_string(data_classify);
  } else if (data_classify < 1000000000) {
    int num = 0;
    for (int i = 8; i <= 1; i--) {
      if (floor(data_classify * 1.0 / pow(10.0, i)) > 0) {
        num = 9 - i - 1;
        break;
      }
    }
    // display.Format("%d",data_classify);
    display = to_string(data_classify);
    for (int j = 1; j <= num; j++)
      display = "0" + display;
    display = "消息源识别码：" + display;
  } else
    // display.Format("消息源识别码：%d",data_classify);
    display = "消息源识别码：" + to_string(data_classify);
  oneRec.field = display;
  allInfo.push_back(oneRec);

  oneRec.field = "备用码：不用应设为0";
  allInfo.push_back(oneRec);

  // 安全信息内容
  string info = data.substr(40);
  display = "安全信息内容(六位ASCII码显示)为：" + ascii6(info);
  oneRec.field = display;
  allInfo.push_back(oneRec);
}
void AIS::re_message15(string mes) {
  Record oneRec;
  oneRec.val = "";
  oneRec.field = "**********所恢复的信息如下**********";
  allInfo.push_back(oneRec);
  oneRec.field = "消息识别码：消息15(询问)";
  allInfo.push_back(oneRec);

  int tag = 1;
  int p = binstr2decnum(mes.substr(tag - 1, 2), "left-msb");
  string ps;
  // ps.Format("%d",p);
  ps = to_string(p);
  oneRec.field = "转发次数：" + ps;
  ;
  allInfo.push_back(oneRec);
  tag += 2;

  //%%%%%%%%MMSI的判别，需要后续修改完整%%%%%%%%%%%%%%%%%%%
  p = binstr2decnum(mes.substr(tag - 1, 30), "left-msb");
  // ps.Format("%d",p);
  ps = to_string(p);
  if (ps.length() > 9)
    ps += "[错误，超过9位]";
  while (ps.length() < 9)
    ps = "0" + ps;
  oneRec.field = "消息源识别码：" + ps;
  allInfo.push_back(oneRec);
  tag += 30;

  tag += 2; // 备用码，应设为0

  p = binstr2decnum(mes.substr(tag - 1, 30), "left-msb");
  // ps.Format("%d",p);
  ps = to_string(p);
  if (ps.length() > 9)
    ps += "[错误，超过9位]";
  while (ps.length() < 9)
    ps = "0" + ps;
  oneRec.field = "目的台识别码ID1：" + ps;
  allInfo.push_back(oneRec);
  tag += 30;

  p = binstr2decnum(mes.substr(tag - 1, 6), "left-msb");
  // ps.Format("%d",p);
  ps = to_string(p);
  oneRec.field = "信息识别码ID1.1：" + ps;
  allInfo.push_back(oneRec);
  tag += 6;

  p = binstr2decnum(mes.substr(tag - 1, 12), "left-msb");
  // ps.Format("%d",p);
  ps = to_string(p);
  oneRec.field = "时隙偏移1.1：" + ps;
  allInfo.push_back(oneRec);
  tag += 12;

  tag += 2; // 备用码，应设为0

  if (mes.length() > tag - 1) {
    p = binstr2decnum(mes.substr(tag - 1, 6), "left-msb");
    // ps.Format("%d",p);
    ps = to_string(p);
    oneRec.field = "信息识别码ID1.2：" + ps;
    tag += 6;

    p = binstr2decnum(mes.substr(tag - 1, 12), "left-msb");
    // ps.Format("%d",p);
    ps = to_string(p);
    oneRec.field = "时隙偏移1.2：" + ps;
    allInfo.push_back(oneRec);
    tag += 12;

    tag += 2;
  }

  if (mes.length() > tag - 1) {
    p = binstr2decnum(mes.substr(tag - 1, 30), "left-msb");
    // ps.Format("%d",p);
    ps = to_string(p);
    if (ps.length() > 9)
      ps += "[错误，超过9位]";
    while (ps.length() < 9)
      ps = "0" + ps;
    oneRec.field = "目的台识别码ID2：" + ps;
    allInfo.push_back(oneRec);
    tag += 30;

    p = binstr2decnum(mes.substr(tag - 1, 6), "left-msb");
    // ps.Format("%d",p);
    ps = to_string(p);
    oneRec.field = "信息识别码ID2.1：" + ps;
    allInfo.push_back(oneRec);
    tag += 6;

    p = binstr2decnum(mes.substr(tag - 1, 12), "left-msb");
    // ps.Format("%d",p);
    ps = to_string(p);
    oneRec.field = "时隙偏移2.1：" + ps;
    allInfo.push_back(oneRec);
    tag += 12;

    tag += 2;
  }
}
void AIS::re_message16(string data) {
  Record oneRec;
  oneRec.val = "";
  oneRec.field = "**********所恢复的信息如下**********";
  allInfo.push_back(oneRec);
  data = "010000" + data;

  string identify = data.substr(0, 6);
  int data_type = binstr2decnum(identify, "left-msb");
  string display;
  // display.Format("消息类型为：消息%d(分配模式指令)",data_type);
  display =
      "消息类型为：消息" + to_string(data_type) + "(分配模式指令)";
  oneRec.field = display;
  allInfo.push_back(oneRec);

  // 转发指示符2bit
  string transmit = data.substr(6, 2);
  int data_transmit = binstr2decnum(transmit, "left-msb");
  // display.Format("转发次数为：%d",data_transmit);
  display = "转发次数为：" + to_string(data_transmit);
  if (3 == data_transmit)
    display = "不再转发";
  oneRec.field = display;
  allInfo.push_back(oneRec);

  // 消息源识别码30bit
  string classify = data.substr(8, 30);
  int data_classify = binstr2decnum(classify, "left-msb");
  if (data_classify >= 10000000000) {
    data_classify = floor(data_classify * 1.0 / 10);
    // display.Format("消息源识别码：%d",data_classify);
    display = "消息源识别码：" + to_string(data_classify);
  } else if (data_classify < 1000000000) {
    int num = 0;
    for (int i = 8; i <= 1; i--) {
      if (floor(data_classify * 1.0 / pow(10.0, i)) > 0) {
        num = 9 - i - 1;
        break;
      }
    }
    // display.Format("%d",data_classify);
    display = to_string(data_classify);
    for (int j = 1; j <= num; j++)
      display = "0" + display;
    display = "消息源识别码：" + display;
  } else
    // display.Format("消息源识别码：%d",data_classify);
    display = "消息源识别码：" + to_string(data_classify);
  oneRec.field = display;
  allInfo.push_back(oneRec);

  oneRec.field = "备用码：不用应设为0";
  allInfo.push_back(oneRec);

  // 目标台识别码IDA
  transmit = data.substr(6, 2);
  data_transmit = binstr2decnum(transmit, "left-msb");
  // display.Format("转发次数为：%d",data_transmit);
  display = "转发次数为：" + to_string(data_transmit);
  if (3 == data_transmit)
    display = "不再转发";
  oneRec.field = display;
  allInfo.push_back(oneRec);

  // 消息源识别码30bit
  classify = data.substr(40, 30);
  data_classify = binstr2decnum(classify, "left-msb");
  if (data_classify >= 10000000000) {
    data_classify = floor(data_classify * 1.0 / 10);
    // display.Format("目标台识别码IDA：%d",data_classify);
    display = "目标台识别码IDA：" + to_string(data_classify);
  } else if (data_classify < 1000000000) {
    int num = 0;
    for (int i = 8; i <= 1; i--) {
      if (floor(data_classify * 1.0 / pow(10.0, i)) > 0) {
        num = 9 - i - 1;
        break;
      }
    }
    // display.Format("%d",data_classify);
    display = to_string(data_classify);
    for (int j = 1; j <= num; j++)
      display = "0" + display;
    display = "目标台识别码IDA：" + display;
  } else
    // display.Format("目标台识别码IDA：%d",data_classify);
    display = "目标台识别码IDA：" + to_string(data_classify);
  oneRec.field = display;
  allInfo.push_back(oneRec);
}
void AIS::re_message17(string mes) {
  Record oneRec;
  oneRec.val = "";
  oneRec.field = "**********所恢复的信息如下**********";
  allInfo.push_back(oneRec);
  oneRec.field = "消息识别码：消息17(全球导航卫星系统广播二进制信息)";
  allInfo.push_back(oneRec);

  int tag = 1;
  int p = binstr2decnum(mes.substr(tag - 1, 2), "left-msb");
  string ps;
  // ps.Format("%d",p);
  ps = to_string(p);
  oneRec.field = "转发次数：" + ps;
  ;
  allInfo.push_back(oneRec);
  tag += 2;

  //%%%%%%%%MMSI的判别，需要后续修改完整%%%%%%%%%%%%%%%%%%%
  p = binstr2decnum(mes.substr(tag - 1, 30), "left-msb");
  // ps.Format("%d",p);
  ps = to_string(p);
  if (ps.length() > 9)
    ps += "[错误，超过9位]";
  while (ps.length() < 9)
    ps = "0" + ps;
  oneRec.field = "消息源识别码：" + ps;
  allInfo.push_back(oneRec);
  tag += 30;

  p = binstr2decnum(mes.substr(tag - 1, 2), "left-msb");
  // ps.Format("%d",p);
  ps = to_string(p);
  if (0 == p)
    ps += "(2 bit)";
  else
    ps += "[错误，必须为0(2 bit)]";
  oneRec.field = "备用码：" + ps;
  allInfo.push_back(oneRec);
  tag += 2;

  //
  double p1;
  string ps2;
  if (mes.substr(tag - 1, 1) == "0") {
    ps = "东经";
    p1 = binstr2decnum(mes.substr(tag, 17), "left-msb") / 10 / 60;
  } else {
    ps = "西经";
    p1 = (binstr2decnum(bitNot(mes.substr(tag, 17)), "left-msb") + 1) / 10 /
         60;
  }
  // ps2.Format("%f度",p1);
  ps2 = to_string(float(p1)) + "度";
  if (mes.substr(tag - 1, 1) == "0" && p1 == 181)
    ps2 = "不可用";
  if ((mes.substr(tag - 1, 1) == "0" && p1 > 181) ||
      (mes.substr(tag - 1, 1) == "1" && p1 > 180))
    ps += "[错误，度数超过180度]";
  oneRec.field = "经度：" + ps + ps2;
  allInfo.push_back(oneRec);
  tag += 18;

  //
  if (mes.substr(tag - 1, 1) == "0") {
    ps = "北纬";
    p1 = binstr2decnum(mes.substr(tag, 16), "left-msb") / 10 / 60;
  } else {
    ps = "南纬";
    p1 = (binstr2decnum(bitNot(mes.substr(tag, 16)), "left-msb") + 1) / 10 /
         60;
  }
  // ps2.Format("%f度",p1);
  ps2 = to_string(float(p1)) + "度";
  if (mes.substr(tag - 1, 1) == "0" && p1 == 91)
    ps2 = "不可用";
  if ((mes.substr(tag - 1, 1) == "0" && p1 > 91) ||
      (mes.substr(tag - 1, 1) == "1" && p1 > 90))
    ps2 += "[错误，度数超过90度]";
  oneRec.field = "纬度：" + ps + ps2;
  allInfo.push_back(oneRec);
  tag += 17;

  p = binstr2decnum(mes.substr(tag - 1, 5), "left-msb");
  // ps.Format("%d",p);
  ps = to_string(p);
  if (0 == p)
    ps += "(5 bit)";
  else
    ps += "[错误，必须为0(5 bit)]";
  oneRec.field = "备用码：" + ps;
  allInfo.push_back(oneRec);
  tag += 5;

  oneRec.field = "***差分修正数据组成如下***";
  allInfo.push_back(oneRec);
  p = binstr2decnum(mes.substr(tag - 1, 6), "left-msb");
  // ps.Format("%d",p);
  ps = to_string(p);
  oneRec.field = "消息类型：" + ps;
  allInfo.push_back(oneRec);
  tag += 6;

  p = binstr2decnum(mes.substr(tag - 1, 10), "left-msb");
  // ps.Format("%d",p);
  ps = to_string(p);
  oneRec.field = "台站识别码：" + ps;
  allInfo.push_back(oneRec);
  tag += 10;

  p = binstr2decnum(mes.substr(tag - 1, 13), "left-msb");
  // ps.Format("%fs",p*0.6);
  ps = to_string(float(p * 0.6)) + "s";
  if (p > (3599.4 / 0.6))
    ps += "[错误，超过3599.4s]";
  oneRec.field = "Z计数：" + ps;
  allInfo.push_back(oneRec);
  tag += 13;

  p = binstr2decnum(mes.substr(tag - 1, 3), "left-msb");
  // ps.Format("%d",p);
  ps = to_string(p);
  oneRec.field = "消息序列号：" + ps;
  allInfo.push_back(oneRec);
  tag += 3;

  p = binstr2decnum(mes.substr(tag - 1, 5), "left-msb");
  // ps.Format("%d",p);
  ps = to_string(p);
  if (p > 29)
    ps += "[错误，超过29]";
  oneRec.field = "N：" + ps;
  allInfo.push_back(oneRec);
  tag += 5;

  p = binstr2decnum(mes.substr(tag - 1, 3), "left-msb");
  // ps.Format("%d",p);
  ps = to_string(p);
  oneRec.field = "健康状态：" + ps;
  allInfo.push_back(oneRec);
  tag += 3;

  ps = mes.substr(tag - 1);
  oneRec.field = "DGNSS数据字：" + ps;
  allInfo.push_back(oneRec);
}
void AIS::re_message18(string data) {
  Record oneRec;
  oneRec.val = "";
  oneRec.field = "**********所恢复的信息如下**********";
  allInfo.push_back(oneRec);
  data = "010010" + data;

  string identify = data.substr(0, 6);
  int data_type = binstr2decnum(identify, "left-msb");
  string display;
  // display.Format("消息类型为：消息%d(标准B类设备位置报告)",data_type);
  display = "消息类型为：消息" + to_string(data_type) +
            "(标准B类设备位置报告)";
  oneRec.field = display;
  allInfo.push_back(oneRec);

  // 转发指示符2bit
  string transmit = data.substr(6, 2);
  int data_transmit = binstr2decnum(transmit, "left-msb");
  // display.Format("转发次数为：%d",data_transmit);
  display = "转发次数为：" + to_string(data_transmit);
  if (3 == data_transmit)
    display = "不再转发";
  oneRec.field = display;
  allInfo.push_back(oneRec);

  // 消息源识别码30bit
  string classify = data.substr(8, 30);
  int data_classify = binstr2decnum(classify, "left-msb");
  if (data_classify >= 10000000000) {
    data_classify = floor(data_classify * 1.0 / 10);
    // display.Format("消息源识别码：%d",data_classify);
    display = "消息源识别码：" + to_string(data_classify);
  } else if (data_classify < 1000000000) {
    int num = 0;
    for (int i = 8; i <= 1; i--) {
      if (floor(data_classify * 1.0 / pow(10.0, i)) > 0) {
        num = 9 - i - 1;
        break;
      }
    }
    // display.Format("%d",data_classify);
    display = to_string(data_classify);
    for (int j = 1; j <= num; j++)
      display = "0" + display;
    display = "消息源识别码：" + display;
  } else
    // display.Format("消息源识别码：%d",data_classify);
    display = "消息源识别码：" + to_string(data_classify);
  oneRec.field = display;
  allInfo.push_back(oneRec);

  // 为地区或区域应用保留8bit
  string savefor = data.substr(38, 7);
  if (binstr2decnum(savefor, "left-msb") != 0)
    display = "为地区或区域应用保留";
  else
    display = "未定义，默认为0";
  oneRec.field = display;
  allInfo.push_back(oneRec);

  // 对地航速10bit
  string rate = data.substr(46, 10);
  int data_rate = binstr2decnum(rate, "left-msb");
  if (1023 == data_rate)
    display = "错误，不可用";
  else
    // display.Format("对地航速为：%fkn",data_rate*1.0/10);
    display =
        "对地航速为：" + to_string(float(data_rate * 1.0 / 10)) + "kn";
  oneRec.field = display;
  allInfo.push_back(oneRec);

  // 航位精确度1bit
  if (data.substr(56, 1) == "1")
    display = "航位精确度：高";
  else
    display = "航位精确度：低";
  oneRec.field = display;
  allInfo.push_back(oneRec);

  // 经度28bit
  string longitude = data.substr(57, 28);
  if (longitude.substr(0, 1) == "0") {
    int data_longitude = binstr2decnum(longitude.substr(1, 27), "left-msb");
    // display.Format("经度为：东经%f度",data_longitude*1.0/600000);
    display = "经度为：东经" +
              to_string(float(data_longitude * 1.0 / 600000)) + "度";
  } else {
    int data_longitude =
        binstr2decnum(bitNot(longitude.substr(1, 27)), "left-msb") + 1;
    // display.Format("经度为：西经%f度",data_longitude*1.0/600000);
    display = "经度为：西经" +
              to_string(float(data_longitude * 1.0 / 600000)) + "度";
  }
  oneRec.field = display;
  allInfo.push_back(oneRec);

  // 纬度27bit
  string latitude = data.substr(85, 27);
  if (latitude.substr(0, 1) == "0") {
    int data_latitude = binstr2decnum(latitude.substr(1, 26), "left-msb");
    // display.Format("纬度为：北纬%f度",data_latitude);
    display = "纬度为：北纬" + to_string(float(data_latitude)) + "度";
  } else {
    int data_latitude =
        binstr2decnum(bitNot(latitude.substr(1, 26)), "left-msb") + 1;
    // display.Format("纬度为：南纬%f度",data_latitude);
    display = "纬度为：南纬" + to_string(float(data_latitude)) + "度";
  }
  oneRec.field = display;
  allInfo.push_back(oneRec);

  // 对地航向12bit
  string course = data.substr(112, 12);
  int data_course = binstr2decnum(course, "left-msb");
  if (3600 == data_course)
    display = "对地航向系统默认,此项指标不可用";
  else if (data_course < 3600)
    // display.Format("对地航向为：%f度",data_course*1.0/10);
    display = "对地航向为：" + to_string(float(data_course * 1.0 / 10)) +
              "度";
  else
    display = "对地航向显示错误";
  oneRec.field = display;
  allInfo.push_back(oneRec);

  // 真航向9bit
  string realcourse = data.substr(124, 9);
  int data_realcourse = binstr2decnum(realcourse, "left-msb");
  if (511 == data_realcourse)
    display = "真航向系统默认,此项指标不可用";
  else if (data_realcourse < 360)
    // display.Format("真航向为：%d度",data_realcourse);
    display = "真航向为：" + to_string(data_realcourse) + "度";
  else
    display = "真航向显示错误";
  oneRec.field = display;
  allInfo.push_back(oneRec);

  // 时间标记6bit
  string timemark = data.substr(133, 6);
  int data_timemark = binstr2decnum(timemark, "left-msb");
  // display.Format("报告产生时的UTC为%d秒");
  display = "报告产生时的UTC为" + to_string(data_timemark) + "秒";
  if (63 == data_timemark)
    display = "定位系统未运行";
  else if (62 == data_timemark)
    display = "电子定位系统以估算模式(船位推算法)运行";
  else if (61 == data_timemark)
    display = "定位系统以人工输人方法运行";
  else if (60 == data_timemark)
    display = "时间标记系统默认,此项指标不可用";
  oneRec.field = display;
  allInfo.push_back(oneRec);

  // 此项为地区性应用所保留4bit
  string protect = data.substr(139, 4);
  int data_protect = binstr2decnum(protect, "left-msb");
  if (0 == data_protect)
    display = "地区性应用: 系统默认,此项指标不可用";
  else
    display = "地区性使用指标(尚待查明)";
  oneRec.field = display;
  allInfo.push_back(oneRec);

  // 备用码
  oneRec.field = "备用码应设为0";
  allInfo.push_back(oneRec);

  // RAIM标志
  if (data.substr(147, 1) == "1")
    display = "RAIM(接收机自主完整性监测)使用";
  else
    display = "RAIM(接收机自主完整性监测)未使用";
  oneRec.field = display;
  allInfo.push_back(oneRec);

  // 通信状态
  if (data.substr(148, 1) == "1")
    display = "通行状态：ITDMA";
  else
    display = "通行状态：SOTDMA";
  oneRec.field = display;
  allInfo.push_back(oneRec);

  // 通信状态19bit
  string comstate = data.substr(149, 19);
  if (data.substr(148, 1) == "0") {
    int data_comstate = binstr2decnum(comstate.substr(0, 2), "left-msb");
    if (0 == data_comstate)
      display = "同步状态：直接获取UTC";
    else if (1 == data_comstate)
      display = "同步状态：间接获取UTC";
    else if (2 == data_comstate)
      display = "同步状态：台站同步于基地台";
    else
      display = "同步状态：台站同步于另一个接收台数量最多的台站";
    oneRec.field = display;
    allInfo.push_back(oneRec);

    data_comstate = binstr2decnum(comstate.substr(2, 3), "left-msb");
    if (0 == data_comstate)
      display = "该时隙最后一次传输";
    else
      // display.Format("时隙超时：在时隙改变前将保留该时隙%d帧",data_comstate);
      display = "时隙超时：在时隙改变前将保留该时隙" +
                to_string(data_comstate) + "帧";
    oneRec.field = display;
    allInfo.push_back(oneRec);

    if (0 == data_comstate)
      // display.Format("时隙偏移量为：%d",binstr2decnum(comstate.substr(5,14),"left-msb"));
      display =
          "时隙偏移量为：" +
          to_string(int(binstr2decnum(comstate.substr(5, 14), "left-msb")));
    else if (1 == data_comstate)
      // display.Format("UTC小时和分钟为：%d时%d分",binstr2decnum(comstate.substr(5,5),"left-msb"),binstr2decnum(comstate.substr(10,7),"left-msb"));
      display =
          "UTC小时和分钟为：" +
          to_string(int(binstr2decnum(comstate.substr(5, 5), "left-msb"))) +
          "时" +
          to_string(
              int(binstr2decnum(comstate.substr(10, 7), "left-msb"))) +
          "分";
    else if (2 == data_comstate || 4 == data_comstate || 6 == data_comstate)
      // display.Format("时隙号为：%d",binstr2decnum(comstate.substr(5,14),"left-msb"));
      display =
          "时隙号为：" +
          to_string(int(binstr2decnum(comstate.substr(5, 14), "left-msb")));
    else
      // display.Format("接收到的台站数量为：%d",binstr2decnum(comstate.substr(5,14),"left-msb"));
      display =
          "接收到的台站数量为：" +
          to_string(int(binstr2decnum(comstate.substr(5, 14), "left-msb")));
    oneRec.field = display;
    allInfo.push_back(oneRec);
  } else {
    int data_comstate = binstr2decnum(comstate.substr(0, 2), "left-msb");
    if (0 == data_comstate)
      display = "同步状态：直接获取UTC";
    else if (1 == data_comstate)
      display = "同步状态：间接获取UTC";
    else if (2 == data_comstate)
      display = "同步状态：台站同步于基地台";
    else
      display = "同步状态：台站同步于另一个接收台数量最多的台站";
    oneRec.field = display;
    allInfo.push_back(oneRec);

    data_comstate = binstr2decnum(comstate.substr(2, 13), "left-msb");
    // display.Format("时隙偏移量为：%d",data_comstate);
    display = "时隙偏移量为：" + to_string(data_comstate);
    oneRec.field = display;
    allInfo.push_back(oneRec);

    data_comstate = binstr2decnum(comstate.substr(15, 3), "left-msb");
    // display.Format("应分配的时隙数量为：%d",data_comstate);
    display = "应分配的时隙数量为：" + to_string(data_comstate);
    oneRec.field = display;
    allInfo.push_back(oneRec);

    if (comstate.substr(18, 1) == "1")
      display = "时隙分配应多保留一帧";
    else
      display = "时隙分配不保留";
    oneRec.field = display;
    allInfo.push_back(oneRec);
  }
}
void AIS::re_message19(string mes) {
  Record oneRec;
  oneRec.val = "";
  oneRec.field = "**********所恢复的信息如下**********";
  allInfo.push_back(oneRec);
  oneRec.field = "消息识别码：消息19(扩展B类设备位置报告)";
  allInfo.push_back(oneRec);

  CHAR asc2[64] = {'@', 'A', 'B', 'C', 'D', 'E', 'F',  'G', 'H', 'I', 'J',
                   'K', 'L', 'M', 'N', 'O', 'P', 'Q',  'R', 'S', 'T', 'U',
                   'V', 'W', 'X', 'Y', 'Z', '[', '\\', ']', '^', '_', ' ',
                   '!', '"', '#', '$', '%', '&', '\'', '(', ')', '*', '+',
                   ',', '-', '.', '/', '0', '1', '2',  '3', '4', '5', '6',
                   '7', '8', '9', ':', ';', '<', '=',  '>', '?'};
  int tag = 1;
  int p = binstr2decnum(mes.substr(tag - 1, 2), "left-msb");
  string ps;
  // ps.Format("%d",p);
  ps = to_string(p);
  oneRec.field = "转发次数：" + ps;
  allInfo.push_back(oneRec);
  tag += 2;

  //%%%%%%%%MMSI的判别，需要后续修改完整%%%%%%%%%%%%%%%%%%%
  p = binstr2decnum(mes.substr(tag - 1, 30), "left-msb");
  // ps.Format("%d",p);
  ps = to_string(p);
  if (ps.length() > 9)
    ps += "[错误，超过9位]";
  while (ps.length() < 9)
    ps = "0" + ps;
  oneRec.field = "用户识别码：" + ps;
  allInfo.push_back(oneRec);
  tag += 30;

  p = binstr2decnum(mes.substr(tag - 1, 8), "left-msb");
  // ps.Format("%d",p);
  ps = to_string(p);
  if (0 == p)
    ps = "做地区性使用";
  else
    ps = "不做地区性使用";
  oneRec.field = "地区性使用：" + ps;
  allInfo.push_back(oneRec);
  tag += 8;

  p = binstr2decnum(mes.substr(tag - 1, 10), "left-msb");
  // ps.Format("%f",p*1.0/10);
  ps = to_string(float(p * 1.0 / 10));
  if (1023 == p)
    ps = "不可用";
  else if (1022 == p)
    ps += "kn或以上";
  else
    ps += "kn";
  oneRec.field = "对地航速：" + ps;
  allInfo.push_back(oneRec);
  tag += 10;

  if (mes.substr(tag - 1, 1) == "1")
    ps = "高(<10m，DGNSS接收机的差分模式)";
  else
    ps = "低(>10m，GNSS接收机或其他电子定位系统的自主模式)";
  oneRec.field = "航位精确度：" + ps;
  allInfo.push_back(oneRec);
  tag++;

  double p1;
  string ps2;
  if (mes.substr(tag - 1, 1) == "0") {
    ps = "东经";
    p1 = binstr2decnum(mes.substr(tag, 27), "left-msb") / 10000 / 60;
  } else {
    ps = "西经";
    p1 = (binstr2decnum(bitNot(mes.substr(tag, 27)), "left-msb") + 1) /
         10000 / 60;
  }
  // ps2.Format("%f度",p1);
  ps2 = to_string(float(p1)) + "度";
  if (mes.substr(tag - 1, 1) == "0" && p1 == 181)
    ps2 = "不可用";
  if ((mes.substr(tag - 1, 1) == "0" && p1 > 181) ||
      (mes.substr(tag - 1, 1) == "1" && p1 > 180))
    ps += "[错误，度数超过180度]";
  oneRec.field = "经度：" + ps + ps2;
  allInfo.push_back(oneRec);
  tag += 28;

  //
  if (mes.substr(tag - 1, 1) == "0") {
    ps = "北纬";
    p1 = binstr2decnum(mes.substr(tag, 26), "left-msb") / 10000 / 60;
  } else {
    ps = "南纬";
    p1 = (binstr2decnum(bitNot(mes.substr(tag, 26)), "left-msb") + 1) /
         10000 / 60;
  }
  // ps2.Format("%f度",p1);
  ps2 = to_string(float(p1)) + "度";
  if (mes.substr(tag - 1, 1) == "0" && p1 == 91)
    ps2 = "不可用";
  if ((mes.substr(tag - 1, 1) == "0" && p1 > 91) ||
      (mes.substr(tag - 1, 1) == "1" && p1 > 90))
    ps2 += "[错误，度数超过90度]";
  oneRec.field = "纬度：" + ps + ps2;
  allInfo.push_back(oneRec);
  tag += 27;

  //
  p = binstr2decnum(mes.substr(tag, 12), "left-msb");
  if (3600 == p)
    ps = "不可用";
  else
    // ps.Format("%f度",p*1.0/10);
    ps2 = to_string(float(p * 1.0 / 10)) + "度";
  if (p > 3600)
    ps += "[错误，度数超过359度]";
  oneRec.field = "对地航向：" + ps;
  allInfo.push_back(oneRec);
  tag += 12;

  //
  p = binstr2decnum(mes.substr(tag - 1, 9), "left-msb");
  if (511 == p)
    ps = "不可用";
  else
    // ps.Format("%d度",p);
    ps2 = to_string(p) + "度";
  if (p > 359)
    ps += "[错误，度数超过359度]";
  oneRec.field = "真航向：" + ps;
  allInfo.push_back(oneRec);
  tag += 9;

  p = binstr2decnum(mes.substr(tag - 1, 6), "left-msb");
  if (60 == p)
    ps = "不可用";
  else if (62 == p)
    ps = "电子定位系统以估算模式(船位推算法)运行";
  else if (61 == p)
    ps = "电子定位系统以人工输入方法运行";
  else if (63 == p)
    ps = "定位系统未运行";
  else
    // ps.Format("报告产生时的UTC为%d秒",p);
    ps = "报告产生时的UTC为" + to_string(p) + "秒";
  oneRec.field = "时间标记：" + ps;
  allInfo.push_back(oneRec);
  tag += 6;

  p = binstr2decnum(mes.substr(tag - 1, 4), "left-msb");
  if (0 == p)
    ps = "未作地区性使用";
  else
    ps = "作地区性使用";
  oneRec.field = "地区性应用：" + ps;
  allInfo.push_back(oneRec);
  tag += 4;

  //
  p = binstr2decnum(mes.substr(tag - 1, 6), "left-msb");
  string ps1;
  // ps1.Format("%c",asc2[p]);
  ps1 = char(asc2[p]);
  tag += 6;
  ps = ps1;
  for (int i = 1; i <= 19; i++) {
    p = binstr2decnum(mes.substr(tag - 1, 6), "left-msb");
    // ps1.Format("%c",asc2[p]);
    ps1 = char(asc2[p]);
    ps += ps1;
    tag += 6;
  }
  if ("@@@@@@@@@@@@@@@@@@@@" == ps)
    ps = "不可用";
  oneRec.field = "名称：" + ps;
  allInfo.push_back(oneRec);

  p = binstr2decnum(mes.substr(tag - 1, 8), "left-msb");
  if (0 == p)
    ps = "不可用或非船舶";
  else if (50 == p)
    ps = "引航船";
  else if (51 == p)
    ps = "搜救船";
  else if (52 == p)
    ps = "拖轮";
  else if (53 == p)
    ps = "港口供应船";
  else if (54 == p)
    ps = "载有防污染装置和设备的船舶";
  else if (55 == p)
    ps = "执法船";
  else if (56 == p)
    ps = "备用-分配给当地船舶使用";
  else if (57 == p)
    ps = "备用-分配给当地船舶使用";
  else if (58 == p)
    ps = "医疗船";
  else if (59 == p)
    ps = "符合《无线电规则》第18号决议(Mob-83)的船舶";
  else if (30 == p)
    ps = "船舶(从事捕鱼)";
  else if (31 == p)
    ps = "船舶(从事拖带作业)";
  else if (32 == p)
    ps = "船舶(从事拖带作业，且拖带长度超过200m，或宽度超过25m)";
  else if (33 == p)
    ps = "船舶(从事疏浚或水下作业)";
  else if (34 == p)
    ps = "船舶(从事潜水作业)";
  else if (35 == p)
    ps = "船舶(参与军事行动)";
  else if (36 == p)
    ps = "船舶(驶帆航行)";
  else if (37 == p)
    ps = "船舶(娱乐船)";
  else if (38 == p)
    ps = "船舶(为将来使用保留)";
  else if (39 == p)
    ps = "船舶(为将来使用保留)";
  vector<string> second;
  second.push_back("(这一类型的所有船舶)");
  second.push_back("(载有危险品，有害物质或海上污染物，属IMO规定的A类有害或"
                   "污染物质的船舶)");
  second.push_back("(载有危险品，有害物质或海上污染物，属IMO规定的B类有害或"
                   "污染物质的船舶)");
  second.push_back("(载有危险品，有害物质或海上污染物，属IMO规定的C类有害或"
                   "污染物质的船舶)");
  second.push_back("(载有危险品，有害物质或海上污染物，属IMO规定的D类有害或"
                   "污染物质的船舶)");
  second.push_back("(为将来使用保留)");
  second.push_back("(为将来使用保留)");
  second.push_back("(为将来使用保留)");
  second.push_back("(为将来使用保留)");
  second.push_back("(无附加信息)");
  int firstno = floor(p * 1.0 / 10);
  int secondno = p - firstno * 10;
  if (1 == firstno)
    ps = "为将来使用保留";
  else if (2 == firstno)
    ps = "地效应船";
  else if (4 == firstno)
    ps = "高速船";
  else if (6 == firstno)
    ps = "客船";
  else if (7 == firstno)
    ps = "货船";
  else if (8 == firstno)
    ps = "油轮";
  else if (9 == firstno)
    ps = "其他类型的船舶";
  if (firstno < 10 && firstno != 0 && firstno != 3 && firstno != 5)
    ps += second.at(secondno);
  if (p >= 100 && p < 200)
    ps = "为地区性使用保留";
  if (p >= 200)
    ps = "为将来使用保留";
  if (p > 0 && p < 10)
    ps = "标准中无定义";
  oneRec.field = "船舶及载货类型：" + ps;
  allInfo.push_back(oneRec);
  tag += 8;

  ps = "";
  p = binstr2decnum(mes.substr(tag - 1, 9), "left-msb");
  // ps1.Format("A-%dm；",p);
  ps1 = "A-" + to_string(p) + "m；";
  ps += ps1;
  tag += 9;
  p = binstr2decnum(mes.substr(tag - 1, 9), "left-msb");
  // ps1.Format("B-%dm；",p);
  ps1 = "B-" + to_string(p) + "m；";
  ps += ps1;
  tag += 9;
  p = binstr2decnum(mes.substr(tag - 1, 6), "left-msb");
  if (63 == p)
    // ps1.Format("C-%dm或以上；",p);
    ps1 = "C-" + to_string(p) + "m或以上；";
  else
    // ps1.Format("C-%dm；",p);
    ps1 = "C-" + to_string(p) + "m；";
  ps += ps1;
  tag += 6;
  p = binstr2decnum(mes.substr(tag - 1, 6), "left-msb");
  if (63 == p)
    // ps1.Format("D-%dm或以上；",p);
    ps1 = "D-" + to_string(p) + "m或以上；";
  else
    // ps1.Format("D-%dm；",p);
    ps1 = "D-" + to_string(p) + "m；";
  ps += ps1;
  tag += 6;
  oneRec.field = "尺度和位置参照：" + ps;
  allInfo.push_back(oneRec);

  //%%%%%%%%%%%%%已按国际标准Edition 1.5修改%%%%%%%%%%%%%%
  p = binstr2decnum(mes.substr(tag - 1, 4), "left-msb");
  if (0 == p)
    ps = "未定义";
  else if (1 == p)
    ps = "GPS";
  else if (2 == p)
    ps = "GLONASS";
  else if (3 == p)
    ps = "结合GPS/GLONASS";
  else if (4 == p)
    ps = "罗兰C";
  else if (5 == p)
    ps = "Chayka";
  else if (6 == p)
    ps = "结合导航系统";
  else if (7 == p)
    ps = "观测";
  else if (8 == p)
    ps = "伽利略";
  else
    ps = "[错误]";
  oneRec.field = "电子定位装置的类型：" + ps;
  allInfo.push_back(oneRec);
  tag += 4;

  //%%%%%%%%%%%%%已按国际标准Edition 1.5修改%%%%%%%%%%%%%%

  if (mes.substr(tag - 1, 1) == "1")
    ps = "不可用";
  else
    ps = "可用";
  oneRec.field = "数据终端设备：" + ps;
  allInfo.push_back(oneRec);
  tag++;

  if (mes.substr(tag - 1, 1) == "1")
    ps = "不可用";
  else
    ps = "可用";
  oneRec.field = "数据终端设备：" + ps;
  allInfo.push_back(oneRec);
  tag++;

  p = binstr2decnum(mes.substr(tag - 1, 5), "left-msb");
  if (0 == p)
    // ps.Format("%d(5 bit)",p);
    ps = to_string(p) + "(5 bit)";
  else
    // ps.Format("%d[错误，应为0(5 bit)]",p);
    ps = to_string(p) + "[错误，应为0(5 bit)]";
  oneRec.field = "备用码：" + ps;
  allInfo.push_back(oneRec);
}
void AIS::re_message20(string data) {
  Record oneRec;
  oneRec.val = "";
  oneRec.field = "**********所恢复的信息如下**********";
  allInfo.push_back(oneRec);
  data = "010100" + data;

  string identify = data.substr(0, 6);
  int data_type = binstr2decnum(identify, "left-msb");
  string display;
  // display.Format("消息类型为：消息%d(数据链路管理消息)",data_type);
  display =
      "消息类型为：消息" + to_string(data_type) + "(数据链路管理消息)";
  oneRec.field = display;
  allInfo.push_back(oneRec);

  // 转发指示符2bit
  string transmit = data.substr(6, 2);
  int data_transmit = binstr2decnum(transmit, "left-msb");
  // display.Format("转发次数为：%d",data_transmit);
  display = "转发次数为：" + to_string(data_transmit);
  if (3 == data_transmit)
    display = "不再转发";
  oneRec.field = display;
  allInfo.push_back(oneRec);

  // 消息源识别码30bit
  string classify = data.substr(8, 30);
  int data_classify = binstr2decnum(classify, "left-msb");
  if (data_classify >= 10000000000) {
    data_classify = floor(data_classify * 1.0 / 10);
    // display.Format("消息源识别码：%d",data_classify);
    display = "消息源识别码：" + to_string(data_classify);
  } else if (data_classify < 1000000000) {
    int num = 0;
    for (int i = 8; i <= 1; i--) {
      if (floor(data_classify * 1.0 / pow(10.0, i)) > 0) {
        num = 9 - i - 1;
        break;
      }
    }
    // display.Format("%d",data_classify);
    display = to_string(data_classify);
    for (int j = 1; j <= num; j++)
      display = "0" + display;
    display = "消息源识别码：" + display;
  } else
    // display.Format("消息源识别码：%d",data_classify);
    display = "消息源识别码：" + to_string(data_classify);
  oneRec.field = display;
  allInfo.push_back(oneRec);
}
void AIS::re_message21(string mes) {
  Record oneRec;
  oneRec.val = "";
  oneRec.field = "**********所恢复的信息如下**********";
  allInfo.push_back(oneRec);
  oneRec.field = "消息识别码：消息21(助航设备报告)";
  allInfo.push_back(oneRec);

  CHAR asc2[64] = {'@', 'A', 'B', 'C', 'D', 'E', 'F',  'G', 'H', 'I', 'J',
                   'K', 'L', 'M', 'N', 'O', 'P', 'Q',  'R', 'S', 'T', 'U',
                   'V', 'W', 'X', 'Y', 'Z', '[', '\\', ']', '^', '_', ' ',
                   '!', '"', '#', '$', '%', '&', '\'', '(', ')', '*', '+',
                   ',', '-', '.', '/', '0', '1', '2',  '3', '4', '5', '6',
                   '7', '8', '9', ':', ';', '<', '=',  '>', '?'};
  int tag = 1;
  int p = binstr2decnum(mes.substr(tag - 1, 2), "left-msb");
  string ps;
  // ps.Format("%d",p);
  ps = to_string(p);
  oneRec.field = "转发次数：" + ps;
  allInfo.push_back(oneRec);
  tag += 2;

  //%%%%%%%%MMSI的判别，需要后续修改完整%%%%%%%%%%%%%%%%%%%
  p = binstr2decnum(mes.substr(tag - 1, 30), "left-msb");
  // ps.Format("%d",p);
  ps = to_string(p);
  if (ps.length() > 9)
    ps += "[错误，超过9位]";
  while (ps.length() < 9)
    ps = "0" + ps;
  oneRec.field = "用户识别码：" + ps;
  allInfo.push_back(oneRec);
  tag += 30;

  p = binstr2decnum(mes.substr(tag - 1, 5), "left-msb");
  // ps.Format("%d",p);
  ps = to_string(p);
  if (0 == p)
    ps = "不可用";
  if (p >= 1 && p <= 15)
    ps += "(固定助航设备)";
  if (p >= 16 && p <= 31)
    ps += "(漂浮助航设备)";
  oneRec.field = "'助航类型：" + ps;
  allInfo.push_back(oneRec);
  tag += 5;

  string ps1;
  p = binstr2decnum(mes.substr(tag - 1, 6), "left-msb");
  // ps1.Format("%c",asc2[p]);
  ps1 = char(asc2[p]);
  ps = ps1;
  tag += 6;
  for (int i = 1; i <= 19; i++) {
    p = binstr2decnum(mes.substr(tag - 1, 6), "left-msb");
    // ps1.Format("%c",asc2[p]);
    ps1 = char(asc2[p]);
    ps += ps1;
    tag += 6;
  }
  if (ps == "@@@@@@@@@@@@@@@@@@@@")
    ps = "不可用";
  oneRec.field = "助航名称：" + ps;
  allInfo.push_back(oneRec);

  if (mes.substr(tag - 1, 1) == "1")
    ps = "高(<10m，DGNSS接收机的差分模式)";
  else
    ps = "低(>10m，GNSS接收机或其他电子定位系统的自主模式)";
  oneRec.field = "船位精确度：" + ps;
  allInfo.push_back(oneRec);
  tag++;

  double p1;
  string ps2;
  if (mes.substr(tag - 1, 1) == "0") {
    ps = "东经";
    p1 = binstr2decnum(mes.substr(tag, 27), "left-msb") / 10000 / 60;
  } else {
    ps = "西经";
    p1 = (binstr2decnum(bitNot(mes.substr(tag, 27)), "left-msb") + 1) /
         10000 / 60;
  }
  // ps2.Format("%f度",p1);
  ps2 = to_string(float(p1)) + "度";
  if (mes.substr(tag - 1, 1) == "0" && p1 == 181)
    ps2 = "不可用";
  if ((mes.substr(tag - 1, 1) == "0" && p1 > 181) ||
      (mes.substr(tag - 1, 1) == "1" && p1 > 180))
    ps += "[错误，度数超过180度]";
  oneRec.field = "经度：" + ps + ps2;
  allInfo.push_back(oneRec);
  tag += 28;

  //
  if (mes.substr(tag - 1, 1) == "0") {
    ps = "北纬";
    p1 = binstr2decnum(mes.substr(tag, 26), "left-msb") / 10000 / 60;
  } else {
    ps = "南纬";
    p1 = (binstr2decnum(bitNot(mes.substr(tag, 26)), "left-msb") + 1) /
         10000 / 60;
  }
  // ps2.Format("%f度",p1);
  ps2 = to_string(float(p1)) + "度";
  if (mes.substr(tag - 1, 1) == "0" && p1 == 91)
    ps2 = "不可用";
  if ((mes.substr(tag - 1, 1) == "0" && p1 > 91) ||
      (mes.substr(tag - 1, 1) == "1" && p1 > 90))
    ps2 += "[错误，度数超过90度]";
  oneRec.field = "纬度：" + ps + ps2;
  allInfo.push_back(oneRec);
  tag += 27;

  //
  ps = "";
  p = binstr2decnum(mes.substr(tag - 1, 9), "left-msb");
  // ps1.Format("A-%dm；",p);
  ps1 = "A-" + to_string(p) + "m；";
  ps += ps1;
  tag += 9;
  p = binstr2decnum(mes.substr(tag - 1, 9), "left-msb");
  // ps1.Format("B-%dm；",p);
  ps1 = "B-" + to_string(p) + "m；";
  ps += ps1;
  tag += 9;
  p = binstr2decnum(mes.substr(tag - 1, 6), "left-msb");
  if (63 == p)
    // ps1.Format("C-%dm或以上；",p);
    ps1 = "C-" + to_string(p) + "m或以上；";
  else
    // ps1.Format("C-%dm；",p);
    ps1 = "C-" + to_string(p) + "m；";
  ps += ps1;
  tag += 6;
  p = binstr2decnum(mes.substr(tag - 1, 6), "left-msb");
  if (63 == p)
    // ps1.Format("D-%dm或以上；",p);
    ps1 = "D-" + to_string(p) + "m或以上；";
  else
    // ps1.Format("D-%dm；",p);
    ps1 = "D-" + to_string(p) + "m；";
  ps += ps1;
  tag += 6;
  oneRec.field = "尺度和位置参照：" + ps;
  allInfo.push_back(oneRec);

  //%%%%%%%%%%%%%已按国际标准Edition 1.5修改%%%%%%%%%%%%%%
  p = binstr2decnum(mes.substr(tag - 1, 4), "left-msb");
  if (0 == p)
    ps = "未定义";
  else if (1 == p)
    ps = "GPS";
  else if (2 == p)
    ps = "GLONASS";
  else if (3 == p)
    ps = "结合GPS/GLONASS";
  else if (4 == p)
    ps = "罗兰C";
  else if (5 == p)
    ps = "Chayka";
  else if (6 == p)
    ps = "结合导航系统";
  else if (7 == p)
    ps = "观测";
  else if (8 == p)
    ps = "伽利略";
  else
    ps = "[错误]";
  oneRec.field = "电子定位装置的类型：" + ps;
  allInfo.push_back(oneRec);
  tag += 4;

  p = binstr2decnum(mes.substr(tag - 1, 6), "left-msb");
  if (60 == p)
    ps = "不可用";
  else if (62 == p)
    ps = "电子定位系统以估算模式(船位推算法)运行";
  else if (61 == p)
    ps = "电子定位系统以人工输入方法运行";
  else if (63 == p)
    ps = "定位系统未运行";
  else
    // ps.Format("报告产生时的UTC为%d秒",p);
    ps = "报告产生时的UTC为" + to_string(p) + "秒";
  oneRec.field = "时间标记：" + ps;
  allInfo.push_back(oneRec);
  tag += 6;

  if (mes.substr(tag - 1, 1) == "0")
    ps = "在位置";
  else
    ps = "偏移位置";
  oneRec.field = "偏移位置标志：" + ps;
  allInfo.push_back(oneRec);
  tag++;

  p = binstr2decnum(mes.substr(tag - 1, 9), "left-msb");
  if (0 == p)
    ps = "未作地区性使用";
  else
    ps = "作地区性使用";
  oneRec.field = "地区性应用：" + ps;
  allInfo.push_back(oneRec);
  tag += 9;

  if (mes.substr(tag - 1, 1) == "0")
    ps = "RAIM未使用";
  else
    ps = "RAIM使用";
  oneRec.field = "接收机自主完整性监测(RAIM)标志：" + ps;
  allInfo.push_back(oneRec);
  tag++;

  if (mes.substr(tag - 1, 1) == "0")
    ps = "自主和连续模式";
  else
    ps = "分配模式";
  oneRec.field = "分配模式标志：" + ps;
  allInfo.push_back(oneRec);
}
void AIS::re_message22(string data) {
  Record oneRec;
  oneRec.val = "";
  oneRec.field = "**********所恢复的信息如下**********";
  allInfo.push_back(oneRec);
  data = "010110" + data;

  string identify = data.substr(0, 6);
  int data_type = binstr2decnum(identify, "left-msb");
  string display;
  // display.Format("消息类型为：消息%d(信道管理)",data_type);
  display = "消息类型为：消息" + to_string(data_type) + "(信道管理)";
  oneRec.field = display;
  allInfo.push_back(oneRec);

  // 转发指示符2bit
  string transmit = data.substr(6, 2);
  int data_transmit = binstr2decnum(transmit, "left-msb");
  // display.Format("转发次数为：%d",data_transmit);
  display = "转发次数为：" + to_string(data_transmit);
  if (3 == data_transmit)
    display = "不再转发";
  oneRec.field = display;
  allInfo.push_back(oneRec);

  // 消息源识别码30bit
  string classify = data.substr(8, 30);
  int data_classify = binstr2decnum(classify, "left-msb");
  if (data_classify >= 10000000000) {
    data_classify = floor(data_classify * 1.0 / 10);
    // display.Format("消息源识别码：%d",data_classify);
    display = "消息源识别码：" + to_string(data_classify);
  } else if (data_classify < 1000000000) {
    int num = 0;
    for (int i = 8; i <= 1; i--) {
      if (floor(data_classify * 1.0 / pow(10.0, i)) > 0) {
        num = 9 - i - 1;
        break;
      }
    }
    // display.Format("%d",data_classify);
    display = to_string(data_classify);
    for (int j = 1; j <= num; j++)
      display = "0" + display;
    display = "消息源识别码：" + display;
  } else
    // display.Format("消息源识别码：%d",data_classify);
    display = "消息源识别码：" + to_string(data_classify);
  oneRec.field = display;
  allInfo.push_back(oneRec);

  // 备用码
  oneRec.field = "备用码（2位）：不用应设为0";
  allInfo.push_back(oneRec);

  // 信道A(12bit)
  string channelA = data.substr(40, 12);
  // display.Format("信道A信道号为：%d",binstr2decnum(channelA,"left-msb"));
  display = "信道A信道号为：" +
            to_string(int(binstr2decnum(channelA, "left-msb")));
  oneRec.field = display;
  allInfo.push_back(oneRec);

  // 信道B(12bit)
  channelA = data.substr(52, 12);
  // display.Format("信道B信道号为：%d",binstr2decnum(channelA,"left-msb"));
  display = "信道B信道号为：" +
            to_string(int(binstr2decnum(channelA, "left-msb")));
  oneRec.field = display;
  allInfo.push_back(oneRec);

  // 收发模式4bit
  string mode = data.substr(64, 4);
  int data_mode = binstr2decnum(mode, "left-msb");
  if (0 == data_mode)
    display = "收发模式为：TxA/TxB,RxA/RxB(预设)";
  else if (1 == data_mode)
    display = "收发模式为：TxA,RxA/RxB";
  else if (2 == data_mode)
    display = "收发模式为：TxB,RxA/RxB";
  else
    display = "收发模式为：不可用";
  oneRec.field = display;
  allInfo.push_back(oneRec);

  // 功率1bit
  if (data.substr(68, 1) == "0")
    display = "功率为：高（预设）";
  else
    display = "功率为：低";
  oneRec.field = display;
  allInfo.push_back(oneRec);

  // 经度1  18bit
  string longitude = data.substr(69, 18);
  if (longitude.substr(0, 1) == "0") {
    int data_longitude = binstr2decnum(longitude.substr(1, 17), "left-msb");
    // display.Format("经度1为：东经%f度",data_longitude*1.0/600);
    display = "经度1为：东经" +
              to_string(float(data_longitude * 1.0 / 600)) + "度";
  } else {
    int data_longitude =
        binstr2decnum(bitNot(longitude.substr(1, 17)), "left-msb") + 1;
    // display.Format("经度1为：西经%f度",data_longitude*1.0/600);
    display = "经度1为：西经" +
              to_string(float(data_longitude * 1.0 / 600)) + "度";
  }
  oneRec.field = display;
  allInfo.push_back(oneRec);

  // 纬度1 17bit
  string latitude = data.substr(87, 17);
  if (latitude.substr(0, 1) == "0") {
    int data_latitude = binstr2decnum(latitude.substr(1, 16), "left-msb");
    // display.Format("纬度1为：北纬%f度",data_latitude*1.0/600);
    display = "纬度1为：北纬" +
              to_string(float(data_latitude * 1.0 / 600)) + "度";
  } else {
    int data_latitude =
        binstr2decnum(bitNot(latitude.substr(1, 16)), "left-msb") + 1;
    // display.Format("纬度1为：南纬%f度",data_latitude*1.0/600);
    display = "纬度1为：南纬" +
              to_string(float(data_latitude * 1.0 / 600)) + "度";
  }
  oneRec.field = display;
  allInfo.push_back(oneRec);

  // 经度2  18bit
  longitude = data.substr(104, 18);
  if (longitude.substr(0, 1) == "0") {
    int data_longitude = binstr2decnum(longitude.substr(1, 17), "left-msb");
    // display.Format("经度2为：东经%f度",data_longitude*1.0/600);
    display = "经度2为：东经" +
              to_string(float(data_longitude * 1.0 / 600)) + "度";
  } else {
    int data_longitude =
        binstr2decnum(bitNot(longitude.substr(1, 17)), "left-msb") + 1;
    // display.Format("经度2为：西经%f度",data_longitude*1.0/600);
    display = "经度2为：西经" +
              to_string(float(data_longitude * 1.0 / 600)) + "度";
  }
  oneRec.field = display;
  allInfo.push_back(oneRec);

  // 纬度2 17bit
  latitude = data.substr(122, 17);
  if (latitude.substr(0, 1) == "0") {
    int data_latitude = binstr2decnum(latitude.substr(1, 16), "left-msb");
    // display.Format("纬度2为：北纬%f度",data_latitude*1.0/600);
    display = "纬度2为：北纬" +
              to_string(float(data_latitude * 1.0 / 600)) + "度";
  } else {
    int data_latitude =
        binstr2decnum(bitNot(latitude.substr(1, 16)), "left-msb") + 1;
    // display.Format("纬度2为：南纬%f度",data_latitude*1.0/600);
    display = "纬度2为：南纬" +
              to_string(float(data_latitude * 1.0 / 600)) + "度";
  }
  oneRec.field = display;
  allInfo.push_back(oneRec);

  //%信息标志
  if (data.substr(139, 1) == "0")
    display = "信息类别：广播地理区域信息";
  else
    display = "信息类别：编址信息(独立台站)";
  oneRec.field = display;
  allInfo.push_back(oneRec);

  // 信息A带宽
  if (data.substr(140, 1) == "0")
    display = "信息A带宽：默认(由信道号指定)";
  else
    display = "信息A带宽：12.5kHz带宽";
  oneRec.field = display;
  allInfo.push_back(oneRec);

  // 信息B带宽
  if (data.substr(141, 1) == "0")
    display = "信息B带宽：默认(由信道号指定)";
  else
    display = "信息B带宽：12.5kHz带宽";
  oneRec.field = display;
  allInfo.push_back(oneRec);

  // 过渡区大小
  string scale = data.substr(142, 3);
  int data_scale = binstr2decnum(scale, "left-msb");
  // display.Format("过渡区大小为：%dnm",data_scale+1);
  display = "过渡区大小为：" + to_string(data_scale + 1) + "nm";
  oneRec.field = display;
  allInfo.push_back(oneRec);

  oneRec.field = "备用码（23位）：不用应设为0";
  allInfo.push_back(oneRec);
}
string AIS::ascii6(string str) {
    CHAR asc2[64] = {'@', 'A', 'B', 'C', 'D', 'E', 'F',  'G', 'H', 'I', 'J',
                     'K', 'L', 'M', 'N', 'O', 'P', 'Q',  'R', 'S', 'T', 'U',
                     'V', 'W', 'X', 'Y', 'Z', '[', '\\', ']', '^', '_', ' ',
                     '!', '"', '#', '$', '%', '&', '\'', '(', ')', '*', '+',
                     ',', '-', '.', '/', '0', '1', '2',  '3', '4', '5', '6',
                     '7', '8', '9', ':', ';', '<', '=',  '>', '?'};
    int len = str.length();
    int num = ceil(len * 1.0 / 6) * 6 - len;
    string tmp('0', num);
    string x_tmp = tmp + str;
    int loop = x_tmp.length() / 6;
    string ret('0', loop);
    for (int i = 1; i <= loop; i++) {
        int index = binstr2decnum(x_tmp.substr((i - 1) * 6, 6), "left-msb");
        // ret.SetAt(i-1,asc2[index]);
        ret[i - 1] = asc2[index];
    }
    return ret;
}
