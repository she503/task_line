#ifndef TASK_REFLINE_EDITOR_TASK_EDIT_FRAME_H
#define TASK_REFLINE_EDITOR_TASK_EDIT_FRAME_H

#include <QFrame>
#include "data_manager.h"

namespace Ui {
class TaskEditFrame;
}

class TaskEditFrame : public QFrame
{
    Q_OBJECT

public:
    explicit TaskEditFrame(DataManager* data_manager, QWidget *parent = 0);
    ~TaskEditFrame();

private:
    DataManager* _data_manager;
    Ui::TaskEditFrame *ui;
};

#endif // TASK_EDIT_FRAME_H
