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

#include "mainwindow.h"
#include <QMessageBox>
#include "ui_mainwindow.h"
#include "child_global_win.h"
#include "rx_check_win.h"
#include "latencywin.h"
#include "histogramWin.h"
#include "perportwin.h"
#include "zmq_gcontext.h"
#include <QComboBox>
#include <QLabel>
#include "ledwidget.h"
#include <QDebug>
#include <QMdiSubWindow>
#include <QSplashScreen>
#include <QSettings>
#include <QCloseEvent>
#include <QDockWidget>
#include <QSplitter>



MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    setWindowIcon(QIcon(":/resource/trex.ico" ));

    m_mdiArea = new QMdiArea;

    m_mdiArea->setHorizontalScrollBarPolicy(Qt::ScrollBarAsNeeded);
    m_mdiArea->setVerticalScrollBarPolicy(Qt::ScrollBarAsNeeded);
    setCentralWidget(m_mdiArea);


    setWindowTitle(tr("Trex viewer"));

    m_child_Window = new GlobalChildMainWindow(this);


   // QSize s_max = m_child_Window->maximumSize();
   // m_child_Window->setMaximumSize(s_max);


    m_mdiArea->addSubWindow(m_child_Window);
//    m_mdiArea->setWindowState(Qt::WindowFullScreen);

    m_child_Window->showFullScreen();
    m_child_Window->showMaximized();
    m_child_Window->setWindowState(windowState() | Qt::WindowFullScreen);

    connect( m_child_Window->get_zmq_trex(), SIGNAL(have_data(QString)), this, SLOT(update_new_json(QString)) );

//    globalCounter = new QDockWidget(tr("Trex Global counters"), this);
//    globalCounter->setAllowedAreas(Qt::AllDockWidgetAreas);
//    globalCounter->setWidget(m_child_Window);
//    addDockWidget(Qt::LeftDockWidgetArea, globalCounter);

    //=============rx check counters window ================
//    m_rx_check_Window = new RxCheckWin(this);
//    m_mdiArea->addSubWindow(m_rx_check_Window);
//    QSize rx_max = m_rx_check_Window->maximumSize();
//    m_rx_check_Window->setFixedSize(rx_max);
//    m_rx_check_Window->show();
//    QObject::connect(m_child_Window,SIGNAL(update_rx_counters(CGlobalRxCheckInfo *)),m_rx_check_Window,SLOT(update_html(CGlobalRxCheckInfo *)) );


    //============latency window ================
    m_latency =new LatencyWin(this);
    lDock = new QDockWidget(tr("latency"), this);
    lDock ->setObjectName("latency");
    lDock->setAllowedAreas(Qt::LeftDockWidgetArea | Qt::RightDockWidgetArea);
    lDock->setWidget(m_latency);
    addDockWidget(Qt::RightDockWidgetArea, lDock);

    QObject::connect(m_child_Window,SIGNAL(latency_changed(double,double)),m_latency,SLOT(update_new_data(double,double)) );

    //Histogram win for showing histogram graph for rx-check , latency counters
    m_histogram =new HistogramWin(this);

    //add rx counter histogram window to docking
    rxDock = new QDockWidget(tr("Rx_Histogram"), this);
    rxDock ->setObjectName("Rx_Histogram");
    rxDock->setAllowedAreas(Qt::LeftDockWidgetArea | Qt::RightDockWidgetArea);
    rxDock->setWidget(m_histogram);
    addDockWidget(Qt::RightDockWidgetArea, rxDock);
    QObject::connect(m_child_Window,SIGNAL(update_rx_counters(CGlobalInfo *)),m_histogram,SLOT(update_new_data(CGlobalInfo *)) );


    l2_histogram = new HistogramWin(this);

    lv2Dock = new QDockWidget(tr("Latency-v2_Histogram"), this);
    lv2Dock ->setObjectName("Latency-v2_Histogram");
    lv2Dock->setAllowedAreas(Qt::LeftDockWidgetArea | Qt::RightDockWidgetArea);
    lv2Dock->setWidget(l2_histogram);
    addDockWidget(Qt::RightDockWidgetArea, lv2Dock);

    QObject::connect(m_child_Window,SIGNAL(update_l2_counters(CParserLatencyV2Info *)),l2_histogram,SLOT(update_new_data(CParserLatencyV2Info *)) );

    m_per_port =new PerPortWin(this);

   // m_mdiArea->removeSubWindow(m_per_port);
    m_mdiArea->addSubWindow(m_per_port);
    m_per_port->show();

//    dw_ports = new QDockWidget(tr("Ports"), this);
//    dw_ports->setAllowedAreas(Qt::AllDockWidgetAreas);
//    dw_ports->setWidget(m_per_port);

//    addDockWidget(Qt::LeftDockWidgetArea, dw_ports);


    QObject::connect(m_child_Window,SIGNAL(update_port_counters(CGlobalInfo *)),m_per_port,SLOT(update_html(CGlobalInfo *)) );

    m_rx_template =new PerPortWin(this);
    m_rx_template->setWindowTitle("Per Template window");
    m_mdiArea->addSubWindow(m_rx_template);
    m_rx_template->show();
    QObject::connect(m_child_Window,SIGNAL(update_rx_template(CGlobalInfo *)),m_rx_template,SLOT(update_rx_check_template_html(CGlobalInfo *)) );

    createActions();
    updateMenus();
    m_is_connected=false;
    m_is_connected_data=false;
    m_valid_data_wd_timer=startTimer(1000);/* 2 sec wd*/
    m_valid_data_cnt=0;
    update_status();

    loadSettings();

    m_mdiArea->setViewMode(QMdiArea::SubWindowView);
}

void MainWindow::ShowAll(){


}

void MainWindow::closeEvent(QCloseEvent *event)
{
    saveSettings();
    event->accept();
    QMainWindow::closeEvent(event);
}


MainWindow::~MainWindow()
{

    delete ui;
}

void MainWindow::about(){
    QMessageBox about;
    about.setIconPixmap (QPixmap (":/resource/trex.jpg" ));
    about.setText("<h1>TRex viewer v0.6</h1>");
    about.setStandardButtons(QMessageBox::Ok);
    about.setDefaultButton(QMessageBox::Ok);
    about.show();
    about.exec();
}


void MainWindow::connect_disconnect(){
    if ( m_is_connected  ) {
        do_disconnect();
    }else{
        do_reconnect(m_ip_cb->currentText());
    }
}

void MainWindow::connect_disconnect(const QString &s){
  do_reconnect((QString &)s);
}


void MainWindow::createActions()
{
    exitAct = new QAction(QIcon(":/resource/exit.png"),tr("E&xit"), this);
    exitAct->setShortcuts(QKeySequence::Quit);
    exitAct->setStatusTip(tr("Exit the application"));
    connect(exitAct, SIGNAL(triggered()), qApp, SLOT(closeAllWindows()));

    m_connect = new QAction(QIcon(":/resource/player_play.png"),tr("&Connect"), this);
    m_connect->setStatusTip(tr("connect to trex"));
    connect(m_connect, SIGNAL(triggered()), this, SLOT(connect_disconnect()));

    m_disconnect = new QAction(QIcon(":/resource/player_stop.png"),tr("&Disconnect"), this);
    m_disconnect->setStatusTip(tr("disconnect from trex"));
    connect(m_disconnect, SIGNAL(triggered()), this, SLOT(connect_disconnect()));

    tileAct = new QAction(tr("&Tile"), this);
    tileAct->setStatusTip(tr("Tile the windows"));
    connect(tileAct, SIGNAL(triggered()), m_mdiArea, SLOT(tileSubWindows()));

    cascadeAct = new QAction(tr("&Cascade"), this);
    cascadeAct->setStatusTip(tr("Cascade the windows"));
    connect(cascadeAct, SIGNAL(triggered()), m_mdiArea, SLOT(cascadeSubWindows()));

    nextAct = new QAction(tr("Ne&xt"), this);
    nextAct->setShortcuts(QKeySequence::NextChild);
    nextAct->setStatusTip(tr("Move the focus to the next window"));
    connect(nextAct, SIGNAL(triggered()),
            m_mdiArea, SLOT(activateNextSubWindow()));

    previousAct = new QAction(tr("Pre&vious"), this);
    previousAct->setShortcuts(QKeySequence::PreviousChild);
    previousAct->setStatusTip(tr("Move the focus to the previous "
                                 "window"));
    connect(previousAct, SIGNAL(triggered()),
            m_mdiArea, SLOT(activatePreviousSubWindow()));

    separatorAct = new QAction(this);
    separatorAct->setSeparator(true);

    aboutAct = new QAction(tr("&About"), this);
    aboutAct->setStatusTip(tr("Show the application's About box"));
    connect(aboutAct, SIGNAL(triggered()), this, SLOT(about()));

}


void MainWindow::updateMenus()
{

    fileMenu = menuBar()->addMenu(tr("&File"));
    fileMenu->addAction(exitAct);

    windowMenu = menuBar()->addMenu(tr("&Window"));
    windowMenu->clear();
    windowMenu->addAction(tileAct);
    windowMenu->addAction(cascadeAct);
    windowMenu->addSeparator();
    windowMenu->addAction(nextAct);
    windowMenu->addAction(previousAct);
    windowMenu->addAction(separatorAct);

    viewMenu = menuBar()->addMenu(tr("&View"));
    viewMenu->addAction(lDock->toggleViewAction());
    viewMenu->addAction(rxDock->toggleViewAction());
    viewMenu->addAction(lv2Dock->toggleViewAction());

    menuBar()->addSeparator();

    helpMenu = menuBar()->addMenu(tr("&Help"));
    helpMenu->addAction(aboutAct);


    bool hasMdiChild=true;
    tileAct->setEnabled(hasMdiChild);
    cascadeAct->setEnabled(hasMdiChild);
    nextAct->setEnabled(hasMdiChild);
    previousAct->setEnabled(hasMdiChild);
    separatorAct->setVisible(hasMdiChild);

    fileToolBar = addToolBar(tr("File"));
    fileToolBar->setObjectName("file");
    fileToolBar->addAction(exitAct);
    m_ip_labtel= new QLabel(this);
    m_ip_labtel->setText("<b>Trex ip: </b>");
    m_ip_cb=new QComboBox(this);

    m_ip_cb->setToolTip(" Trex ip string format ip[:port]");
    m_ip_cb->setToolTipDuration(1000);
    m_ip_cb->setStatusTip(" Trex ip string format ip[:port]");

    m_ip_cb->setMinimumWidth(170);
    m_ip_cb->setDuplicatesEnabled ( false);
    m_ip_cb->setEditable(true);




    connect(m_ip_cb, SIGNAL(activated(const QString & )),
            this, SLOT(connect_disconnect(const QString &)));

    fileToolBar->addAction(separatorAct);
    fileToolBar->addWidget(m_ip_labtel);
    fileToolBar->addWidget(m_ip_cb);
    fileToolBar->addAction(m_connect);
    fileToolBar->addAction(m_disconnect);
    statusBar()->showMessage(tr("Connected"));

    m_led = new LEDWidget(this);
    m_led->setVisible(true);

    m_status_label=new QLabel();
    m_status_label->setText("connected");

    statusBar()->addPermanentWidget(m_status_label);

    statusBar()->addPermanentWidget(m_led);
}

void MainWindow::update_status(void){
    if (m_is_connected) {
        m_status_label->setText("<b>Connected</b> ");
        m_connect->setEnabled(false);
        m_disconnect->setEnabled(true);
    }else{
        m_status_label->setText("Disconnected ");
        m_connect->setEnabled(true);
        m_disconnect->setEnabled(false);

    }
}


void MainWindow::do_reconnect(QString & trex_ip){
    CZmqThread   * trex=m_child_Window->get_zmq_trex();
    if ( m_is_connected ) {
        do_disconnect();
    }
    QString zmq_str="tcp://"+trex_ip;

    if ( trex_ip.contains(':')==false ) { 
        zmq_str+=":4500";
    }

    trex->set_host(zmq_str);
    if ( trex->init_socket() != 0){
        QMessageBox::warning ( this, "Not a valid ip string", " <p>Provide ip[:port] e.g server-name </p> <p>127.0.0.1:5559 </p>  " );
        return;
    }
    trex->start();
    m_is_connected=true;
    update_status();
}

void MainWindow::do_disconnect(){
    CZmqThread   * trex=m_child_Window->get_zmq_trex();
    trex->close_socket_and_wait();
    m_is_connected=false;
    update_status();
}

void MainWindow::update_windows(bool enable){
    (void)enable;
    m_child_Window->reset();
    m_per_port->reset();
    m_latency->reset();
    m_rx_template->reset();
    m_histogram->reset();
    l2_histogram->reset();
}

void MainWindow::update_data_valid(bool data_is_valid){
    m_is_connected_data =data_is_valid;
    if ( data_is_valid ) {
        m_led->start_flash();
        //update_windows(true);
    }else{
        m_led->stop_flash();
        m_led->set_off();
        update_windows(false);
        //update_windows(false);
    }
}

void MainWindow::timerEvent(QTimerEvent *event){
    (void*)event;
    if ( m_is_connected_data ){
        m_valid_data_cnt++;
        if (m_valid_data_cnt>2) {
            update_data_valid(false);
        }
    }
}

void MainWindow::update_new_json(QString s){
    if ( m_is_connected_data ==false ){
        update_data_valid(true);
    }
    m_valid_data_cnt=0;
}

void MainWindow::write_window(QSettings * set,
                              QString window_name,
                              const QMainWindow & win){
    set->beginGroup( window_name );

    set->setValue( "geometry", win.saveGeometry() );
    set->setValue( "savestate", win.saveState() );
    set->setValue( "maximized", win.isMaximized() );
    if ( !win.isMaximized() ) {
           set->setValue( "pos", win.pos() );
           set->setValue( "size", win.size() );
    }
    set->endGroup();
}

void MainWindow::load_window(QSettings * set,
                             QString window_name,
                             QMainWindow & win){
    set->beginGroup( window_name );
    win.restoreGeometry(set->value( "geometry", win.saveGeometry() ).toByteArray());
    win.restoreState(set->value( "savestate", win.saveState() ).toByteArray());
    win.move(set->value( "pos", win.pos() ).toPoint());
    win.resize(set->value( "size", win.size() ).toSize());
    if ( set->value( "maximized", win.isMaximized() ).toBool() )
        win.showMaximized();
    set->endGroup();
}

void MainWindow::write_window(QSettings * set,
                              QString window_name,
                              const QWidget * win){
    set->beginGroup( window_name );
    set->setValue( "geometry", win->saveGeometry() );
    set->setValue( "maximized", win->isMaximized() );
    if ( !win->isMaximized() ) {
           set->setValue( "pos", win->pos() );
           set->setValue( "size", win->size() );
    }
    set->endGroup();
}

void MainWindow::load_window(QSettings * set,
                             QString window_name,
                             QWidget * win){
    set->beginGroup( window_name );
    win->restoreGeometry(set->value( "geometry", win->saveGeometry() ).toByteArray());
    win->move(set->value( "pos", win->pos() ).toPoint());
    win->resize(set->value( "size", win->size() ).toSize());
    if ( set->value( "maximized", win->isMaximized() ).toBool() )
        win->showMaximized();
    set->endGroup();
}

void MainWindow::update_size(){
    QSettings settings(QSettings::SystemScope, "Trex-v1");
    QList<QMdiSubWindow *> windows = m_mdiArea->subWindowList();
    for (int i = 0; i < windows.size(); ++i) {
        QMainWindow *child = qobject_cast<QMainWindow *>(windows.at(i)->widget());
        load_window(&settings,child->windowTitle() ,child->parentWidget());
    }
}


void MainWindow::loadSettings(){
    QSettings settings(QSettings::SystemScope, "Trex-v1");
    QStringList sText = settings.value("text", "").toStringList();
    m_ip_cb->addItems(sText);
    load_window(&settings,"main",*this);
}

 

void MainWindow::saveSettings(){
    QSettings settings(QSettings::SystemScope, "Trex-v1");

    QStringList slist;
    int cnt=m_ip_cb->count();
    int i;
    for (i=0; i<cnt; i++){
        slist+=m_ip_cb->itemText(i);
    }
    settings.setValue("text", slist);
    write_window(&settings,"main",*this);
}
