#include "mainwindow.h"
#include "ui_mainwindow.h"
#include <qdebug.h>
#include <graphwidget.h>

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{

    ui->setupUi(this);

    // Get all available COM Ports and store them in a QList.
    QList<QextPortInfo> ports = QextSerialEnumerator::getPorts();

    // Read each element on the list, but
    // add only USB ports to the combo box.
    for (int i = 0; i < ports.size(); i++) {
        if (ports.at(i).portName.contains("USB")){
            ui->comboBox_Interface->addItem(ports.at(i).portName.toLocal8Bit().constData());
        }
    }
    // Show a hint if no USB ports were found.
    if (ui->comboBox_Interface->count() == 0){
        ui->textEdit_Status->insertPlainText("No USB ports available.\nConnect a USB device and try again.");
    }

    ui->comboBox_Interface_source->addItem("All");
}

MainWindow::~MainWindow()
{
    delete ui;
}

// SLOT: Configures the serial port upon clicking the "open" button.
void MainWindow::on_pushButton_open_clicked() {

    // The instance of qextserialport is configured like so:
    port.setQueryMode(QextSerialPort::EventDriven);
    port.setPortName("/dev/" + ui->comboBox_Interface->currentText());
    port.setBaudRate(BAUD115200);
    port.setFlowControl(FLOW_OFF);
    port.setParity(PAR_NONE);
    port.setDataBits(DATA_8);
    port.setStopBits(STOP_1);

    // Open sesame!!
    port.open(QIODevice::ReadWrite);

    // Check if the port opened without problems.
    if (!port.isOpen())
    {
        error.setText("Unable to open port!");
        error.show();
        return;
    }

    /* This is where the MAGIC HAPPENS and it basically means:
     *      If the object "port" uses its "readyRead" function,
     *      it will trigger MainWindow's "receive" function.
     */
    QObject::connect(&port, SIGNAL(readyRead()), this, SLOT(receive()));

    // Only ONE button can be enabled at any given moment.
    ui->pushButton_close->setEnabled(true);
    ui->pushButton_open->setEnabled(false);
    ui->comboBox_Interface->setEnabled(false);

    ui->pushButton_send->setEnabled(true);
    ui->plainTextEdit_command->setEnabled(true);
}

// SLOT: Closes the port and re-enables the open button.
void MainWindow::on_pushButton_close_clicked()
{
    if (port.isOpen())port.close();
    ui->pushButton_close->setEnabled(false);
    ui->pushButton_open->setEnabled(true);
    ui->comboBox_Interface->setEnabled(true);

    ui->pushButton_send->setEnabled(false);
    ui->plainTextEdit_command->setEnabled(false);
}

// SLOT: Sends whatever is written on plainTextEdit_command to the USB port.
void MainWindow::on_pushButton_send_clicked() {

    QString command;
    command = ui->plainTextEdit_command->toPlainText();

    QByteArray byteArray = command.toLocal8Bit();

    byteArray.append('\n');
    port.write(byteArray);
}

// SLOT: Prints data received from the port on the QTextEdit widget.
void MainWindow::receive()
{

    QByteArray data = port.readAll();
    QString str = data;
    ui->textEdit_Status->insertPlainText(data);

    double temp;
    double batt;
    // GraphWidget graph = (GraphWidget) centralWidget();

    QStringList data_list = str.split(";");
    if(!data_list.isEmpty()){
        qDebug() << "received data: " << data;
        qDebug() << "data_list(0): " << data_list.at(0);
        if(data_list.at(0) == "DATA"){      // data is received     DATA;ID:%2d;SRC:%1d;DEST:%1d;PAYLOAD:%s
            int route_array [6];
            int src = 0;
            for(int i=1; i < data_list.size(); i++)
                {
                    qDebug() << "data list value " << i << " " << data_list.at(i);
                    QString str = data_list.at(i);
                    if(str.contains("PAYLOAD:")){       //PAYLOAD:Temperature %d Luminance %d
                        QStringList pay_list = str.split(" ");
                        qDebug() << "pay_list(0): " << pay_list.at(0);  //PAYLOAD:Temperature
                        qDebug() << "pay_list(1): " << pay_list.at(1);  //'Temperature value'
                        qDebug() << "pay_list(2): " << pay_list.at(2);  //Luminance
                        qDebug() << "pay_list(3): " << pay_list.at(3);  //'Battery value'
                        temp = pay_list.at(1).toDouble();
                        batt = pay_list.at(3).toDouble();
                    }
                    if(str.contains("ROUTE:")){       //ROUTE:%d,%d,%d,%d,%d,%d
                        QStringList route_list = str.split(",");
                        qDebug() << "route_list(0): " << route_list.at(0);  //ROUTE:node1
                        qDebug() << "route_list(1): " << route_list.at(1);  //'Temperature value'
                        qDebug() << "route_list(2): " << route_list.at(2);  //Luminance
                        qDebug() << "route_list(3): " << route_list.at(3);  //'Battery value'
                        qDebug() << "route_list(4): " << route_list.at(4);  //'Temperature value'
                        qDebug() << "proute_list(5): " << route_list.at(5);  //Luminance
                        route_array[0] = route_list.at(0).split(":").at(1).toInt();
                        route_array[1] = route_list.at(1).toInt();
                        route_array[2] = route_list.at(2).toInt();
                        route_array[3] = route_list.at(3).toInt();
                        route_array[4] = route_list.at(4).toInt();
                        route_array[5] = route_list.at(5).toInt();
                    }
                }
            addToFile(temp, batt, src);
            qDebug() << "current comboBox text: " << ui->comboBox_Interface_source->currentText();
            if((ui->comboBox_Interface_source->currentText() == src) || (ui->comboBox_Interface_source->currentText() == "All")){
                updateProgressBar(temp, batt);
            }

            widget->addToRouteList(route_array, src);
            widget->drawRoutes();
        }
    }
}

void MainWindow::addToFile(double temp, double batt, int src){
    QFile file("Data.txt");
    file.open(QIODevice::ReadOnly);

    QString lines[file_size] = {0, 0, 0, 0, 0, 0, 0, 0, 0};
    for(int i = 0; i < file_size; i++){
        lines[i] = file.readLine();
    }
    file.close();

    QFile file2("Data.txt");
    file2.open(QIODevice::WriteOnly | QFile::Truncate);
    QTextStream out(&file2);
    if(src < file_size){
        lines[src-1] = QString::number(temp) + " " + QString::number(batt);         //z.B. '24 12'
        qDebug() << "Add to file: " << lines[src-1];
    }

    for(int i = 0; i < file_size; i++){
        if(lines[i] == "" || lines[i] == "\n"){
            out << endl;
        }else{
            out << lines[i];
            if(combo_flags[i] == 0){
                qDebug() << "Add source " << i+1 << "to comboBox.";
                ui->comboBox_Interface_source->addItem(QString::number(i+1));
                combo_flags[i] = 1;
            }
        }
    }
    file2.close();
}

void MainWindow::updateProgressBar(double temp, double batt){
    temp = temp / 1000;
    printf("Temperature: %f\n",temp);
    ui->progressBar_light->setMaximum(40);
    qDebug() << "Temp value " << QString::number(temp);
    ui->lcdNumber_light->display(temp);
    ui->progressBar_light->setValue((int)temp);

    batt = batt / 1000;
    printf("Battery: %f\n",batt);
    ui->progressBar_battery->setMaximum(40);
    qDebug() << "Temp value " << QString::number(batt);
    ui->lcdNumber_battery->display(batt);
    ui->progressBar_battery->setValue((int)batt);
}

void MainWindow::on_comboBox_Interface_source_currentIndexChanged(const QString &arg1)
{
    qDebug() << "current text of comboBox: " << arg1;

    if(arg1 != "All"){
        qDebug() << "crrent number of comboBox: " << arg1.toInt();
        int index = arg1.toInt();

        QFile file("Data.txt");
        file.open(QIODevice::ReadOnly);
        QString lines[file_size] = {0, 0, 0, 0, 0, 0, 0, 0, 0};
        for(int i = 0; i < file_size; i++){
            lines[i] = file.readLine();
        }
        file.close();

        if(lines[index] != "" || lines[index] != "\n" || lines[index] != NULL){
            qDebug() << "data of src " << index << ": " << lines[index];
            QStringList data_list = lines[index].split(" ");
            updateProgressBar(data_list.at(0).toDouble(), data_list.at(1).toDouble());
        }
    }


}
