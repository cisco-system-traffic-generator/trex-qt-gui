#ifndef CZMQ_THREAD_H
#define CZMQ_THREAD_H
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

#include <zmq.h>
#include <zmq_utils.h>
#include <assert.h>
#include <QString>
#include <QObject>
#include <QTimerEvent>
#include <stdint.h>



class CZmqThread : public QObject
{
    Q_OBJECT

public:
    CZmqThread():QObject(){
        m_host="";
        m_socket=0;
        m_timer_id=0;
    }

    CZmqThread(QString host):QObject(){
        m_host=host;
    }
    ~CZmqThread();


    void set_host(const QString s){
        m_host=s;
    }


signals:
    void have_data(QString s);

public:
    /* should be called from main thread */
    int         init_socket(void);
    int         close_socket_and_wait(void);
    int         start();


private:
    int         init_msg();
    int         block_rcv_socket(QString & s);
    void        timerEvent(QTimerEvent *event);
    void do_pool()  ;



private:
    int      m_timer_id;
    QString  m_host;
    void*    m_socket;
    zmq_msg_t m_msg;
};


#endif


