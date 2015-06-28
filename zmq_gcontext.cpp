#include "zmq_gcontext.h"
#include <zmq.h>
#include <zmq_utils.h>
#include <stdio.h>
#include <QDebug>

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



static void *g_ctx=0;
                        
int  zmq_gcontext_init(void){
    if (g_ctx) {
        return (0);
    }
    g_ctx = zmq_init (1);
    if (!g_ctx) {
        printf ("error in zmq_init: %s\n", zmq_strerror (errno));
        return -1;
    }
    return (0);
}

void *  zmq_gcontext_get(void){
    if (g_ctx) {
        return (g_ctx);
    }
    zmq_gcontext_init();
    return(g_ctx);
}

void zmq_gcontext_term(){
    if ( g_ctx ){
        zmq_term (g_ctx);
        g_ctx=0;
    }
}




