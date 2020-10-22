#ifndef TASK_REFLINE_EDITOR_DATA_MANAGER_H
#define TASK_REFLINE_EDITOR_DATA_MANAGER_H

#include <QObject>

#include "map_manager.h"
#include "task_manager.h"

class DataManager : public QObject
{
    Q_OBJECT
public:
    explicit DataManager(QObject *parent = nullptr);

    MapManager *getMapManager();
    TaskManager *getTaskManager();

    bool loadTaskFile(const std::string& task_file);
    bool extractBagData(const std::string& bag_file);
    void startRecordRefline();
    void stopRecordRefline();

    void setDrawReflineFlag(const bool flag);

signals:
    void emitStartRosSpin();
    void emitStopRosSpin();
    void emitReflineUpdate();
    void emitDrawReflineFlag(const bool flag);

public slots:
    void appendRefPoint(const float x, const float y, const float theta);
    void appendRefPoint(const float x, const float y);
    void popRefPoint();
    void finishDrawRefline();
    void clearRefPoint();

private:
    MapManager* _map_manager;
    TaskManager* _task_manager;

    float _resample_dist = 0.15;
};

#endif // DATA_MANAGER_H
