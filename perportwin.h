#ifndef PERPORTWIN_H
#define PERPORTWIN_H
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
#include <QString>
#include <QVariantHash>

class CGlobalInfo;

namespace Ui {
class PerPortWin;
}

class PerPortWin : public QMainWindow
{
    Q_OBJECT

public:
    explicit PerPortWin(QWidget *parent = 0);
    ~PerPortWin();

private:
    QString add_counter(CGlobalInfo *info,int id);

private slots:
    void update_html( CGlobalInfo *info);
    void update_rx_check_template_html(CGlobalInfo *info);

public  slots:
    void reset();

private:
    Ui::PerPortWin *ui;
    QString         m_global_html_tp;

};

#endif // PERPORTWIN_H
