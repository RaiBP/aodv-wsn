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
    QWidget *layoutWidget;
    QVBoxLayout *verticalLayout;
    QLabel *label;
    QComboBox *comboBox_Interface;
    QPushButton *pushButton_open;
    QPushButton *pushButton_close;
    QProgressBar *progressBar_lux;
    QLCDNumber *lcdNumber_temp;
    QLabel *label_lux;
    QProgressBar *progressBar_temp;
    QLabel *label_temp;
    QLCDNumber *lcdNumber_lux;
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
        MainWindow->resize(887, 445);
        centralWidget = new QWidget(MainWindow);
        centralWidget->setObjectName(QStringLiteral("centralWidget"));
        textEdit_Status = new QTextEdit(centralWidget);
        textEdit_Status->setObjectName(QStringLiteral("textEdit_Status"));
        textEdit_Status->setEnabled(true);
        textEdit_Status->setGeometry(QRect(160, 10, 431, 331));
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

        progressBar_lux = new QProgressBar(centralWidget);
        progressBar_lux->setObjectName(QStringLiteral("progressBar_lux"));
        progressBar_lux->setGeometry(QRect(760, 130, 71, 211));
        progressBar_lux->setMaximum(40);
        progressBar_lux->setValue(0);
        progressBar_lux->setTextVisible(false);
        progressBar_lux->setOrientation(Qt::Vertical);
        progressBar_lux->setInvertedAppearance(false);
        lcdNumber_temp = new QLCDNumber(centralWidget);
        lcdNumber_temp->setObjectName(QStringLiteral("lcdNumber_temp"));
        lcdNumber_temp->setGeometry(QRect(620, 100, 71, 23));
        lcdNumber_temp->setFrameShape(QFrame::Box);
        lcdNumber_temp->setFrameShadow(QFrame::Plain);
        lcdNumber_temp->setSegmentStyle(QLCDNumber::Flat);
        label_lux = new QLabel(centralWidget);
        label_lux->setObjectName(QStringLiteral("label_lux"));
        label_lux->setGeometry(QRect(750, 80, 111, 16));
        progressBar_temp = new QProgressBar(centralWidget);
        progressBar_temp->setObjectName(QStringLiteral("progressBar_temp"));
        progressBar_temp->setGeometry(QRect(620, 130, 71, 211));
        progressBar_temp->setMaximum(40);
        progressBar_temp->setValue(0);
        progressBar_temp->setTextVisible(false);
        progressBar_temp->setOrientation(Qt::Vertical);
        progressBar_temp->setInvertedAppearance(false);
        label_temp = new QLabel(centralWidget);
        label_temp->setObjectName(QStringLiteral("label_temp"));
        label_temp->setGeometry(QRect(610, 80, 91, 16));
        lcdNumber_lux = new QLCDNumber(centralWidget);
        lcdNumber_lux->setObjectName(QStringLiteral("lcdNumber_lux"));
        lcdNumber_lux->setGeometry(QRect(760, 100, 71, 23));
        lcdNumber_lux->setFrameShape(QFrame::Box);
        lcdNumber_lux->setFrameShadow(QFrame::Plain);
        lcdNumber_lux->setSegmentStyle(QLCDNumber::Flat);
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
        menuBar->setGeometry(QRect(0, 0, 887, 22));
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
        label->setText(QApplication::translate("MainWindow", "Port", Q_NULLPTR));
        pushButton_open->setText(QApplication::translate("MainWindow", "Open", Q_NULLPTR));
        pushButton_close->setText(QApplication::translate("MainWindow", "Close", Q_NULLPTR));
        label_lux->setText(QApplication::translate("MainWindow", "Luminity Value", Q_NULLPTR));
        label_temp->setText(QApplication::translate("MainWindow", "Temp Value", Q_NULLPTR));
        label_source->setText(QApplication::translate("MainWindow", "Source", Q_NULLPTR));
    } // retranslateUi

};

namespace Ui {
    class MainWindow: public Ui_MainWindow {};
} // namespace Ui

QT_END_NAMESPACE

#endif // UI_MAINWINDOW_H
