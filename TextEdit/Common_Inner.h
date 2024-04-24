#ifndef COMMON_INNER_H
#define COMMON_INNER_H
#include <QString>

// 信道编码参数
struct ChannelCodingPara
{
    // 帧长检测范围
    QString range; // 帧结构范围

    // BCH编码
    short bch_n;  // 码长
    short bch_k;  // 信息码长
    QString bch_pins; // 生成多项式

    // RS编码
    short rs_m;  //2的幂次
    short rs_n; // 码长
    short rs_k; // 信息码长
    QString rs_g; // 生成多项式

    // CONV  卷积码
    short conv_m; // 约束长度
    short conv_n; // 码长
    short conv_k; // 信息码长
    QString conv_pins; // 生成多项式

    // DESSR  扰码
    QString dessr_g; // 生成多项式

    // TPC
    short tpc_n; // 码长
    short tpc_k; // 信息码长
    QString tpc_pins; // 生成多项式

    // LDPC
    short ldpc_n; // 码长
    short ldpc_k; // 信息码长

    ChannelCodingPara()
    {
        range = "";
        bch_n = 0;
        bch_k = 0;
        bch_pins = "";
        rs_m = 0;
        rs_n = 0;
        rs_k = 0;
        rs_g = "";
        conv_m = 0;
        conv_n = 0;
        conv_k = 0;
        conv_pins = "";
        dessr_g = "";
        tpc_n = 0;
        tpc_k = 0;
        tpc_pins = "";
        ldpc_n = 0;
        ldpc_k = 0;
    }
};

// 打开文件参数
struct OpenFilePara{
    QString fileAbsolutePath;   // 文件绝对路径
    QString fileName;           // 文件名称
    QString fileSuffix;         // 文件后缀
    qint64 fileSize;            // 文件大小（字节）
    OpenFilePara(){
        fileAbsolutePath = "";
        fileName = "";
        fileSuffix = "";
        fileSize = 0;
    }
};

#endif // COMMON_INNER_H
