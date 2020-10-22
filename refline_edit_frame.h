#ifndef TASK_REFLINE_EDITOR_REFLINE_EDIT_FRAME_H
#define TASK_REFLINE_EDITOR_REFLINE_EDIT_FRAME_H

#include <QFrame>
#include "data_manager.h"

namespace Ui {
class RefLineEditFrame;
}

class RefLineEditFrame : public QFrame
{
    Q_OBJECT

public:
    explicit RefLineEditFrame(DataManager* data_manager, QWidget *parent = 0);
    ~RefLineEditFrame();

signals:
    void emitStartEditRefLine();

private slots:
    void updateReflineInfoSlot();
    void editRefLineCheckdChangedSlot();

private:
    void setUnEditMode();
    void setEditMode();

private:
    DataManager* _data_manager;
    Ui::RefLineEditFrame *ui;
};

#endif // TASK_EDIT_FRAME_H
