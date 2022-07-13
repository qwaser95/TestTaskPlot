#pragma once

#include <QMainWindow>
#include <QThread>
#include <QTimer>
#include "qcustomplot.h"

QT_BEGIN_NAMESPACE
namespace Ui { class MainPlot; }
QT_END_NAMESPACE

// генератор массива точек
class GenerateArray : public QObject
{
    Q_OBJECT

public:
    GenerateArray(QObject *parent = nullptr);

protected:
    void timerEvent(QTimerEvent *event) override;

public slots:
    void pause();
    void unpause();

Q_SIGNALS:
    void throwArray(const QVector<QPoint> &arrPoints);

private:
    int timerID;
    QVector<QPoint> arrPoints;
};


class MainPlot : public QMainWindow
{
    Q_OBJECT

public:
    MainPlot(QWidget *parent = nullptr);
    ~MainPlot() override;

public slots:
    void getArray(const QVector<QPoint> &arrPoints);

private slots:
    void on_pushButton_start_clicked();
    void on_pushButton_pause_clicked();
    void on_pushButton_stop_clicked();

Q_SIGNALS:
    void unpause();
    void pause();

private:
    void settingPlot();

    Ui::MainPlot *ui;
    QThread *generateThread = nullptr;
    bool isPaused = false;
};
