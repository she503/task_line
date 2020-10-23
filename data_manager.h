#ifndef TASK_REFLINE_EDITOR_DATA_MANAGER_H
#define TASK_REFLINE_EDITOR_DATA_MANAGER_H

#include <QObject>

#include "map_manager.h"
#include "task_manager.h"
#include "refline_manager.h"

class DataManager : public QObject
{
    Q_OBJECT
public:
    explicit DataManager(QObject *parent = nullptr);

    MapManager *getMapManager();
    TaskManager *getTaskManager();
    RefLineManager *getRefLineManager();

    void copyTaskToRefLine();
    void copyRefLineToTask();

private:
    MapManager* _map_manager;
    TaskManager* _task_manager;
    RefLineManager* _refline_manager;
};

#endif // DATA_MANAGER_H
