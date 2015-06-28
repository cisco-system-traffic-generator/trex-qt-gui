#ifndef MAINWINDOW_H
#define MAINWINDOW_H
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
#include <QMdiArea>



namespace Ui {
class MainWindow;
}

class GlobalChildMainWindow;
class HistogramWin;
class RxCheckWin;
class LatencyWin;
class PerPortWin;
class QAction;
class QMenu;
class QToolBar;
class QComboBox;
class QLabel;
class LEDWidget;
class QSplashScreen;
class QSettings;
class QCloseEvent;


class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = 0);
    ~MainWindow();
    void update_size();



private slots:
    void ShowAll();
    void about();
    void update_new_json(QString s);
    void connect_disconnect();
    void connect_disconnect(const QString &s);


private:
    Ui::MainWindow *ui;
    QMdiArea *      m_mdiArea;
    GlobalChildMainWindow * m_child_Window;
    LatencyWin            * m_latency;
    PerPortWin           * m_per_port;
    PerPortWin           * m_rx_template;
    RxCheckWin          * m_rx_check_Window;
    HistogramWin            * m_histogram;
    HistogramWin            * l2_histogram;
    QDockWidget              *rxDock;
    QDockWidget              *lv2Dock;
    QDockWidget              *lDock;

    QSplashScreen   *     m_about;

private:
    void closeEvent(QCloseEvent *event);

    void update_status();
    void do_reconnect(QString & trex_ip);
    void do_disconnect();
    void update_data_valid(bool data_is_valid);
    void timerEvent(QTimerEvent *event);
    void createActions();
    void updateMenus();
    void update_windows(bool enable);

    void write_window(QSettings * set,
                      QString window_name,
                      const QMainWindow & win);
    void load_window(QSettings * set,
                     QString window_name,
                     QMainWindow & win);


    void write_window(QSettings * set,
                      QString window_name,
                      const QWidget * win);

    void load_window(QSettings * set,
                     QString window_name,
                     QWidget * win);



    QMenu *fileMenu;
    QMenu *windowMenu;
    QMenu *helpMenu;
    QMenu *viewMenu;

    QAction * showAct ;
    QAction * exitAct;
    QAction * tileAct;
    QAction * cascadeAct ;
    QAction * nextAct;
    QAction * previousAct ;
    QAction * separatorAct ;
    QAction * aboutAct ;

    QToolBar *fileToolBar;
    QComboBox * m_ip_cb;
    QLabel *    m_ip_labtel;

    QAction *  m_connect;
    QAction *  m_disconnect;
    LEDWidget * m_led;
    QLabel *    m_status_label;

    bool       m_is_connected;/* is connected */
    bool       m_is_connected_data;/* is connected data in*/
    int        m_valid_data_cnt;
    int        m_valid_data_wd_timer;
    QString    m_trex_ip;  /* format ip:port */

private:
    void loadSettings();
    void saveSettings();

};



#endif // MAINWINDOW_H
