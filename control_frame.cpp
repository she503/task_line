#include "control_frame.h"
#include "ui_control_frame.h"

#include <QDebug>
#include <QFileDialog>
#include <QMessageBox>

ControlFrame::ControlFrame(DataManager *data_manager, QWidget *parent) :
       _data_manager(data_manager), QFrame(parent), ui(new Ui::ControlFrame)
{
    ui->setupUi(this);

    ui->groupBox_extract_bag->setVisible(false);
    ui->groupBox_record->setVisible(false);
    ui->groupBox_road_edge->setVisible(false);
    ui->groupBox_task->setVisible(false);
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
    connect(ui->pushButton_begin_record, SIGNAL(clicked(bool)),
            this, SLOT(beginRecordSlot()));
    connect(ui->pushButton_stop_record, SIGNAL(clicked(bool)),
            this, SLOT(stopRecordSlot()));

    connect(_data_manager, SIGNAL(emitReflineUpdate()),
            this, SLOT(updateReflineInfoSlot()));

}

ControlFrame::~ControlFrame()
{
    delete ui;
}

void ControlFrame::loadMapSlot()
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

void ControlFrame::generateRoadEdgeSlot()
{
    if (ui->lineEdit_edge_dist->text().isEmpty()) {
        QMessageBox::warning(this, tr("No edge dist"), tr("Please set a edge dist!"));
        return;
    }
    float edge_dist = ui->lineEdit_edge_dist->text().toFloat();
    _data_manager->getMapManager()->generateEdge(edge_dist);
}

void ControlFrame::taskResourceChangedSlot(const int index)
{
    ui->pushButton_save_task->setVisible(false);
    ui->lineEdit_task_name->setText("");
    ui->lineEdit_distance->setText("");
    if (index == 0) {
        ui->groupBox_record->setVisible(true);
        ui->pushButton_begin_record->setEnabled(true);
        ui->pushButton_stop_record->setEnabled(false);
        ui->groupBox_extract_bag->setVisible(false);
        ui->pushButton_load_task->setVisible(false);
    } else if (index == 1) {
        ui->groupBox_record->setVisible(false);
        ui->groupBox_extract_bag->setVisible(true);
        ui->pushButton_load_task->setVisible(false);
    } else if (index == 2) {
        ui->groupBox_record->setVisible(false);
        ui->groupBox_extract_bag->setVisible(false);
        ui->pushButton_load_task->setVisible(true);
    } else {
        ui->groupBox_record->setVisible(false);
        ui->groupBox_extract_bag->setVisible(false);
        ui->pushButton_load_task->setVisible(false);
        _data_manager->setDrawReflineFlag(true);
        return;
    }
    _data_manager->setDrawReflineFlag(false);
}

void ControlFrame::loadTaskSlot()
{
    QString file_path = QFileDialog::getOpenFileName(this, tr("Open Task"),
            QString::fromStdString(_data_manager->getMapManager()->getMapDir()), "*.task");
    if (file_path.isEmpty()) {
        QMessageBox::warning(this, tr("Empty task path"), tr("Please select a task file!"));
        return;
    }
    if (_data_manager->loadTaskFile(file_path.toStdString())) {
        QMessageBox::information(this, tr("Succeed"), tr("Load task succeed!"));
    } else {
        QMessageBox::critical(this, tr("Error"), tr("Load task failed!"));
        return;
    }
}

void ControlFrame::saveTaskSlot()
{
    if (ui->lineEdit_task_name->text().isEmpty()) {
        QMessageBox::critical(this, tr("Empty task name"), tr("Please give a name for this task!"));
        return;
    }
    if (_data_manager->getTaskManager()->getRefline().empty()) {
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

void ControlFrame::loadBagSlot()
{
    QString file_path = QFileDialog::getOpenFileName(this, tr("Select Bag Data"),
            QString::fromStdString(_data_manager->getMapManager()->getMapDir()), "*.bag");
    if (file_path.isEmpty()) {
        QMessageBox::warning(this, tr("Empty bag path"), tr("Please select a bag file!"));
        return;
    }
    ui->lineEdit_bag_path->setText(file_path);
}

void ControlFrame::extractBagSlot()
{
    if (ui->lineEdit_bag_path->text().isEmpty()) {
        QMessageBox::warning(this, tr("Empty bag path"), tr("Please select a bag file first!"));
        return;
    }
    if (_data_manager->extractBagData(ui->lineEdit_bag_path->text().toStdString())) {
        QMessageBox::information(this, tr("Succeed"), tr("Extract bag succeed!"));
    } else {
        QMessageBox::critical(this, tr("Error"), tr("Extract bag failed!"));
        return;
    }
}

void ControlFrame::beginRecordSlot()
{
    _data_manager->startRecordRefline();
    ui->pushButton_begin_record->setEnabled(false);
    ui->pushButton_stop_record->setEnabled(true);
}

void ControlFrame::stopRecordSlot()
{
    _data_manager->stopRecordRefline();
    ui->pushButton_begin_record->setEnabled(true);
    ui->pushButton_stop_record->setEnabled(false);
}

void ControlFrame::updateReflineInfoSlot()
{
    ui->lineEdit_task_name->setText(QString::fromStdString(
            _data_manager->getTaskManager()->getTask().name));
    ui->lineEdit_distance->setText(QString::number(
            _data_manager->getTaskManager()->getRefline().distance()));
    if (_data_manager->getTaskManager()->getRefline().empty()) {
        ui->pushButton_save_task->setVisible(false);
    } else {
        ui->pushButton_save_task->setVisible(true);
    }
}
