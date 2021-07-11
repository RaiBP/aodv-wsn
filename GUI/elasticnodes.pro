QT += widgets

HEADERS += \
        edge.h \
        node.h \
        graphwidget.h \
	mainwindow.h


SOURCES += \
        edge.cpp \
        main.cpp \
        node.cpp \
        graphwidget.cpp \
	mainwindow.cpp

FORMS += \
        mainwindow.ui

# install
target.path = $$[QT_INSTALL_EXAMPLES]/widgets/graphicsview/elasticnodes
INSTALLS += target

HOMEDIR = $$(HOME)
include($$HOMEDIR/qextserialport/src/qextserialport.pri)
