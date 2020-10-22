#ifndef TASK_REFLINE_EDITOR_CONTROL_FRAME_H
#define TASK_REFLINE_EDITOR_CONTROL_FRAME_H

#include <QFrame>

#include "data_manager.h"

namespace Ui {
class ControlFrame;
}

class ControlFrame : public QFrame
{
    Q_OBJECT

public:
    explicit ControlFrame(DataManager* data_manager, QWidget *parent = 0);
    ~ControlFrame();

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

    void updateReflineInfoSlot();

private:
    DataManager* _data_manager;

    Ui::ControlFrame *ui;
};

#endif // CONTROL_FRAME_H
