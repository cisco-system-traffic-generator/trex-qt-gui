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


#include "histogramWin.h"
#include "ui_histogramwin.h"
#include "qcustomplot.h"
#include "QGlobalStats.h"
#include <QDebug>

void HistogramWin::init(QCustomPlot * cp){
    cb = new QCheckBox(this);
    cb->setText("linear");
    connect(cb, SIGNAL(clicked(bool)), this, SLOT(changeGraphScaleType(bool)));


    cp->replot();
    cp->setNoAntialiasingOnDrag(true); // more performance/responsiveness during dragging
    cp->addGraph();
    cp->setGeometry(0, 0, 500, 350);
    cp->graph(0)->setPen(QPen(Qt::blue));
    cp->graph(0)->setBrush(QBrush(QColor(0, 0, 255, 20)));
    cp->graph(0)->setName("latency history");

    QVector<double> keyData;
    QVector<double> valueData;

    cp->graph(0)->setData(keyData, valueData);

    cp->yAxis->grid()->setSubGridVisible(true);
    cp->xAxis->grid()->setSubGridVisible(true);
    cp->yAxis->setScaleType(QCPAxis::stLogarithmic);
    cp->yAxis->setScaleLogBase(100);
    cp->yAxis->setNumberFormat("eb"); // e = exponential, b = beautiful decimal powers
    cp->yAxis->setNumberPrecision(0); // makes sure "1*10^4" is displayed only as "10^4"
    cp->yAxis->setSubTickCount(10);
    cp->xAxis->setRange(0, 19.9);
    cp->yAxis->setRange(1e-2, 1e10);

    // make range draggable and zoomable:
    cp->setInteractions(QCP::iRangeDrag | QCP::iRangeZoom);

    // make top right axes clones of bottom left axes:
    cp->axisRect()->setupFullAxesBox();
    // connect signals so top and right axes move in sync with bottom and left axes:
    connect(cp->xAxis, SIGNAL(rangeChanged(QCPRange)), cp->xAxis2, SLOT(setRange(QCPRange)));
    connect(cp->yAxis, SIGNAL(rangeChanged(QCPRange)), cp->yAxis2, SLOT(setRange(QCPRange)));

    cp->legend->setVisible(true);
    cp->legend->setBrush(QBrush(QColor(255,255,255,150)));
    cp->axisRect()->insetLayout()->setInsetAlignment(0, Qt::AlignLeft|Qt::AlignTop); // make legend align in top left corner or axis rect


//    myBars = new QCPBars(cp->xAxis, cp->yAxis);
//    cp->addPlottable(myBars);
//    // now we can modify properties of myBars:
//    myBars->setName("Bars Series 1");

}

HistogramWin::HistogramWin(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::HistogramWin)
{
    ui->setupUi(this);
    setGeometry(0, 0, 250, 100);
    setWindowTitle("Histogram view");
    statusBar()->clearMessage();
    init(ui->customPlot);
}

HistogramWin::~HistogramWin()
{
    delete ui;
}



void HistogramWin::reset(){
    QCustomPlot * cp=ui->customPlot;
    cp->graph(0)->clearData();
    cp->replot();
}

void HistogramWin::update_new_data(CGlobalRxCheckInfo *global){
    HistogramWin::update_new_data(&(global->histInfo));
}

void HistogramWin::update_new_data(CParserLatencyV2Info *global){
    HistogramWin::update_new_data(&(global->histInfo));
}

void HistogramWin::update_new_data(CGlobalInfo *global){
    HistogramWin::update_new_data(&(global->m_rx_info.histInfo));
}

void HistogramWin::changeGraphScaleType(bool isChecked) {
  QCustomPlot * cp=ui->customPlot;

  if (isChecked) {
      cp->yAxis->setScaleType(QCPAxis::stLinear);
  } else {
       cp->yAxis->setScaleType(QCPAxis::stLogarithmic);
  }

}

void HistogramWin::update_new_data(HistInfo *histInfo) {
    QCustomPlot * cp =ui->customPlot;
    QList<int> keys = (*histInfo).histogram.keys();

    QVector<double> keyData;
    QVector<double> valueData;

    for (int i = 0; i < keys.size(); ++i) {
      keyData << keys.at(i);
      valueData << (*histInfo).histogram[keys.at(i)];
    }

    cp->graph(0)->setData(keyData, valueData);

    cp->rescaleAxes();
    cp->replot();
}


