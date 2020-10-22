#include "task_edit_frame.h"
#include "ui_task_edit_frame.h"

TaskEditFrame::TaskEditFrame(DataManager *data_manager, QWidget *parent) :
        _data_manager(data_manager), QFrame(parent), ui(new Ui::TaskEditFrame)
{
    ui->setupUi(this);
}

TaskEditFrame::~TaskEditFrame()
{
    delete ui;
}
