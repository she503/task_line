#include "task_control_frame.h"
#include "ui_task_control_frame.h"

#include <QDebug>
#include <QFileDialog>
#include <QMessageBox>

TaskControlFrame::TaskControlFrame(DataManager *data_manager, QWidget *parent) :
       _data_manager(data_manager), QFrame(parent), ui(new Ui::TaskControlFrame)
{
    ui->setupUi(this);

    ui->groupBox_extract_bag->setVisible(false);
    ui->groupBox_record->setVisible(false);
    ui->groupBox_road_edge->setVisible(false);
    ui->groupBox_task->setVisible(false);
    ui->groupBox_draw->setVisible(false);
    ui->pushButton_load_task->setVisible(false);
    ui->pushButton_save_task->setVisible(false);

    connect(ui->pushButton_load_map, SIGNAL(clicked(bool)),
            this, SLOT(loadMapSlot()));
    connect(ui->pushButton_generate_edge, SIGNAL(clicked(bool)),
            this, SLOT(generateRoadEdgeSlot()));
    connect(ui->comboBox_task_resource, SIGNAL(currentIndexChanged(int)),
            this, SLOT(taskResourceChangedSlot(int)));
    connect(ui->pushButton_load_task, SIGNAL(clicked(bool)),
            this, SLOT(loadTaskSlot()));
    connect(ui->pushButton_save_task, SIGNAL(clicked(bool)),
            this, SLOT(saveTaskSlot()));
    connect(ui->pushButton_load_bag, SIGNAL(clicked(bool)),
            this, SLOT(loadBagSlot()));
    connect(ui->pushButton_extract_bag, SIGNAL(clicked(bool)),
            this, SLOT(extractBagSlot()));
    connect(ui->pushButton_start_record, SIGNAL(clicked(bool)),
            this, SLOT(startRecordSlot()));
    connect(ui->pushButton_stop_record, SIGNAL(clicked(bool)),
            this, SLOT(stopRecordSlot()));
    connect(ui->pushButton_start_draw, SIGNAL(clicked(bool)),
            this, SLOT(startDrawSlot()));
    connect(ui->pushButton_stop_draw, SIGNAL(clicked(bool)),
            this, SLOT(stopDrawSlot()));

    connect(_data_manager->getTaskManager(), SIGNAL(emitTaskUpdate()),
            this, SLOT(updateTaskInfoSlot()));
}

TaskControlFrame::~TaskControlFrame()
{
    delete ui;
}

void TaskControlFrame::refLineUnEditModeSlot()
{
    this->setEnabled(true);
}

void TaskControlFrame::refLineEditModeSlot()
{
    this->setEnabled(false);
}

void TaskControlFrame::loadMapSlot()
{
    QString map_dir = QFileDialog::getExistingDirectory(this);
    if (map_dir.isEmpty()) {
        QMessageBox::warning(this, tr("Empty map dir"), tr("Please select a map dir!"));
        return;
    }
    if (!_data_manager->getMapManager()->loadMap(map_dir.toStdString())) {
        return;
    }
    ui->lineEdit_map_dir->setText(map_dir);
    ui->lineEdit_map_name->setText(QString::fromStdString(
            _data_manager->getMapManager()->getMapName()));

    ui->groupBox_road_edge->setVisible(true);
    ui->groupBox_task->setVisible(true);
    ui->comboBox_task_resource->setCurrentIndex(2);
}

void TaskControlFrame::generateRoadEdgeSlot()
{
    if (ui->lineEdit_edge_dist->text().isEmpty()) {
        QMessageBox::warning(this, tr("No edge dist"), tr("Please set a edge dist!"));
        return;
    }
    float edge_dist = ui->lineEdit_edge_dist->text().toFloat();
    _data_manager->getMapManager()->generateEdge(edge_dist);
}

void TaskControlFrame::taskResourceChangedSlot(const int index)
{
    _data_manager->getTaskManager()->clearRefLine();
    ui->pushButton_save_task->setVisible(false);
    ui->lineEdit_task_name->setText("");
    ui->lineEdit_distance->setText("");
    if (index == 0) {
        ui->groupBox_record->setVisible(true);
        ui->pushButton_start_record->setEnabled(true);
        ui->pushButton_stop_record->setEnabled(false);
        ui->groupBox_extract_bag->setVisible(false);
        ui->pushButton_load_task->setVisible(false);
        ui->groupBox_draw->setVisible(false);
    } else if (index == 1) {
        ui->groupBox_record->setVisible(false);
        ui->groupBox_extract_bag->setVisible(true);
        ui->pushButton_load_bag->setEnabled(true);
        ui->pushButton_extract_bag->setEnabled(false);
        ui->pushButton_load_task->setVisible(false);
        ui->groupBox_draw->setVisible(false);
    } else if (index == 2) {
        ui->groupBox_record->setVisible(false);
        ui->groupBox_extract_bag->setVisible(false);
        ui->pushButton_load_task->setVisible(true);
        ui->groupBox_draw->setVisible(false);
    } else {
        ui->groupBox_record->setVisible(false);
        ui->groupBox_extract_bag->setVisible(false);
        ui->pushButton_load_task->setVisible(false);
        ui->groupBox_draw->setVisible(true);
        ui->pushButton_start_draw->setEnabled(true);
        ui->pushButton_stop_draw->setEnabled(false);
    }
}

void TaskControlFrame::loadTaskSlot()
{
    QString file_path = QFileDialog::getOpenFileName(this, tr("Open Task"),
            QString::fromStdString(_data_manager->getMapManager()->getMapDir()), "*.task");
    if (file_path.isEmpty()) {
        QMessageBox::warning(this, tr("Empty task path"), tr("Please select a task file!"));
        return;
    }
    if (_data_manager->getTaskManager()->loadTask(file_path.toStdString())) {
        QMessageBox::information(this, tr("Succeed"), tr("Load task succeed!"));
    } else {
        QMessageBox::critical(this, tr("Error"), tr("Load task failed!"));
        return;
    }
}

void TaskControlFrame::saveTaskSlot()
{
    if (ui->lineEdit_task_name->text().isEmpty()) {
        QMessageBox::critical(this, tr("Empty task name"), tr("Please give a name for this task!"));
        return;
    }
    if (_data_manager->getTaskManager()->getTask().reference_line.empty()) {
        QMessageBox::critical(this, tr("Empty refline"), tr("Refline is empty!"));
        return;
    }
    if (!_data_manager->getTaskManager()->saveTask(
                _data_manager->getMapManager()->getMapDir(),
                _data_manager->getMapManager()->getMapName(),
                ui->lineEdit_task_name->text().toStdString())) {
        QMessageBox::critical(this, tr("Failed"), tr("Save refline failed!"));
    } else {
        QMessageBox::information(this, tr("Succeed"), tr("Save refline succeed!"));
    }
}

void TaskControlFrame::loadBagSlot()
{
    QString file_path = QFileDialog::getOpenFileName(this, tr("Select Bag Data"),
            QString::fromStdString(_data_manager->getMapManager()->getMapDir()), "*.bag");
    if (file_path.isEmpty()) {
        QMessageBox::warning(this, tr("Empty bag path"), tr("Please select a bag file!"));
        ui->pushButton_extract_bag->setEnabled(false);
        return;
    }
    ui->lineEdit_bag_path->setText(file_path);
    ui->pushButton_extract_bag->setEnabled(true);
}

void TaskControlFrame::extractBagSlot()
{
    if (ui->lineEdit_bag_path->text().isEmpty()) {
        QMessageBox::warning(this, tr("Empty bag path"), tr("Please select a bag file first!"));
        return;
    }
    if (_data_manager->getTaskManager()->extractBagData(
                ui->lineEdit_bag_path->text().toStdString())) {
        QMessageBox::information(this, tr("Succeed"), tr("Extract bag succeed!"));
    } else {
        QMessageBox::critical(this, tr("Error"), tr("Extract bag failed!"));
        return;
    }
}

void TaskControlFrame::startRecordSlot()
{
    _data_manager->getTaskManager()->startRecordRefLine();
    emit emitStartRosSpin();
    ui->pushButton_start_record->setEnabled(false);
    ui->pushButton_stop_record->setEnabled(true);
}

void TaskControlFrame::stopRecordSlot()
{
    _data_manager->getTaskManager()->stopRecordRefLine();
    emit emitStopRosSpin();
    ui->pushButton_start_record->setEnabled(true);
    ui->pushButton_stop_record->setEnabled(false);
}

void TaskControlFrame::startDrawSlot()
{
    _data_manager->getTaskManager()->startDrawRefLine();
    emit emitStartDrawRefLine();
    ui->pushButton_start_draw->setEnabled(false);
    ui->pushButton_stop_draw->setEnabled(true);
}

void TaskControlFrame::stopDrawSlot()
{
    _data_manager->getTaskManager()->finishDrawRefline();
    emit emitStopDrawRefLine();
    ui->pushButton_start_draw->setEnabled(true);
    ui->pushButton_stop_draw->setEnabled(false);
}

void TaskControlFrame::updateTaskInfoSlot()
{
    ui->lineEdit_task_name->setText(QString::fromStdString(
            _data_manager->getTaskManager()->getTask().name));
    ui->lineEdit_distance->setText(QString::number(
            _data_manager->getTaskManager()->getTask().reference_line.distance()));
    if (_data_manager->getTaskManager()->getTask().reference_line.empty()) {
        ui->pushButton_save_task->setVisible(false);
    } else {
        ui->pushButton_save_task->setVisible(true);
    }
}

