#include "textedit.h"
#include "ui_textedit.h"

TextEdit::TextEdit(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::TextEdit)
{
    ui->setupUi(this);
    InitSignalSlot();           // 初始化信号和槽
    InitPlainTextEdit();        // 初始化文本编辑器
    InitTextEdit();             // 初始化行号
}

TextEdit::~TextEdit()
{
    delete ui;
}

// 初始化文本编辑器
void TextEdit::InitPlainTextEdit()
{
    ui->plainTextEdit->setReadOnly(true);
    ui->plainTextEdit->setFont(QFont("Consolas", 12)); // 设置等宽字体
    ui->plainTextEdit->setFixedWidth(897);  // 设置文本编辑器的宽度
//    QFont font;
//    font.setPointSize(13);
//    font.setBold(true);
//    ui->plainTextEdit->setFont(font);
    QTextCharFormat fmt;
    fmt.setForeground(QBrush(QColor(147,112,219)));  // 中紫色
    ui->plainTextEdit->mergeCurrentCharFormat(fmt);
//    ui->plainTextEdit->setStyleSheet("QPlainTextEdit {background-color: rgb(30,30,30);}");
}

// 初始化行号
void TextEdit::InitTextEdit()
{
    ui->textEdit->setReadOnly(true);
    ui->textEdit->setFont(QFont("Consolas", 12)); // 设置等宽字体
//    QFont font;
//    font.setPointSize(13);
//    font.setBold(true);
//    ui->plainTextEdit->setFont(font);
    QTextCharFormat fmt;
    fmt.setForeground(QBrush(QColor(120,120,120))); // 浅灰色
    ui->textEdit->mergeCurrentCharFormat(fmt);
    ui->textEdit->setAlignment(Qt::AlignRight);     // 对齐方式
}

// 初始化信号和槽
void TextEdit::InitSignalSlot()
{
//    connect(ui->plainTextEdit, &QPlainTextEdit::mouseMoveEvent, this, &TextEdit::PlainTextEditMouseMove);
    connect(ui->plainTextEdit, SIGNAL(mousePress(QMouseEvent*)), this, SLOT(PlainTextEditMousePress(QMouseEvent*)));
    connect(ui->plainTextEdit, SIGNAL(mouseRelease(QMouseEvent*)), this, SLOT(PlainTextEditMouseRelease(QMouseEvent*)));
    connect(ui->plainTextEdit, SIGNAL(mouseWheel(QWheelEvent*)), this, SLOT(PlainTextEditWheel(QWheelEvent*)));
}

// 获取鼠标点击位置
void TextEdit::PlainTextEditMousePress(QMouseEvent *event)
{
    qDebug() << "dfdsfsdfs";
}

// 获取鼠标移动位置
void TextEdit::PlainTextEditMouseMove(QMouseEvent *event)
{

}

// 获取鼠标松开的位置
void TextEdit::PlainTextEditMouseRelease(QMouseEvent *event)
{

}

// 获取鼠标滚轮事件
void TextEdit::PlainTextEditWheel(QWheelEvent *event)
{

}

// 启动帧结构检测
void TextEdit::OpenFrameStructureDetection(ChannelCodingPara * channelCodingPara)
{
    channelCoding = new ChannelCoding;
    connect(this,&TextEdit::SendCodingIndex,channelCoding,&ChannelCoding::ReceiveIndex);
    connect(channelCoding,&ChannelCoding::finished,this,
            [=]()
            {
                delete channelCoding;
                emit SendThreadFinish(0);
            });
    connect(this,&TextEdit::SendChannelCodingPara,channelCoding,&ChannelCoding::ReceiveChannelCodingPara);
    emit SendCodingIndex(0);
    emit SendChannelCodingPara((char *)channelCodingPara,sizeof(ChannelCodingPara)); // 发送编码参数
    channelCoding->start();
}

// 启动BCH编码
void TextEdit::OpenBCH(ChannelCodingPara * channelCodingPara)
{
    channelCoding = new ChannelCoding;
    connect(this,&TextEdit::SendCodingIndex,channelCoding,&ChannelCoding::ReceiveIndex);
    connect(channelCoding,&ChannelCoding::finished,this,
            [=]()
            {
                delete channelCoding;
                emit SendThreadFinish(4);
            });
    connect(this,&TextEdit::SendChannelCodingPara,channelCoding,&ChannelCoding::ReceiveChannelCodingPara);
    emit SendCodingIndex(4);
    emit SendChannelCodingPara((char *)channelCodingPara,sizeof(ChannelCodingPara)); // 发送编码参数
    channelCoding->start();
}

// 启动RS编码
void TextEdit::OpenRS(ChannelCodingPara * channelCodingPara)
{
    channelCoding = new ChannelCoding;
    connect(this,&TextEdit::SendCodingIndex,channelCoding,&ChannelCoding::ReceiveIndex);
    connect(channelCoding,&ChannelCoding::finished,this,
            [=]()
            {
                delete channelCoding;
                emit SendThreadFinish(2);
            });
    connect(this,&TextEdit::SendChannelCodingPara,channelCoding,&ChannelCoding::ReceiveChannelCodingPara);
    emit SendCodingIndex(2);
    emit SendChannelCodingPara((char *)channelCodingPara,sizeof(ChannelCodingPara)); // 发送编码参数
    channelCoding->start();
}

// 启动CONV编码
void TextEdit::OpenCONV(ChannelCodingPara * channelCodingPara)
{
    channelCoding = new ChannelCoding;
    connect(this,&TextEdit::SendCodingIndex,channelCoding,&ChannelCoding::ReceiveIndex);
    connect(channelCoding,&ChannelCoding::finished,this,
            [=]()
            {
                delete channelCoding;
                ProgressPercent = 100;
                emit SendThreadFinish(3);
            });
    connect(this,&TextEdit::SendChannelCodingPara,channelCoding,&ChannelCoding::ReceiveChannelCodingPara);
    emit SendCodingIndex(3);
    emit SendChannelCodingPara((char *)channelCodingPara,sizeof(ChannelCodingPara)); // 发送编码参数
    channelCoding->start();
    ProgressPercent = 0;
}

// 启动DESSR编码
void TextEdit::OpenDESSR(ChannelCodingPara * channelCodingPara)
{
    channelCoding = new ChannelCoding;
    connect(this,&TextEdit::SendCodingIndex,channelCoding,&ChannelCoding::ReceiveIndex);
    connect(channelCoding,&ChannelCoding::finished,this,
            [=]()
            {
                delete channelCoding;
                emit SendThreadFinish(1);
            });
    connect(this,&TextEdit::SendChannelCodingPara,channelCoding,&ChannelCoding::ReceiveChannelCodingPara);
    emit SendCodingIndex(1);
    emit SendChannelCodingPara((char *)channelCodingPara,sizeof(ChannelCodingPara)); // 发送编码参数
    channelCoding->start();
}

// 启动TPC
void TextEdit::OpenTPC(ChannelCodingPara * channelCodingPara)
{
    channelCoding = new ChannelCoding;
    connect(this,&TextEdit::SendCodingIndex,channelCoding,&ChannelCoding::ReceiveIndex);
    connect(channelCoding,&ChannelCoding::finished,this,
            [=]()
            {
                delete channelCoding;
                emit SendThreadFinish(5);
            });
    connect(this,&TextEdit::SendChannelCodingPara,channelCoding,&ChannelCoding::ReceiveChannelCodingPara);
    emit SendCodingIndex(5);
    emit SendChannelCodingPara((char *)channelCodingPara,sizeof(ChannelCodingPara)); // 发送编码参数
    channelCoding->start();
}

// 启动LDPC
void TextEdit::OpenLdpc(ChannelCodingPara * channelCodingPara, int ldpc_n,int ldpc_k)
{
    channelCoding = new ChannelCoding;
    connect(this,&TextEdit::SendCodingIndex,channelCoding,&ChannelCoding::ReceiveIndex);
    connect(channelCoding,&ChannelCoding::finished,this,
            [=]()
            {
                delete channelCoding;
                ProgressPercent = 100;
                emit SendThreadFinish(6);
            });
    connect(this,&TextEdit::SendChannelCodingPara,channelCoding,&ChannelCoding::ReceiveChannelCodingPara);
    emit SendCodingIndex(6);
    switch(ldpc_n){
        case 256:{
            if(ldpc_k != 128) {
                QMessageBox::warning(0, "Warning", "请输入正确的码长和信息码长!");
                return;
            }
        }break;
        case 504:{
            if(ldpc_k != 252) {
                QMessageBox::warning(0, "Warning", "请输入正确的码长和信息码长!");
                return;
            }
        }break;
        case 512:{
            if(ldpc_k != 256) {
                QMessageBox::warning(0, "Warning", "请输入正确的码长和信息码长!");
                return;
            }
        }break;
        case 1008:{
            if(ldpc_k != 504) {
                QMessageBox::warning(0, "Warning", "请输入正确的码长和信息码长!");
                return;
            }
        }break;
        case 1024:{
            if(ldpc_k != 512) {
                QMessageBox::warning(0, "Warning", "请输入正确的码长和信息码长!");
                return;
            }
        }break;
        case 2500:{
            if(ldpc_k != 1250) {
                QMessageBox::warning(0, "Warning", "请输入正确的码长和信息码长!");
                return;
            }
        }break;
        default:{
            QMessageBox::warning(0, "Warning", "请输入正确的码长和信息码长!");
            return;
        }break;
    }
    emit SendChannelCodingPara((char *)channelCodingPara,sizeof(ChannelCodingPara)); // 发送编码参数
    ProgressPercent = 0;
    channelCoding->start();
}

// 启动编码识别
void TextEdit::OpenCodingIdentify()
{
    channelCoding = new ChannelCoding;
    connect(this,&TextEdit::SendCodingIndex,channelCoding,&ChannelCoding::ReceiveIndex);
    connect(channelCoding,&ChannelCoding::IdentifyResult,this,
            [=](int result)
            {
                //return:识别类型
                //1:BCH
                //2:RS
                //3:系统卷积码
                //4:非系统卷积码
                //5:LDPC
                //6:扰码
                IdentifyResult = result - 1;
            });
    connect(channelCoding,&ChannelCoding::finished,this,
            [=]()
            {
                delete channelCoding;
                emit SendThreadFinish(7);
            });
    emit SendCodingIndex(7);
    channelCoding->start();
}

// 打开文件
void TextEdit::OpenFile()
{
    QString fileName = QFileDialog::getOpenFileName(
                this, tr("请选择一个数据文件"),
                "./", tr("*.dat;;*.txt;;All files (*.*)"));

    if(fileName.isEmpty())
    {
        QMessageBox mesg;
        mesg.warning(this,"Warning","打开文件失败!");
        return;
    }else{
        QFileInfo file(fileName);   // QFile 负责文件的 IO 设备接口，即与文件的物理交互
        openFilePara.fileAbsolutePath = file.absoluteFilePath();
        openFilePara.fileName = file.fileName();
        openFilePara.fileSuffix = file.suffix();    // dat
        openFilePara.fileSize = file.size();        // 字节数
        emit filePath();
        fileReadThread = new FileReadThread;
        connect(fileReadThread, &FileReadThread::SendData, this,
                [=](QString binaryString){
                    // 显示处理后的数据
                    ui->plainTextEdit->appendPlainText(binaryString);
                    // 显示行号
                    ui->textEdit->append(QString::number(lineNumber++));
                });
        fileReadThread->start();
    }
}


// 信道编码线程
ChannelCoding::ChannelCoding(QWidget *parent)
{

}

ChannelCoding::~ChannelCoding()
{

}

void ChannelCoding::run()
{
    switch (index) {
    case 0: // 帧检测结构
    {
        QStringList stringList = channelCodingPara.range.split("-");
        int nb; // 搜索帧长
        int m; // 最长帧
        for(int i = 0; i < stringList.size(); i++)
        {
            if(i == 0) nb = stringList[0].toInt();
            if(i == 1) m = stringList[1].toInt();
        }

        if(m < nb){
            QMessageBox::warning(0, "Warning", "请输入合适的范围,重新进行帧结构检测!");
            return;
        }

        //读取文件数据
        QFile _file(bitFileNameCoding);
        if(!_file.open(QIODevice::ReadOnly))       //只读文件
        {
            QMessageBox::warning(0, "Warning", "打开文件时出错!");
            return;
        }
        QDataStream _data(&_file);
        fileSize = _file.size();
        char * fileData = new char[fileSize]; // 文件读取数据
        _data.readRawData(fileData, fileSize);

        if(ResultData != NULL){
            delete ResultData;
            ResultData = NULL;
        }
        ResultDataLen = 0;
        ResultData = FrameLengthDetect(fileData, fileSize, m, nb, &ResultDataLen);
        if(ResultDataLen == 0){
            QMessageBox::warning(0, "Warning", "请输入合适的范围,重新进行帧结构检测!");
        }

        delete [] fileData;
        _file.close();
    }break;
    case 1: // DESSR编码 扰码
    {
        QDateTime Timedata = QDateTime::currentDateTime();
        QString temp = Timedata.toString("yyyy-MM-dd_hh.mm.ss");
        QString path;
        path = "./ChannelCoding/";
        path += temp;
        path += "DESSR.txt";
        QFile file(path);    //打开文件，不存在则创建
        file.open(QIODevice::WriteOnly|QIODevice::Append);
        QTextStream stream(&file);

        QStringList stringList = channelCodingPara.dessr_g.split(" ");
        int dessr_g[stringList.size()];
        for(int i = 0; i < stringList.size(); i++)
        {
            dessr_g[i] = stringList[i].toInt();
        }

        int gSize = sizeof(channelCodingPara.dessr_g) / sizeof(dessr_g[0]);
        char * result = new char[ResultDataLen - dessr_g[0]];
        demo_dessr(dessr_g, gSize, ResultData, ResultDataLen, result);

        for(int i = 0; i< ResultDataLen - dessr_g[0]; i++)
        {
            stream << QString::number(result[i]);
        }
        delete [] result;
        file.close();
    }break;
    case 2: // RS编码
    {
        QDateTime Timedata = QDateTime::currentDateTime();
        QString temp = Timedata.toString("yyyy-MM-dd_hh.mm.ss");
        QString path;
        path = "./ChannelCoding/";
        path += temp;
        path += "RS.txt";
        QFile file(path);    //打开文件，不存在则创建
        file.open(QIODevice::WriteOnly|QIODevice::Append);
        QTextStream stream(&file);

        int ResultLen = ResultDataLen / channelCodingPara.rs_n;
        ResultLen *= channelCodingPara.rs_k;
        char* result = new char[ResultLen];

        QStringList stringList = channelCodingPara.rs_g.split(" ");
        char rs_g[stringList.size()];
        for(int i = 0; i < stringList.size(); i++)
        {
            rs_g[i] = stringList[i].toInt();
        }

        demo_rs(channelCodingPara.rs_m, rs_g, channelCodingPara.rs_n, channelCodingPara.rs_k, ResultData, ResultDataLen, result);

        for(int i = 0; i< ResultLen; i++)
        {
            stream << QString::number(result[i]);
        }
        delete [] result;
        file.close();
    }break;
    case 3: // CONV编码 卷积码
    {
        QDateTime Timedata = QDateTime::currentDateTime();
        QString temp = Timedata.toString("yyyy-MM-dd_hh.mm.ss");
        QString path;
        path = "./ChannelCoding/";
        path += temp;
        path += "CONV.txt";
        QFile file(path);    //打开文件，不存在则创建
        file.open(QIODevice::WriteOnly|QIODevice::Append);
        QTextStream stream(&file);

        char* result = new char[ResultDataLen];

        QStringList stringList = channelCodingPara.conv_pins.split(" ");
        int conv_pins[stringList.size()];
        for(int i = 0; i < stringList.size(); i++)
        {
            conv_pins[i] = stringList[i].toInt();
        }

        demo_conv(channelCodingPara.conv_n, channelCodingPara.conv_k, channelCodingPara.conv_m, conv_pins, ResultData, ResultDataLen, result);

        for(int i = 0; i< ResultDataLen; i++)
        {
            stream << QString::number(result[i]);
        }
        delete [] result;
        file.close();
    }break;
    case 4: // BCH编码
    {
        QDateTime Timedata = QDateTime::currentDateTime();
        QString temp = Timedata.toString("yyyy-MM-dd_hh.mm.ss");
        QString path;
        path = "./ChannelCoding/";
        path += temp;
        path += "BCH.txt";
        QFile file(path);    //打开文件，不存在则创建
        file.open(QIODevice::WriteOnly|QIODevice::Append);
        QTextStream stream(&file);

        QStringList stringList = channelCodingPara.bch_pins.split(" ");
        size_t bch_pins[stringList.size()];
        for(int i = 0; i < stringList.size(); i++)
        {
            bch_pins[i] = stringList[i].toInt();
        }

        int ResultLen = ResultDataLen / channelCodingPara.bch_n;
        ResultLen *= channelCodingPara.bch_k;
        char* result = new char[ResultLen];
        int bch_Gnum = sizeof(channelCodingPara.bch_pins) / sizeof(channelCodingPara.bch_pins[0]);
        demo_bch(channelCodingPara.bch_n, channelCodingPara.bch_k, bch_Gnum, bch_pins, ResultData, ResultDataLen, result);

        for(int i = 0; i< ResultLen; i++)
        {
            stream << QString::number(result[i]);
        }
        delete [] result;
        file.close();
    }break;
    case 5: // TPC
    {
        QDateTime Timedata = QDateTime::currentDateTime();
        QString temp = Timedata.toString("yyyy-MM-dd_hh.mm.ss");
        QString path;
        path = "./ChannelCoding/";
        path += temp;
        path += "TPC.txt";
        QFile file(path);    //打开文件，不存在则创建
        file.open(QIODevice::WriteOnly|QIODevice::Append);
        QTextStream stream(&file);

        QStringList stringList = channelCodingPara.tpc_pins.split(" ");
        size_t tpc_pins[stringList.size()];
        for(int i = 0; i < stringList.size(); i++)
        {
            tpc_pins[i] = stringList[i].toInt();
        }

        int ResultLen = ResultDataLen / channelCodingPara.tpc_n / channelCodingPara.tpc_n;
        ResultLen *= channelCodingPara.tpc_k*channelCodingPara.tpc_k;
        char* result = new char[ResultLen];
        int tpc_Gnum = sizeof(tpc_pins) / sizeof(tpc_pins[0]);
        demo_tpc(channelCodingPara.tpc_n, channelCodingPara.tpc_k, tpc_Gnum, tpc_pins, ResultData, ResultDataLen, result);

        for(int i = 0; i< ResultLen; i++)
        {
            stream << QString::number(result[i]);
        }
        delete [] result;

        file.close();
    }break;
    case 6: // LDPC
    {
        QDateTime Timedata = QDateTime::currentDateTime();
        QString temp = Timedata.toString("yyyy-MM-dd_hh.mm.ss");
        QString path;
        path = "./ChannelCoding/";
        path += temp;
        path += "LDPC.txt";
        QFile file(path);    //打开文件，不存在则创建
        file.open(QIODevice::WriteOnly|QIODevice::Append);
        QTextStream stream(&file);

        vector<int> result;
        result = demo_ldpc(channelCodingPara.ldpc_n, channelCodingPara.ldpc_k, ResultData, ResultDataLen, result);

        for(int i = 0; i < (int)result.size(); i++)
        {
            stream << QString::number(result[i]);
        }

        file.close();
    }break;
    case 7: // 编码识别
    {
        for (int i = 0; i < ResultDataLen; i++)
        {
            if (ResultData[i] == '1') ResultData[i] = 1;
            else if (ResultData[i] == '0') ResultData[i] = 0;
            else if (ResultData[i] == 1 || ResultData[i] == 0) ResultData[i] = ResultData[i];
            else printf("输入数据文件有误\n");
        }

        int result = BCH_n_detect(ResultData, ResultDataLen);
        if (result == 0) result = LDPCdetect(ResultData, ResultDataLen);
        if (result == 0) result = desser(ResultData, ResultDataLen);

        emit IdentifyResult(result);
        //return:识别类型
        //1:BCH
        //2:RS
        //3:系统卷积码
        //4:非系统卷积码
        //5:LDPC
        //6:扰码
    }break;
    default:
        break;
    }
}

// 接受信道编码第几个模式
void ChannelCoding::ReceiveIndex(short index)
{
    this->index = index;
}

// 接受编码参数
void ChannelCoding::ReceiveChannelCodingPara(char *cCmdContent, int nDataLength)
{
    memcpy(&channelCodingPara,cCmdContent,nDataLength);
    if(cCmdContent == NULL)    delete cCmdContent;
}

// 文件处理线程
FileReadThread::FileReadThread(QWidget *parent)
{

}

FileReadThread::~FileReadThread()
{

}

void FileReadThread::run()
{
    // 如果是dat文件
    if(openFilePara.fileSuffix == "dat"){
        QFile file(openFilePara.fileAbsolutePath);
        if (!file.open(QIODevice::ReadOnly)) return;
        for(int i = 0; i < 34; i++)  // 显示的行数
        {
            // 根据滑动条位置计算读取位置
            file.seek(fileOffset);
            QDataStream in(&file);
            // 读取并显示文件的一部分
            QByteArray buf;
            buf.resize(92);
            qint64 bytesReadSize = in.readRawData(buf.data(), 92);
            fileOffset += 92;
            QString binaryString;
            for (char byte : buf) {
                QString byteBinary = QString::number(static_cast<unsigned char>(byte), 2);
                if(byteBinary == "0"){
                    byteBinary = ".";
                }
                //QString byteBinary = QString::number(static_cast<unsigned char>(byte), 2).rightJustified(8, '0');  // 转化为8二进制数据
                binaryString += byteBinary;
            }
            emit SendData(binaryString);
        }
        file.close();
    }
}


// 编码算法
void ChannelCoding::demo_bch(int n, int k, int Gnum, size_t *pins, char *cpData,int DataLen, char *result)
{
    using std::vector;
    using std::string;
    RC_DICTCODEC::rc_codec_bch codec(n, k, pins, Gnum);
    RC_DICTCODEC::rc_dict<char> dict;

    size_t s = dict.init(&codec);

    char *check = new char[n];
    char *code = new char[n];
    memset(code, 0, n);
    int DataNum = DataLen / n;

    for (int num = 0; num < DataNum; num++)
    {
        for (int SetCount = 0; SetCount < n; SetCount++)
        {
            code[SetCount] = cpData[num*n + SetCount];
        }

        memset(check, 0, n);
        codec.chcode(code, check);

        string errk = codec.hash(check);

        vector<RC_DICTCODEC::rc_dict<char>::err_mask > err = dict.err_find(errk);

        for (size_t i = 0; i<err.size(); ++i)
        {
            if (err[i].index >= 0 && err[i].index<n)
                code[err[i].index] = codec.mod_m_add(code[err[i].index], err[i].err);
        }

        codec.chcode(code, check);

        for (int i = 0; i < k; i++)
        {
            result[num*k + i] = check[i];
        }
    }

    delete[]check;
    delete[]code;

}

void ChannelCoding::demo_rs(int m, char *g, int n, int k, char *cpData, int DataLen, char *result)
{
    using std::vector;
    using std::string;

    RC_DICTCODEC::Galois_domain A(g, m);

    int t = (n - k) / 2;
    std::vector<unsigned short> Gx = A.geneGx(A(77), 2 * t);

    RC_DICTCODEC::rc_codec_rs codec(n, k, Gx.data(), Gx.size(), A);

    unsigned short *code = new unsigned short[n];
    memset(code, 0, n);

    int DataNum = DataLen / m;
    unsigned short *allcode = new unsigned short[DataNum*n];
    memset(allcode, 0, DataNum*n);

    for (int num = 0; num < DataNum; num++)
    {
        for (int setnum = 0; setnum < m; setnum++)
        {
            allcode[num] = allcode[num] << 1;
            if (cpData[num*m + setnum] == '1') cpData[num*m + setnum] = 1;
            if (cpData[num*m + setnum] == '0') cpData[num*m + setnum] = 0;
            if (cpData[num*m + setnum] == 1) allcode[num]++;
        }
    }

    unsigned short *check = new unsigned short[n];
    unsigned short *decoded = new unsigned short[n];
    int DataCount = DataNum / n;
    for (int num = 0; num < DataCount; num++)
    {
        memset(code, 0, n);
        for (int i = 0; i < n; i++)
        {
            code[i] = allcode[num*n+i];
        }

        memset(check, 0, n);
        codec.chcode(code, check);

        memset(decoded, 0, n);
        codec.decode_rs(code, decoded);

        codec.chcode(decoded, check);

        //for (int i = 0; i < k; ++i)
        //	result[i+k*num] = check[i];

        for (int i = 0; i < k; ++i)
        {
            for (int setnum = m-1; setnum >=0; setnum--)
            {
                result[i*m + k*num + setnum] = check[i] % 2;
                check[i] = check[i] / 2;
            }
        }

    }
    delete[]allcode;
    delete[]check;
    delete[]decoded;
}

void ChannelCoding::demo_conv(int n, int k, int m, int *pins, char *cpData, int DataLen, char *result)
{
    int L = pow(2,m)-1;
    void * codec = LSMVIT::new_conv_codec(n, k, m, pins, 0, L+1);

    int *data = new int[1024 + L];
    int *code = new int[1024 + L];
    int *errcode = new int[1024 + L];
    int *recv = new int[1024 + L];

    int base = 1;
    while (n / 2 != 0)
    {
        base++;
        n = n / 2;
    }
    unsigned short *allcode = new unsigned short[DataLen / base];
    memset(allcode, 0, DataLen / base);
    for (int i = 0; i < DataLen/base; i++)
    {
        for (int j = 0; j < base; j++)
        {
            if (cpData[i*base + j] == '0') cpData[i*base + j] = 0;
            if (cpData[i*base + j] == '1') cpData[i*base + j] = 1;
            allcode[i] = allcode[i] << 1;
            allcode[i] = allcode[i] + cpData[i*base + j];
        }
    }

    //100Kb data, 200KbCode
    int CountNum = DataLen / base / (1024);
    for (int i = 0; i<CountNum; ++i)
    {
        ProgressPercent = (100.0 / (CountNum)) * i;
        for (int j = 0; j < (1024); ++j)
        {
            errcode[j] = allcode[i*(1024) + j];
        }
        for (int j = 1024; j < (1024 + L); ++j)
        {
            errcode[j] = 0;
        }

        LSMVIT::reset_status(codec);
        LSMVIT::decode(codec, errcode, 1024 + L, false);
        int e = LSMVIT::curr_best_hamming(codec);
        int poped = LSMVIT::pop_data(codec, recv, 1024);

        for (int j = 0; j < 1024; ++j)
        {
            result[i*1024+j] = recv[j];
        }

    }

    delete[]data;
    delete[]code;
    delete[]errcode;
    delete[]recv;

    LSMVIT::delete_lsm_codec(codec);

}

void ChannelCoding::demo_dessr(int *g, int gSize, char* cpData, int DataLen, char* result)
{
    char *cp_DessrOut = new char[DataLen];
    char *cp_DessrReg = new char[g[0]];
    memset(cp_DessrOut, 0, DataLen);
    memset(cp_DessrReg, 0, g[0]);
    for (int i = 0; i < g[0]; i++)
    {
        cp_DessrReg[i] = 1;
    }

    for (int i = 0; i < DataLen; i++)
    {
        if (cpData[i] == '0') cpData[i] = 0;
        if (cpData[i] == '1') cpData[i] = 1;
        int s1 = 0x00;
        for (int j = 0; j < gSize - 1; j++)
        {
            s1 ^= cp_DessrReg[g[j] - 1];
        }
        cp_DessrOut[i] = s1^cpData[i];

        for (int j = g[0] - 1; j > 0; j--)
        {
            cp_DessrReg[j] = cp_DessrReg[j - 1];
        }
        cp_DessrReg[0] = cpData[i];
    }

    for (int i = 0; i < DataLen - g[0]; i++)
    {
        result[i] = cp_DessrOut[i + g[0]];
    }
}

void ChannelCoding::demo_tpc(int n, int k, int Gnum, size_t *pins, char *cpData,int DataLen, char *result)
{
    using std::vector;
    using std::string;
    rc_codec_bch codec(n, k, pins, Gnum);
    rc_dict<char> dict;

    size_t s = dict.init(&codec);

    int iterations = (n - k) / 2 + 1; //迭代次数

    char *check = new char[n];
    char *code = new char[n];
    memset(code, 0, n);
    int DataNum = DataLen / n / n;

    char *MatrixCode = new char[n*n]; //n*n单位上的码字
    memset(MatrixCode, 0, n*n);

    for (int num = 0; num < DataNum; num++)
    {
        for (int round = 0; round < iterations; round++)
        {
            //1、第num个矩阵
            for (int i = 0; i < n*n; i++)
            {
                MatrixCode[i] = cpData[num*n*n + i];
            }

            //2、排列出行，进行矫正
            for (int ColumnNum = 0; ColumnNum < n; ColumnNum++)
            {
                for (int LineNum = 0; LineNum < n; LineNum++)
                {
                    code[LineNum] = MatrixCode[ColumnNum*n + LineNum];
                }

                //进行纠错
                memset(check, 0, n);
                codec.chcode(code, check);
                string errk = codec.hash(check);
                vector<rc_dict<char>::err_mask > err = dict.err_find(errk);
                //此处输出的code[n]为带有监督位的校正后正确的代码
                for (size_t i = 0; i<err.size(); ++i)
                {
                    if (err[i].index >= 0 && err[i].index<n)
                        code[err[i].index] = codec.mod_m_add(code[err[i].index], err[i].err);
                }

                //将行码储存到单位矩阵中
                for (int LineNum = 0; LineNum < n; LineNum++)
                {
                    MatrixCode[n*ColumnNum + LineNum] = code[LineNum];
                }
            }

            //3、排列出列，进行矫正
            for (int LineNum = 0; LineNum < n; LineNum++)
            {
                for (int ColumnNum = 0; ColumnNum < n; ColumnNum++)
                {
                    code[ColumnNum] = MatrixCode[ColumnNum*n + LineNum];
                }

                //进行纠错
                memset(check, 0, n);
                codec.chcode(code, check);
                string errk = codec.hash(check);
                vector<rc_dict<char>::err_mask > err = dict.err_find(errk);
                //此处输出的code[n]为带有监督位的校正后正确的代码
                for (size_t i = 0; i<err.size(); ++i)
                {
                    if (err[i].index >= 0 && err[i].index<n)
                        code[err[i].index] = codec.mod_m_add(code[err[i].index], err[i].err);
                }

                //将行码储存到单位矩阵中
                for (int ColumnNum = 0; ColumnNum < n; ColumnNum++)
                {
                    MatrixCode[ColumnNum*n + LineNum] = code[ColumnNum];
                }

            }
        }

        //4、将纠错后的结果保存
        for (int i = 0; i < k; i++)
        {
            for (int j = 0; j < k; j++)
            {
                result[i*k + j + num*k*k] = MatrixCode[i*n + j];
            }
        }


        //codec.chcode(code, check);

        //for (int i = 0; i < k; i++)
        //{
        //	result[num*k + i] = check[i];
        //}
    }

    delete[]check;
    delete[]code;

}

char* ChannelCoding::FrameLengthDetect(char* cpData, DWORD DataLen, int m, int nb, int *ResultDataLen)
{
    //测试参数
    double DnTest = 0, maxFramDn = 0;
    int maxFramLen = 0;

    //转化为01
    for (int i = 0; i < DataLen; i++)
    {
        if (cpData[i] == '0') cpData[i] = 0;
        if (cpData[i] == '1') cpData[i] = 1;
    }

    //帧长分析
    for (int FrameLen = nb; FrameLen < m; FrameLen++)
    {
        DnTest = 0;
        //分析列均值
        double *mean = new double[FrameLen];
        //memset(mean, 0, 100);
        int FrameHigh = DataLen / FrameLen;
        for (int FramDist = 0; FramDist < FrameLen; FramDist++)
        {
            mean[FramDist] = 0;
            for (int FrameNum = 0; FrameNum < FrameHigh; FrameNum++)
            {
                mean[FramDist] += int(cpData[FrameLen*FrameNum + FramDist]);
            }
            mean[FramDist] /= FrameHigh;
            if (mean[FramDist] > 0.5) mean[FramDist] = 1 - mean[FramDist];
        }
        //相关值计算
        double q = 10, a = 10, Dn = 0;
        for (int i = 0; i < FrameLen; i++)
        {
            Dn = pow(q, (-a*mean[i]));
            if (Dn > 4 / FrameLen)
                DnTest += Dn;
        }
        if (maxFramDn < DnTest)
        {
            maxFramDn = DnTest;
            maxFramLen = FrameLen;
        }
        delete[]mean;
    }
    //printf("搜索帧长为：%d\n", maxFramLen);
    //qDebug() << "搜索帧长为：" << maxFramLen;

    //大致标记重复码位置
    int *FrameMark = new int[maxFramLen];
    int Hight = DataLen / maxFramLen;
    for (int i = 0; i < maxFramLen; i++)
    {
        FrameMark[i] = 0;
        int sum = 0;
        for (int j = 0; j < Hight; j++)
        {
            sum += cpData[j*maxFramLen + i];
        }
        if (sum > Hight / 2) sum = Hight - sum;
        if (sum < Hight / 10) FrameMark[i] = 1;
    }

    //重新标记重复码位置（去掉一些不必要值）
    int mark = 0;
    for (int i = 2; i < maxFramLen - 2; i++)
    {
        mark = FrameMark[i - 2] + FrameMark[i - 1] + FrameMark[i] + FrameMark[i + 1] + FrameMark[i + 2];
        if (mark > 3 && FrameMark[i] == 0) FrameMark[i] = 1;
        else if (mark <= 2 && FrameMark[i] == 1) FrameMark[i] = 0;
    }

    //提取帧开始位置（也就是解码起点）
    int begin = 0, surebegin = 0, flag = 0, maxflag = 0;
    for (int i = 0; i < maxFramLen; i++)
    {
        if (FrameMark[i] == 1 && flag != 0)
        {
            flag++;
        }
        else if (FrameMark[i] == 1 && flag == 0)
        {
            flag++;
            begin = i;
        }
        else if (FrameMark[i] == 0)
        {
            if (maxflag < flag)
            {
                maxflag = flag;
                surebegin = begin;
            }
            flag = 0;
        }
    }
    //printf("起始点：%d\n", surebegin);

    //提取帧头
    int *FrameHead = new int[maxflag];
    double mean_head = 0;
    int count = 0;
    printf("同步帧头为：");
    for (int FramDist = surebegin; FramDist < surebegin + maxflag; FramDist++)
    {
        mean_head = 0;
        for (int FrameNum = 0; FrameNum < Hight; FrameNum++)
        {
            mean_head += int(cpData[maxFramLen*FrameNum + FramDist]);
        }
        mean_head /= Hight;
        mean_head = mean_head > 0.5 ? 1 : 0;
        FrameHead[count] = mean_head;
        printf("%d", FrameHead[count]);
        count++;
    }
    printf("\n");

    //查找第一个帧头存在的位置
    int BeginPosit = 0;
    int sumflag = 0;
    for (int i = 0; i < DataLen / 2; i++)
    {
        sumflag = 0;
        for (int j = 0; j < maxflag; j++)
        {
            if (cpData[i + j] == FrameHead[j])
                sumflag++;
            else
                break;
        }
        if (sumflag == maxflag)
        {
            BeginPosit = i;
            break;
        }
    }
    printf("帧头位置：%d\n", BeginPosit);
    qDebug() << "帧头位置：" << BeginPosit;
    delete[]FrameHead;

    //帧标志位总个数
    int FrameMark1 = 0;
    for (int i = 0; i < maxFramLen; i++)
    {
        FrameMark1 += FrameMark[i];
    }

    //重新编排FrameMark
    int MarkPosition = 0;
    sumflag = 0;
    //printf("FrameMark = ");
    for (int i = 0; i < maxFramLen; i++)
    {
        if (FrameMark[i] == 1 && sumflag != (maxflag - 1)) sumflag++;
        else if (FrameMark[i] == 1 && sumflag == (maxflag - 1)) MarkPosition = i - maxflag + 1;
        else sumflag = 0;
        //printf("%d", FrameMark[i]);
    }
    //printf("\n");

    int *FrameMarkRemake = new int[maxFramLen];
    count = MarkPosition;
    //printf("FrameMarkRemake = ");
    for (int i = 0; i < maxFramLen; i++)
    {
        FrameMarkRemake[i] = FrameMark[(count%maxFramLen)];
        //printf("%d",count%maxFramLen)
        //printf("%d", FrameMarkRemake[i]);
        count++;
    }
    //printf("\n");

    //提取信息位
    int remain = DataLen - BeginPosit + 1;
    int FrameNum = remain / maxFramLen;
    int sumData = FrameNum * maxFramLen;
    *ResultDataLen = FrameNum*(maxFramLen - FrameMark1);
    int ResultLenCount = 0;
    char* resultData = new char[*ResultDataLen];
    for (int i = 0; i < FrameNum; i++)
    {
        for (int j = 0; j < maxFramLen; j++)
        {
            if (FrameMarkRemake[j] != 1)
            {
                resultData[ResultLenCount] = cpData[BeginPosit + i*maxFramLen + j];
                ResultLenCount++;
            }
        }
    }
    delete[]FrameMark;
    delete[]FrameMarkRemake;

    return resultData;
}

vector<int> ChannelCoding::demo_ldpc(int n, int k, char* cpData, DWORD DataLen, vector<int> result)
{
    ios::sync_with_stdio(false);
    ldpcdecoder decoder;
    decoder.IitialLdpcDecoder(n);
    decoder.GetHMatrix(n, k);

    int *dpData = new int[DataLen];
    for (int i = 0; i < DataLen; i++)
    {
        if (cpData[i] == '1' || cpData[i] == 1) dpData[i] = 1;
        else if (cpData[i] == '0' || cpData[i] == 0) dpData[i] = 0;
        else printf("文件数据有误");
    }

    for (int num = 0; num < DataLen / n; num++)
    {
        ProgressPercent = (100.0 / (DataLen / n)) * num;
        vector<int> code;
        vector<int> code_result;
        for (int i = 0; i < n; i++)
        {
            code.push_back(dpData[i]);
        }
        double wer = 0.0;
        double ber = 0.0;
        double aver_iter = 0.0;
        int sum_iter = 0;
        int success = 0;
        int wrong = 0;
        int errorbits = 0;
        int c = 0;

        //signalgenerator signal;
        //signal.GenerateRandomeSignal(k);
        //vector<int> show_signalresult = signal.GetSignal();//随机产生的原始数据
        //encoder.StartEncode(signal.GetSignal());
        //vector<int> show_encoderesult = encoder.GetEncodeResult();//编码完成之后的数据
        bpskmodulator bpsk(code);/////
        vector<double> show_bpsksignal = bpsk.GetBpskSignal();//bpsk调制之后的数据：1变为-1，0变为1
        awgnchannel guass(0, 0, bpsk.GetBpskSignal());/////
        vector<double> show_SignalWithNoise = guass.GetSignalWithNoise();
        code_result = decoder.StartDecode(guass.GetSignalWithNoise(), guass.GetN0());

        for (int i = 0; i < k; i++)
        {
            result.push_back(code_result[i]);
        }
        code.clear();
        code_result.clear();
        //decoder.Clear();
        guass.Clear();
        bpsk.Clear();
    }

    decoder.Clear();
    int length = result.size();
    //if (c == 0) ++success;
    //else ++wrong;
    //errorbits += c;
    /*wer = static_cast<double>(wrong) / static_cast<double>(times);
    ber = static_cast<double>(errorbits) / static_cast<double>(times*k);
    sum_iter += decoder.GetTheIter();
    aver_iter = static_cast<double>(sum_iter) / static_cast<double>(times);*/
    //cout << "snr=" << snr << "   " << "success=" << success << "   " << "wrong=" << wrong << "   "
    //	<< "wer=" << wer << "   " << "ber=" << ber << "   " << "aver_iter=" << aver_iter << "   " << endl;
    //out << "snr=" << snr << "   " << "success=" << success << "   " << "wrong=" << wrong << "   "
    //	<< "wer=" << wer << "   " << "ber=" << ber << "   " << "aver_iter=" << aver_iter << "   " << endl;
    //signal.Clear();

    //encoder.Clear();

    //if (wrong == 100) break;
    delete []dpData;
    return result;

}

int ChannelCoding::gcd(int a, int b)
{
    int n = max(a, b);
    int m = min(a, b);
    if (n%m == 0)
        return m;
    return gcd(n%m, m);
}

int ChannelCoding::RC_ModeAnalysis(int compare)
{
    if (compare <= 3 && compare >1) return 4;
    else if (compare <= 8 && compare > 3) return 3;
    switch (compare) {
    case 15:
        return 1;
    case 60:
        return 2;
    case 31:
        return 1;
    case 31 * 5:
        return 2;
    case 63:
        return 1;
    case 63*6:
        return 2;
    case 127:
        return 1;
    case 127*7:
        return 2;
    case 255:
        return 1;
    case 255*8:
        return 2;
    default:
        return 0;
    }
}

int ChannelCoding::BCH_n_detect(char* cpData, int DataLen)
{
    int p = 256; //固定将要排列的矩阵行数p，p>二倍的分组长度，通常系统分组码的长度不会大于255，因此可以取p>510
    int q = 3; //将矩阵排列成p行q列的形式，其中3<=q<=p
    int ranksize = 0;
    int flag = 0;
    int compare = 0; //对之前的最大公约数暂存
    int recompare = 0;

    ////cpData = &cpData[4];
    Matrix matrix(30, 15, cpData);
    //PrintMatrix(matrix);
    //ranksize = matrix.Rank();
    //printf("%d\n", ranksize);
    for (q = 3; q < p; q++)
    {
        if (q*p > DataLen)
        {
            //printf("没有BCH");
            return 0;
        }

        Matrix m_temp(q,q);
        for (int i = 0; i < m_temp.GetRowCount(); i++)
        {
            for (int j = 0; j < m_temp.GetColumnCount(); j++)
            {
                m_temp[i][j] = cpData[j + i*m_temp.GetColumnCount()];
            }
        }
        matrix.Reshape(m_temp);
        //if (q == 15) PrintMatrix(matrix);
        ranksize = matrix.Rank();
        //printf("%d的秩为%d\n", q, ranksize);
        //if (ranksize != q)
            //printf("好耶！！%d的秩为%d\n", q, ranksize);
        double qc = q;
        double ranksizeComp = 0;
        if (q <= 15) ranksizeComp = q - 3;
        else ranksizeComp = qc * 13 / 15;
        if (ranksize < ranksizeComp)
        {
            //printf("%d的秩为%d\n", q, ranksize);
            if (flag == 0)
            {
                flag = 1;
                compare = q;
            }
            else
            {
                recompare = gcd(q, compare);
                if (recompare == compare && recompare != 1) break;
                if (recompare == 1) recompare = compare;
                compare = recompare;
            }
        }
    }
    //printf("得到线性分组码码长为：%d\n", compare);
    //对得到的码长进行分析，判断是BCH还是RS
    if (q == 5) compare = 2;
    int result = RC_ModeAnalysis(compare);
    return result;
}

int ChannelCoding::LDPCdetect(char* cpData, int DataLen)
{
    int n_min = 1000;
    int n_max = 5000;
    double Framlen = 0; //行数
    int sum = 0;	 //每列的1的数量
    double resum = 0;   //每列0的数量
    double weight = 0;  //每列的权重
    double max = 0;
    int maxindex = 0;
    int flag = 0;
    for (double n = n_min; n < n_max; n++)
    {
        const int x = n;  // 修改过 zeze
        double* Pi = new double[x];
        double* Cu = new double[x];
        for (int i = 0; i < n; i++) {
            Pi[i] = 0; Cu[i] = 0;
        }
        resum = 0;
        weight = 0;
        Framlen = int (DataLen / n);
        for (int i = 0; i < n; i++)
        {
            sum = 0;
            for (int j = 0; j < Framlen; j++)
            {
                sum += cpData[i + j*int(n)];
            }
            Pi[i] = double(sum)/Framlen;
            Cu[i] = 1 - double(sum) / Framlen;
            //printf("%f\n", Pi[i]);
        }
        double av_p = 0;
        double av_cu = 0;
        for (int i = 0; i < n; i++)
        {
            //weight += pow((Pi[i] - 1 / (n + 1)), 2);
            av_p += Pi[i];
            av_cu += Cu[i];
        }
        av_p /= n;
        av_cu /= n;
        //if (av_p > 0.5) av_p = 1 - av_p;

        if (av_p > av_cu)weight = av_p - av_cu;
        else weight = av_cu - av_p;

        //weight = (n + 1) / n*weight;
        if (max < weight*100)
        {
            max = weight*100;
            maxindex = n;
            //printf("%f:  %f\n", weight*100,n);
            flag++;
        }
        //printf("%f\n", weight*100);
        delete[]Pi;
    }
    if (flag < 10) return 5;
    return 0;
}

int ChannelCoding::demo_dessr(vector<int> g, int gSize, char* cpData, int DataLen, char* result)
{
    char *cp_DessrOut = new char[DataLen];
    char *cp_DessrReg = new char[g[0]];
    memset(cp_DessrOut, 0, DataLen);
    memset(cp_DessrReg, 0, g[0]);
    for (int i = 0; i < g[0]; i++)
    {
        cp_DessrReg[i] = 1;
    }

    for (int i = 0; i < DataLen; i++)
    {
        int s1 = 0x00;
        for (int j = 0; j < gSize - 1; j++)
        {
            s1 ^= cp_DessrReg[g[j] - 1];
        }
        cp_DessrOut[i] = s1^cpData[i];

        for (int j = g[0] - 1; j > 0; j--)
        {
            cp_DessrReg[j] = cp_DessrReg[j - 1];
        }
        cp_DessrReg[0] = cpData[i];
    }

    for (int i = 0; i < DataLen - g[0]; i++)
    {
        result[i] = cp_DessrOut[i + g[0]];
    }

    delete[]cp_DessrOut;
    delete[]cp_DessrReg;

    return 0;
}

double ChannelCoding::average(double *x, int len)
{
    double sum = 0;
    for (int i = 0; i <len; i++)
        sum += x[i];
    return sum / len;
}

double ChannelCoding::variance(double *x, int len)
{
    double average(double *x, int len);
    double avg = ChannelCoding::average(x, len);
    double sum = 0;
    for (int i = 0; i<len; i++)
        sum += pow(x[i] - avg, 2);
    return sum / (len - 1);
}

double ChannelCoding::t_test(double *x, double *y, int len_x, int len_y)
{
    //double average(double *x, int len);
    //double variance(double *x, int len);
    double avg_x, avg_y, var_x, var_y;
    avg_x = average(x, len_x);
    avg_y = average(y, len_y);
    var_x = variance(x, len_x);
    var_y = variance(y, len_y);
    int df = len_x + len_y - 2;
    double s_e, s1_2, t;
    s_e = (var_x*(len_x - 1) + var_y*(len_y - 1)) / (df);
    s1_2 = sqrt(s_e / len_x + s_e / len_y);
    t = (avg_x - avg_y) / s1_2;
    t = abs(t);
    //cout << "均值变化（前-后）：" << abs(avg_x - 0.5) - abs(0.5 - avg_y) << "T-value is " << t << endl;
    //if (t>1.96)
    //{
    //	cout << "t-vale > T_0.05 (1.96)" << endl << "So there is significant difference" << endl;
    //}
    return t;
}

int ChannelCoding::desser(char* cpData, int DataLen)
{
    int t = 0;
    double* dpData = new double[DataLen];
    for (int i = 0; i < DataLen; i++)
    {
        if (cpData[i] == '1' || cpData[i] == 1) dpData[i] = 1;
        else if (cpData[i] == '0' || cpData[i] == 0) dpData[i] = 0;
        else printf("输入数据文件有误\n");
    }

    for (int i = 0; i < PrimePloySize; i++)
    {
        //将多项式从八进制转换为{5，3，0}的形式
        int temp = PrimePloy[i];
        int binLenth = 0;
        int gSize = 0;
        vector<int> dessr_g;
        while (temp != 0)
        {
            int temp2 = temp % 10;
            for (int j = 0; j < 3; j++)
            {
                if (temp2 % 2 == 1)
                {
                    gSize++;
                    for_each(dessr_g.begin(), dessr_g.end(), [](int& k) {k++; });
                    dessr_g.push_back(0);
                }
                else
                {
                    for_each(dessr_g.begin(), dessr_g.end(), [](int& k) {k++; });
                }
                temp2 = temp2 / 2;
            }
            temp = temp / 10;
        }
        int pass = dessr_g[dessr_g.size() - 1];
        for (int j = 0; j < pass; j++)
        {
            for_each(dessr_g.begin(), dessr_g.end(), [](int& k) {k--; });
        }

        //解扰
        char * result = new char[DataLen - dessr_g[0]];
        demo_dessr(dessr_g, gSize, cpData, DataLen, result);

        double * resultData = new double[DataLen - dessr_g[0]];
        for (int i = 0; i < DataLen - dessr_g[0]; i++)
        {
            if (result[i] == '1' || result[i] == 1) resultData[i] = 1;
            else if (result[i] == '0' || result[i] == 0) resultData[i] = 0;
            else printf("解扰结果有误\n");
        }

        //printf("本原多项式为：");
        //for (int j = 0; j < dessr_g.size(); j++)
        //{
        //	printf("%d ", dessr_g[j]);
        //}
        //printf("\n");
        t = t_test(dpData, resultData, DataLen, DataLen - dessr_g[0]);
        if (t > 1.5) break;

        delete[]result;
        delete[]resultData;

        dessr_g.clear();
    }
    delete[]dpData;
    if (t > 1.5) return 6;
    else return 0;
}


