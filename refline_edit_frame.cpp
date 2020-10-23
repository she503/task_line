#include "refline_edit_frame.h"
#include "ui_refline_edit_frame.h"

#define IndexRole Qt::UserRole + 10

RefLineEditFrame::RefLineEditFrame(DataManager *data_manager, QWidget *parent) :
        _data_manager(data_manager), QFrame(parent), ui(new Ui::RefLineEditFrame)
{
    ui->setupUi(this);

    _refline_manager = _data_manager->getRefLineManager();

    ui->comboBox_intent->clear();
    QMap<tergeo::task::ReferencePoint::Type, QString>::const_iterator iter;
    for (iter = PointTypeStringMap.constBegin();
         iter != PointTypeStringMap.constEnd(); ++iter) {
        ui->comboBox_intent->addItem(iter.value());
    }

    ui->treeWidget->setColumnCount(3);
    QStringList headers; headers << tr("Index") << tr("Type") << tr("Edge Dist");
    ui->treeWidget->setHeaderLabels(headers);

    connect(ui->checkBox_edit_ref_line, SIGNAL(clicked(bool)),
            this, SLOT(editRefLineCheckdChangedSlot()));
    connect(ui->pushButton_ok, SIGNAL(clicked(bool)), this, SLOT(okSlot()));
    connect(ui->pushButton_reset, SIGNAL(clicked(bool)), this, SLOT(resetSlot()));
    connect(ui->pushButton_set_intent, SIGNAL(clicked(bool)),
            this, SLOT(setPointsTypeSlot()));

    connect(ui->treeWidget, SIGNAL(itemClicked(QTreeWidgetItem*,int)),
            this, SLOT(treeWidgetItemClickedSlot(QTreeWidgetItem*,int)));
    connect(ui->treeWidget, SIGNAL(itemDoubleClicked(QTreeWidgetItem*,int)),
            this, SLOT(treeWidgetItemDoubleClickedSlot(QTreeWidgetItem*,int)));

    connect(_refline_manager, SIGNAL(emitRefLineUpdate()),
            this, SLOT(updateReflineInfoSlot()));
    connect(_refline_manager, SIGNAL(emitSelectedPointsChanged()),
            this, SLOT(updateSelectedPointsSlot()));
    connect(_refline_manager, SIGNAL(emitPointsTypeChanged()),
            this, SLOT(updatePointsTypeSlot()));

    this->setUnEditMode();
}

RefLineEditFrame::~RefLineEditFrame()
{
    delete ui;
}

void RefLineEditFrame::updateReflineInfoSlot()
{
    int pt_num = _refline_manager->getRefLine().size();
    QTreeWidgetItem* root_item = ui->treeWidget->invisibleRootItem();
    while (root_item->childCount() < pt_num) {
        QTreeWidgetItem* item = new QTreeWidgetItem(root_item);
        item->setCheckState(0, Qt::Unchecked);
    }
    for (int i = 0; i < root_item->childCount(); ++i) {
        if (i >= pt_num) {
            delete root_item->child(i);
            continue;
        }
        const RefPoint& ref_pt = _refline_manager->getRefLine().at(i);
        root_item->child(i)->setCheckState(0,
                ref_pt.is_selected ? Qt::Checked : Qt::Unchecked);
        root_item->child(i)->setText(0, QString::number(i));
        root_item->child(i)->setText(1, PointTypeStringMap[ref_pt.type]);
        root_item->child(i)->setBackgroundColor(1, PointTypeColorMap[ref_pt.type]);
        root_item->child(i)->setText(2,
                ref_pt.is_edge_wise ? QString::number(ref_pt.edge_dist) : QString(""));
        root_item->child(i)->setData(0, IndexRole, i);
    }
}

void RefLineEditFrame::updateSelectedPointsSlot()
{
    int first_selected_index = -1;
    QTreeWidgetItem* root_item = ui->treeWidget->invisibleRootItem();
    for (int i = 0; i < root_item->childCount(); ++i) {
        if (i >= _refline_manager->getRefLine().size()) {
            break;
        }
        bool has_change = false;
        if (_refline_manager->getRefLine().at(i).is_selected) {
            if (first_selected_index < 0) {
                first_selected_index = i;
            }
            if (root_item->child(i)->checkState(0) == Qt::Unchecked) {
                has_change = true;
            }
        } else {
            if (root_item->child(i)->checkState(0) == Qt::Checked) {
                has_change = true;
            }
        }
        if (has_change) {
            root_item->child(i)->setCheckState(0, _refline_manager->
                getRefLine().at(i).is_selected ? Qt::Checked : Qt::Unchecked);
        }
    }
    if (first_selected_index >= 0) {
        ui->comboBox_intent->setCurrentText(PointTypeStringMap[
                _refline_manager->getRefLine().at(first_selected_index).type]);
    } else {
        ui->comboBox_intent->setCurrentIndex(0);
    }
}

void RefLineEditFrame::updatePointsTypeSlot()
{
    QTreeWidgetItem* root_item = ui->treeWidget->invisibleRootItem();
    for (int i = 0; i < root_item->childCount(); ++i) {
        if (i >= _refline_manager->getRefLine().size()) {
            break;
        }
        const RefPoint& ref_pt = _refline_manager->getRefLine().at(i);
        bool has_change = false;
        if (PointTypeStringMap[ref_pt.type] != root_item->child(i)->text(1)) {
            has_change = true;
        }
        if (has_change) {
            root_item->child(i)->setText(1, PointTypeStringMap[ref_pt.type]);
            root_item->child(i)->setBackgroundColor(1, PointTypeColorMap[ref_pt.type]);
            root_item->child(i)->setText(2,
                    ref_pt.is_edge_wise ? QString::number(ref_pt.edge_dist) : QString(""));
        }
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

void RefLineEditFrame::treeWidgetItemClickedSlot(QTreeWidgetItem *item, int column)
{
    if (column != 0) return;

    int cur_index = item->data(column, IndexRole).toInt();
    int first_index = -1;
    QTreeWidgetItem* root_item = ui->treeWidget->invisibleRootItem();
    for (int i = 0; i < root_item->childCount(); ++i) {
        if (root_item->child(i)->checkState(column) == Qt::Checked) {
            first_index = i;
            break;
        }
    }
    if (first_index < 0) {
        _refline_manager->updateSelectedPoints(cur_index, cur_index);
    } else if (first_index >= cur_index) {
        _refline_manager->updateSelectedPoints(-1, -1);
    } else {
        _refline_manager->updateSelectedPoints(first_index, cur_index);
    }
}

void RefLineEditFrame::treeWidgetItemDoubleClickedSlot(QTreeWidgetItem *item, int column)
{
    if (column != 0) return;

    int cur_index = item->data(column, IndexRole).toInt();
    _refline_manager->updateSelectedPoints(cur_index, cur_index);
}

void RefLineEditFrame::resetSlot()
{
    _data_manager->copyTaskToRefLine();
    this->setEditMode();
}

void RefLineEditFrame::okSlot()
{
    _data_manager->copyRefLineToTask();
    ui->checkBox_edit_ref_line->setChecked(false);
    this->setUnEditMode();
}

void RefLineEditFrame::setPointsTypeSlot()
{
    QMap<tergeo::task::ReferencePoint::Type, QString>::const_iterator iter;
    for (iter = PointTypeStringMap.constBegin();
         iter != PointTypeStringMap.constEnd(); ++iter) {
        if (ui->comboBox_intent->currentText() == iter.value()) {
            _refline_manager->setSelectedPointsType(iter.key());
            break;
        }
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
    emit emitUnEditMode();
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
    emit emitEditMode();
}


