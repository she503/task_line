#include "data_manager.h"

#include <rosbag/bag.h>
#include <rosbag/view.h>
#include <ros_msgs/Odometry.h>

DataManager::DataManager(QObject *parent) : QObject(parent)
{
    _map_manager = new MapManager(this);
    _task_manager = new TaskManager(this);
}

MapManager *DataManager::getMapManager()
{
    return _map_manager;
}

TaskManager *DataManager::getTaskManager()
{
    return _task_manager;
}

bool DataManager::loadTaskFile(const std::string &task_file)
{
    _task_manager->setEditMode(TaskManager::MODE_LOAD);
    _task_manager->clearRefline();
    if (!_task_manager->loadTask(task_file)) {
        return false;
    }
    _task_manager->computeReflineDistance();
    _task_manager->updateTaskItem();
    _task_manager->setEditMode(TaskManager::MODE_NORMAL);
    emit emitReflineUpdate();
    return !_task_manager->getRefline().empty();
}

bool DataManager::extractBagData(const std::string &bag_file)
{
    _task_manager->setEditMode(TaskManager::MODE_RECORD);
    _task_manager->clearRefline();

    tergeo::common::math::Pose3d pose_3d;
    rosbag::Bag ros_bag; ros_bag.open(bag_file, rosbag::bagmode::Read);
    rosbag::View odometry_view(ros_bag, rosbag::TopicQuery("/tergeo/localization/pose"));
    BOOST_FOREACH(rosbag::MessageInstance const m, odometry_view) {
        ros_msgs::Odometry::ConstPtr odometry = m.instantiate<ros_msgs::Odometry>();
        if (odometry == nullptr) {
            continue;
        }
        pose_3d.x = odometry->pose.position.x;
        pose_3d.y = odometry->pose.position.y;
        pose_3d.z = odometry->pose.position.z;
        pose_3d.qw = odometry->pose.orientation.w;
        pose_3d.qx = odometry->pose.orientation.x;
        pose_3d.qy = odometry->pose.orientation.y;
        pose_3d.qz = odometry->pose.orientation.z;
        this->appendRefPoint(pose_3d.x, pose_3d.y, pose_3d.getYaw());
    }
    _task_manager->computeReflineDistance();
    _task_manager->updateTaskItem();
    _task_manager->setEditMode(TaskManager::MODE_NORMAL);
    emit emitReflineUpdate();
    return !_task_manager->getRefline().empty();
}

void DataManager::startRecordRefline()
{
    _task_manager->setEditMode(TaskManager::MODE_RECORD);
    _task_manager->clearRefline();
    emit emitStartRosSpin();
}

void DataManager::stopRecordRefline()
{
    _task_manager->computeReflineDistance();
    _task_manager->updateTaskItem();
    _task_manager->setEditMode(TaskManager::MODE_NORMAL);
    emit emitStopRosSpin();
    emit emitReflineUpdate();
}

void DataManager::setDrawReflineFlag(const bool flag)
{
    _task_manager->setEditMode(TaskManager::MODE_DRAW);
    if (flag) {
        _task_manager->clearRefline();
    }
    emit emitDrawReflineFlag(flag);
}

void DataManager::appendRefPoint(const float x, const float y, const float theta)
{
    tergeo::common::math::Pose2d pose_2d(x, y, theta);
    if (_task_manager->getRefline().empty() ||
            _task_manager->getRefline().last().pose.distanceTo(pose_2d) >
            _resample_dist) {
        _task_manager->appendRefpoint(pose_2d);
    }
}

void DataManager::appendRefPoint(const float x, const float y)
{
    if (_task_manager->getRefline().empty()) {
        _task_manager->appendRefpoint(tergeo::common::math::Pose2d(x, y, 0));
    } else if (_task_manager->getRefline().size() == 1) {
        float theta = std::atan2(y - _task_manager->getRefline().first().pose.y,
                                 x - _task_manager->getRefline().first().pose.x);
        tergeo::common::math::Pose2d first_pose(
                    _task_manager->getRefline().first().pose.x,
                    _task_manager->getRefline().first().pose.y, theta);
        tergeo::common::math::Pose2d pose(x, y, theta);
        _task_manager->updateRefPoint(0, first_pose);
        _task_manager->appendRefpoint(pose);
    } else {
        float theta = std::atan2(y - _task_manager->getRefline().last().pose.y,
                                 x - _task_manager->getRefline().last().pose.x);
        tergeo::common::math::Pose2d pose(x, y, theta);
        _task_manager->appendRefpoint(pose);
    }
    _task_manager->updateTaskItem();
}

void DataManager::popRefPoint()
{
    _task_manager->popRefPoint();
    _task_manager->updateTaskItem();
}

void DataManager::finishDrawRefline()
{
    _task_manager->computeReflineDistance();
    _task_manager->updateTaskItem();
    _task_manager->setEditMode(TaskManager::MODE_NORMAL);
    emit emitReflineUpdate();
    emit emitDrawReflineFlag(false);
}

void DataManager::clearRefPoint()
{
    _task_manager->clearRefline();
}
