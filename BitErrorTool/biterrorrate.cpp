#include "biterrorrate.h"
#include "ui_biterrorrate.h"

BitErrorRate::BitErrorRate(QWidget *parent)
    : QWidget(parent)
    , ui(new Ui::BitErrorRate)
{
    ui->setupUi(this);
    setWindowTitle("误码率测试工具");
    resize(700,550);
    ui->progressBar->setValue(0);
    InitPieSeries();    // 初始化饼图
    ReadIniFile();      // 读取配置文件
    // 连接线程误码率参数
    connect(this, &BitErrorRate::EmitBitErrorPara, bitErrorRateThread, &BitErrorRateThread::RecBitErrorPara);
    // 连接误码率结果
    connect(bitErrorRateThread, &BitErrorRateThread::EmitBitNumResult,this,
            [=](char* result, int length){
                ber_test_result berResult;
                memcpy(&berResult, result, length);
                ui->lineEdit_biterrornum->setText(QString::number(berResult.bit_error_num));
                ui->lineEdit_bitnum->setText(QString::number(berResult.bit_num));
                ui->lineEdit_ber->setText(QString::number(berResult.ber));
                ui->lineEdit_bitlength->setText(QString::number(berResult.bit_length));
                ShowPieSeriesResult(berResult.bit_error_num, berResult.bit_num - berResult.bit_error_num);
                delete[] result;
                result = nullptr;
            });
    // 连接进度条比例
    connect(bitErrorRateThread, &BitErrorRateThread::ProgressValue, this,
            [=](double value){
                ui->progressBar->setValue(value);
            });
    // 线程运行结束
    connect(bitErrorRateThread, &QThread::finished, this,
            [=](){
                ui->progressBar->setValue(100);
                ui->pushButton_start->setEnabled(true);
            });
}

BitErrorRate::~BitErrorRate()
{
    WriteIniFile(); // 写入配置文件
    delete ui;
}

// 初始化饼图
void BitErrorRate::InitPieSeries()
{
    pie_series = new QPieSeries(ui->widget);
    connect(pie_series, SIGNAL(clicked(QPieSlice*)), this, SLOT(onPieSeriesClicked(QPieSlice*)));

    //定义各扇形切片的颜色
    static const QStringList list_pie_color = {
        "#CD0000","#B7D5AC",
    };

    //设置数据
    QList<qreal> list_data = {0, 100};

    //扇形
    for (int i = 0; i < list_pie_color.size(); i++) {
        QPieSlice* pie_slice = new QPieSlice(this);
        pie_slice->setLabelVisible(true); // 图上标签
        pie_slice->setValue(list_data[i]);
        pie_slice->setLabel(QString::number(list_data[i]));
        pie_slice->setColor(list_pie_color[i]);
        pie_slice->setLabelColor(list_pie_color[i]);
        pie_slice->setBorderColor(list_pie_color[i]);
        pie_series->append(pie_slice);
    }

    //图表视图
    QChart* chart = new QChart;
    // chart->setTitle("误码率统计饼图");
    // 设置暗黑主题
    // chart->setTheme(QChart::ChartThemeDark);

    // //标题字体
    // QFont font = qApp->font();
    // font.setBold(true);
    // font.setPointSize(8);
    // chart->setTitleFont(font);

    //加入饼图
    chart->addSeries(pie_series);
    chart->setAnimationOptions(QChart::SeriesAnimations);

    // //图例
    // chart->legend()->setAlignment(Qt::AlignBottom);
    // chart->legend()->setBackgroundVisible(false);

    chart->legend()->hide(); // 隐藏图例

    //加入绘画视图
    QChartView* chartView = new QChartView(this);
    chartView->setRenderHint(QPainter::Antialiasing);
    chartView->setChart(chart);

    //加入布局
    QVBoxLayout* layout = new QVBoxLayout;
    layout->setContentsMargins(0, 0, 0, 0);
    layout->addWidget(chartView);
    ui->widget->setLayout(layout);
}

// 显示结果在饼图上
void BitErrorRate::ShowPieSeriesResult(unsigned int bit_error, unsigned int bit_right)
{
    QList<qreal> list_data = {bit_error, bit_right};
    // 更新每个切片的值和标签
    for (int i = 0; i < list_data.size(); i++) {
        QPieSlice* slice = pie_series->slices().at(i);
        slice->setValue(list_data[i]);
        slice->setLabel(QString::number(list_data[i]));
    }
    // Qt 会自动更新图表，因此不需要显式调用 update()
}

// 读取配置文件
void BitErrorRate::ReadIniFile()
{
    QString IniFilePath("./Setup.ini");
    QFileInfo FileInfo(IniFilePath);
    QSettings Setting(IniFilePath, QSettings::IniFormat); // QSettings::IniFormat 可读写ini文件
    if(FileInfo.isFile()){
        fileNamePath = Setting.value("fileName").toString();
        bitFileNamePath = Setting.value("bitFileName").toString();
    }else{
        fileNamePath = "./";
        bitFileNamePath = "./";
    }
}

// 写入配置文件
void BitErrorRate::WriteIniFile()
{
    QString IniFilePath("./Setup.ini");
    QSettings Setting(IniFilePath, QSettings::IniFormat);
    //文件路径
    Setting.setValue("fileName", fileNamePath);
    Setting.setValue("bitFileName", fileNamePath);
}

// 点击饼图
void BitErrorRate::onPieSeriesClicked(QPieSlice* slice)
{
    slice->setExploded(!slice->isExploded());
}

// 测试文件路径
void BitErrorRate::on_pushButton_filefile_clicked()
{
    fileName = QFileDialog::getOpenFileName(this,"打开一个数据文件",fileNamePath,"TXT(*.txt)");
    QFileInfo fileInfo(fileName);
    fileNamePath = fileInfo.absolutePath();

    ui->lineEdit_filepath->setText(fileName);
}

// 比特序列文件路径
void BitErrorRate::on_pushButton_bitfilepath_clicked()
{
    bitFileName = QFileDialog::getOpenFileName(this,"打开一个数据文件",bitFileNamePath,"TXT(*.txt)");
    QFileInfo fileInfo(bitFileName);
    bitFileNamePath = fileInfo.absolutePath();

    ui->lineEdit_bitpath->setText(bitFileName);
}

// 开始测试
void BitErrorRate::on_pushButton_start_clicked()
{
    fileName = ui->lineEdit_filepath->text();
    bitFileName = ui->lineEdit_bitpath->text();
    if(fileName == ""){
        QMessageBox::warning(0, "Warning", "请输入或浏览正确的测试文件路径!");
        return;
    }
    if(bitFileName == ""){
        QMessageBox::warning(0, "Warning", "请输入或浏览正确的比特序列文件路径!");
        return;
    }
    unsigned int startOffset = ui->lineEdit_start->text().toUInt();
    emit EmitBitErrorPara(fileName, bitFileName, startOffset);
    bitErrorRateThread->start();
    ui->pushButton_start->setEnabled(false);
}



// 误码率测试线程
BitErrorRateThread::BitErrorRateThread()
{

}

BitErrorRateThread::~BitErrorRateThread()
{

}

// 接收误码率参数
void BitErrorRateThread::RecBitErrorPara(QString fileName, QString bitFileName,
                                         unsigned int startOffset)
{
    this->fileName = fileName.toStdString();
    this->bitFileName = bitFileName.toStdString();
    this->startOffset = startOffset;
}

void BitErrorRateThread::run()
{
    ber_test_result *test = new ber_test_result;

    /* 读取数据 */
    // int check_len = 100e3;
    FILE *fp_demod =
        fopen(fileName.c_str(), "rb");

    fseek(fp_demod, 0, SEEK_END);
    int check_len = ftell(fp_demod);
    fseek(fp_demod, 0, SEEK_SET);

    char *demod_bit = (char *)malloc(sizeof(char) * check_len);
    fread(demod_bit, sizeof(char), check_len, fp_demod);

    FILE *fp_trans = fopen(bitFileName.c_str(), "rb");

    fseek(fp_trans, 0, SEEK_END);
    bit_len = ftell(fp_trans);
    fseek(fp_trans, 0, SEEK_SET);

    char *trans_bit = (char *)malloc(sizeof(char) * bit_len);
    fread(trans_bit, sizeof(char), bit_len, fp_trans);

    fclose(fp_demod);
    fclose(fp_trans);

    string ttt(trans_bit, bit_len);  // 注意换行符的问题
    string ddd(demod_bit, check_len);

    bit_error_rate(test, (unsigned long)startOffset, ttt, ddd);

    // cout << "BER: " << test->bit_error_num << "/" << test->bit_num << " = " << test->ber
    //      << endl;
    emit EmitBitNumResult((char*)test, sizeof(ber_test_result)); // 发送误码率结果
}

/*
 * @ description:   该函数假设发射源Transmitter一直循环发送transmit_seq
 * @ result:        结构体指针，最后的计算结果会保存到该结构体中
 * @ start_postion: 起始位置
 * @ transmit_seq:  发射序列
 * @ demod_seq:     解调所得序列
 **/
bool BitErrorRateThread::bit_error_rate(ber_test_result *result, unsigned long start_postion,
                    string transmit_seq, string demod_seq) {
    if (!transmit_seq.size() || !demod_seq.size() ||
        (demod_seq.size() < transmit_seq.size())) {
        /* 对发射序列长度、解调序列产长度做检查。若不满足要求，可以打印相关信息 */

        /* coding here... */
        QMessageBox::warning(0, "Warning", "序列长度有误,请重新选择文件!");

        return false;
    }

    result->bit_num = demod_seq.size();
    result->bit_error_num = 0;

    /* 根据start_postion删去字节 */
    demod_seq.erase(0, start_postion);

    /* 在demod_seq中找到transmit_seq出现的位置，匹配区间长度默认取40bits。如果transmit_seq，长度小于512，则取30bits
   */
    int match_range;

    if (transmit_seq.size() > bit_len) {
        match_range = 40;
    } else {
        match_range = 30;
    }

    /* 开始匹配，如果在 4 * transmit_seq 个循环内还未匹配成功，则返回错误 */
    string::size_type n = 0;
    n = demod_seq.substr(0, 4 * transmit_seq.size())
            .find(transmit_seq.substr(0, match_range));
    if (n == string::npos) {
        /* 在demod_seq中无法匹配到transmit_seq */

        /* coding here */
        QMessageBox::warning(0, "Warning", "无法匹配,请重新选择文件!");

        return false;
    }
    if (n > 0) {
        demod_seq.erase(0, n);
    }

    /* 统计错误个数 */
    char *transmit_seq_char = transmit_seq.data();
    char *demod_seq_char = demod_seq.data();

    for (int i = 0, j = 0; i < demod_seq.size(); i++) {
        /* 若相同，按位异或后结果为0 */
        result->bit_error_num += transmit_seq_char[j] ^ demod_seq_char[i];
        j = (j == transmit_seq.size() - 1) ? 0 : (j + 1);

        double percent = i * (100.0 / (demod_seq.size() - 1));
        emit ProgressValue(percent);
    }

    /* 计算误码率 */
    result->ber = (double)result->bit_error_num / (double)result->bit_num;
    result->bit_length = bit_len;

    return true;
}
