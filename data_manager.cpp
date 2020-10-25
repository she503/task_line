#include "data_manager.h"

#include <rosbag/bag.h>
#include <rosbag/view.h>
#include <ros_msgs/Odometry.h>

DataManager::DataManager(QObject *parent) : QObject(parent)
{
    _map_manager = new MapManager(this);
    _task_manager = new TaskManager(this);
    _refline_manager = new RefLineManager(this);
}

MapManager *DataManager::getMapManager()
{
    return _map_manager;
}

TaskManager *DataManager::getTaskManager()
{
    return _task_manager;
}

RefLineManager *DataManager::getRefLineManager()
{
    return _refline_manager;
}

bool DataManager::copyTaskToRefLine()
{
    if (_task_manager->getTask().reference_line.empty()) {
        return false;
    }
    _task_manager->toRefLine(_refline_manager->getRefLine());
    _refline_manager->updateRefLine();
    return true;
}

void DataManager::copyRefLineToTask()
{
    _task_manager->fromRefLine(_refline_manager->getRefLine());
    _refline_manager->setUnEditMode();
}
