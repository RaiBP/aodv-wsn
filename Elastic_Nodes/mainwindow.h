#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QMessageBox>
#include <graphwidget.h>
#include "qextserialport.h"         // Enables use of the qextserialport library.
#include "qextserialenumerator.h"   // Helps list of open ports.

namespace Ui {
class MainWindow;
}

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = 0);
    ~MainWindow();

private:
    Ui::MainWindow *ui;
    QextSerialPort port;            // Creates a serial port instance.
    QMessageBox error;              // USed to process error messages.
    int file_size = 9;
    int combo_flags[9] = {0,0,0,0,0,0,0,0,0};
    GraphWidget *widget = new GraphWidget;


    void addToFile(double temp, double batt, int src);
    void updateProgressBar(double temp, double batt);

private slots:
        void on_pushButton_close_clicked(); // Closes a port.
        void on_pushButton_open_clicked();  // Open a port.
        void receive();                     // Receives data from a port.

        void on_pushButton_send_clicked();  // Sends the command specified.
        void on_comboBox_Interface_source_currentIndexChanged(const QString &arg1);
};


#endif // MAINWINDOW_H
