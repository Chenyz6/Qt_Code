#ifndef SECONDPAGEWIDGET_H
#define SECONDPAGEWIDGET_H

#include <QWidget>
#include <QVBoxLayout>

class QSecondPageWidget : public QWidget
{
    Q_OBJECT
public:
    explicit QSecondPageWidget(QWidget *parent = nullptr);

signals:

public slots:

private:
    void setupUI();

    QVBoxLayout *mainLayout;
};

#endif // SECONDPAGEWIDGET_H
