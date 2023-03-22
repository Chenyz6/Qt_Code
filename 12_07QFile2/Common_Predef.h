#ifndef COMMON_PREDEF_H
#define COMMON_PREDEF_H
/***************************************************************************
**			This header	file is integrated	all macro definitions		  **
**          Author:Wang Shuling                                           **
**			Version: 1.0												  **
****************************************************************************/

#define		ONEBW				80						//单带宽
#define		WF_Y_RANGE			100						//瀑布图Y轴范围
#define		WF_X_RANGE			200						//瀑布图X轴范围
#define     WF_MIN_VALUE        -130                    //瀑布图最低显示数值
#define     WF_MAX_VALUE        140                     //瀑布图最高显示数值
#define		DF_WF_Y_RANGE		WF_Y_RANGE + 30			//测向雨图Y轴范围
#define		MAX_IFDATA_LENGTH	4096					//中频频谱最大数据长度
#define		MAX_DEMODDATA_LENGTH	4096 * 2			//解调数据最大长度
#define     MAX_SELECTRECT      2                       //全景界面选中矩形框个数：一个当前选择框，一个绘历史选择框
#define     MAX_RESULT          10                      //最大识别信号个数
#define     MAX_SP_XCOORNUM     8.0                     //频谱图X轴单侧最大轴标签数
#define     MAX_DIS_FRAMENUM_INTERVAL   40              //识别帧数间隔
#define     MAX_COL             11                      //识别结果列表列数


/**********信息类别号：高位为1,则是下发指令；高位为2,则是回执指令*******/
//控制指令下发信息类别号（显控->主控）（启动指令）
#define		DO_START_PANO		0x1001					//全景指令（启动全景）
#define		DO_START_ANALYSE	0x1002					//识别指令（启动识别）
#define		DO_START_DEMOD		0x1003					//解调指令（启动解调）
#define		DO_START_DIRFIND	0x1004					//测向指令（启动测向）

//控制指令下发信息类别号（显控->主控）（停止指令）
#define		DO_END_PANO			0x3001					//全景指令（停止全景）
#define		DO_END_ANALYSE		0x3002					//识别指令（停止识别）
#define		DO_END_DEMOD		0x3003					//解调指令（停止解调）
#define		DO_END_DIRFIND		0x3004					//测向指令（停止测向）

//设备数据上报信息类别号（主控->显控）
#define		R_PANO				0x2001					//全景回执
#define		R_ANALUSE			0x2002					//识别回执
#define		R_DEMOD				0x2003					//解调回执
#define		R_DIRFIND			0x2004					//测向回执

//各模块代号
#define     MOUDLE_MAINFRAME    0                       //主模块
#define		MOUDLE_NETCOMM		1						//网络模块
#define		MOUDLE_PANORAMA_SP	2						//全景模块:频谱图
#define     MOUDLE_PANORAMA_WF  3                       //全景模块:瀑布图
#define		MOUDLE_ANALYSE		4						//识别模块
#define		MOUDLE_DEMOD		5						//解调模块
#define		MOUDLE_DIRFIND		6						//测向模块
#define     MOUDLE_FILECTRL     7                       //文件操作模块
#define     MOUDLE_DATAMANAGE   8                       //文件数据管理模块
#define     MOUDLE_MODUFEATUREMAIN  9                   //调制特征分析主界面
#define     MOUDLE_BASEBAND_MF  10                      //基带调制特征
#define     MOUDLE_FREQDOMAIN   11                      //频域特征
#define     MOUDLE_PARAMRESULT  12                      //参数测量结果
#define     MOUDLE_TIMEDOMAIN   13                      //时域特征

//内部信息类别号
#define		INNER_PARAM_PANORAMA	0					//全景参数设置
#define		INNER_PARAM_ANALYSE		1					//识别参数设置
#define		INNER_PARAM_DEMOD		2					//解调参数设置
#define		INNER_PARAM_DIRFIND		3					//测向参数设置
#define     INNER_PARAM_GETFILEDATA 4                   //获取文件数据
#define     INNER_PARAM_UIDSP       5                   //界面信息显示
#define     INNER_PARAM_WATERFULL   6                   //瀑布图参数设置
#define     INNER_PARAM_WFDATASET   7                   //瀑布图数据处理
#define     INNER_PARAM_RANGECHANGE 8                   //瀑布图随全景范围移动
#define     INNER_PARAM_SLIDER      9                   //全景进度条显示
#define     INNER_PARAM_UIENABLE    10                  //设置界面UI使能
#define     INNER_PARAM_STARTDIST   11                  //启动信号识别
#define     INNER_PARAM_STARTDEMO   12                  //启动信号解调
#define     INNER_PARAM_WINDISPLAY  13                  //MDI子界面显示
#define     INNER_PARAM_STOPDIST    14                  //停止识别流程
#define     INNER_PARAM_STOPDEMO    15                  //停止解调流程
#define     INNER_PARAM_COORDINATE  16                  //全景坐标轴绘制
#define     INNER_PARAM_GETINIMSG   17                  //获取配置文件信息
#define     INNER_PARAM_DEMOPARAM   18                  //解调参数更新

//分辨率
#define	RESOLUTION_0			0.025					//分辨率 0
#define RESOLUTION_1			0.0125					//分辨率 1
#define RESOLUTION_2			0.00625					//分辨率 2
#define RESOLUTION_3			0.00315					//分辨率 3

//全景播放速度
#define PANO_PLAYSPEED_1        35                      //0.5倍速度
#define PANO_PLAYSPEED_2        25                      //一倍速
#define PANO_PLAYSPEED_3        15                      //二倍速
#endif // COMMON_PREDEF_H
