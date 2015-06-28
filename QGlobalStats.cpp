#include "QGlobalStats.h"
#include <QJsonObject>
#include <QJsonArray>
#include <QJsonDocument>
#include <QDebug>
#include <QJsonArray>
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



void CGlobalInfo::get_max(CMaxLatencyCS & latency ){

    latency.m_clients = 0.0;
    latency.m_server  = 0.0;

    int i;
    for (i=0; i<(int)MAX_PORTS; i+=2 ) {
        if ( latency.m_clients <m_ports[i].m_latency.m_c_max ){
            latency.m_clients =m_ports[i].m_latency.m_c_max;
        }
        if ( latency.m_server <m_ports[i+1].m_latency.m_c_max ){
            latency.m_server =m_ports[i+1].m_latency.m_c_max;
        }
    }
}


void CPerPortLatency::clear(){
   m_avg=0.0;
   m_max=0.0;
   m_c_max=0.0;
   m_errors=0;
}

void CPerPortLatency::dump(FILE *fd){
   DPD(m_avg);
   DPD(m_max);
   DPD(m_c_max);
   DPD(m_errors);
}



void CPerPort::clear(){
    m_opackets=0;
    m_obytes=0;
    m_ipackets=0;
    m_ibytes=0;
    m_ierrors=0;
    m_oerrors=0;
    m_tx_bps=0.0;
    m_latency.clear();
}

void CPerPort::dump(FILE *fd){
    DP(m_opackets);
    DP(m_obytes);
    DP(m_ipackets);
    DP(m_ibytes);
    DP(m_ierrors);
    DP(m_oerrors);
    DPD(m_tx_bps);
    m_latency.dump(fd);
}


void CGlobalInfo::clear(){

    int i,j;
    m_cpu_util=0.0;

    for (i=0; i<CGlobalInfo::grNUM; i++) {
        for (j=0; j<CGlobalInfo::gtNUM; j++) {
                    m_global[i][j]=0.0;
        }
    }
   m_rx_bps=0.0;
   m_rx_drop_bps=0;
   m_active_flows=0;
   m_open_flows=0;
   m_total_tx_pkts=0;
   m_total_tx_bytes=0;
   m_total_rx_pkts=0;
   m_total_rx_bytes=0;

   m_num_ports=0;
   for (i=0; i<(int)MAX_PORTS; i++ ) {
       m_ports[i].clear();
   }

   m_rx_info.histInfo.histogram.clear();


}


void CGlobalInfo::dump(FILE *fd){
    fprintf(fd,"----------------\n");
    fprintf(fd," cpu : %f \n",m_cpu_util);
    int i,j;
    for (i=0; i<CGlobalInfo::grNUM; i++) {
        for (j=0; j<CGlobalInfo::gtNUM; j++) {
                if (m_global[i][j] > 0.0) {
                    fprintf(fd," [%d][%d] : %f \n",i,j,m_global[i][j]);
                }
        }
    }

    DP(m_rx_drop_bps);
    DP(m_active_flows);
    DP(m_open_flows);
    DP(m_total_tx_pkts);
    DP(m_total_tx_bytes);
    DP(m_total_rx_pkts);
    DP(m_total_rx_bytes);
    for (i=0; i<(int)m_num_ports; i++ ) {
        fprintf(fd," port : %d \n",i);
        fprintf(fd," ----------\n",i);
        m_ports[i].dump(fd);
    }
}


int CParserGlobal::process(CGlobalInfo & ginfo){

    QJsonParseError err;
    QJsonDocument doc=QJsonDocument::fromJson(m_json.toUtf8(),&err);

    if ( err.error != QJsonParseError::NoError ){
        printf(" error parsing json ");
        return (-1);
    }
    QJsonObject obj=doc.object();

    if ( doc.isEmpty() ){
        return (-1);
    }

    if ( doc.isNull() ){
        return (-1);
    }

    if ( obj["name"]!="trex-global" ){
        return (-1);
    }
    QJsonObject dobj=obj["data"].toObject();
    ginfo.m_cpu_util=dobj["m_cpu_util"].toDouble();\


    ginfo.m_rx_bps =dobj["m_rx_bps"].toDouble();

    ginfo.m_global[CGlobalInfo::grPPS][CGlobalInfo::gtEXP]=dobj["m_tx_expected_pps"].toDouble();
    ginfo.m_global[CGlobalInfo::grCPS][CGlobalInfo::gtEXP]=dobj["m_tx_expected_cps"].toDouble();
    ginfo.m_global[CGlobalInfo::grBPS][CGlobalInfo::gtEXP]=dobj["m_tx_expected_bps"].toDouble();

    ginfo.m_global[CGlobalInfo::grPPS][CGlobalInfo::gtMEASURE]=dobj["m_tx_pps"].toDouble();
    ginfo.m_global[CGlobalInfo::grCPS][CGlobalInfo::gtMEASURE]=dobj["m_tx_cps"].toDouble();
    ginfo.m_global[CGlobalInfo::grBPS][CGlobalInfo::gtMEASURE]=dobj["m_tx_bps"].toDouble();


    ginfo.m_rx_drop_bps    =dobj["m_rx_drop_bps"].toInt();
    ginfo.m_active_flows   =dobj["m_active_flows"].toInt();
    ginfo.m_open_flows     =dobj["m_open_flows"].toInt();
    ginfo.m_total_tx_pkts  =(uint64_t)dobj["m_total_tx_pkts"].toDouble();
    ginfo.m_total_tx_bytes =(uint64_t)dobj["m_total_tx_bytes"].toDouble();
    ginfo.m_total_rx_pkts  =(uint64_t)dobj["m_total_rx_pkts"].toDouble();
    ginfo.m_total_rx_bytes =(uint64_t)dobj["m_total_rx_bytes"].toDouble();

    ginfo.m_num_ports=0;
    int i;
    for (i=0; i<MAX_PORTS; i++) {
        QString s;
        s.sprintf("opackets-%d", i);
        if ( dobj.contains(s) ){
            ginfo.m_num_ports++;
            CPerPort  *lp=&ginfo.m_ports[i];

            s.sprintf("opackets-%d", i);
            lp->m_opackets=(uint64_t)dobj[s].toDouble();

            s.sprintf("obytes-%d", i);
            lp->m_obytes=(uint64_t)dobj[s].toDouble();

            s.sprintf("ipackets-%d", i);
            lp->m_ipackets=(uint64_t)dobj[s].toDouble();

            s.sprintf("ibytes-%d", i);
            lp->m_ibytes=(uint64_t)dobj[s].toDouble();

            s.sprintf("ierrors-%d", i);
            lp->m_ierrors=(uint64_t)dobj[s].toDouble();

            s.sprintf("oerrors-%d", i);
            lp->m_oerrors=(uint64_t)dobj[s].toDouble();

        }else{
            break;
        }
    }

    //"template":[616,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0]
    QJsonArray dtemplate_array = dobj["template"].toArray();

    if ( !dtemplate_array.isEmpty() ){
        int i;
        for (i=0; i<dtemplate_array.size(); i++){
            uint64_t  cnt=(uint64_t)dtemplate_array[i].toDouble();
            if (i<MAX_TEMPLATES_STATS) {
                ginfo.m_rx_info.m_template[i].m_tx_pkts=cnt;
            }
        }
    }
    return (0);
}



int CParserLatency::process(CGlobalInfo & ginfo){
    QJsonParseError err;
    QJsonDocument doc=QJsonDocument::fromJson(m_json.toUtf8(),&err);


    if ( err.error != QJsonParseError::NoError ){
        printf(" error parsing json ");

        return (-1);
    }
    QJsonObject obj=doc.object();

    if ( doc.isEmpty() ){
        return (-1);
    }

    if ( doc.isNull() ){
        return (-1);
    }

    if ( obj["name"]!="trex-latecny" ) {
        return (-1);

    }
    QJsonObject dobj=obj["data"].toObject();


//    QString s2="{\"name\":\"trex-latecny\",\"type\":0,\"data\":{\"avg-0\":-nan,\"max-0\":0.0,\"c-max-0\":0.0,\"error-0\":0.0,\"avg-1\":20.0,\"max-1\":29.0,\"c-max-1\":29.0,\"error-1\":0.0,\"avg-2\":20.0,\"max-2\":25.0,\"c-max-2\":25.0,\"error-2\":0.0,\"avg-3\":20.0,\"max-3\":26.0,\"c-max-3\":26.0,\"error-3\":0.0,\"unknown\":0}}";

    int i;
    for (i=0; i<MAX_PORTS; i++) {
        QString s;
        s.sprintf("avg-%d", i);
        if ( dobj.contains(s) ){
            CPerPortLatency  *lp=&ginfo.m_ports[i].m_latency;

            s.sprintf("avg-%d", i);
            lp->m_avg=dobj[s].toDouble();

            s.sprintf("max-%d", i);
            lp->m_max=dobj[s].toDouble();

            s.sprintf("c-max-%d", i);
            lp->m_c_max=dobj[s].toDouble();

            s.sprintf("error-%d", i);
            lp->m_errors=dobj[s].toString().toULong();


        }else{
            break;
        }
    }


    return (0);
}

/*
int CParserRxCheck::process(CGlobalRxCheckInfo & ginfo){
    QJsonParseError err;
    QJsonDocument doc=QJsonDocument::fromJson(m_json.toUtf8(),&err);

    if ( err.error != QJsonParseError::NoError ){
        printf(" error parsing json ");

        return (-1);
    }
    QJsonObject obj=doc.object();

    if ( doc.isEmpty() ){
        return (-1);
    }

    if ( doc.isNull() ){
        return (-1);
    }

    if ( obj["name"]!="rx-check" ) {
        return (-1);
    }
    QJsonObject dobj=obj["data"].toObject();

    QJsonArray tarray=dobj["template"].toArray();

    if (!tarray.empty()) {
        int i;
        for (i=0; i<tarray.size(); i++){
            QJsonObject t_info=tarray[i].toObject();
            uint32_t id=t_info["id"].toInt();
            uint64_t error= (uint64_t)t_info["val"].toInt();
            uint64_t rx_pkts= (uint64_t)t_info["rx_pkts"].toDouble();
            double jitter=t_info["jitter"].toDouble();
            if (id<MAX_TEMPLATES_STATS) {
                ginfo.m_template[id].m_jitter =jitter;
                ginfo.m_template[id].m_rx_pkts =rx_pkts;
                ginfo.m_template[id].m_errors_pkts = error;
            }
        }
    }
    return (0);
}*/


int CParserRxCheck::process(CGlobalInfo & ginfo){

    QJsonParseError err;
    QJsonDocument doc=QJsonDocument::fromJson(m_json.toUtf8(),&err);

    if ( err.error != QJsonParseError::NoError ){
        printf(" error parsing json ");
        return (-1);
    }

    QJsonObject obj=doc.object();

    if ( doc.isEmpty() ){
        return (-1);
    }

    if ( doc.isNull() ){
        return (-1);
    }

    if ( obj["name"]!="rx-check" ){
        return (-1);
    }

    QJsonObject dobj=obj["data"].toObject();
    QJsonArray tarray=dobj["template"].toArray();
    QJsonObject dobj2=dobj["stats"].toObject();
    QJsonObject lHist =dobj["latency_hist"].toObject();

    ginfo.m_rx_info.m_total_rx_bytes=dobj2["m_total_rx_bytes"].toDouble();
    ginfo.m_rx_info.m_total_rx=dobj2["m_total_rx"].toDouble();

    ginfo.m_rx_info.m_lookup=dobj2["m_lookup"].toDouble();
    ginfo.m_rx_info.m_found=dobj2["m_found"].toDouble();

    ginfo.m_rx_info.m_fif=dobj2["m_fif"].toDouble();
    ginfo.m_rx_info.m_add=dobj2["m_add"].toDouble();
    ginfo.m_rx_info.m_remove=dobj2["m_remove"].toDouble();
    ginfo.m_rx_info.m_active=dobj2["m_active"].toDouble();

    ginfo.m_rx_info.m_err_no_magic=dobj2["m_err_no_magic"].toDouble();
    ginfo.m_rx_info.m_err_drop=dobj2["m_err_drop"].toDouble();

    ginfo.m_rx_info.m_err_aged=dobj2["m_err_aged"].toDouble();
    ginfo.m_rx_info.m_err_no_magic=dobj2["m_err_no_magic"].toDouble();

    ginfo.m_rx_info.m_err_wrong_pkt_id=dobj2["m_err_wrong_pkt_id"].toDouble();
    ginfo.m_rx_info.m_err_fif_seen_twice=dobj2["m_err_fif_seen_twice"].toDouble();

    ginfo.m_rx_info.m_err_open_with_no_fif_pkt=dobj2["m_err_open_with_no_fif_pkt"].toDouble();
    ginfo.m_rx_info.m_err_oo_dup=dobj2["m_err_oo_dup"].toDouble();
    ginfo.m_rx_info.m_err_oo_early=dobj2["m_err_oo_early"].toDouble();

    ginfo.m_rx_info.m_err_oo_late=dobj2["m_err_oo_late"].toDouble();
    ginfo.m_rx_info.m_err_flow_length_changed=dobj2["m_err_flow_length_changed"].toDouble();

    //fill history info data
    ginfo.m_rx_info.histInfo.min_usec = lHist["min_usec"].toDouble();
    ginfo.m_rx_info.histInfo.max_usec = lHist["max_usec"].toDouble();
    ginfo.m_rx_info.histInfo.high_cnt = lHist["high_cnt"].toDouble();
    ginfo.m_rx_info.histInfo.cnt = lHist["cnt"].toDouble();
    ginfo.m_rx_info.histInfo.t_avg = lHist["t_avg"].toDouble();
    ginfo.m_rx_info.histInfo.s_avg = lHist["s_avg"].toDouble();

    QJsonArray histElements = lHist["histogram"].toArray();

    if (!histElements.empty()) {
        foreach (const QJsonValue & value, histElements) {
             QJsonObject obj = value.toObject();
             //qDebug()<<obj["key"].toInt();
            // qDebug()<<obj["val"].toInt();
              ginfo.m_rx_info.histInfo.histogram[obj["key"].toInt()] = obj["val"].toInt();
        }
    }

    if (!tarray.empty()) {
        int i;
        for (i=0; i<tarray.size(); i++){
            QJsonObject t_info=tarray[i].toObject();
            uint32_t id=t_info["id"].toInt();
            uint64_t error= (uint64_t)t_info["val"].toInt();
            uint64_t rx_pkts= (uint64_t)t_info["rx_pkts"].toDouble();
            double jitter=t_info["jitter"].toDouble();
            if (id<MAX_TEMPLATES_STATS) {
                ginfo.m_rx_info.m_template[id].m_jitter =jitter;
                ginfo.m_rx_info.m_template[id].m_rx_pkts =rx_pkts;
                ginfo.m_rx_info.m_template[id].m_errors_pkts = error;
            }
        }
    }

    return (0);
}

void CGlobalRxCheckInfo::clear(){
    int i;
    for (i=0; i<MAX_TEMPLATES_STATS; i++) {
        m_template[i].clear();
    }
}

//{"name":"template_info","type":0,"data":["cap2/dns.pcap"]}
int CParserTemplateInfo::process(CGlobalInfo & ginfo){
    QJsonParseError err;
    QJsonDocument doc=QJsonDocument::fromJson(m_json.toUtf8(),&err);


    if ( err.error != QJsonParseError::NoError ){
        printf(" error parsing json ");

        return (-1);
    }
    QJsonObject obj=doc.object();

    if ( doc.isEmpty() ){
        return (-1);
    }

    if ( doc.isNull() ){
        return (-1);
    }

    if ( obj["name"]!="template_info" ) {
        return (-1);

    }
    QJsonArray dobj=obj["data"].toArray();

    if (!dobj.isEmpty()) {
        int i;
        for (i=0; i<dobj.size(); i++){
            QString s=dobj[i].toString();
            if (i<MAX_TEMPLATES_STATS) {
                ginfo.m_rx_info.m_template[i].m_name=s;
            }
        }
    }
    return(0);
}


int CParserLatencyV2::process(CParserLatencyV2Info & ginfo){
    QJsonParseError err;
    QJsonDocument doc=QJsonDocument::fromJson(m_json.toUtf8(),&err);


    if ( err.error != QJsonParseError::NoError ){
        printf(" error parsing json ");

        return (-1);
    }
    QJsonObject obj=doc.object();

    if ( doc.isEmpty() ){
        return (-1);
    }

    if ( doc.isNull() ){
        return (-1);
    }

    if ( obj["name"]!="trex-latecny-v2" ) {
        return (-1);

    }
    QJsonObject dobj=obj["data"].toObject();
  //  "{"name":"trex-latecny-v2","type":0,"data":{"cpu_util":0.7,"port-0": {"hist":{"min_usec":10,"max_usec":371,"high_cnt":38080,"cnt":38080,"t_avg":59.6,"s_avg":74.3, "histogram": [{"key":20,"val":9529},{"key":30,"val":1492},{"key":40,"val":11382},{"key":50,"val":12560},{"key":60,"val":2552},{"key":70,"val":515},{"key":80,"val":36},{"key":90,"val":7},{"key":100,"val":5},{"key":200,"val":1},{"key":300,"val":1}  ] } ,"stats" : {"m_tx_pkt_ok":38080,"m_pkt_ok":38080,"m_no_magic":0,"m_no_id":0,"m_seq_error":0,"m_length_error":0,"m_rx_check":104108}},"port-1": {"hist":{"min_usec":10,"max_usec":191,"high_cnt":37748,"cnt":37748,"t_avg":51.1,"s_avg":60.9, "histogram": [{"key":20,"val":9272},{"key":30,"val":5712},{"key":40,"val":20950},{"key":50,"val":1760},{"key":60,"val":43},{"key":70,"val":6},{"key":80,"val":1},{"key":100,"val":4}  ] } ,"stats" : {"m_tx_pkt_ok":38080,"m_pkt_ok":37748,"m_no_magic":0,"m_no_id":0,"m_seq_error":1,"m_length_error":0,"m_rx_check":104125}},"port-2": {"hist":{"min_usec":10,"max_usec":466,"high_cnt":38080,"cnt":38080,"t_avg":61.8,"s_avg":77.3, "histogram": [{"key":20,"val":9174},{"key":30,"val":1676},{"key":40,"val":7349},{"key":50,"val":16139},{"key":60,"val":2999},{"key":70,"val":669},{"key":80,"val":59},{"key":90,"val":8},{"key":100,"val":4},{"key":200,"val":2},{"key":400,"val":1}  ] } ,"stats" : {"m_tx_pkt_ok":38080,"m_pkt_ok":38080,"m_no_magic":0,"m_no_id":0,"m_seq_error":0,"m_length_error":0,"m_rx_check":100153}},"port-3": {"hist":{"min_usec":10,"max_usec":206,"high_cnt":38080,"cnt":38080,"t_avg":51.0,"s_avg":60.5, "histogram": [{"key":20,"val":9286},{"key":30,"val":6105},{"key":40,"val":20991},{"key":50,"val":1648},{"key":60,"val":43},{"key":80,"val":2},{"key":90,"val":1},{"key":100,"val":3},{"key":200,"val":1}  ] } ,"stats" : {"m_tx_pkt_ok":38080,"m_pkt_ok":38080,"m_no_magic":0,"m_no_id":0,"m_seq_error":0,"m_length_error":0,"m_rx_check":92655}},"unknown":0}}

    QJsonObject dobj2=dobj["port-0"].toObject();
    QJsonObject lHist =dobj2["hist"].toObject();

    //fill history info data
    ginfo.histInfo.min_usec = lHist["min_usec"].toDouble();
    ginfo.histInfo.max_usec = lHist["max_usec"].toDouble();
    ginfo.histInfo.high_cnt = lHist["high_cnt"].toDouble();
    ginfo.histInfo.cnt = lHist["cnt"].toDouble();
    ginfo.histInfo.t_avg = lHist["t_avg"].toDouble();
    ginfo.histInfo.s_avg = lHist["s_avg"].toDouble();

    QJsonArray histElements = lHist["histogram"].toArray();

    foreach (const QJsonValue & value, histElements) {
       QJsonObject obj = value.toObject();
       //qDebug()<<obj["key"].toInt();
      // qDebug()<<obj["val"].toInt();
       ginfo.histInfo.histogram[obj["key"].toInt()] = obj["val"].toInt();
    }

    return (0);
}


#if 0
int test1(){
    //printf(" %s ",s1.toUtf8().data());
    CParserGlobal p(s1);
    CParserLatency l(s2);
    CGlobalInfo  ginfo;
    p.process(ginfo);
    l.process(ginfo);

    ginfo.dump(stdout);

    return (0);
}

#endif


