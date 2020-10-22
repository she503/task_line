#include "task_edit_frame.h"
#include "ui_task_edit_frame.h"

TaskEditFrame::TaskEditFrame(DataManager *data_manager, QWidget *parent) :
        _data_manager(data_manager), QFrame(parent), ui(new Ui::TaskEditFrame)
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

    connect(_data_manager, SIGNAL(emitReflineUpdate()), this, SLOT(updateReflineInfoSlot()));
}

TaskEditFrame::~TaskEditFrame()
{
    delete ui;
}

void TaskEditFrame::updateReflineInfoSlot()
{
    int pt_num = _data_manager->getTaskManager()->getRefline().size();
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
        root_item->child(i)->setText(0, QString("Point %1").arg(i));
        root_item->child(i)->setHidden(false);
//        root_item->child(i)->setData(0, IndexRole, i);
    }
}
