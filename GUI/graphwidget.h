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

#ifndef GRAPHWIDGET_H
#define GRAPHWIDGET_H

#include <QGraphicsView>
#include <node.h>
#include <QTimer>

class Node;

class Connection {
    public:
        int prv;
        int nxt;

    bool isEquals(Connection conn) {
        // direction doesn't matter
        return ((prv == conn.prv) && (nxt == conn.nxt)) || ((prv == conn.nxt) && (nxt == conn.prv));
    }
};

class Route {
  public:
    std::list<Connection> connections;
    int src;
    int dst;
    int age;

    Route(int s, int d, std::list<Connection> conn)
        {
            src = s;
            dst = d;
            connections = conn;
            age = 0;
        }

    void setSource(int s) {
        src = s;
    }

    void setConnections(std::list<Connection> conn) {
        connections = conn;
    }

bool isConnectionInRoute(Connection conn) const {
        for (const Connection & c : connections)
        {
            if (conn.isEquals(c))
                return true;
        }
        return false;
    }
/*
bool isEquals(const Route r) {
    bool equalSource = src == r.src;
    bool equalDestination = dst == r.dst;
    bool equalConnections = 1;
    for (const Connection & c : connections)
    {
        equalConnections = equalConnections && r.isConnectionInRoute(c);
    }
    for (const Connection & c_other : r.connections)
    {
        equalConnections = equalConnections && isConnectionInRoute(c_other);
    }
    return equalSource && equalDestination && equalConnections;
}
*/

bool operator == (const Route &r) const
{
    bool equalSource = this->src == r.src;
    bool equalDestination = this->dst == r.dst;
    bool equalConnections = 1;
    for (const Connection & c : connections)
    {
        equalConnections = equalConnections && r.isConnectionInRoute(c);
    }
    for (const Connection & c_other : r.connections)
    {
        equalConnections = equalConnections && this->isConnectionInRoute(c_other);
    }
    return equalSource && equalDestination && equalConnections;
}

~Route(){}
};


//! [0]
class GraphWidget : public QGraphicsView
{
    Q_OBJECT

private:
    int timerId = 0;
    std::list<Route> route_list;
    Node *node1;
    Node *node2;
    Node *node3;
    Node *node4;
    Node *node5;
    Node *node6;
    Node *node7;

public:
    GraphWidget(QWidget *parent = nullptr);
    void addToRouteList(int route_array[], int src);
    void drawRoutes();
    void itemMoved();
    void drawRoute(Route r);
    void addNodeLabels(char *node_id, int pos_x, int pos_y);
    void setNodesPosition(Node *node_1, Node *node_2, Node *node_3, Node *node_4, Node *node_5, Node *node_6, Node *node_7);
    std::list<Connection> getConnectionListfromArray(int route_array[]);

    void setNode(Node *node, int node_id) {
        switch (node_id)
        {
            case 1:
                node1 = node;
                break;
            case 2:
                node2 = node;
                break;
            case 3:
                node3 = node;
                break;
            case 4:
                node4 = node;
                break;
            case 5:
                node5 = node;
                break;
            case 6:
                node6 = node;
                break;
            case 7:
                node7 = node;
                break;
        }
    }

    Node *getNode(int node_id) {
        switch (node_id)
        {
            case 1:
                return node1;
            case 2:
                return node2;
            case 3:
                return node3;
            case 4:
                return node4;
            case 5:
                return node5;
            case 6:
                return node6;
            case 7:
                return node7;
        }
    }

public slots:
    void shuffle();
    void zoomIn();
    void zoomOut();
    void removefromRouteList(int);

protected:
    void keyPressEvent(QKeyEvent *event) override;
    void timerEvent(QTimerEvent *event) override;
#if QT_CONFIG(wheelevent)
    void wheelEvent(QWheelEvent *event) override;
#endif
    void drawBackground(QPainter *painter, const QRectF &rect) override;

    void scaleView(qreal scaleFactor);

};
//! [0]

#endif // GRAPHWIDGET_H


