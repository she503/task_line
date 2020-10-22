#include "refline_edit_frame.h"
#include "ui_refline_edit_frame.h"

RefLineEditFrame::RefLineEditFrame(DataManager *data_manager, QWidget *parent) :
        _data_manager(data_manager), QFrame(parent), ui(new Ui::RefLineEditFrame)
{
    ui->setupUi(this);

    ui->comboBox_intent->clear();
    QMap<tergeo::task::ReferencePoint::Type, QString>::const_iterator iter;
    for (iter = PointTypeStringMap.constBegin(); iter != PointTypeStringMap.constEnd(); ++iter) {
        ui->comboBox_intent->addItem(iter.value());
    }

    ui->treeWidget->setColumnCount(3);
    QStringList headers; headers << tr("Index") << tr("Type") << tr("Edge Dist");
    ui->treeWidget->setHeaderLabels(headers);

    connect(ui->checkBox_edit_ref_line, SIGNAL(clicked(bool)),
            this, SLOT(editRefLineCheckdChangedSlot()));

    connect(_data_manager->getRefLineManager(), SIGNAL(emitRefLineUpdate()),
            this, SLOT(updateReflineInfoSlot()));

    this->setUnEditMode();
}

RefLineEditFrame::~RefLineEditFrame()
{
    delete ui;
}

void RefLineEditFrame::updateReflineInfoSlot()
{
    int pt_num = _data_manager->getRefLineManager()->getRefLine().size();
    QTreeWidgetItem* root_item = ui->treeWidget->invisibleRootItem();
    while (root_item->childCount() < pt_num) {
        QTreeWidgetItem* item = new QTreeWidgetItem(root_item);
        item->setCheckState(0, Qt::Checked);
    }
    for (int i = 0; i < root_item->childCount(); ++i) {
        if (i >= pt_num) {
            delete root_item->child(i);
            continue;
        }
//        root_item->child(i)->setCheckState(0,
//                _task_record_manager->isChecked(i) ? Qt::Checked : Qt::Unchecked);
        root_item->child(i)->setText(0, QString::number(i));
        root_item->child(i)->setHidden(false);
//        root_item->child(i)->setData(0, IndexRole, i);
    }
}

void RefLineEditFrame::editRefLineCheckdChangedSlot()
{
    if (ui->checkBox_edit_ref_line->isChecked()) {
        _data_manager->copyTaskToRefLine();
        this->setEditMode();
    } else {
        this->setUnEditMode();
    }
}

void RefLineEditFrame::setUnEditMode()
{
    ui->pushButton_ok->setEnabled(false);
    ui->checkBox_edit_ref_line->setEnabled(true);
    ui->groupBox_adjust_edge_dist->setEnabled(false);
    ui->groupBox_adjust_pos->setEnabled(false);
    ui->groupBox_button->setEnabled(false);
    ui->groupBox_intent->setEnabled(false);
    ui->treeWidget->setEnabled(false);
    ui->treeWidget->clear();
}

void RefLineEditFrame::setEditMode()
{
    ui->pushButton_ok->setEnabled(true);
    ui->checkBox_edit_ref_line->setEnabled(false);
    ui->groupBox_adjust_edge_dist->setEnabled(true);
    ui->groupBox_adjust_pos->setEnabled(true);
    ui->groupBox_button->setEnabled(true);
    ui->groupBox_intent->setEnabled(true);
    ui->treeWidget->setEnabled(true);
}
