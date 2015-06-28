#include "rx_check_win.h"
#include "ui_rx_check_win.h"
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


         

RxCheckWin::RxCheckWin(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::RxCheckWin)
{
    ui->setupUi(this);
    setGeometry(500, 400, 542, 390);
    setWindowTitle("Rx counters");

    // read template
    QFile file(":/resource/rx_check.html");
    if (!file.open(QIODevice::ReadOnly | QIODevice::Text)){
      qDebug()<<"error loading /resource/rx_check.html ";
      return;
    }


    QTextStream in(&file);
    m_rx_check_html_tp = in.readAll();
    file.close();
}

void RxCheckWin::reset(){
    ui->webView->setHtml("");
}


void RxCheckWin::update_html( CGlobalRxCheckInfo *info){
    QString s=m_rx_check_html_tp;

    s+="<table class=\"reference\" >\n";
    s+="<tr>\n";   //build table titles
    s+="<th><div style=\"width: 100px\">total_rx_bytes</div></th>\n";
    s+="<th><div style=\"width: 100px\">total_rx</div></th>\n";
    s+="<th><div style=\"width: 100px\">m_lookup</div></th>\n";
    s+="<th><div style=\"width: 100px\">m_found</div></th>\n";
    s+="<th><div style=\"width: 100px\">m_fif</div></th>\n";
    s+="</tr>\n";

    s+="<tr>\n";

    QString t;
    t.sprintf("<td>%lu</td>\n",info->m_total_rx_bytes);
    s+=t;

    t.sprintf("<td>%lu</td>\n",info->m_total_rx);
    s+=t;

    t.sprintf("<td>%lu</td>\n",info->m_lookup);
    s+=t;

    t.sprintf("<td>%lu</td>\n",info->m_found);
    s+=t;

    t.sprintf("<td>%lu</td>\n",info->m_fif);
    s+=t;
    s+="</tr>\n";
    s+="</table>\n";
    s+="<br></br>\n";
    //================================1

    s+="<table class=\"reference\" >\n";
    s+="<tr>\n";   //build table titles
    s+="<th><div style=\"width: 100px\">add</div></th>\n";
    s+="<th><div style=\"width: 100px\">remove</div></th>\n";
    s+="<th><div style=\"width: 100px\">active</div></th>\n";
    s+="<th><div style=\"width: 100px\">err_drop</div></th>\n";
    s+="<th><div style=\"width: 100px\">err_aged</div></th>\n";
    s+="</tr>\n";

    s+="<tr>\n";
    t.sprintf("<td>%lu</td>\n",info->m_add);
    s+=t;

    t.sprintf("<td>%lu</td>\n",info->m_remove);
    s+=t;

    t.sprintf("<td>%lu</td>\n",info->m_active);
    s+=t;

    t.sprintf("<td>%lu</td>\n",info->m_err_drop);
    s+=t;

    t.sprintf("<td>%lu</td>\n",info->m_err_aged);
    s+=t;
    s+="</tr>\n";
    s+="</table>\n";
    s+="<br></br>\n";
    //=========================2

    s+="<table class=\"reference\" >\n";
    s+="<tr>\n";   //build table titles
    s+="<th><div style=\"width: 100px\">err_no_magic</div></th>\n";
    s+="<th><div style=\"width: 100px\">err_wrong_pkt_id</div></th>\n";
    s+="<th><div style=\"width: 200px\">err_fif_seen_twice</div></th>\n";
    s+="<th><div style=\"width: 100px\">err_oo_dup</div></th>\n";
    s+="</tr>\n";
    s+="<tr>\n";

    t.sprintf("<td>%lu</td>\n",info->m_err_no_magic);
    s+=t;

    t.sprintf("<td>%lu</td>\n",info->m_err_wrong_pkt_id);
    s+=t;

    t.sprintf("<td>%lu</td>\n",info->m_err_fif_seen_twice);
    s+=t;

    t.sprintf("<td>%lu</td>\n",info->m_err_oo_dup);
    s+=t;
    s+="</tr>\n";
    s+="</table>\n";
    s+="<br></br>\n";
    //================

    s+="<table class=\"reference\" >\n";
    s+="<tr>\n";   //build table titles
    s+="<th><div style=\"width: 100px\">err_oo_early</div></th>\n";
    s+="<th><div style=\"width: 100px\">err_oo_late</div></th>\n";
    s+="<th><div style=\"width: 150px\">err_flow_length_changed</div></th>\n";
    s+="<th><div style=\"width: 200px\">err_open_no_FifPkt</div></th>\n";
    s+="</tr>\n";

    s+="<tr>\n";

    t.sprintf("<td>%lu</td>\n",info->m_err_oo_early);
    s+=t;

    t.sprintf("<td>%lu</td>\n",info->m_err_oo_late);
    s+=t;

    t.sprintf("<td>%lu</td>\n",info->m_err_flow_length_changed);
    s+=t;

    t.sprintf("<td>%lu</td>\n",info->m_err_open_with_no_fif_pkt);
    s+=t;

    s+="</tr>\n";
    s+="</table>\n";
    s+="</body></html>\n";

    ui->webView->setHtml(s);
}


RxCheckWin::~RxCheckWin()
{
    delete ui;
}
