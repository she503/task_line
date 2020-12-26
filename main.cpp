#include "mainwindow.h"
#include <QApplication>
#include <csignal>
#include <ros/ros.h>

void AppSigintHandler(int signal_num) {
    if (signal_num != SIGINT) {
        return;
    }
    bool static is_stopping = false;
    if (is_stopping) {
        return;
    }
    is_stopping = true;
    if (ros::ok()) {
        ros::shutdown();
    }
    QApplication::exit();
}

int main(int argc, char *argv[])
{
    ros::init(argc, argv, "task_refline_editor");
    QApplication a(argc, argv);
    MainWindow w;
    w.showMaximized();
    signal(SIGINT, AppSigintHandler);
    return a.exec();
}
