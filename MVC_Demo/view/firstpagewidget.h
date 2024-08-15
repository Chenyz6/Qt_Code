#ifndef FIRSTPAGEWIDGET_H
#define FIRSTPAGEWIDGET_H

#include <QWidget>
#include <QVBoxLayout>

class QFirstPageWidget : public QWidget
{
    Q_OBJECT
public:
    explicit QFirstPageWidget(QWidget *parent = nullptr);

signals:

private:
    void setupUI();

    QVBoxLayout *mainLayout;

private slots:
    void showOneFunctionUISlot();
    void showFinishedCountResultUISlot();

};

#endif // FIRSTPAGEWIDGET_H
