#ifndef TASK_REFLINE_EDITOR_TASK_MANAGER_H
#define TASK_REFLINE_EDITOR_TASK_MANAGER_H

#include <QMap>
#include <QGraphicsPathItem>
#include <QGraphicsItemGroup>
#include <task/task.hpp>

#include <QObject>

static QMap<tergeo::task::ReferencePoint::Type, QString> PointTypeStringMap {
    {tergeo::task::ReferencePoint::Type::TYPE_NULL, "NULL"},
    {tergeo::task::ReferencePoint::Type::TYPE_NORMAL, "NORMAL"},
    {tergeo::task::ReferencePoint::Type::TYPE_GO_STRAIGHT, "GO_STRAIGHT"},
    {tergeo::task::ReferencePoint::Type::TYPE_GO_STRAIGHT_CURVE, "GO_STRAIGHT_CURVE"},
    {tergeo::task::ReferencePoint::Type::TYPE_TURN_LEFT, "TURN_LEFT"},
    {tergeo::task::ReferencePoint::Type::TYPE_TURN_RIGHT, "TURN_RIGHT"},
    {tergeo::task::ReferencePoint::Type::TYPE_INNER_TURN_LEFT, "INNER_TURN_LEFT"},
    {tergeo::task::ReferencePoint::Type::TYPE_INNER_TURN_RIGHT, "INNER_TURN_RIGHT"},
    {tergeo::task::ReferencePoint::Type::TYPE_OUTER_TURN_LEFT, "OUTER_TURN_LEFT"},
    {tergeo::task::ReferencePoint::Type::TYPE_OUTER_TURN_RIGHT, "OUTER_TURN_RIGHT"},
    {tergeo::task::ReferencePoint::Type::TYPE_U_TURN_LEFT, "U_TURN_LEFT"},
    {tergeo::task::ReferencePoint::Type::TYPE_U_TURN_RIGHT, "U_TURN_RIGHT"},
    {tergeo::task::ReferencePoint::Type::TYPE_INNER_U_TURN_LEFT, "INNER_U_TURN_LEFT"},
    {tergeo::task::ReferencePoint::Type::TYPE_INNER_U_TURN_RIGHT, "INNER_U_TURN_RIGHT"},
    {tergeo::task::ReferencePoint::Type::TYPE_OUTER_U_TURN_LEFT, "OUTER_U_TURN_LEFT"},
    {tergeo::task::ReferencePoint::Type::TYPE_OUTER_U_TURN_RIGHT, "OUTER_U_TURN_RIGHT"},
    {tergeo::task::ReferencePoint::Type::TYPE_EDGE_WISE_LINE, "EDGE_WISE_LINE"},
    {tergeo::task::ReferencePoint::Type::TYPE_EDGE_WISE_CURVE, "EDGE_WISE_CURVE"}
};

struct PointProperty {
    int index;
    bool is_edge_wise = false;
    float edge_dist;
    bool is_checked = false;
};

class TaskManager : public QObject
{
    Q_OBJECT
public:
    explicit TaskManager(QObject* parent = nullptr);

    enum EditMode {
        MODE_NORMAL = 0,
        MODE_LOAD = 1,
        MODE_RECORD = 2,
        MODE_DRAW = 3,
        MODE_ADJUST = 4
    };

    bool loadTask(const std::string& file_path);
    bool saveTask(const std::string& save_dir, const std::string& map_name,
                  const std::string& task_name);
    const tergeo::task::Task& getTask() const;
    const tergeo::task::ReferenceLine& getRefline() const;

    void clearRefline();
    void setEditMode(EditMode edit_mode);
    void appendRefpoint(const tergeo::common::math::Pose2d& ref_pose);
    void updateRefPoint(const int index, const tergeo::common::math::Pose2d& ref_pose);
    void popRefPoint();

    void updateTask();
    QGraphicsItemGroup *getTaskItemGroup();

private:
    void updatePathItem();
    void updatePointsGroup();

private:
    tergeo::task::Task _task;
    QVector<PointProperty> _refpt_pros;

    EditMode _edit_mode = MODE_NORMAL;

    QGraphicsItemGroup* _task_item;
    QGraphicsItemGroup* _points_group;
    QGraphicsPathItem* _path_item;
};

#endif // REF_LINE_H
