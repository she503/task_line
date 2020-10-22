#include "task_manager.h"

#include <common/file.hpp>
#include <rosbag/bag.h>
#include <rosbag/view.h>
#include <ros_msgs/Odometry.h>
#include <QPen>
#include <QBrush>
#include <QColor>
#include <QMessageBox>
#include <QPainterPath>
#include <QGraphicsPathItem>

TaskManager::TaskManager(QObject *parent) : QObject(parent)
{
    _task_group = new QGraphicsItemGroup();
    _task_group->setZValue(10);

    _points_group = new QGraphicsItemGroup(_task_group);
    _points_group->setZValue(12);
    _path_group = new QGraphicsItemGroup(_task_group);
    _path_group->setZValue(11);
}

bool TaskManager::loadTask(const std::string &file_path)
{
    this->clearRefLine();
    tergeo::task::proto::Task proto_task;
    if (!tergeo::common::file::GetProtoFromFile(file_path, proto_task)) {
        return false;
    }
    _task.fromProto(proto_task);
    this->updateTask();
    return true;
}

bool TaskManager::saveTask(const std::string &save_dir,
        const std::string &map_name, const std::string &task_name)
{
    _task.name = task_name;
    _task.task_type = tergeo::task::TaskType::TASK_TRACKING;
    _task.scene_info.map_area_name = map_name;
    _task.scene_info.enable_avoid_obstacle = true;
    _task.scene_info.work_flag = tergeo::task::WorkFlag::WORK_CLEAN;

    std::string save_path = save_dir + "/" + task_name + ".task";
    tergeo::task::proto::Task proto_task;
    _task.toProto(proto_task);
    tergeo::common::file::SetProtoToASCIIFile(proto_task, save_path);
    return true;
}

bool TaskManager::extractBagData(const std::string &bag_file)
{
    this->clearRefLine();
    _edit_mode = MODE_EXTRACT_BAG;
    rosbag::Bag ros_bag; ros_bag.open(bag_file, rosbag::bagmode::Read);
    rosbag::View odometry_view(ros_bag, rosbag::TopicQuery("/tergeo/localization/pose"));
    BOOST_FOREACH(rosbag::MessageInstance const m, odometry_view) {
        ros_msgs::Odometry::ConstPtr odometry = m.instantiate<ros_msgs::Odometry>();
        if (odometry == nullptr) {
            continue;
        }
        this->appendRefPoint(odometry->pose.position.x, odometry->pose.position.y);
    }
    this->updateTask();
    return !_task.reference_line.empty();
}

void TaskManager::startRecordRefLine()
{
    this->clearRefLine();
    _edit_mode = MODE_RECORD;
}

void TaskManager::stopRecordRefLine()
{
    this->updateTask();
}

const tergeo::task::Task &TaskManager::getTask() const
{
    return _task;
}

QGraphicsItemGroup *TaskManager::getTaskItemGroup()
{
    return _task_group;
}

void TaskManager::fromRefLine(const RefLine &ref_line)
{
    _task.reference_line.clear();
    if (ref_line.size() < 2) {
        return;
    }
    _task.reference_line.points.resize(ref_line.size());
    for (int i = 0; i < ref_line.size(); ++i) {
        _task.reference_line.points[i].type = ref_line.at(i).type;
        _task.reference_line.points[i].pose.x = ref_line.at(i).pos.x();
        _task.reference_line.points[i].pose.y = ref_line.at(i).pos.y();
        if (i == ref_line.size() - 1) {
            _task.reference_line.points[i].pose.theta =
                    std::atan2(ref_line.at(i).pos.y() - ref_line.at(i - 1).pos.y(),
                               ref_line.at(i).pos.x() - ref_line.at(i - 1).pos.x());
        } else {
            _task.reference_line.points[i].pose.theta =
                    std::atan2(ref_line.at(i + 1).pos.y() - ref_line.at(i).pos.y(),
                               ref_line.at(i + 1).pos.x() - ref_line.at(i).pos.x());
        }
    }
    this->updateTask();
}

void TaskManager::toRefLine(RefLine &ref_line)
{
    ref_line.clear();
    if (_task.reference_line.size() < 2) {
        return;
    }
    ref_line.reserve(_task.reference_line.size());
    for (int i = 0; i < _task.reference_line.size(); ++i) {
        RefPoint ref_point;
        ref_point.type = _task.reference_line.at(i).type;
        ref_point.pos.setX(_task.reference_line.at(i).pose.x);
        ref_point.pos.setY(_task.reference_line.at(i).pose.y);
        ref_point.is_edge_wise = (
                    ref_point.type == tergeo::task::ReferencePoint::Type::TYPE_EDGE_WISE_LINE ||
                    ref_point.type == tergeo::task::ReferencePoint::Type::TYPE_EDGE_WISE_CURVE);
        ref_line.append(ref_point);
    }
}

void TaskManager::appendRefPoint(const float x, const float y, const float theta)
{
    if (_edit_mode == MODE_NORMAL) {
        return;
    }

    tergeo::task::ReferencePoint ref_point;
    ref_point.type = tergeo::task::ReferencePoint::Type::TYPE_GO_STRAIGHT;
    ref_point.pose.x = x; ref_point.pose.y = y;
    if (!_task.reference_line.empty() &&
            _task.reference_line.last().pose.distanceTo(ref_point.pose) < ResampleDist) {
        return;
    }
    if (!_task.reference_line.empty()) {
        ref_point.pose.theta = std::atan2(y - _task.reference_line.last().pose.y,
                                          x - _task.reference_line.last().pose.x);
        if (_task.reference_line.size() == 1) {
            _task.reference_line.points[0].pose.theta = ref_point.pose.theta;
        }
    }
    _task.reference_line.push_back(ref_point);

    if (_edit_mode == MODE_RECORD || _edit_mode == MODE_DRAW) {
        QGraphicsEllipseItem* point_item = new QGraphicsEllipseItem(_points_group);
        QPen pen; pen.setWidthF(0.02); pen.setColor(QColor(0, 255, 0));
        point_item->setPen(pen);
        point_item->setBrush(QColor(0, 128, 0));
        QPointF point(x, y);
        QRectF rect(point - QPointF(0.04, 0.04), point + QPointF(0.04, 0.04));
        point_item->setRect(rect);
    }
    if (_edit_mode == MODE_DRAW) {
        this->updatePathItemGroup();
    }
}

void TaskManager::popRefPoint()
{
    _task.reference_line.points.pop_back();
}

void TaskManager::finishDrawRefline()
{
    this->updateTask();
}

void TaskManager::clearRefLine()
{
    _task.reference_line.clear();
    QList<QGraphicsItem* > item_list = _points_group->childItems();
    for (QGraphicsItem* item : item_list) {
        delete item;
    }
    QList<QGraphicsItem* > item_list_2 = _path_group->childItems();
    for (QGraphicsItem* item : item_list_2) {
        delete item;
    }
}

void TaskManager::updateTask()
{
    _edit_mode = MODE_NORMAL;
    _task.reference_line.computeDistance();
    this->updateTaskItemGroup();
    emit emitTaskUpdate();
}

void TaskManager::updateTaskItemGroup()
{
    this->updatePathItemGroup();
    QList<QGraphicsItem* > item_list = _points_group->childItems();
    for (QGraphicsItem* item : item_list) {
        delete item;
    }
}

void TaskManager::updatePathItemGroup()
{
    QList<QGraphicsItem* > item_list = _path_group->childItems();
    for (QGraphicsItem* item : item_list) {
        delete item;
    }
    if (_task.reference_line.empty()) {
        return;
    }
    QPen pen; pen.setWidthF(0.04);
    tergeo::task::ReferencePoint::Type cur_type = _task.reference_line.first().type;
    int cur_index = 0, pt_num = _task.reference_line.size();
    while (cur_index < pt_num) {
        QPainterPath path;
        path.moveTo(QPointF(_task.reference_line.at(cur_index).pose.x,
                            _task.reference_line.at(cur_index).pose.y));
        QGraphicsPathItem* path_item = new QGraphicsPathItem(_path_group);
        pen.setColor(PointTypeColorMap[cur_type]);
        path_item->setPen(pen);
        while (_task.reference_line.at(cur_index+1).type == cur_type) {
            path.lineTo(QPointF(_task.reference_line.at(cur_index+1).pose.x,
                                _task.reference_line.at(cur_index+1).pose.y));
            if (++cur_index >= (pt_num-1)) {
                break;
            }
        }
        path_item->setPath(path);
        if (cur_index >= (pt_num-1)) {
            break;
        }
        cur_type = _task.reference_line.at(cur_index+1).type;
    }
    _path_group->update();
}

