#ifndef TASK_REFLINE_EDITOR_TASK_CONTROL_FRAME_H
#define TASK_REFLINE_EDITOR_TASK_CONTROL_FRAME_H

#include <QFrame>

#include "data_manager.h"

namespace Ui {
class TaskControlFrame;
}

class TaskControlFrame : public QFrame
{
    Q_OBJECT

public:
    explicit TaskControlFrame(DataManager* data_manager, QWidget *parent = 0);
    ~TaskControlFrame();

signals:
    void emitStartDrawRefLine();
    void emitStartRosSpin();
    void emitStopRosSpin();

private slots:
    void loadMapSlot();
    void generateRoadEdgeSlot();

    void taskResourceChangedSlot(const int index);
    void loadTaskSlot();
    void saveTaskSlot();

    void loadBagSlot();
    void extractBagSlot();

    void beginRecordSlot();
    void stopRecordSlot();

    void updateTaskInfoSlot();

private:
    DataManager* _data_manager;

    Ui::TaskControlFrame *ui;
};

#endif // CONTROL_FRAME_H
