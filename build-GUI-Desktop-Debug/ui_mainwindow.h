/********************************************************************************
** Form generated from reading UI file 'mainwindow.ui'
**
** Created by: Qt User Interface Compiler version 5.9.5
**
** WARNING! All changes made in this file will be lost when recompiling UI file!
********************************************************************************/

#ifndef UI_MAINWINDOW_H
#define UI_MAINWINDOW_H

#include <QtCore/QVariant>
#include <QtWidgets/QAction>
#include <QtWidgets/QApplication>
#include <QtWidgets/QButtonGroup>
#include <QtWidgets/QComboBox>
#include <QtWidgets/QHeaderView>
#include <QtWidgets/QLCDNumber>
#include <QtWidgets/QLabel>
#include <QtWidgets/QMainWindow>
#include <QtWidgets/QMenuBar>
#include <QtWidgets/QPlainTextEdit>
#include <QtWidgets/QProgressBar>
#include <QtWidgets/QPushButton>
#include <QtWidgets/QStatusBar>
#include <QtWidgets/QTextEdit>
#include <QtWidgets/QToolBar>
#include <QtWidgets/QVBoxLayout>
#include <QtWidgets/QWidget>

QT_BEGIN_NAMESPACE

class Ui_MainWindow
{
public:
    QWidget *centralWidget;
    QTextEdit *textEdit_Status;
    QPushButton *pushButton_send;
    QLabel *label_2;
    QPlainTextEdit *plainTextEdit_command;
    QWidget *layoutWidget;
    QVBoxLayout *verticalLayout;
    QLabel *label;
    QComboBox *comboBox_Interface;
    QPushButton *pushButton_open;
    QPushButton *pushButton_close;
    QProgressBar *progressBar_battery;
    QLCDNumber *lcdNumber_light;
    QLabel *label_battery;
    QProgressBar *progressBar_light;
    QLabel *label_light;
    QLCDNumber *lcdNumber_battery;
    QWidget *layoutWidget_2;
    QVBoxLayout *verticalLayout_2;
    QLabel *label_source;
    QComboBox *comboBox_Interface_source;
    QMenuBar *menuBar;
    QToolBar *mainToolBar;
    QStatusBar *statusBar;

    void setupUi(QMainWindow *MainWindow)
    {
        if (MainWindow->objectName().isEmpty())
            MainWindow->setObjectName(QStringLiteral("MainWindow"));
        MainWindow->resize(869, 440);
        centralWidget = new QWidget(MainWindow);
        centralWidget->setObjectName(QStringLiteral("centralWidget"));
        textEdit_Status = new QTextEdit(centralWidget);
        textEdit_Status->setObjectName(QStringLiteral("textEdit_Status"));
        textEdit_Status->setEnabled(true);
        textEdit_Status->setGeometry(QRect(160, 10, 431, 331));
        pushButton_send = new QPushButton(centralWidget);
        pushButton_send->setObjectName(QStringLiteral("pushButton_send"));
        pushButton_send->setEnabled(false);
        pushButton_send->setGeometry(QRect(20, 230, 121, 27));
        label_2 = new QLabel(centralWidget);
        label_2->setObjectName(QStringLiteral("label_2"));
        label_2->setGeometry(QRect(20, 170, 111, 17));
        plainTextEdit_command = new QPlainTextEdit(centralWidget);
        plainTextEdit_command->setObjectName(QStringLiteral("plainTextEdit_command"));
        plainTextEdit_command->setEnabled(false);
        plainTextEdit_command->setGeometry(QRect(20, 190, 121, 31));
        layoutWidget = new QWidget(centralWidget);
        layoutWidget->setObjectName(QStringLiteral("layoutWidget"));
        layoutWidget->setGeometry(QRect(20, 10, 121, 118));
        verticalLayout = new QVBoxLayout(layoutWidget);
        verticalLayout->setSpacing(6);
        verticalLayout->setContentsMargins(11, 11, 11, 11);
        verticalLayout->setObjectName(QStringLiteral("verticalLayout"));
        verticalLayout->setContentsMargins(0, 0, 0, 0);
        label = new QLabel(layoutWidget);
        label->setObjectName(QStringLiteral("label"));

        verticalLayout->addWidget(label);

        comboBox_Interface = new QComboBox(layoutWidget);
        comboBox_Interface->setObjectName(QStringLiteral("comboBox_Interface"));

        verticalLayout->addWidget(comboBox_Interface);

        pushButton_open = new QPushButton(layoutWidget);
        pushButton_open->setObjectName(QStringLiteral("pushButton_open"));

        verticalLayout->addWidget(pushButton_open);

        pushButton_close = new QPushButton(layoutWidget);
        pushButton_close->setObjectName(QStringLiteral("pushButton_close"));
        pushButton_close->setEnabled(false);

        verticalLayout->addWidget(pushButton_close);

        progressBar_battery = new QProgressBar(centralWidget);
        progressBar_battery->setObjectName(QStringLiteral("progressBar_battery"));
        progressBar_battery->setGeometry(QRect(760, 130, 71, 211));
        progressBar_battery->setMaximum(40);
        progressBar_battery->setValue(0);
        progressBar_battery->setTextVisible(false);
        progressBar_battery->setOrientation(Qt::Vertical);
        progressBar_battery->setInvertedAppearance(false);
        lcdNumber_light = new QLCDNumber(centralWidget);
        lcdNumber_light->setObjectName(QStringLiteral("lcdNumber_light"));
        lcdNumber_light->setGeometry(QRect(620, 100, 71, 23));
        lcdNumber_light->setFrameShape(QFrame::Box);
        lcdNumber_light->setFrameShadow(QFrame::Plain);
        lcdNumber_light->setSegmentStyle(QLCDNumber::Flat);
        label_battery = new QLabel(centralWidget);
        label_battery->setObjectName(QStringLiteral("label_battery"));
        label_battery->setGeometry(QRect(750, 80, 101, 16));
        progressBar_light = new QProgressBar(centralWidget);
        progressBar_light->setObjectName(QStringLiteral("progressBar_light"));
        progressBar_light->setGeometry(QRect(620, 130, 71, 211));
        progressBar_light->setMaximum(40);
        progressBar_light->setValue(0);
        progressBar_light->setTextVisible(false);
        progressBar_light->setOrientation(Qt::Vertical);
        progressBar_light->setInvertedAppearance(false);
        label_light = new QLabel(centralWidget);
        label_light->setObjectName(QStringLiteral("label_light"));
        label_light->setGeometry(QRect(610, 80, 91, 16));
        lcdNumber_battery = new QLCDNumber(centralWidget);
        lcdNumber_battery->setObjectName(QStringLiteral("lcdNumber_battery"));
        lcdNumber_battery->setGeometry(QRect(760, 100, 71, 23));
        lcdNumber_battery->setFrameShape(QFrame::Box);
        lcdNumber_battery->setFrameShadow(QFrame::Plain);
        lcdNumber_battery->setSegmentStyle(QLCDNumber::Flat);
        layoutWidget_2 = new QWidget(centralWidget);
        layoutWidget_2->setObjectName(QStringLiteral("layoutWidget_2"));
        layoutWidget_2->setGeometry(QRect(610, 10, 121, 61));
        verticalLayout_2 = new QVBoxLayout(layoutWidget_2);
        verticalLayout_2->setSpacing(6);
        verticalLayout_2->setContentsMargins(11, 11, 11, 11);
        verticalLayout_2->setObjectName(QStringLiteral("verticalLayout_2"));
        verticalLayout_2->setContentsMargins(0, 0, 0, 0);
        label_source = new QLabel(layoutWidget_2);
        label_source->setObjectName(QStringLiteral("label_source"));

        verticalLayout_2->addWidget(label_source);

        comboBox_Interface_source = new QComboBox(layoutWidget_2);
        comboBox_Interface_source->setObjectName(QStringLiteral("comboBox_Interface_source"));

        verticalLayout_2->addWidget(comboBox_Interface_source);

        MainWindow->setCentralWidget(centralWidget);
        menuBar = new QMenuBar(MainWindow);
        menuBar->setObjectName(QStringLiteral("menuBar"));
        menuBar->setGeometry(QRect(0, 0, 869, 22));
        MainWindow->setMenuBar(menuBar);
        mainToolBar = new QToolBar(MainWindow);
        mainToolBar->setObjectName(QStringLiteral("mainToolBar"));
        MainWindow->addToolBar(Qt::TopToolBarArea, mainToolBar);
        statusBar = new QStatusBar(MainWindow);
        statusBar->setObjectName(QStringLiteral("statusBar"));
        MainWindow->setStatusBar(statusBar);

        retranslateUi(MainWindow);

        QMetaObject::connectSlotsByName(MainWindow);
    } // setupUi

    void retranslateUi(QMainWindow *MainWindow)
    {
        MainWindow->setWindowTitle(QApplication::translate("MainWindow", "MainWindow", Q_NULLPTR));
        pushButton_send->setText(QApplication::translate("MainWindow", "Send", Q_NULLPTR));
        label_2->setText(QApplication::translate("MainWindow", "Command", Q_NULLPTR));
        label->setText(QApplication::translate("MainWindow", "Port", Q_NULLPTR));
        pushButton_open->setText(QApplication::translate("MainWindow", "Open", Q_NULLPTR));
        pushButton_close->setText(QApplication::translate("MainWindow", "Close", Q_NULLPTR));
        label_battery->setText(QApplication::translate("MainWindow", "Battery Value", Q_NULLPTR));
        label_light->setText(QApplication::translate("MainWindow", "Temp Value", Q_NULLPTR));
        label_source->setText(QApplication::translate("MainWindow", "Source", Q_NULLPTR));
    } // retranslateUi

};

namespace Ui {
    class MainWindow: public Ui_MainWindow {};
} // namespace Ui

QT_END_NAMESPACE

#endif // UI_MAINWINDOW_H
