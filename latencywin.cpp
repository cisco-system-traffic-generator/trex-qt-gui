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

#include "latencywin.h"
#include "ui_latencywin.h"
#include "qcustomplot.h"
#include <QDebug>

void LatencyWin::init(QCustomPlot * cp){

    cp->replot();

    cp->addGraph(); // blue line
    cp->graph(0)->setPen(QPen(Qt::blue));
    //ui->customPlot->graph(0)->setBrush(QBrush(QColor(240, 255, 200)));
    cp->graph(0)->setAntialiasedFill(false);
    cp->addGraph(); // red line
    cp->graph(1)->setPen(QPen(Qt::red));
    cp->graph(1)->setAntialiasedFill(false);

    cp->graph(0)->setChannelFillGraph(cp->graph(1));

    cp->graph(0)->setName("Client");
    cp->graph(1)->setName("Server");

    cp->xAxis->setTickLabelType(QCPAxis::ltDateTime);
    cp->xAxis->setDateTimeFormat("ss");
    cp->xAxis->setAutoTickStep(false);
    cp->xAxis->setTickStep(2);
    cp->axisRect()->setupFullAxesBox();
    cp->legend->setVisible(true); 

    // make left and bottom axes transfer their ranges to right and top axes:
    connect(cp->xAxis, SIGNAL(rangeChanged(QCPRange)), cp->xAxis2, SLOT(setRange(QCPRange)));
    connect(cp->yAxis, SIGNAL(rangeChanged(QCPRange)), cp->yAxis2, SLOT(setRange(QCPRange)));
    m_cnt=0;
}

LatencyWin::LatencyWin(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::LatencyWin)
{
    ui->setupUi(this);
    setGeometry(400, 250, 542, 390);
    setWindowTitle("Latency");
    statusBar()->clearMessage();
    init(ui->customPlot);
}

LatencyWin::~LatencyWin()
{
    delete ui;
}



void LatencyWin::reset(){
    QCustomPlot * cp=ui->customPlot;
    cp->graph(0)->clearData();
    cp->graph(1)->clearData();
    cp->replot();
}


void LatencyWin::update_new_data(double client_cmax,double server_cmax){


    QCustomPlot * cp=ui->customPlot;
    // calculate two new data points:
    double key = QDateTime::currentDateTime().toMSecsSinceEpoch()/1000.0;
    static double lastPointKey = 0;
    if (key-lastPointKey > 0.01) // at most add point every 10 ms
    {
      // add data to lines:
      cp->graph(0)->addData(key, client_cmax);
      cp->graph(1)->addData(key, server_cmax);
      // set data of dots:
      cp->graph(0)->removeDataBefore(key-16);
      cp->graph(1)->removeDataBefore(key-16);
      // rescale value (vertical) axis to fit the current data:
      m_cnt++;
      if ((m_cnt % 8)==0){
          cp->graph(0)->rescaleValueAxis();
          cp->graph(1)->rescaleValueAxis(true);
      }
      lastPointKey = key;
    }
    // make key axis range scroll with the data (at a constant range size of 8):
    cp->xAxis->setRange(key+0.25, 16, Qt::AlignRight);
    cp->replot();
}
