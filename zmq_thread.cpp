#include "zmq_thread.h"
#include <stdio.h>
#include "zmq_gcontext.h"
#include <QDebug>
#include <QtEndian>

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


#if 1


int  CZmqThread::init_msg(void){
    int rc;
    rc = zmq_msg_init(&m_msg);
    if (rc != 0) {
        printf ("error in zmq_msg_init : %s\n", zmq_strerror (errno));
        return -1;
    }
    return (0);
}


void CZmqThread::do_pool()  {
    QString  s;
    int rc=0;
    //init_socket();
    rc=block_rcv_socket(s);
    if (rc==0) {
        emit have_data(s);
    }
}

CZmqThread::~CZmqThread(){
}


void CZmqThread::timerEvent(QTimerEvent *event){
    (void*)event;
    //(void)event;
    do_pool();
}


int   CZmqThread::start(){
    m_timer_id=startTimer(100);
    return (m_timer_id);
}


int CZmqThread::init_socket(){

    int rc;
    void *ctx=zmq_gcontext_get();

    m_socket = zmq_socket (ctx, ZMQ_SUB);
    if (!m_socket) {
        printf ("error in zmq_socket: %s\n", zmq_strerror (errno));
        return -1;
    }

    rc = zmq_connect (m_socket, ( char *)m_host.toUtf8().data());
    if (rc != 0) {
        printf ("error in zmq_connect: %s\n", zmq_strerror (errno));
        return -1;
    }

    rc=zmq_setsockopt (m_socket, ZMQ_SUBSCRIBE, 0,0);
    if (rc != 0) {
        printf ("error in zmq_setoption : %s\n", zmq_strerror (errno));
        return -1;
    }

    init_msg();
    return (0);
}


int CZmqThread::close_socket_and_wait(){
    if (m_timer_id) {
        killTimer(m_timer_id);
    }
    if ( m_socket) {
        assert(zmq_close (m_socket)==0);
        zmq_msg_close(&m_msg);
        m_socket=0;
    }
    return (0);
}

int CZmqThread::block_rcv_socket(QString & s){

    QString msg;
    int rc;
    int64_t more;
    size_t more_size = sizeof (more);
    s="";
    do {
        rc = zmq_recvmsg (m_socket, &m_msg, ZMQ_DONTWAIT);
        if (rc < 0) {
            if ( zmq_errno () ==  EAGAIN ){
                return (1);
            }
            printf ("error in zmq_rcv : %s\n", zmq_strerror (errno));
            return(-1);
        }else{
            char * p = (char *)zmq_msg_data (&m_msg);
            int lsize= (int)zmq_msg_size (&m_msg);
            if ( lsize > 4 ) {
                /* check MAGIC in the first 4 bytes in case we have it, it is compressed */
                uint32_t magic = qToBigEndian(*(uint32_t*)p);
                if (magic == 0xABE85CEA){
                    p+=4;
                    lsize-=4;
                    QByteArray compressed_data = qUncompress(QByteArray(p,lsize));
                    s +=(QString::fromLocal8Bit((char *)compressed_data.data() , compressed_data.size() ) );
                }else{
                    s +=(QString::fromLocal8Bit((char *)zmq_msg_data (&m_msg), (int)zmq_msg_size (&m_msg) ) );
                }
            }else{
                s +=(QString::fromLocal8Bit((char *)zmq_msg_data (&m_msg), (int)zmq_msg_size (&m_msg) ) );
            }

            rc = zmq_getsockopt (m_socket, ZMQ_RCVMORE, &more, &more_size);
            if (rc != 0) {
                printf ("error in zmq_getsockopt : %s\n", zmq_strerror (errno));
                return(-1);
            }
        }
    } while ( more );
    return (0);
}



#endif


