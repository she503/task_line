#include "ros_manager.h"

#include <QMutexLocker>
#include <common/math.hpp>

RosManager::RosManager(QObject *parent) : QObject(parent)
{
    _is_stop = true;
}

RosManager::~RosManager()
{
    _is_stop = true;
}

void RosManager::startRosSpin()
{
    _node_handle.reset(new ros::NodeHandle("~"));
    _localization_subscriber = _node_handle->subscribe(
            "/tergeo/localization/pose", 1, &RosManager::localizationCallback, this);
    {
        QMutexLocker locker(&_stop_mutex);
        _is_stop = false;
    }

    ros::Rate loop_rate(100);
    while (true) {
        {
            QMutexLocker locker(&_stop_mutex);
            if (_is_stop) {
                break;
            }
        }
        ros::spinOnce();
        loop_rate.sleep();
    }
    {
        QMutexLocker locker(&_stop_mutex);
        _is_stop = true;
    }
}

void RosManager::stopRosSpin()
{
    QMutexLocker locker(&_stop_mutex);
    _is_stop = true;
}

void RosManager::localizationCallback(const ros_msgs::OdometryPtr &odometry)
{
    tergeo::common::math::Pose3d pose_3d;
    pose_3d.x = odometry->pose.position.x;
    pose_3d.y = odometry->pose.position.y;
    pose_3d.z = odometry->pose.position.z;
    pose_3d.qw = odometry->pose.orientation.w;
    pose_3d.qx = odometry->pose.orientation.x;
    pose_3d.qy = odometry->pose.orientation.y;
    pose_3d.qz = odometry->pose.orientation.z;
    emit emitLocalization(pose_3d.x, pose_3d.y, pose_3d.getYaw());
}
