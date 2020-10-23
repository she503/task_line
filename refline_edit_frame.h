#ifndef TASK_REFLINE_EDITOR_REFLINE_EDIT_FRAME_H
#define TASK_REFLINE_EDITOR_REFLINE_EDIT_FRAME_H

#include <QFrame>
#include <QTreeWidgetItem>

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
    void emitUnEditMode();
    void emitEditMode();

private slots:
    void updateReflineInfoSlot();
    void updateSelectedPointsSlot();
    void updatePointsTypeSlot();

    void editRefLineCheckdChangedSlot();

    void treeWidgetItemClickedSlot(QTreeWidgetItem *item, int column);
    void treeWidgetItemDoubleClickedSlot(QTreeWidgetItem *item, int column);

    void resetSlot();
    void okSlot();

    void setPointsTypeSlot();


private:
    void setUnEditMode();
    void setEditMode();

private:
    DataManager* _data_manager;
    RefLineManager* _refline_manager;
    Ui::RefLineEditFrame *ui;
};

#endif // TASK_EDIT_FRAME_H
