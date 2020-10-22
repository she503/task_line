#include "mainwindow.h"
#include "ui_mainwindow.h"

#include <QTabWidget>
#include <QDockWidget>

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    this->setWindowTitle(tr("Task Refline Editor"));
    this->setWindowIcon(QIcon(":/icons/main_icon"));

    _data_manager = new DataManager(this);
    _editor_viewer = new EditorViewer(_data_manager, this);
    _control_frame = new ControlFrame(_data_manager, this);
    _task_edit_frame = new TaskEditFrame(_data_manager, this);

    _ros_manager = new RosManager();
    _ros_thread = new QThread();
    _ros_manager->moveToThread(_ros_thread);
    connect(_ros_thread, &QThread::finished, _ros_manager, &QObject::deleteLater);
    connect(_ros_thread, &QThread::finished, _ros_thread, &QObject::deleteLater);
    connect(_data_manager, SIGNAL(emitStartRosSpin()), _ros_manager, SLOT(startRosSpin()));
    connect(_ros_manager, SIGNAL(emitLocalization(float,float,float)),
            _data_manager, SLOT(appendRefPoint(float,float,float)));
    connect(_ros_manager, SIGNAL(emitLocalization(float,float,float)),
            _editor_viewer, SLOT(updateLocalization(float,float,float)));
    connect(_data_manager, SIGNAL(emitStopRosSpin()), this, SLOT(stopRosSpinSlot()));
    _ros_thread->start();

    this->setCentralWidget(_editor_viewer);
    QTabWidget* tab_widget = new QTabWidget(this);
    tab_widget->addTab(_control_frame, tr("Control Frame"));
    tab_widget->addTab(_task_edit_frame, tr("Task Edit"));
    QDockWidget* control_dock = new QDockWidget(this);
    control_dock->setWidget(tab_widget);
    control_dock->setFeatures(QDockWidget::NoDockWidgetFeatures);
    this->addDockWidget(Qt::RightDockWidgetArea, control_dock);
}

MainWindow::~MainWindow()
{
    delete ui;
    this->stopRosSpinSlot();
    if(_ros_thread) {
        _ros_thread->quit();
    }
    _ros_thread->wait();
}

void MainWindow::stopRosSpinSlot()
{
    _ros_manager->stopRosSpin();
}
