#include "global.h"

QString                  bitFileNameCoding;                 // 编码比特流保存文件名
QString                  bitFileNameSpread;                 // 扩频比特流保存文件名
int                      ResultDataLen = 0;                 // 帧结构检测长度
char*                    ResultData;                        // 帧结构检测文件路径
double                   ProgressPercent;                   // 进度百分比
struct OpenFilePara      openFilePara;                      // 打开文件参数
int                      IdentifyResult;                    // 编码识别结果
unsigned long long       fileOffset = 0;                    // 文件偏移位置
