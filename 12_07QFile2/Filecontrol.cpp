#include "Filecontrol.h"

QMutex _mutex;
QWaitCondition _cond;
bool FileProIsFinished = false;

Filecontrol::Filecontrol(QObject *parent)
{
    Init();
    GetIniFileMsg();
}

Filecontrol::~Filecontrol()
{
    delete FileCtrl;
}

//初始化文件选择器
void Filecontrol::Init()
{
    QStringList filters;
    filters << "数据文件(*.dat)" << "数据文件(*.bin)" << "数据文件(*.wav)" << "数据文件(*.IQ)"
            << "数据文件(*.AD)" << "数据文件(*.STD)" << "数据文件(*.DIF)";
    _strlist << "dat" << "bin" << "wav" << "IQ" << "AD" << "STD" << "DIF";
    FileCtrl = new QFileDialog;
    FileCtrl->setOption(QFileDialog::DontUseNativeDialog, true);    //不使用本地默认的框架，第二个参数必须设置为true，否则接下来设置的所有页面不会生效
    FileCtrl->setNameFilters(filters);              //设置过滤器
    FileCtrl->setViewMode(QFileDialog::Detail);     //对话框显示信息的详细程度：详细
    FileCtrl->setSizeGripEnabled(false);            //设置右下角不可拖动大小
    FileCtrl->setDirectory(dir);                    //设置默认路径
    FileCtrl->setMinimumSize(QSize(800, 600));

    //初始化数据文件参数
    cBx_FileType->addItems(filters);
    QStringList dataTablist, ChannelTypelist, InTablist;
    dataTablist << "uint8_t" << "uint16_t" << "uint32_t";
    cBx_DataType->addItems(dataTablist);
    ChannelTypelist << "实通道(AD)" << "复通道(IQ)";
    cBx_ChannelType->addItems(ChannelTypelist);
    InTablist << "False" << "True";
    cBx_InSpecFlag->addItems(InTablist);

    QGridLayout *layout = static_cast<QGridLayout*>(FileCtrl->layout());
    QGroupBox *groupBox = new QGroupBox(tr("数据文件参数选择"));
    QVBoxLayout *vbox1 = new QVBoxLayout;
    vbox1->addWidget(lab_FileType);
    vbox1->addWidget(lab_DataType);
    vbox1->addWidget(lab_ChannelType);
    vbox1->addWidget(lab_InTab);
    QVBoxLayout *vbox2 = new QVBoxLayout;
    vbox2->addWidget(cBx_FileType);
    vbox2->addWidget(cBx_DataType);
    vbox2->addWidget(cBx_ChannelType);
    vbox2->addWidget(cBx_InSpecFlag);
    QVBoxLayout *vbox3 = new QVBoxLayout;
    vbox3->addWidget(lab_SampRate);
    vbox3->addWidget(lab_TunFreq);
    vbox3->addWidget(lab_CenFreq);
    vbox3->addWidget(lab_DataOffset);
    QVBoxLayout *vbox4 = new QVBoxLayout;
    vbox4->addWidget(lEd_SampRate);
    vbox4->addWidget(lEd_TunFreq);
    vbox4->addWidget(lEd_CenFreq);
    vbox4->addWidget(lEd_DataOffset);
    QHBoxLayout* hbox = new QHBoxLayout;
    hbox->addLayout(vbox1);
    hbox->addLayout(vbox2);
    hbox->addLayout(vbox3);
    hbox->addLayout(vbox4);
    hbox->setStretch(0,1);
    hbox->setStretch(1,3);
    hbox->setStretch(2,1);
    hbox->setStretch(3,3);
    groupBox->setLayout(hbox);
    layout->addWidget(groupBox, 4, 0, -1, -1);
    G_ThreadPool = QThreadPool::globalInstance();       //直接获取QApplication生成的默认线程池对象进行管理
}

//打开文件选择框
void Filecontrol::onSelected()
{
    //先预设各种参数为上一次选择的参数
    dir = G_stIniFileMsg.DatFilePath;  //   DatFilePath文件复选框路径
    //配置文件信息 结构体IniFileMsg 的 G_stIniFileMsg
    FileCtrl->setDirectory(dir);    // QFileDialog*FileCtrl  QFIleDialog是用于打开和保存文件的标准对话框
    //数据文件参数
    cBx_FileType->setCurrentIndex           (G_stDataParam.cFileType);
    cBx_DataType->setCurrentIndex           (G_stDataParam.cDataType);
    cBx_ChannelType->setCurrentIndex        (G_stDataParam.cChannelType);
    cBx_InSpecFlag->setCurrentIndex         (G_stDataParam.bInSpecFlag);
    lEd_SampRate->setText(QString::number   (G_stDataParam.dSampFreq));
    lEd_TunFreq->setText(QString::number    (G_stDataParam.dTunFreq));
    lEd_CenFreq->setText(QString::number    (G_stDataParam.dCFFreq));
    lEd_DataOffset->setText(QString::number (G_stDataParam.unDataOffset));

    FileCtrl->exec();  // 循环等待
    if(_file_path != "")    //QString _file_path 当前选取文件的路径
        _file_history.append(_file_path);  // QStringList  _file_history;  //文件浏览历史记录
    FileCtrl->setHistory(_file_history);
    QStringList file;
    file = FileCtrl->selectedFiles();
    _file_path = file[0];
    QFileInfo fileInfo(_file_path);  // QFileInfo类为我们提供了系统无关的文件信息，包括文件的名字和在文件系统中位置，文件的访问权限
    Data_File_Attribute stData_File_Attribute; //文件属性参数(主界面显示  struct Data_File_Attribute
    stData_File_Attribute._fileName = fileInfo.filePath();
    stData_File_Attribute._fileType = fileInfo.suffix();
    stData_File_Attribute.lDataSzie = fileInfo.size();
    dir = fileInfo.path();
    if(!GetFileData(stData_File_Attribute))
        return;

    //将参数保存到配置文件和全局参数
    SetFileMsg();
}

//内部信息路由
void Filecontrol::Inner_ModuleDataRouter(char* cCmdContent, int nDataLength, unsigned short usInfoType, unsigned short usDestID)
{
    switch(usInfoType)
    {
    case INNER_PARAM_GETINIMSG:
    {

    }break;
    }
}

//判决文件数据是否可读，并开启读数据任务
bool Filecontrol::GetFileData(Data_File_Attribute& stData_File_Attribute) //文件属性参数(主界面显示  struct Data_File_Attribute
{
    if(stData_File_Attribute._fileName == "" || stData_File_Attribute.lDataSzie == 0)
    {
        QMessageBox::warning(0, "Warning", "选择文件出错,请重新选择正确的文件!");
        return false;
    }
    else
    {
        if(0 != G_stDataSave.nDataLen)
        {
            switch(QMessageBox::question(NULL, tr("Warning"), "当前打开的文档尚未关闭，确认要关闭吗?", QMessageBox::Yes, QMessageBox::No))
            {
            case QMessageBox::Yes:
            {
                G_stDataSave.Reset(); // .Reset()全局数据文件保存   G_stDataSave数据文件保存(uint8_t, uint16_t, uint32_t)
            }break;
            case QMessageBox::No:
            {
                return false;
            }break;
            }
        }
        switch(_strlist.indexOf(stData_File_Attribute._fileType))  //QStringList _strlist  文件类型
        {       // indexOf()：查找一个字符串中，第一次出现指定字符串的位置    QString _fileType   文件类型
        case 0: //dat
        {
            //再创建数据处理线程，在处理完成后唤醒FFT线程
            DealData* _dealData = new DealData();
            connect(this, &Filecontrol::SendFileMsg, _dealData, &DealData::RecvFileMsg);  // 发送Data_File_Attribute文件属性参数 名类型大小
            connect(_dealData, &DealData::Finished, this, &Filecontrol::Finish);
            emit SendFileMsg(stData_File_Attribute);
            G_ThreadPool->start(_dealData);   // 打开线程池
        }break;
        case 1: //bin
        {

        }break;
        case 2: //wav
        {

        }break;
        case 3: //IQ
        {

        }break;
        case 4: //AD
        {

        }break;
        case 5: //STD
        {

        }break;
        case 6: //DIF
        {

        }break;
        default: break;
        }
    }
    return true;
}

//获取配置文件信息
void Filecontrol::GetIniFileMsg()
{
    QString IniFilePath("./Setup.ini");
    QFileInfo FileInfo(IniFilePath);
    dir = ".";
    if(FileInfo.isFile())
    {
        QSettings Setting(IniFilePath, QSettings::IniFormat);
        //上一次访问文件路径
        dir = Setting.value("DatFilePath/Path").toString();
        //数据文件参数
        cBx_FileType->setCurrentIndex           (Setting.value("UIParam/FileType").toInt());
        cBx_DataType->setCurrentIndex           (Setting.value("UIParam/DataType").toInt());
        cBx_ChannelType->setCurrentIndex        (Setting.value("UIParam/ChannelType").toInt());
        cBx_InSpecFlag->setCurrentIndex         (Setting.value("UIParam/InSpecFlag").toBool());
        lEd_SampRate->setText(QString::number   (Setting.value("UIParam/SampFreq").toDouble()));
        lEd_TunFreq->setText(QString::number    (Setting.value("UIParam/TunFreq").toDouble()));
        lEd_CenFreq->setText(QString::number    (Setting.value("UIParam/CFFreq").toDouble()));
        lEd_DataOffset->setText(QString::number (Setting.value("UIParam/DataOffset").toDouble()));
        SetFileMsg();
    }
}

//将参数保存到配置文件
void Filecontrol::SetFileMsg()
{
    //配置文件
    G_stIniFileMsg.DatFilePath =                    dir;
    G_stIniFileMsg.stDataParam.cFileType =          cBx_FileType->currentIndex();
    G_stIniFileMsg.stDataParam.cDataType =          cBx_DataType->currentIndex();
    G_stIniFileMsg.stDataParam.cChannelType =       cBx_ChannelType->currentIndex();
    G_stIniFileMsg.stDataParam.bInSpecFlag =        cBx_InSpecFlag->currentIndex();
    G_stIniFileMsg.stDataParam.dSampFreq =          lEd_SampRate->text().toDouble();
    G_stIniFileMsg.stDataParam.dTunFreq =           lEd_TunFreq->text().toDouble();
    G_stIniFileMsg.stDataParam.dCFFreq =            lEd_CenFreq->text().toDouble();
    G_stIniFileMsg.stDataParam.unDataOffset =       lEd_DataOffset->text().toInt();

    //全局参数
    G_stDataParam.cFileType =                       cBx_FileType->currentIndex();
    G_stDataParam.cDataType =                       cBx_DataType->currentIndex();
    G_stDataParam.cChannelType =                    cBx_ChannelType->currentIndex();
    G_stDataParam.bInSpecFlag =                     cBx_InSpecFlag->currentIndex();
    G_stDataParam.dSampFreq =                       lEd_SampRate->text().toDouble();
    G_stDataParam.dTunFreq =                        lEd_TunFreq->text().toDouble();
    G_stDataParam.dCFFreq =                         lEd_CenFreq->text().toDouble();
    G_stDataParam.unDataOffset =                    lEd_DataOffset->text().toInt();
}
