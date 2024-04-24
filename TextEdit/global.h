#ifndef GLOBAL_H
#define GLOBAL_H

#include "Common_Inner.h"

extern QString                  bitFileNameCoding;                 // 编码比特流保存文件名
extern QString                  bitFileNameSpread;                 // 扩频比特流保存文件名
extern int                      ResultDataLen;                     // 帧结构检测长度
extern char*                    ResultData;                        // 帧结构检测文件路径
extern double                   ProgressPercent;                   // 进度百分比
extern struct OpenFilePara      openFilePara;                      // 打开文件参数
extern int                      IdentifyResult;                    // 编码识别结果
extern unsigned long long       fileOffset;                        // 文件偏移位置

#endif // GLOBAL_H
