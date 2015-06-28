#include "perportwin.h"
#include "ui_perportwin.h"
#include <QDebug>
#include "QGlobalStats.h"
#include <QFile>
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



PerPortWin::PerPortWin(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::PerPortWin)
{
    ui->setupUi(this);

    setGeometry(300, 250, 542, 390);
    setWindowTitle("Per Port Counters");

    // read template
    QFile file(":/resource/per_port.html");
    if (!file.open(QIODevice::ReadOnly | QIODevice::Text)){
      qDebug()<<"error loading /resource/per_port.html ";
      return;
    }


    QTextStream in(&file);
    m_global_html_tp = in.readAll();
    file.close();

}

QString PerPortWin::add_counter(CGlobalInfo *info,int id){
    int i;
    QString s="";
    int num_ports=(int)info->m_num_ports;


    s+="    <tr>\n";

    for (i=0; i<num_ports; i++) {
        uint64_t cnt=0;
        CPerPort *lp=&info->m_ports[i];
        switch (id) {
        case 0:
            if (i==0) {
                s+="<td>opackets</td>\n";
            }
            cnt=lp->m_opackets;
            break;
        case 1:
            if (i==0) {
                s+="<td>obytes</td>\n";
            }
            cnt=lp->m_obytes;
            break;
        case 2:
            if (i==0) {
                s+="<td>ipackets</td>\n";
            }
            cnt=lp->m_ipackets;
            break;
        case 3:
            if (i==0) {
                s+="<td>ibytes</td>\n";
            }
            cnt=lp->m_ibytes;
            break;
        case 4:
            if (i==0) {
                s+="<td>ierrors</td>\n";
            }
            cnt=lp->m_ierrors;
            break;
        case 5:
            if (i==0) {
                s+="<td>oerrors</td>\n";
            }
            cnt=lp->m_oerrors;
            break;
        default:
            break;
        };

        QString t;
        t.sprintf("<td>%lu</td>\n",cnt);
        s+=t;


    }// for 
    s+="    </tr>\n";
    return (s);
 }

void PerPortWin::reset(){
    ui->webView->setHtml("");
}



void PerPortWin::update_rx_check_template_html(CGlobalInfo *info){
    QString s=m_global_html_tp;
    s+="<table class=\"reference\" >\n";
    s+="<tr>\n";
    QString t;

    t.sprintf("<th><div style=\"width: 100px\">Name</div></th>"); s+=t;
    t.sprintf("<th><div style=\"width: 100px\">Tx Pkts</div></th>"); s+=t;
    t.sprintf("<th><div style=\"width: 100px\">Rx Pkts</div></th>"); s+=t;
    t.sprintf("<th><div style=\"width: 50px\">Rx Errors</div></th>"); s+=t;
    t.sprintf("<th><div style=\"width: 50px\">Jitter</div></th>"); s+=t;
    s+="</tr>\n";
    int i;
    for (i=0; i<MAX_TEMPLATES_STATS;i++){
        if ( info->m_rx_info.m_template[i].m_name.length() >0  ) {
            CPerTemplateInfo *l=&info->m_rx_info.m_template[i];
            //qDebug()<<l->m_tx_pkts<<l->m_rx_pkts<<l->m_errors_pkts<<l->m_jitter;
            bool has_error=false;
            if (l->m_tx_pkts>0) {
                if  (  ( l->m_errors_pkts > 100 ) ||
                    (( ( (double)l->m_rx_pkts/(double)l->m_tx_pkts )<0.7) && (l->m_tx_pkts>300) ) 
                    ) {
                    has_error=true;
                }
            }
            s+="<tr>\n";
            if ( !has_error ) {
                t.sprintf("<td>%s</td><td>%llu</td><td>%llu</td><td>%llu</td><td>%.0f</td> ", l->m_name.toUtf8().data(),l->m_tx_pkts,l->m_rx_pkts,l->m_errors_pkts,l->m_jitter); 
            }else{
                t.sprintf("<td bgcolor=\"#FF330000\" >%s</td><td>%llu</td><td>%llu</td><td>%llu</td><td>%.0f</td> ", l->m_name.toUtf8().data(),l->m_tx_pkts,l->m_rx_pkts,l->m_errors_pkts,l->m_jitter);
            }
            s+=t;
            s+="</tr>\n";

        }
    }
    s+="</table></body></html>\n";
    ui->webView->setHtml(s);
}

void PerPortWin::update_html( CGlobalInfo *info){
    QString s=m_global_html_tp;
    uint8_t num_port=info->m_num_ports;
    int i;
    s+="<table class=\"reference\" >\n";
    s+="<tr>\n";

    QString t;
    t.sprintf("<th><div style=\"width: 80px\">Counter</div></th>");
    s+=t;
    for (i=0; i<(int)num_port; i++) {
        QString t;
        t.sprintf("<th><div style=\"width: 100px\">If-%d</div></th>",i);
        s+=t;
    }
    s+="</tr>\n";
    // number of counters
    for (i=0; i<(int)6; i++) {
        s+=add_counter(info,i);
    }
    s+="</table></body></html>\n";
    ui->webView->setHtml(s);
}


PerPortWin::~PerPortWin()
{
    delete ui;
}

