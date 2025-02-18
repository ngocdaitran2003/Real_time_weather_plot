#include "weatherviewer.h"
#include "ui_weatherviewer.h"
#include <QDateTime>

const qint8 SECONDS_SHOW_ON_GRAPH = 120;  // Display 120 seconds on the graph
QVector<double> time_axis;
QVector<double> temperature_axis;
QVector<double> humidity_axis;

static qint64 temp_data_idx = 0;   // to be used later for x-axis range setting
static qint64 startTime;

WeatherViewer::WeatherViewer(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::WeatherViewer)
{
    ui->setupUi(this);

    qDebug() << "Detecting Available Serial Ports";

    QList<QSerialPortInfo> serial_port_infos = QSerialPortInfo::availablePorts();
    for (const QSerialPortInfo &port_info : serial_port_infos )
    {
        qDebug() << "Port:" << port_info.portName();
        // Add these found com ports to the combo box
        ui->cb_COM->addItem(port_info.portName());
    }

    // Plotting Stuff
    // Create Temp graph and assign data to it
    ui->Temp_Plot->addGraph();
    // give axis some labels
    ui->Temp_Plot->xAxis->setLabel("Time");
    ui->Temp_Plot->xAxis->setLabelFont(QFont("Arial", 10, QFont::Bold));

    ui->Temp_Plot->yAxis->setLabel("Temperature");
    ui->Temp_Plot->yAxis->setLabelFont(QFont("Arial", 12, QFont::Bold));

    QColor color_Temp(255, 110, 40); // Red line
    ui->Temp_Plot->graph(0)->setLineStyle(QCPGraph::lsLine);
    ui->Temp_Plot->graph(0)->setPen(QPen(color_Temp));
    ui->Temp_Plot->graph(0)->setBrush(QBrush(color_Temp));

    // Create Humidity graph and assign data to it
    ui->Humidity_Plot->addGraph();
    // give axis some labels
    ui->Humidity_Plot->xAxis->setLabel("Time");
    ui->Humidity_Plot->xAxis->setLabelFont(QFont("Arial", 10, QFont::Bold));

    ui->Humidity_Plot->yAxis->setLabel("Humidity");
    ui->Humidity_Plot->yAxis->setLabelFont(QFont("Arial", 12, QFont::Bold));

    QColor color_Humidity(40, 110, 255); // Blue line
    ui->Humidity_Plot->graph(0)->setLineStyle(QCPGraph::lsLine);
    ui->Humidity_Plot->graph(0)->setPen(QPen(color_Humidity));
    ui->Humidity_Plot->graph(0)->setBrush(QBrush(color_Humidity));

    // configure bottom axis to show date instead of number:
    QSharedPointer<QCPAxisTickerDateTime> date_time_ticker(new QCPAxisTickerDateTime);
    date_time_ticker->setDateTimeFormat("hh:mm:ss");
    ui->Temp_Plot->xAxis->setTicker(date_time_ticker);
    ui->Humidity_Plot->xAxis->setTicker(date_time_ticker);

    // make left and bottom axes transfer their ranges to right and top axes:
    connect(ui->Temp_Plot->xAxis, SIGNAL(rangeChanged(QCPRange)), ui->Temp_Plot->xAxis2, SLOT(setRange(QCPRange)));
    connect(ui->Temp_Plot->yAxis, SIGNAL(rangeChanged(QCPRange)), ui->Temp_Plot->yAxis2, SLOT(setRange(QCPRange)));

    connect(ui->Humidity_Plot->xAxis, SIGNAL(rangeChanged(QCPRange)), ui->Humidity_Plot->xAxis2, SLOT(setRange(QCPRange)));
    connect(ui->Humidity_Plot->yAxis, SIGNAL(rangeChanged(QCPRange)), ui->Humidity_Plot->yAxis2, SLOT(setRange(QCPRange)));

    // Allow user to drag axis ranges with mouse, zoom with mouse wheel and select graphs by clicking:
    ui->Temp_Plot->setInteractions(QCP::iRangeDrag | QCP::iRangeZoom | QCP::iSelectPlottables);
    ui->Humidity_Plot->setInteractions(QCP::iRangeDrag | QCP::iRangeZoom | QCP::iSelectPlottables);

    // first we create and prepare a text layout element:
    QCPTextElement *title_graph1 = new QCPTextElement(ui->Temp_Plot);
    title_graph1->setText("Temperature Real-Time Plot");
    title_graph1->setFont(QFont("sans", 12, QFont::Bold));

    QCPTextElement *title_graph2 = new QCPTextElement(ui->Humidity_Plot);
    title_graph2->setText("Humidity Real-Time Plot");
    title_graph2->setFont(QFont("sans", 12, QFont::Bold));

    // then we add it to the plot layout:
    // insert an empty row above the axis rect
    ui->Temp_Plot->plotLayout()->insertRow(0);
    ui->Humidity_Plot->plotLayout()->insertRow(0);

    // place the title in the empty cell we've just created
    ui->Temp_Plot->plotLayout()->addElement(0, 0, title_graph1);
    ui->Humidity_Plot->plotLayout()->addElement(0, 0, title_graph2);

    // Start Timer to Refresh the graph
    QTimer *timer = new QTimer(this);
    connect(timer, &QTimer::timeout, this, &WeatherViewer::refreshGraph);
    // Start Timer @ 1 second
    timer->start(1000);

}

WeatherViewer::~WeatherViewer()
{
    delete ui;
}

void WeatherViewer::on_btn_ConnectDisconnect_clicked()
{
    double now;
    // if false, we have to connect, else disconnect
    if( connect_status == false){
        qInfo() << "Connecting...";
        m_serial.setBaudRate(QSerialPort::Baud9600);
        m_serial.setDataBits(QSerialPort::Data8);
        m_serial.setParity( QSerialPort::NoParity);
        m_serial.setStopBits(QSerialPort::OneStop);
        m_serial.setFlowControl(QSerialPort::NoFlowControl);

        // Select the COM Port from Combo Box
        m_serial.setPortName(ui->cb_COM->currentText());

        if(m_serial.open(QIODevice::ReadWrite)){
            qDebug() << "Serial Port Opened Successfully";
            m_serial.write("Hello World from Qt\r\n");
            connect_status = true;
            ui->btn_ConnectDisconnect->setText("Disconnect");

            // disable the combo box
            ui->cb_COM->setEnabled(false);

            // Connect Signal and Slots
            connect(&m_serial, SIGNAL( readyRead() ), this, SLOT(read_data() ) );

            startTime = QDateTime::currentSecsSinceEpoch();
            now = startTime;

            // set axes ranges, so we see all data:
            ui->Temp_Plot->xAxis->setRange(now, now+SECONDS_SHOW_ON_GRAPH);
            ui->Temp_Plot->yAxis->setRange(0, 60);

            ui->Humidity_Plot->xAxis->setRange(now, now+SECONDS_SHOW_ON_GRAPH);
            ui->Humidity_Plot->yAxis->setRange(0, 100);
        }else{
            qDebug() << "Unable to open the Selected Serial Port" << m_serial.error();
        }
    }else{
        qInfo() << "Disconnecting...";
        // close the serial port
        m_serial.close();
        connect_status = false;
        ui->btn_ConnectDisconnect->setText("Connect");
        // Enable the combo box
        ui->cb_COM->setEnabled(true);
    }
}

void WeatherViewer::read_data()
{
    QByteArray serialData;
    QByteArray temperature;
    QByteArray humidity;

    double now = QDateTime::currentSecsSinceEpoch();

    while(m_serial.canReadLine()){
        serialData = m_serial.readLine();
        temperature.append(serialData[0]);
        temperature.append(serialData[1]);
        humidity.append(serialData[3]);
        humidity.append(serialData[4]);

        //qDebug() << temperature.toUInt() << humidity.toUInt();

        short temp_value = temperature.toUShort();
        short humidity_value = humidity.toUShort();

        time_axis.append(now);
        temperature_axis.append(temperature.toUShort());
        humidity_axis.append(humidity_value);

        temp_data_idx++;
        qDebug() << temp_data_idx << temp_value << ' ' << humidity_value;
    }
}

void WeatherViewer::refreshGraph()
{
    double now = QDateTime::currentSecsSinceEpoch();
    if( connect_status )
    {
        ui->Temp_Plot->graph()->setData(time_axis, temperature_axis);
        ui->Humidity_Plot->graph()->setData(time_axis, humidity_axis);
        // if time has elapsed then only start shifting the graph
        if( ((qint64)(now) - startTime) > SECONDS_SHOW_ON_GRAPH )
        {
            // If SECONDS_SHOW_GRAPH
            ui->Temp_Plot->xAxis->setRange(now, SECONDS_SHOW_ON_GRAPH, Qt::AlignRight);
            ui->Humidity_Plot->xAxis->setRange(now, SECONDS_SHOW_ON_GRAPH, Qt::AlignRight);
        }
        ui->Temp_Plot->replot();
        ui->Humidity_Plot->replot();
    }
}

