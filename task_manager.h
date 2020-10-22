#ifndef TASK_REFLINE_EDITOR_TASK_MANAGER_H
#define TASK_REFLINE_EDITOR_TASK_MANAGER_H

#include <QGraphicsPathItem>
#include <QGraphicsItemGroup>
#include <task/task.hpp>

#include <QObject>

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

    void computeReflineDistance();
    void updateTaskItem();
    QGraphicsItemGroup *getTaskItemGroup();

private:

    void updatePathItem();
    void updatePointsGroup();

private:
    tergeo::task::Task _task;

    EditMode _edit_mode = MODE_NORMAL;

    QGraphicsItemGroup* _task_item;
    QGraphicsItemGroup* _points_group;
    QGraphicsPathItem* _path_item;
};

#endif // REF_LINE_H
