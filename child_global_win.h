#ifndef CHILD_MAINWINDOW_H
#define CHILD_MAINWINDOW_H

#include <QMainWindow>
#include "zmq_thread.h"
#include "QGlobalStats.h"
#include <QVariantHash>
/**  Copyright (c) 2015-2015 Cisco Systems, Inc.  **
**                                                                        **
**  This program is free software: you can redistribute it and/or modify  **
**  it under the terms of the GNU General Public License as published by  **
**  the Free Software Foundation, either version 3 of the License, or     **
**  (at your option) any later version.                                   **
**                                                                        **
**  This program is distributed in the hope that it will be useful,       **
**  but WITHOUT ANY WARRANTY; without even the implied warranty of        **
**  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the         **
**  GNU General Public License for more details.                          **
**                                                                        **
**  You should have received a copy of the GNU General Public License     **
**  along with this program.  If not, see http://www.gnu.org/licenses/.   **
*/



namespace Ui {
class GlobalChildMainWindow;
}

class GlobalChildMainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit GlobalChildMainWindow(QWidget *parent = 0);
    ~GlobalChildMainWindow();

public slots:
    void update_new_json(QString s);
    void reset();

private:
    void update_html();

signals:
    void latency_changed(double client,double server);
    void update_port_counters(CGlobalInfo * info);
    void update_rx_template(CGlobalInfo * info);

    void update_rx_counters(CGlobalInfo * info);
    void update_l2_counters(CParserLatencyV2Info * info);

public:

    inline CZmqThread   * get_zmq_trex(){
        return (m_thread);
    }

private:
    Ui::GlobalChildMainWindow *ui;
    CZmqThread   *  m_thread;
    CGlobalInfo     m_stats;
   // CGlobalRxCheckInfo     rx_stats;
    CParserLatencyV2Info   l2_stats;
    QString         m_global_html_tp;
    QVariantHash    m_global_vals;

};

#endif // MAINWINDOW_H
