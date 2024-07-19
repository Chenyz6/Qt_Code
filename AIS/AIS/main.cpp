#include "ACARS.h"
#include "AIS.h"
#include "type.h"
#include <cstdint>
#include <cstdio>

using namespace std;

int main() {
#if 1
  ACARS acars_det;
  string file = "/mnt/g/1_data_sample/usrp/usrp-b210-sample/acars/"
                "ACARS_2_Freq131.456MHzFs100kHz.dat";
  uint32_t len = 1507200 * 20;
  uint32_t type = 2;
  acars_det.data_input(file, type, len);
  acars_det.demodulation(100e3);
  acars_det.decode();

#else
  // Decode_AIS stDecode_AIS;
  // string DecodeAIS;
  // char *file =
  // (char*)"/mnt/e/1_data_sample/usrp/usrp-b210-sample/ais/WV_AIS_102.4kHz_1638400.dat";
  //    DWORD Datalen = 1638400;
  // short*AISdata = GetFileshort(file, Datalen);
  // int m_dSamplingFreq = 102.4e3;
  // while (1) {
  // 	DecodeAIS = stDecode_AIS.Decode(AISdata, Datalen, m_dSamplingFreq);
  // 	cout << DecodeAIS << endl;
  // }

  AIS ais_det;
  string file = "/mnt/g/1_data_sample/usrp/usrp-b210-sample/ais/"
                "WV_AIS_102.4kHz_1638400.dat";
  string result;

  ais_det.data_input(file, 2, 1638400);
  ais_det.demodulation(102.4e3);
  result = ais_det.decode();

  cout << result << endl;

#endif
  // system("pause");
  return 0;
}
