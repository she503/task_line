#ifndef TASK_REFLINE_EDITOR_ROS_MANAGER_H
#define TASK_REFLINE_EDITOR_ROS_MANAGER_H

#include <QMutex>
#include <QObject>
#include <QThread>
#include <ros/ros.h>
#include <ros_msgs/Odometry.h>

class RosManager : public QObject
{
    Q_OBJECT
public:
    explicit RosManager(QObject *parent = nullptr);
    ~RosManager();

signals:
    void emitLocalization(const float x, const float y, const float theta);

public slots:
    void startRosSpin();
    void stopRosSpin();

private:
    void localizationCallback(const ros_msgs::OdometryPtr& odometry);

private:
    QMutex _stop_mutex;
    bool _is_stop = true;
    ros::Subscriber _localization_subscriber;
    std::unique_ptr<ros::NodeHandle> _node_handle;
};

#endif // ROS_THREAD_H
