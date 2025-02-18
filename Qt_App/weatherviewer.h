#ifndef WEATHERVIEWER_H
#define WEATHERVIEWER_H

#include <QMainWindow>
#include <QDebug>
#include <QTimer>
#include <QSerialPort>
#include <QSerialPortInfo>

QT_BEGIN_NAMESPACE
namespace Ui {
class WeatherViewer;
}
QT_END_NAMESPACE

class WeatherViewer : public QMainWindow
{
    Q_OBJECT

public:
    WeatherViewer(QWidget *parent = nullptr);
    ~WeatherViewer();

private slots:
    void on_btn_ConnectDisconnect_clicked();

public slots:
    void read_data(void);

private:
    Ui::WeatherViewer *ui;
    bool connect_status = false;
    QSerialPort m_serial;

    void refreshGraph(void);

};
#endif // WEATHERVIEWER_H
