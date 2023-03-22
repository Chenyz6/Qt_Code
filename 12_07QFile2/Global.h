#ifndef GLOBAL_H
#define GLOBAL_H
#include <QtWidgets>
#include "Common_Inner.h"
#include "Common_Predef.h"
//算法
//#include "SgnlPrcsDll.h"
//#include "SignalParamProbe.h"
//#include "DataChannelize.h"

extern Inner_Flow_Running_State G_Inner_Flow_Running_State;		//各模块运行状态
extern AnalyseSgnType			G_AnalyseSgnType;				//识别解调信号类型总表
extern QThreadPool*             G_ThreadPool;                   //全局线程池对象
extern DataParam                G_stDataParam;                  //全局UI参数保存
extern DataSave                 G_stDataSave;                   //全局数据文件保存
extern unsigned int             nSpecDataOffset;                //频谱数据偏移量(用于进度条显示及当前帧数位置的确定)
extern bool                     bReplayflag;                    //是否重复播放标志
extern IniFileMsg               G_stIniFileMsg;                 //配置文件信息保存
//extern CDataChannelize          stCDataChannelize[MAX_RESULT];  //抽取算法
extern QMutex                   mtx;                            //全景+识别线程锁
extern DisSgnType               stDisSgnType;                   //信号识别表
#endif // GLOBAL_H
