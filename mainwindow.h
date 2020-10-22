#ifndef TASK_REFLINE_EDITOR_MAINWINDOW_H
#define TASK_REFLINE_EDITOR_MAINWINDOW_H

#include <QThread>
#include <QMainWindow>

#include "ros_manager.h"
#include "data_manager.h"
#include "editor_viewer.h"
#include "control_frame.h"
#include "task_edit_frame.h"

namespace Ui {
class MainWindow;
}

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = 0);
    ~MainWindow();

private slots:
    void stopRosSpinSlot();

private:
    DataManager* _data_manager;
    EditorViewer* _editor_viewer;
    ControlFrame* _control_frame;
    TaskEditFrame* _task_edit_frame;

    RosManager* _ros_manager;
    QThread* _ros_thread;

    Ui::MainWindow *ui;
};

#endif // MAINWINDOW_H
