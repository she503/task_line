#ifndef TASK_REFLINE_EDITOR_TASK_MANAGER_H
#define TASK_REFLINE_EDITOR_TASK_MANAGER_H

#include <QGraphicsItemGroup>
#include "utils.h"

#include <QObject>

class TaskManager : public QObject
{
    Q_OBJECT
public:
    explicit TaskManager(QObject* parent = nullptr);

    enum EditMode {
        MODE_NORMAL = 0,
        MODE_RECORD = 1,
        MODE_DRAW = 2,
        MODE_EXTRACT_BAG = 3
    };

    bool loadTask(const std::string& file_path);
    bool saveTask(const std::string& save_dir, const std::string& map_name,
                  const std::string& task_name);

    bool extractBagData(const std::string& bag_file);
    void startRecordRefLine();
    void stopRecordRefLine();
    void startDrawRefLine();
    void finishDrawRefline();

    const tergeo::task::Task& getTask() const;
    QGraphicsItemGroup *getTaskItemGroup();

    void fromRefLine(const RefLine& ref_line);
    void toRefLine(RefLine& ref_line);

signals:
    void emitTaskUpdate();

public slots:
    void appendRefPoint(const float x, const float y, const float theta);
    void appendRefPoint(const float x, const float y);
    void popRefPoint();
    void clearRefLine();

private:
    void updateTask();
    void updateTaskItemGroupWithoutPoints();
    void updateTaskItemGroupWithPoints();
    void updatePathItemGroup();

private:
    tergeo::task::Task _task;
    EditMode _edit_mode = MODE_NORMAL;

    QGraphicsItemGroup* _task_group;
    QGraphicsItemGroup* _points_group;
    QGraphicsItemGroup* _path_group;
};

#endif // REF_LINE_H
