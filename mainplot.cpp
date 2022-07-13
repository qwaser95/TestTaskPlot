#include "mainplot.h"
#include "ui_mainplot.h"


MainPlot::MainPlot(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainPlot)
{
    ui->setupUi(this);

    ui->pushButton_pause->setEnabled(false);
    ui->pushButton_stop->setEnabled(false);

    settingPlot();
}

// деструктор, остановка и удаление потока (если он запущен)
MainPlot::~MainPlot()
{
    if (generateThread != nullptr && generateThread->isRunning()) {
        generateThread->quit();
        generateThread->wait();
        delete generateThread;
    }
    delete ui;
}

// получение массива точек (x, y)
void MainPlot::getArray(const QVector<QPoint> &arrPoints)
{
    QVector<double> x, y;
    for(const QPoint &point : arrPoints) {
        x << double(point.x());
        y << double(point.y());
    }
    ui->widget_plot->graph(0)->setData(x, y);
    ui->widget_plot->replot();
}

// создание и запуск потока
void MainPlot::on_pushButton_start_clicked()
{
    ui->pushButton_start->setEnabled(false);
    ui->pushButton_pause->setEnabled(true);
    ui->pushButton_stop->setEnabled(true);

    if (generateThread == nullptr) {
        GenerateArray *generateArray = new GenerateArray;
        generateThread = new QThread;

        connect(generateThread, &QThread::finished,         generateArray, &QObject::deleteLater);
        connect(generateArray,   &GenerateArray::throwArray, this,          &MainPlot::getArray);
        connect(this,            &MainPlot::unpause,         generateArray, &GenerateArray::unpause);
        connect(this,            &MainPlot::pause,           generateArray, &GenerateArray::pause);

        generateArray->moveToThread(generateThread);
        generateThread->start();
    } else if (isPaused) {
        emit unpause();
    }
}

// приостановка потока
void MainPlot::on_pushButton_pause_clicked()
{
    ui->pushButton_start->setEnabled(true);
    ui->pushButton_pause->setEnabled(false);

    isPaused = true;
    emit pause();
}

// остановка потока, освобождение памяти, очистка графика
void MainPlot::on_pushButton_stop_clicked()
{
    ui->pushButton_start->setEnabled(true);
    ui->pushButton_stop->setEnabled(false);
    ui->pushButton_pause->setEnabled(false);

    generateThread->quit();
    generateThread->wait();

    delete generateThread;
    generateThread = nullptr;

    ui->widget_plot->graph(0)->data()->clear();
    ui->widget_plot->replot();
}

// настройка диаграммы
void MainPlot::settingPlot()
{
    ui->widget_plot->addGraph();
    ui->widget_plot->xAxis->setLabel("X");
    ui->widget_plot->yAxis->setLabel("Y");
    ui->widget_plot->xAxis->setRange(1, 100);
    ui->widget_plot->yAxis->setRange(1, 100);
    ui->widget_plot->graph(0)->setLineStyle(QCPGraph::lsNone);
    ui->widget_plot->graph(0)->setScatterStyle(QCPScatterStyle(QCPScatterStyle::ssCircle, Qt::blue, Qt::cyan, 10));
    ui->widget_plot->setBackground(QBrush(QColor(0, 79, 70)));
    ui->widget_plot->xAxis->setBasePen(QPen(Qt::white, 1));
    ui->widget_plot->yAxis->setBasePen(QPen(Qt::white, 1));
    ui->widget_plot->xAxis->setTickPen(QPen(Qt::white, 1));
    ui->widget_plot->yAxis->setTickPen(QPen(Qt::white, 1));
    ui->widget_plot->xAxis->setSubTickPen(QPen(Qt::white, 1));
    ui->widget_plot->yAxis->setSubTickPen(QPen(Qt::white, 1));
    ui->widget_plot->xAxis->setTickLabelColor(Qt::white);
    ui->widget_plot->yAxis->setTickLabelColor(Qt::white);
    ui->widget_plot->xAxis->grid()->setPen(QPen(QColor(140, 140, 140), 1, Qt::DotLine));
    ui->widget_plot->yAxis->grid()->setPen(QPen(QColor(140, 140, 140), 1, Qt::DotLine));
    ui->widget_plot->xAxis->grid()->setSubGridPen(QPen(QColor(80, 80, 80), 1, Qt::DotLine));
    ui->widget_plot->yAxis->grid()->setSubGridPen(QPen(QColor(80, 80, 80), 1, Qt::DotLine));
    ui->widget_plot->xAxis->grid()->setSubGridVisible(true);
    ui->widget_plot->yAxis->grid()->setSubGridVisible(true);
    ui->widget_plot->xAxis->grid()->setZeroLinePen(Qt::NoPen);
    ui->widget_plot->yAxis->grid()->setZeroLinePen(Qt::NoPen);
    ui->widget_plot->xAxis->setUpperEnding(QCPLineEnding::esSpikeArrow);
    ui->widget_plot->yAxis->setUpperEnding(QCPLineEnding::esSpikeArrow);
    ui->widget_plot->xAxis->setLabelColor(QColor(Qt::white));
    ui->widget_plot->yAxis->setLabelColor(QColor(Qt::white));
}


GenerateArray::GenerateArray(QObject *parent)
{
    Q_UNUSED(parent);

    qRegisterMetaType<QVector<QPoint>>("QVector<QPoint>");
    timerID = startTimer(100);// ms
}

void GenerateArray::timerEvent(QTimerEvent *event)
{
    Q_UNUSED(event);

    for (int i = 0; i < 1; ++i) {
        arrPoints.append(QPoint(QRandomGenerator::global()->bounded(1,100),
                                 QRandomGenerator::global()->bounded(1,100)));
    }

    emit throwArray(arrPoints);
}

// приостановка потока
void GenerateArray::pause()
{
    killTimer(timerID);
    this->thread()->wait();
}

// запуск потока
void GenerateArray::unpause()
{
    this->thread()->start();
    timerID = startTimer(100);// ms
}
