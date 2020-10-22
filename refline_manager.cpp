#include "refline_manager.h"

#include <QPen>
#include <QBrush>

RefLineManager::RefLineManager(QObject *parent) : QObject(parent)
{
    _refline_group = new QGraphicsItemGroup();
    _refline_group->setZValue(20);
    _path_item = new QGraphicsPathItem(_refline_group);
    _path_item->setZValue(21);
    _points_group = new QGraphicsItemGroup(_refline_group);
    _points_group->setZValue(22);
}

RefLine &RefLineManager::getRefLine()
{
    return _ref_line;
}

const RefLine &RefLineManager::getRefLine() const
{
    return _ref_line;
}

void RefLineManager::updateRefLine()
{
    emit emitRefLineUpdate();
    this->updateRefLineGroup();
}

QGraphicsItemGroup *RefLineManager::getEditRefLineGroup()
{
    return _refline_group;
}

void RefLineManager::updateRefLineGroup()
{
    this->updatePathItem();
    this->updateRefPointGroup();
    _refline_group->update();
}

void RefLineManager::updatePathItem()
{
    int pt_num = _ref_line.size();
    if (pt_num < 2) {
        _path_item->setVisible(false);
        return;
    }

    QPainterPath path;
    path.moveTo(_ref_line.first().pos);
    for (int i = 1; i < _ref_line.size(); ++i) {
        path.lineTo(_ref_line.at(i).pos);
    }
    QPen pen(QColor(0, 0, 255));
    pen.setWidthF(0.02);
    _path_item->setPen(pen);
    _path_item->setPath(path);
    _path_item->setVisible(true);
    _path_item->update();
}

void RefLineManager::updateRefPointGroup()
{
    int pt_num = _ref_line.size();
    while (pt_num > _points_group->childItems().size()) {
        QGraphicsEllipseItem* point_item = new QGraphicsEllipseItem(_points_group);
    }
    QPen pen; pen.setWidthF(0.01);
    QList<QGraphicsItem* > item_list = _points_group->childItems();
    for (int i = 0; i < item_list.size(); ++i) {
        if (i >= pt_num) {
            delete item_list[i];
            continue;
        }
        QGraphicsEllipseItem* point_item =
                dynamic_cast<QGraphicsEllipseItem*>(item_list[i]);
        point_item->setPen(pen);
        point_item->setBrush(PointTypeColorMap[_ref_line.at(i).type]);

        QRectF rect(_ref_line.at(i).pos - QPointF(0.04, 0.04),
                    _ref_line.at(i).pos + QPointF(0.04, 0.04));
        point_item->setRect(rect);
        point_item->setVisible(true);
    }
    _points_group->update();
}
