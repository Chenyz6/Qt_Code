#include "Global.h"

Inner_Flow_Running_State G_Inner_Flow_Running_State;		//各模块运行状态
AnalyseSgnType			 G_AnalyseSgnType;					//识别解调信号类型总表
QThreadPool*             G_ThreadPool;                      //全局线程池对象
DataParam                G_stDataParam;                     //全局UI参数保存
DataSave                 G_stDataSave;                      //全局数据文件保存
unsigned int             nSpecDataOffset = 0;               //频谱数据偏移量(用于进度条显示及当前帧数位置的确定)
bool                     bReplayflag = false;               //是否重复播放标志
IniFileMsg               G_stIniFileMsg;                    //配置文件信息保存
//CDataChannelize          stCDataChannelize[MAX_RESULT];     //抽取算法
QMutex                   mtx;                               //全景+识别线程锁
DisSgnType               stDisSgnType;                      //信号识别表
