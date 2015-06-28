#ifndef LATENCYWIN_H
#define LATENCYWIN_H

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


class QCustomPlot;

namespace Ui {
class LatencyWin;
}

class LatencyWin : public QMainWindow
{
    Q_OBJECT

public:
    explicit LatencyWin(QWidget *parent = 0);
    ~LatencyWin();

private:
    void init(QCustomPlot * cp);

private slots:
    void update_new_data(double client,double server);

public slots:
    void reset();

private:
    Ui::LatencyWin *ui;
    uint32_t        m_cnt;
};

#endif // LATENCYWIN_H
