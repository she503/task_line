#include "refline_edit_frame.h"
#include "ui_refline_edit_frame.h"

#include <QMessageBox>

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
    connect(ui->pushButton_x_plus, SIGNAL(clicked(bool)),
            this, SLOT(plusXSlot()));
    connect(ui->pushButton_x_minus, SIGNAL(clicked(bool)),
            this, SLOT(minusXSlot()));
    connect(ui->pushButton_y_plus, SIGNAL(clicked(bool)),
            this, SLOT(plusYSlot()));
    connect(ui->pushButton_y_minus, SIGNAL(clicked(bool)),
            this, SLOT(minusYSlot()));
    connect(ui->pushButton_edge_dist_plus, SIGNAL(clicked(bool)),
            this, SLOT(plusEdgeDistSlot()));
    connect(ui->pushButton_edge_dist_minus, SIGNAL(clicked(bool)),
            this, SLOT(minusEdgeDistSlot()));
    connect(ui->pushButton_set_edge_dist, SIGNAL(clicked(bool)),
            this, SLOT(setEdgeDistSlot()));
    connect(ui->pushButton_delete, SIGNAL(clicked(bool)),
            this, SLOT(deleteSlot()));
    connect(ui->pushButton_resample, SIGNAL(clicked(bool)),
            this, SLOT(resampleSlot()));
    connect(ui->pushButton_smooth, SIGNAL(clicked(bool)),
            this, SLOT(smoothSlot()));

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
    connect(_refline_manager, SIGNAL(emitEdgeDistChanged()),
            this, SLOT(updateEdgeDistSlot()));

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
        root_item->child(i)->setText(2, ref_pt.edge_dist_info.is_edge_wise ?
                QString::number(ref_pt.edge_dist_info.edge_dist) : QString(""));
        root_item->child(i)->setData(0, IndexRole, i);
    }
}

void RefLineEditFrame::updateSelectedPointsSlot()
{
    int first_selected_index = -1;
    int first_selected_edge_wise_index = -1;
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
            if (first_selected_edge_wise_index < 0 &&
                    _refline_manager->getRefLine().at(i).edge_dist_info.is_edge_wise) {
                first_selected_edge_wise_index = i;
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
    if (first_selected_edge_wise_index >= 0) {
        ui->doubleSpinBox_edge_dist->setValue(_refline_manager->getRefLine().
                at(first_selected_edge_wise_index).edge_dist_info.edge_dist);
    } else {
        ui->doubleSpinBox_edge_dist->setValue(0);
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
            root_item->child(i)->setText(2, ref_pt.edge_dist_info.is_edge_wise ?
                    QString::number(ref_pt.edge_dist_info.edge_dist) : QString(""));
        }
    }
}

void RefLineEditFrame::updateEdgeDistSlot()
{
    int first_selected_edge_wise_index = -1;
    QTreeWidgetItem* root_item = ui->treeWidget->invisibleRootItem();
    for (int i = 0; i < root_item->childCount(); ++i) {
        if (i >= _refline_manager->getRefLine().size()) {
            break;
        }
        const RefPoint& ref_pt = _refline_manager->getRefLine().at(i);
        if (!ref_pt.edge_dist_info.is_edge_wise) {
            continue;
        }
        if (ref_pt.is_selected && first_selected_edge_wise_index < 0) {
            first_selected_edge_wise_index = i;
        }
        bool has_change = false;
        if (std::fabs(ref_pt.edge_dist_info.edge_dist - root_item->child(i)->text(2).toFloat()) > 1e-3) {
            has_change = true;
        }
        if (has_change) {
            root_item->child(i)->setText(2, QString::number(ref_pt.edge_dist_info.edge_dist));
        }
    }
    if (first_selected_edge_wise_index >= 0) {
        ui->doubleSpinBox_edge_dist->setValue(_refline_manager->getRefLine().
                at(first_selected_edge_wise_index).edge_dist_info.edge_dist);
    } else {
        ui->doubleSpinBox_edge_dist->setValue(0);
    }
}

void RefLineEditFrame::editRefLineCheckdChangedSlot()
{
    if (ui->checkBox_edit_ref_line->isChecked()) {
        if (!_data_manager->copyTaskToRefLine()) {
            QMessageBox::warning(this, tr("warning"), tr("Task refline is empty, please check!"));
            ui->checkBox_edit_ref_line->setChecked(false);
            this->setUnEditMode();
            return;
        }
        this->setEditMode();
    } else {
        this->setUnEditMode();
    }
}

void RefLineEditFrame::treeWidgetItemClickedSlot(QTreeWidgetItem *item, int column)
{
    if (column != 0) return;
    int cur_index = item->data(column, IndexRole).toInt();
    _refline_manager->updateSelectedPointIndex(cur_index);
}

void RefLineEditFrame::treeWidgetItemDoubleClickedSlot(QTreeWidgetItem *item, int column)
{
    if (column != 0) return;
    int cur_index = item->data(column, IndexRole).toInt();
    _refline_manager->setSelectedPointIndex(cur_index);
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

void RefLineEditFrame::plusXSlot()
{
    float plus_value = ui->doubleSpinBox_x->value();
    QPointF change_pos(plus_value, 0);
    _refline_manager->updateSelectedPointsPos(change_pos);
}

void RefLineEditFrame::minusXSlot()
{
    float plus_value = -ui->doubleSpinBox_x->value();
    QPointF change_pos(plus_value, 0);
    _refline_manager->updateSelectedPointsPos(change_pos);
}

void RefLineEditFrame::plusYSlot()
{
    float plus_value = ui->doubleSpinBox_y->value();
    QPointF change_pos(0, plus_value);
    _refline_manager->updateSelectedPointsPos(change_pos);
}

void RefLineEditFrame::minusYSlot()
{
    float plus_value = -ui->doubleSpinBox_y->value();
    QPointF change_pos(0, plus_value);
    _refline_manager->updateSelectedPointsPos(change_pos);
}

void RefLineEditFrame::plusEdgeDistSlot()
{
    _refline_manager->updateSelectedEdgeDist(ui->doubleSpinBox_adjust_edge_dist->value());
}

void RefLineEditFrame::minusEdgeDistSlot()
{
    _refline_manager->updateSelectedEdgeDist(-ui->doubleSpinBox_adjust_edge_dist->value());
}

void RefLineEditFrame::setEdgeDistSlot()
{
    _refline_manager->setSelectedEdgeDist(ui->doubleSpinBox_edge_dist->value());
}

void RefLineEditFrame::deleteSlot()
{
    _refline_manager->deleteSelectedPoints();
}

void RefLineEditFrame::resampleSlot()
{
    _refline_manager->resampleSelectedPoints();
}

void RefLineEditFrame::smoothSlot()
{
    _refline_manager->smoothSelectedPoints();
}

void RefLineEditFrame::resetSlot()
{
    _data_manager->copyTaskToRefLine();
    this->setEditMode();
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


