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
    _task_control_frame = new TaskControlFrame(_data_manager, this);
    _refline_edit_frame = new RefLineEditFrame(_data_manager, this);

    _ros_manager = new RosManager();
    _ros_thread = new QThread();
    _ros_manager->moveToThread(_ros_thread);
    connect(_ros_thread, &QThread::finished, _ros_manager, &QObject::deleteLater);
    connect(_ros_thread, &QThread::finished, _ros_thread, &QObject::deleteLater);

    connect(_task_control_frame, SIGNAL(emitStartRosSpin()),
            _ros_manager, SLOT(startRosSpin()));
    connect(_task_control_frame, SIGNAL(emitStopRosSpin()),
            _ros_manager, SLOT(stopRosSpin()));
    connect(_task_control_frame, SIGNAL(emitStartDrawRefLine()),
            _editor_viewer, SLOT(startDrawRefLineSlot()));
    connect(_task_control_frame, SIGNAL(emitStopDrawRefLine()),
            _editor_viewer, SLOT(stopDrawRefLineSlot()));
    connect(_task_control_frame, SIGNAL(emitStartRosSpin()),
            _editor_viewer, SLOT(startRecordSlot()));
    connect(_task_control_frame, SIGNAL(emitStopRosSpin()),
            _editor_viewer, SLOT(stopRecordSlot()));

    connect(_refline_edit_frame, SIGNAL(emitUnEditMode()),
            _task_control_frame, SLOT(refLineUnEditModeSlot()));
    connect(_refline_edit_frame, SIGNAL(emitEditMode()),
            _task_control_frame, SLOT(refLineEditModeSlot()));

    connect(_ros_manager, SIGNAL(emitLocalization(float,float,float)),
            _data_manager->getTaskManager(), SLOT(appendRefPoint(float,float,float)));
    connect(_ros_manager, SIGNAL(emitLocalization(float,float,float)),
            _editor_viewer, SLOT(updateLocalization(float,float,float)));

    _ros_thread->start();

    this->setCentralWidget(_editor_viewer);
    QTabWidget* tab_widget = new QTabWidget(this);
    tab_widget->addTab(_task_control_frame, tr("Task Control Frame"));
    tab_widget->addTab(_refline_edit_frame, tr("Refline Edit Frame"));
    QDockWidget* control_dock = new QDockWidget(this);
    control_dock->setWidget(tab_widget);
    control_dock->setFeatures(QDockWidget::NoDockWidgetFeatures);
    this->addDockWidget(Qt::RightDockWidgetArea, control_dock);
}

MainWindow::~MainWindow()
{
    delete ui;
    _ros_manager->stopRosSpin();
    if(_ros_thread) {
        _ros_thread->quit();
    }
    _ros_thread->wait();
}
