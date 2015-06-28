#include <limits>
#include <qdatetime.h>
#include "mustache.h"
#include "child_global_win.h"
#include "ui_child_global_win.h"
#include <QDebug>
#include <QFile>
#include <QTextStream>
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



GlobalChildMainWindow::GlobalChildMainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::GlobalChildMainWindow)
{
    ui->setupUi(this);

    m_thread = new CZmqThread();

    //m_thread->set_host("tcp://localhost:5556");
    //m_thread->init_socket();
    //m_thread->start();

    QObject::connect(m_thread, SIGNAL(have_data(QString)), this, SLOT(update_new_json(QString)));
    m_stats.clear();
   // rx_stats.clear();

    // read template
    QFile file(":/resource/b.html");
    if (!file.open(QIODevice::ReadOnly | QIODevice::Text)){
      qDebug()<<"error";
      return;
    }

    QTextStream in(&file);
    m_global_html_tp = in.readAll();
    file.close();
    ui->pbCpu->setValue(0.0);
}

GlobalChildMainWindow::~GlobalChildMainWindow()
{
    m_thread->close_socket_and_wait();

    delete m_thread;
    delete ui;
}



void GlobalChildMainWindow::reset(){
    m_stats.clear();
    l2_stats.histInfo.histogram.clear();
    ui->webView->setHtml("");
    ui->pbCpu->setValue(0.0);
}


void GlobalChildMainWindow::update_new_json(QString s){

    //qDebug()<<s;
    //qDebug()<<s;
    bool update=false;
  //  qDebug()<<s;
    CParserGlobal g(s);
    if ( g.process(m_stats) == 0){
        emit update_port_counters(&m_stats);
        update=true;
    }


    CParserLatency l(s);

    if ( l.process(m_stats) == 0 ){
        CMaxLatencyCS  max_latency;
        m_stats.get_max(max_latency);
        emit latency_changed(max_latency.m_clients,max_latency.m_server);
    }

    CParserTemplateInfo t(s);
    if ( t.process(m_stats) == 0 ){
    }

     CParserRxCheck r(s);

     if ( r.process(m_stats) == 0 ){
          emit update_rx_template(&m_stats);
          emit update_rx_counters(&m_stats);
     }


     CParserLatencyV2 x(s);
     if ( x.process(l2_stats) == 0 ){
         emit update_l2_counters(&l2_stats);
     }

    if ( update ){
        // update the ui
        ui->pbCpu->setValue(m_stats.m_cpu_util);
        update_html();
    }
 
}

typedef enum {
    KBYE_1024,
    KBYE_1000
} human_kbyte_t;

static QChar human_tbl[]={
    ' ',
    'K',
    'M',
    'G',
    'T'
};

QString  double_to_human_str(double        num,
                             QString       units,
                             human_kbyte_t etype){
    double abs_num=num;
    if (num<0.0) {
        abs_num=-num;
    }
    int i=0;
    int max_cnt=sizeof(human_tbl)/sizeof(human_tbl[0]);
    double div =1.0;
    double f=1000.0;
    if (etype ==KBYE_1024){
        f=1024.0;
    }
    while ((abs_num > f ) && (i< max_cnt)){
        abs_num/=f;
        div*=f;
        i++;
    }

    QString res;
    res.sprintf("%10.2f %c%s",num/div,human_tbl[i],units.toUtf8().data());
    return (res);
}




void GlobalChildMainWindow::update_html(){

    m_global_vals["mbps_color"]= "<FONT COLOR=green> Total Tx :"+double_to_human_str(m_stats.m_global[CGlobalInfo::grBPS][CGlobalInfo::gtMEASURE],"bps",KBYE_1000)+"</FONT>";
    m_global_vals["mpps_color"]="<FONT COLOR=blue> "+double_to_human_str(m_stats.m_global[CGlobalInfo::grPPS][CGlobalInfo::gtMEASURE],"pps",KBYE_1000)+"</FONT>";
    m_global_vals["ebps"]= double_to_human_str(m_stats.m_global[CGlobalInfo::grBPS][CGlobalInfo::gtEXP],"bps",KBYE_1000);
    m_global_vals["mbps"]= double_to_human_str(m_stats.m_global[CGlobalInfo::grBPS][CGlobalInfo::gtMEASURE],"bps",KBYE_1000);
    double r=100.0*m_stats.m_global[CGlobalInfo::grBPS][CGlobalInfo::gtMEASURE]/m_stats.m_global[CGlobalInfo::grBPS][CGlobalInfo::gtEXP];
    QString sr;
    sr.sprintf("%3.0f ",r);
    m_global_vals["pbps"]= sr;

    m_global_vals["ecps"]= double_to_human_str(m_stats.m_global[CGlobalInfo::grCPS][CGlobalInfo::gtEXP],"cps",KBYE_1000);
    m_global_vals["mcps"]= double_to_human_str(m_stats.m_global[CGlobalInfo::grCPS][CGlobalInfo::gtMEASURE],"cps",KBYE_1000);
    r=100.0*m_stats.m_global[CGlobalInfo::grCPS][CGlobalInfo::gtMEASURE]/m_stats.m_global[CGlobalInfo::grCPS][CGlobalInfo::gtEXP];
    sr.sprintf("%3.0f ",r);
    m_global_vals["pcps"]= sr;

    m_global_vals["epps"]= double_to_human_str(m_stats.m_global[CGlobalInfo::grPPS][CGlobalInfo::gtEXP],"pps",KBYE_1000);
    m_global_vals["mpps"]= double_to_human_str(m_stats.m_global[CGlobalInfo::grPPS][CGlobalInfo::gtMEASURE],"pps",KBYE_1000);
    r=100.0*m_stats.m_global[CGlobalInfo::grPPS][CGlobalInfo::gtMEASURE]/m_stats.m_global[CGlobalInfo::grPPS][CGlobalInfo::gtEXP];
    sr.sprintf("%3.0f ",r);
    m_global_vals["ppps"]= sr;


    m_global_vals["active_flows"]=m_stats.m_active_flows;
    m_global_vals["active_flows_h"]=double_to_human_str(m_stats.m_active_flows,"",KBYE_1000);
    m_global_vals["open_flows"]=m_stats.m_open_flows;

    if (m_stats.m_rx_drop_bps>1.0) {

        m_global_vals["total_drop"]="<FONT COLOR=red>"+double_to_human_str(m_stats.m_rx_drop_bps,"bps",KBYE_1000)+"</FONT>";
    }else{
        m_global_vals["total_drop"]=double_to_human_str(m_stats.m_rx_drop_bps,"bps",KBYE_1000);
        //m_global_vals["total_drop"]=double_to_human_str(m_stats.m_rx_drop_bps,"bps",KBYE_1000);
    }


    Mustache::Renderer renderer;
    Mustache::QtVariantContext context(m_global_vals);

    //qDebug()<<m_global_html_tp;
    QString s=renderer.render(m_global_html_tp, &context);

    ui->webView->setHtml(s);
}
