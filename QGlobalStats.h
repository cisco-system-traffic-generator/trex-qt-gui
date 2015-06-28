#ifndef Q_GLOBAL_STATS
#define Q_GLOBAL_STATS
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


#include <stdio.h>
#include <stdint.h>
#include <QString>
#include <QHash>
#include <assert.h>



#define DP(a) if (a) {     fprintf(fd," %-10s  : %lu \n",#a,a); }
#define DPD(a) if (a>1.0) {     fprintf(fd," %-10s  : %f \n",#a,a); }
#define DPN(a) fprintf(fd," %-10s  : %lu \n",#a,a);
#define MAX_PORTS 8
#define MAX_TEMPLATES_STATS 32

class CPerPortLatency {
public:
    double    m_avg;
    double    m_max;
    double    m_c_max;
    uint64_t  m_errors;
public:
    void dump(FILE *fd);
    void clear();
};

class CPerPort {
public:
    uint64_t  m_opackets;
    uint64_t  m_obytes;
    uint64_t  m_ipackets;
    uint64_t  m_ibytes;
    uint64_t  m_ierrors;
    uint64_t  m_oerrors;
    double    m_tx_bps;
    CPerPortLatency m_latency;

public:
    void dump(FILE *fd);
    void clear();
};

class CMaxLatencyCS {
public:
    double m_clients;
    double m_server;
};

class CPerTemplateInfo {
public:
    CPerTemplateInfo(){
        m_name="";
    }
    QString    m_name;
    uint64_t   m_tx_pkts;
    uint64_t   m_rx_pkts;
    uint64_t   m_errors_pkts;
    double     m_jitter;
public:
    void clear(){
       m_tx_pkts=0;
       m_rx_pkts=0;
       m_errors_pkts=0;
       m_jitter=0.0;
    }
};

class HistInfo {
public:
    uint64_t   min_usec;
    uint64_t   max_usec;
    uint64_t   high_cnt;
    uint64_t   cnt;
    double     t_avg;
    double     s_avg;
    QHash<int,int> histogram;
};

class CGlobalRxCheckInfo {
public:
    uint64_t   m_total_rx_bytes;
    uint64_t   m_total_rx;
    uint64_t   m_lookup;
    uint64_t   m_found;
    uint64_t   m_fif;
    uint64_t   m_add;
    uint64_t   m_remove;

    uint64_t   m_active;
    uint64_t   m_err_no_magic;
    uint64_t   m_err_drop;
    uint64_t   m_err_aged;

    uint64_t   m_err_wrong_pkt_id;
    uint64_t   m_err_fif_seen_twice;
    uint64_t   m_err_open_with_no_fif_pkt;
    uint64_t   m_err_oo_dup;
    uint64_t   m_err_oo_early;

    uint64_t   m_err_oo_late;
    uint64_t   m_err_flow_length_changed;

    HistInfo  histInfo;
    CPerTemplateInfo m_template[MAX_TEMPLATES_STATS];

public:
    void clear();
};



class CGlobalInfo {
public:
    enum {
        grPPS=0,
        grCPS=1,
        grBPS=2,
        grNUM=3

    };
    enum {
        gtEXP=0,
        gtMEASURE=1,
        gtNUM=2
    };

    double     m_cpu_util;
    double     m_global[grNUM][gtNUM];
    double     m_rx_bps;

    uint64_t   m_rx_drop_bps;
    uint64_t   m_active_flows;
    uint64_t   m_open_flows;
    uint64_t   m_total_tx_pkts;
    uint64_t   m_total_tx_bytes;
    uint64_t   m_total_rx_pkts;
    uint64_t   m_total_rx_bytes;

    uint8_t    m_num_ports; /* tell how many ports are valid */
    CPerPort   m_ports[MAX_PORTS];
    CGlobalRxCheckInfo m_rx_info;

public:
    void dump(FILE *fd);
    void clear();
    void get_max(CMaxLatencyCS & latency );

};



class CParserLatencyV2Info {
public:
    double cpu_util;
    HistInfo  histInfo;
public:
    void clear();
};

class CParserGlobal{
public:
    CParserGlobal(QString s){
        m_json=s;
    }

public:
    int process(CGlobalInfo & ginfo);
private:
  QString m_json;

};

class CParserRxCheck{
public:
    CParserRxCheck(QString s){
        m_json=s;
    }

public:
    int process(CGlobalInfo & ginfo);
private:
  QString m_json;

};

class CParserLatency {
public:
    CParserLatency(QString s){
        m_json=s;
    }

public:
    int process(CGlobalInfo & ginfo);
private:
  QString m_json;

};



class CParserTemplateInfo {
public:
    CParserTemplateInfo(QString s){
        m_json=s;
    }

public:
    int process(CGlobalInfo & ginfo);
private:
  QString m_json;

};


class CParserLatencyV2 {
public:
    CParserLatencyV2(QString s){
        m_json=s;
    }

public:
    int process(CParserLatencyV2Info & ginfo);
private:
  QString m_json;

};


#endif
