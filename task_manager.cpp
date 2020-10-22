#include "task_manager.h"

#include <common/file.hpp>

#include <QPen>
#include <QBrush>
#include <QColor>
#include <QMessageBox>
#include <QPainterPath>

using RefPoint = tergeo::task::ReferencePoint;
using RefPointType = tergeo::task::ReferencePoint::Type;

TaskManager::TaskManager(QObject *parent) : QObject(parent)
{
    _task_item = new QGraphicsItemGroup();
    _task_item->setZValue(10);

    _points_group = new QGraphicsItemGroup(_task_item);
    _points_group->setZValue(12);
    _path_item = new QGraphicsPathItem(_task_item);
    _path_item->setZValue(11);
}

bool TaskManager::loadTask(const std::string &file_path)
{
    if (_edit_mode != MODE_LOAD) {
        return false;
    }
    tergeo::task::proto::Task proto_task;
    if (!tergeo::common::file::GetProtoFromFile(file_path, proto_task)) {
        return false;
    }
    _task.fromProto(proto_task);
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

const tergeo::task::Task &TaskManager::getTask() const
{
    return _task;
}

const tergeo::task::ReferenceLine &TaskManager::getRefline() const
{
    return _task.reference_line;
}

void TaskManager::clearRefline()
{
    if (_edit_mode == MODE_NORMAL) {
        return;
    }
    _task.reference_line.clear();
    _path_item->setVisible(false);
    QList<QGraphicsItem* > item_list = _points_group->childItems();
    for (QGraphicsItem* item : item_list) {
        delete item;
    }
}

void TaskManager::setEditMode(TaskManager::EditMode edit_mode)
{
    _edit_mode = edit_mode;
}

void TaskManager::appendRefpoint(const tergeo::common::math::Pose2d &ref_pose)
{
    if (_edit_mode != MODE_RECORD && _edit_mode != MODE_DRAW) {
        return;
    }

    QGraphicsEllipseItem* point_item = new QGraphicsEllipseItem(_points_group);
    QPen pen; pen.setWidthF(0.02); pen.setColor(QColor(0, 255, 0));
    point_item->setPen(pen);
    point_item->setBrush(QColor(0, 128, 0));
    QPointF point(ref_pose.x, ref_pose.y);
    QRectF rect(point - QPointF(0.04, 0.04), point + QPointF(0.04, 0.04));
    point_item->setRect(rect);
    point_item->setVisible(true);

    RefPoint ref_point;
    ref_point.pose = ref_pose;
    ref_point.type = RefPointType::TYPE_GO_STRAIGHT;
    _task.reference_line.push_back(ref_point);
}

void TaskManager::updateRefPoint(const int index,
                                 const tergeo::common::math::Pose2d &ref_pose)
{
    if (_edit_mode == MODE_NORMAL) {
        return;
    }
    if (index < 0 || index >= _task.reference_line.size()) {
        return;
    }
    _task.reference_line.points[index].pose = ref_pose;
}

void TaskManager::popRefPoint()
{
    if (_edit_mode != MODE_DRAW) {
        return;
    }
    _task.reference_line.points.pop_back();
}

QGraphicsItemGroup *TaskManager::getTaskItemGroup()
{
    return _task_item;
}

void TaskManager::updateTask()
{
    _task.reference_line.computeDistance();
    this->updatePathItem();
    this->updatePointsGroup();
}

void TaskManager::updatePathItem()
{
    int pt_num = _task.reference_line.size();
    if (pt_num < 2) return;
    QPainterPath path;
    path.moveTo(QPointF(_task.reference_line.first().pose.x,
                        _task.reference_line.first().pose.y));
    for (int i = 1; i < _task.reference_line.size(); ++i) {
        path.lineTo(QPointF(_task.reference_line.at(i).pose.x,
                            _task.reference_line.at(i).pose.y));
    }
    QPen pen(QColor(0, 0, 255));
    pen.setWidthF(0.03);
    _path_item->setPen(pen);
    _path_item->setPath(path);
    _path_item->setVisible(true);
    _path_item->update();
}

void TaskManager::updatePointsGroup()
{
    int pt_num = _task.reference_line.size();
    while (pt_num > _points_group->childItems().size()) {
        QGraphicsEllipseItem* point_item = new QGraphicsEllipseItem(_points_group);
    }

    QPen pen; pen.setWidthF(0.02);
    QList<QGraphicsItem* > item_list = _points_group->childItems();
    for (int i = 0; i < item_list.size(); ++i) {
        if (i >= pt_num) {
            item_list[i]->setVisible(false);
            continue;
        }
        QGraphicsEllipseItem* point_item =
                dynamic_cast<QGraphicsEllipseItem*>(item_list[i]);
        RefPointType point_type = _task.reference_line.at(i).type;
        if (point_type == RefPointType::TYPE_GO_STRAIGHT ||
                point_type == RefPointType::TYPE_GO_STRAIGHT_CURVE) {
            pen.setColor(QColor(0, 255, 0));
            point_item->setPen(pen);
            point_item->setBrush(QColor(0, 128, 0));
        } else if (point_type == RefPointType::TYPE_TURN_LEFT ||
                   point_type == RefPointType::TYPE_INNER_TURN_LEFT ||
                   point_type == RefPointType::TYPE_OUTER_TURN_LEFT) {
            pen.setColor(QColor(255, 0, 0));
            point_item->setPen(pen);
            point_item->setBrush(QColor(128, 0, 0));
        } else if (point_type == RefPointType::TYPE_TURN_RIGHT ||
                   point_type == RefPointType::TYPE_INNER_TURN_RIGHT ||
                   point_type == RefPointType::TYPE_OUTER_TURN_RIGHT) {
            pen.setColor(QColor(255, 255, 0));
            point_item->setPen(pen);
            point_item->setBrush(QColor(128, 128, 0));
        } else if (point_type == RefPointType::TYPE_U_TURN_LEFT ||
                   point_type == RefPointType::TYPE_INNER_U_TURN_LEFT ||
                   point_type == RefPointType::TYPE_OUTER_U_TURN_LEFT) {
            pen.setColor(QColor(0, 0, 255));
            point_item->setPen(pen);
            point_item->setBrush(QColor(0, 0, 128));
        } else if (point_type == RefPointType::TYPE_U_TURN_RIGHT ||
                   point_type == RefPointType::TYPE_INNER_U_TURN_RIGHT ||
                   point_type == RefPointType::TYPE_OUTER_U_TURN_RIGHT) {
            pen.setColor(QColor(0, 255, 255));
            point_item->setPen(pen);
            point_item->setBrush(QColor(0, 0, 128));
        } else if (point_type == RefPointType::TYPE_EDGE_WISE_LINE ||
                   point_type == RefPointType::TYPE_EDGE_WISE_CURVE) {
            pen.setColor(QColor(255, 0, 255));
            point_item->setPen(pen);
            point_item->setBrush(QColor(128, 0, 128));
        }
        QPointF ref_pt(_task.reference_line.at(i).pose.x,
                       _task.reference_line.at(i).pose.y);
        QRectF rect(ref_pt - QPointF(0.04, 0.04), ref_pt + QPointF(0.04, 0.04));
        point_item->setRect(rect);
        point_item->setVisible(true);
    }
    _points_group->update();
}
