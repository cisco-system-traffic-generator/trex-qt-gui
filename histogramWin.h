#ifndef HISTOGRAMWIN_H
#define HISTOGRAMWIN_H

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

#include <QMainWindow>
#include <stdint.h>
#include "qcustomplot.h"
#include "QGlobalStats.h"



class QCustomPlot;

namespace Ui {
class HistogramWin;
}

class HistogramWin : public QMainWindow
{
    Q_OBJECT

public:
    explicit HistogramWin(QWidget *parent = 0);
    ~HistogramWin();

private:
    void init(QCustomPlot * cp);

private slots:
    void update_new_data(CGlobalRxCheckInfo *global);
    void update_new_data(CParserLatencyV2Info *global);
    void update_new_data(CGlobalInfo *global);
    void update_new_data(HistInfo *histInfo);
    void changeGraphScaleType(bool isChecked);
public slots:
    void reset();

private:
    Ui::HistogramWin *ui;
    QCheckBox *cb;
    //QCPBars *myBars;
    uint32_t        m_cnt;
};

#endif // HISTOGRAMWIN_H
