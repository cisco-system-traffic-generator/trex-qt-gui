#ifndef RX_CHECK_WIN_H
#define RX_CHECK_WIN_H

#include <QMainWindow>
#include <QString>
#include <QVariantHash>

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




class CGlobalRxCheckInfo;

namespace Ui {
class RxCheckWin;
}

class RxCheckWin : public QMainWindow
{
    Q_OBJECT

public:
    explicit RxCheckWin(QWidget *parent = 0);
    ~RxCheckWin();

private:

private slots:
    void update_html( CGlobalRxCheckInfo *info);

public  slots:
    void reset();

private:
    Ui::RxCheckWin *ui;
    QString    m_rx_check_html_tp;
};

#endif // RX_CHECK_WIN_H
