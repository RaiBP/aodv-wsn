/****************************************************************************
**
** Copyright (C) 2016 The Qt Company Ltd.
** Contact: https://www.qt.io/licensing/
**
** This file is part of the examples of the Qt Toolkit.
**
** $QT_BEGIN_LICENSE:BSD$
** Commercial License Usage
** Licensees holding valid commercial Qt licenses may use this file in
** accordance with the commercial license agreement provided with the
** Software or, alternatively, in accordance with the terms contained in
** a written agreement between you and The Qt Company. For licensing terms
** and conditions see https://www.qt.io/terms-conditions. For further
** information use the contact form at https://www.qt.io/contact-us.
**
** BSD License Usage
** Alternatively, you may use this file under the terms of the BSD license
** as follows:
**
** "Redistribution and use in source and binary forms, with or without
** modification, are permitted provided that the following conditions are
** met:
**   * Redistributions of source code must retain the above copyright
**     notice, this list of conditions and the following disclaimer.
**   * Redistributions in binary form must reproduce the above copyright
**     notice, this list of conditions and the following disclaimer in
**     the documentation and/or other materials provided with the
**     distribution.
**   * Neither the name of The Qt Company Ltd nor the names of its
**     contributors may be used to endorse or promote products derived
**     from this software without specific prior written permission.
**
**
** THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS
** "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT
** LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR
** A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT
** OWNER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL,
** SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT
** LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE,
** DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY
** THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
** (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
** OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE."
**
** $QT_END_LICENSE$
**
****************************************************************************/

#include "graphwidget.h"
#include "edge.h"
#include "node.h"

#include <math.h>
#include <iostream>
#include <list>
#include <qdebug.h>
#include <QKeyEvent>
#include <QRandomGenerator>


//! [0]
GraphWidget::GraphWidget(QWidget *parent)
    : QGraphicsView(parent) 
{
    QGraphicsScene *scene = new QGraphicsScene(this);
    scene->setItemIndexMethod(QGraphicsScene::NoIndex);
    scene->setSceneRect(-200, -200, 400, 400);
    setScene(scene);
    setCacheMode(CacheBackground);
    setViewportUpdateMode(BoundingRectViewportUpdate);
    setRenderHint(QPainter::Antialiasing);
    setTransformationAnchor(AnchorUnderMouse);
    scale(qreal(0.8), qreal(0.8));
    setMinimumSize(400, 400);
    setWindowTitle(tr("Elastic Nodes"));
//! [0]

//! [1]
    //std::list<Route> route_list;

    Node *node1 = new Node(this); // Receiving Node
    Node *node2 = new Node(this);
    Node *node3 = new Node(this);
    Node *node4 = new Node(this);
    Node *node5 = new Node(this);
    Node *node6 = new Node(this);
    Node *node7 = new Node(this);

    setNode(node1, 1);
    setNode(node2, 2);
    setNode(node3, 3);
    setNode(node4, 4);
    setNode(node5, 5);
    setNode(node6, 6);
    setNode(node7, 7);


    scene->addItem(node1);
    scene->addItem(node2);
    scene->addItem(node3);
    scene->addItem(node4);
    scene->addItem(node5);
    scene->addItem(node6);
    scene->addItem(node7);

    node1->setPos(0, 0);
    node2->setPos(0, -50);
    node3->setPos(50, -50);
    node4->setPos(-50, 0);
    node5->setPos(50, 0);
    node6->setPos(-50, 50);
    node7->setPos(-50, -50);

}
//! [1]

// SLOT: Prints data received from the port on the QTextEdit widget.
/*
void GraphWidget::receiveTopology()
{

    QByteArray data = port.readAll();
    QString str = data;
    ui->textEdit_Status->insertPlainText(data);

    int src;
    double temp;
    double batt;

    QStringList data_list = str.split(";");
    if(!data_list.isEmpty()){
        qDebug() << "received data: " << data;
        qDebug() << "data_list(0): " << data_list.at(0);
        if(data_list.at(0) == "DATA"){      // data is received     DATA;ID:%2d;SRC:%1d;DEST:%1d;PAYLOAD:%s;ROUTE:%1d,%1d,...
            int[6] route_array = {0};
            int src = 0;
            for(int i=1; i < data_list.size(); i++){
                qDebug() << "data list value " << i << " " << data_list.at(i);
                QString str = data_list.at(i);
                if(str.contains("SRC:")){
                    QStringList src_list = str.split(":");          //SRC:%1d
                    qDebug() << "src_list(1): " << src_list.at(1);
                    src = src_list.at(1).toInt();
                }
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
                    qDebug() << "route_list(0): " << pay_list.at(0);  //PAYLOAD:Temperature
                    qDebug() << "route_list(1): " << pay_list.at(1);  //'Temperature value'
                    qDebug() << "route_list(2): " << pay_list.at(2);  //Luminance
                    qDebug() << "route_list(3): " << pay_list.at(3);  //'Battery value'
                    qDebug() << "route_list(4): " << pay_list.at(4);  //'Temperature value'
                    qDebug() << "proute_list(5): " << pay_list.at(5);  //Luminance
                    route_array[0] = pay_list.at(0).toInt();
                    route_array[1] = pay_list.at(1).toInt();
                    route_array[2] = pay_list.at(2).toInt();
                    route_array[3] = pay_list.at(3).toInt();
                    route_array[4] = pay_list.at(4).toInt();
                    route_array[5] = pay_list.at(5).toInt();
                    }
                }
            addToRouteList(route_array, src);    
            drawRoutes();
        }
    }
}
*/

std::list<Connection> GraphWidget::getConnectionListfromArray(int route_array[])
{
    std::list<Connection> conn_list;
    for (int i=0; i<6; i++) {
        int node_id = route_array[i];
        int next_node_id;
        if (node_id == 0)
            break;
        else {
            if (i==5) {
                next_node_id = 1; // Receiving Node is always 1
            }
            else {
                next_node_id = route_array[i+1];
                if (next_node_id == 0) {
                    next_node_id = 1; // Receiving Node is always 1
                }
            }
            Connection new_connection;
            new_connection.prv = node_id;
            new_connection.nxt = next_node_id;
            conn_list.push_back(new_connection);
        }
    }
    return conn_list;
}


void GraphWidget::addToRouteList(int route_array[], int src)
{   
    std::list<Connection> new_connections = getConnectionListfromArray(route_array);

    for (Route & r : route_list)
    {
            if (r.src == src) {
                r.setSource(src);
                r.setConnections(new_connections);
            }
    }

    Route new_route(src, 1, new_connections);

    route_list.push_back(new_route);
}

void GraphWidget::drawRoutes()
{
    scene()->clear(); // clear all items from scene

    Node *node1 = new Node(this); // Receiving Node
    Node *node2 = new Node(this);
    Node *node3 = new Node(this);
    Node *node4 = new Node(this);
    Node *node5 = new Node(this);
    Node *node6 = new Node(this);
    Node *node7 = new Node(this);

    setNode(node1, 1);
    setNode(node2, 2);
    setNode(node3, 3);
    setNode(node4, 4);
    setNode(node5, 5);
    setNode(node6, 6);
    setNode(node7, 7);

    qDebug() << "adding node1";
    scene()->addItem(node1);
    qDebug() << "adding node2";
    scene()->addItem(node2);
    qDebug() << "adding node3";
    scene()->addItem(node3);
    qDebug() << "adding node4";
    scene()->addItem(node4);
    qDebug() << "adding node5";
    scene()->addItem(node5);
    qDebug() << "adding node6";
    scene()->addItem(node6);
    qDebug() << "adding node7";
    scene()->addItem(node7);

    node1->setPos(0, 0);
    node2->setPos(0, -50);
    node3->setPos(50, -50);
    node4->setPos(-50, 0);
    node5->setPos(50, 0);
    node6->setPos(-50, 50);
    node7->setPos(-50, -50);

    for (const Route & r : route_list)
    {
            drawRoute(r);
    }
}

void GraphWidget::drawRoute(Route r)
{
    for (const Connection & c : r.connections)
    {
        Node *prev;
        Node *next;

        prev = getNode(c.prv);
        next = getNode(c.nxt);

        scene()->addItem(new Edge(prev, next));
    }

}

//! [2]
void GraphWidget::itemMoved()
{
    if (!timerId)
        timerId = startTimer(1000 / 25);
}
//! [2]

//! [3]
void GraphWidget::keyPressEvent(QKeyEvent *event)
{
    switch (event->key()) {
    case Qt::Key_Up:
        node1->moveBy(0, -20);
        break;
    case Qt::Key_Down:
        node1->moveBy(0, 20);
        break;
    case Qt::Key_Left:
        node1->moveBy(-20, 0);
        break;
    case Qt::Key_Right:
        node1->moveBy(20, 0);
        break;
    case Qt::Key_Plus:
        zoomIn();
        break;
    case Qt::Key_Minus:
        zoomOut();
        break;
    case Qt::Key_Space:
    case Qt::Key_Enter:
        shuffle();
        break;
    default:
        QGraphicsView::keyPressEvent(event);
    }
}
//! [3]

//! [4]
void GraphWidget::timerEvent(QTimerEvent *event)
{
    Q_UNUSED(event);

    QVector<Node *> nodes;
    const QList<QGraphicsItem *> items = scene()->items();
    for (QGraphicsItem *item : items) {
        if (Node *node = qgraphicsitem_cast<Node *>(item))
            nodes << node;
    }

    for (Node *node : qAsConst(nodes))
        node->calculateForces();

    bool itemsMoved = false;
    for (Node *node : qAsConst(nodes)) {
        if (node->advancePosition())
            itemsMoved = true;
    }

    if (!itemsMoved) {
        killTimer(timerId);
        timerId = 0;
    }
}
//! [4]

#if QT_CONFIG(wheelevent)
//! [5]
void GraphWidget::wheelEvent(QWheelEvent *event)
{
    scaleView(pow(2., -event->angleDelta().y() / 240.0));
}
//! [5]
#endif

//! [6]
void GraphWidget::drawBackground(QPainter *painter, const QRectF &rect)
{
    Q_UNUSED(rect);

    // Shadow
    QRectF sceneRect = this->sceneRect();
    QRectF rightShadow(sceneRect.right(), sceneRect.top() + 5, 5, sceneRect.height());
    QRectF bottomShadow(sceneRect.left() + 5, sceneRect.bottom(), sceneRect.width(), 5);
    if (rightShadow.intersects(rect) || rightShadow.contains(rect))
        painter->fillRect(rightShadow, Qt::darkGray);
    if (bottomShadow.intersects(rect) || bottomShadow.contains(rect))
        painter->fillRect(bottomShadow, Qt::darkGray);

    // Fill
    QLinearGradient gradient(sceneRect.topLeft(), sceneRect.bottomRight());
    gradient.setColorAt(0, Qt::white);
    gradient.setColorAt(1, Qt::lightGray);
    painter->fillRect(rect.intersected(sceneRect), gradient);
    painter->setBrush(Qt::NoBrush);
    painter->drawRect(sceneRect);

    // Text
    QRectF textRect(sceneRect.left() + 4, sceneRect.top() + 4,
                    sceneRect.width() - 4, sceneRect.height() - 4);
    QString message(tr("Click and drag the nodes around, and zoom with the mouse "
                       "wheel or the '+' and '-' keys"));

    QFont font = painter->font();
    font.setBold(true);
    font.setPointSize(14);
    painter->setFont(font);
    painter->setPen(Qt::lightGray);
    painter->drawText(textRect.translated(2, 2), message);
    painter->setPen(Qt::black);
    painter->drawText(textRect, message);
}
//! [6]

//! [7]
void GraphWidget::scaleView(qreal scaleFactor)
{
    qreal factor = transform().scale(scaleFactor, scaleFactor).mapRect(QRectF(0, 0, 1, 1)).width();
    if (factor < 0.07 || factor > 100)
        return;

    scale(scaleFactor, scaleFactor);
}
//! [7]

void GraphWidget::shuffle()
{
    const QList<QGraphicsItem *> items = scene()->items();
    for (QGraphicsItem *item : items) {
        if (qgraphicsitem_cast<Node *>(item))
            item->setPos(-150 + QRandomGenerator::global()->bounded(300), -150 + QRandomGenerator::global()->bounded(300));
    }
}

void GraphWidget::zoomIn()
{
    scaleView(qreal(1.2));
}

void GraphWidget::zoomOut()
{
    scaleView(1 / qreal(1.2));
}
 
