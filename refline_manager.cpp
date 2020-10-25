#include "refline_manager.h"

#include <QPen>
#include <QBrush>

#include <QDebug>

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

void RefLineManager::setUnEditMode()
{
    this->clearRefLine();
}

void RefLineManager::updateRefLine()
{
    emit emitRefLineUpdate();
    this->updatePathItem();
    this->updateRefPointGroup();
}

void RefLineManager::updateSelectedPointIndex(const int index)
{
    int first_index = -1;
    for (int i = 0; i < _ref_line.size(); ++i) {
        if (_ref_line.at(i).is_selected) {
            first_index = i;
            break;
        }
    }
    if (first_index < 0) {
        this->updateSelectedPoints(index, index);
    } else if (first_index >= index) {
        this->updateSelectedPoints(-1, -1);
    } else {
        this->updateSelectedPoints(first_index, index);
    }
}

void RefLineManager::setSelectedPointIndex(const int index)
{
    this->updateSelectedPoints(index, index);
}

void RefLineManager::setSelectedPointRect(const QRectF &rect)
{
    bool has_enter_select = false;
    int first_index = -1, last_index = -1;
    for (int i = 0; i < _ref_line.size(); ++i) {
        bool is_selected = rect.contains(_ref_line.at(i).pos);
        if (is_selected && !has_enter_select) {
            has_enter_select = true;
            first_index = i;
        }
        if (!is_selected && has_enter_select) {
            last_index = i - 1;
            break;
        }
    }
    this->updateSelectedPoints(first_index, last_index);
}

void RefLineManager::setSelectedPointsType(const tergeo::task::ReferencePoint::Type& type)
{
    QList<QGraphicsItem* > item_list = _points_group->childItems();
    for (int i = 0; i < item_list.size(); ++i) {
        if (i >= _ref_line.size()) {
            break;
        }
        bool has_change = false;
        if (_ref_line.at(i).is_selected && _ref_line.at(i).type != type) {
            _ref_line[i].type = type;
            has_change = true;
        }
        if (has_change) {
            QGraphicsEllipseItem* point_item =
                    dynamic_cast<QGraphicsEllipseItem*>(item_list[i]);
            point_item->setBrush(PointTypeColorMap[_ref_line.at(i).type]);
        }
    }
    emit emitPointsTypeChanged();
}

void RefLineManager::updateSelectedPointsPos(const QPointF &changed_pos)
{
    QList<QGraphicsItem* > item_list = _points_group->childItems();
    for (int i = 0; i < item_list.size(); ++i) {
        if (i >= _ref_line.size()) {
            break;
        }
        if (!_ref_line.at(i).is_selected) {
            continue;
        }
        _ref_line[i].pos += changed_pos;
        QGraphicsEllipseItem* point_item =
                dynamic_cast<QGraphicsEllipseItem*>(item_list[i]);
        float point_radius = 0.08;
        QPointF pt_corner(point_radius, point_radius);
        QRectF rect(_ref_line.at(i).pos - pt_corner, _ref_line.at(i).pos + pt_corner);
        point_item->setRect(rect);
    }
    this->updatePathItem();
}

void RefLineManager::deleteSelectedPoints()
{
    QList<RefPoint>::iterator iter;
    for (iter = _ref_line.begin(); iter != _ref_line.end();) {
        if (iter->is_selected) {
            iter = _ref_line.erase(iter);
        } else {
            ++iter;
        }
    }
    this->updateRefLine();
}

void RefLineManager::resampleSelectedPoints()
{
    int first_index = -1, last_index = _ref_line.size() - 1;
    for (int i = 0; i < _ref_line.size(); ++i) {
        if (_ref_line.at(i).is_selected && first_index < 0) {
            first_index = i;
        }
        if (first_index >= 0 && !_ref_line.at(i).is_selected) {
            last_index = i - 1;
            break;
        }
    }
    if (first_index < 0) {
        first_index = 0;
    }
    if (last_index - first_index < 1) {
        return;
    }
    RefLine ref_line_tmp;
    _ref_line.swap(ref_line_tmp);
    QPointF last_dir = ref_line_tmp.at(first_index + 1).pos - ref_line_tmp.at(first_index).pos;
    QPointF cur_dir;
    for (int i = 0; i < ref_line_tmp.size(); ++i) {
        if (i <= first_index || i > last_index) {
            _ref_line.append(ref_line_tmp.at(i));
            continue;
        }
        cur_dir = ref_line_tmp.at(i).pos - _ref_line.last().pos;
        float dist = std::hypot(cur_dir.x(), cur_dir.y());
        if (dist < ResampleDist * 0.5) {
            continue;
        }
        if ((dist < ResampleDist * 8) &&
                (last_dir.x() * cur_dir.x() + last_dir.y() * cur_dir.y()) < 0) {
            continue;
        }
        last_dir = cur_dir;
        if (dist < ResampleDist * 1.5) {
            _ref_line.append(ref_line_tmp.at(i));
        } else {
            RefPoint ref_pt_tmp = _ref_line.last();
            float ratio = ResampleDist / dist;
            ref_pt_tmp.pos = ref_pt_tmp.pos + ratio * cur_dir;
            _ref_line.append(ref_pt_tmp);
            --i;
        }
    }
    this->updateRefLine();
}

void RefLineManager::smoothSelectedPoints()
{


    this->updateRefLine();
}

int RefLineManager::searchNearestPtIndex(const QPointF &point)
{
    float min_dist = std::numeric_limits<float>::max();
    int min_index = -1;
    for (int i = 0; i < _ref_line.size(); ++i) {
        float dist = std::hypot(point.x() - _ref_line.at(i).pos.x(),
                                point.y() - _ref_line.at(i).pos.y());
        if (dist < min_dist) {
            min_index = i;
            min_dist = dist;
        }
    }
    if (min_dist > SearchRadius) {
        return -1;
    }
    return min_index;
}

QGraphicsItemGroup *RefLineManager::getEditRefLineGroup()
{
    return _refline_group;
}

void RefLineManager::updateSelectedPoints(const int first, const int last)
{
    QList<QGraphicsItem* > item_list = _points_group->childItems();
    for (int i = 0; i < item_list.size(); ++i) {
        if (i >= _ref_line.size()) {
            break;
        }
        bool has_change = false;
        if (i >= first && i <= last) {
            if (!_ref_line.at(i).is_selected) {
                _ref_line[i].is_selected = true;
                has_change = true;
            }
        } else {
            if (_ref_line.at(i).is_selected) {
                _ref_line[i].is_selected = false;
                has_change = true;
            }
        }
        if (has_change) {
            QGraphicsEllipseItem* point_item =
                    dynamic_cast<QGraphicsEllipseItem*>(item_list[i]);
            float point_radius = _ref_line.at(i).is_selected ? 0.08 : 0.04;
            QPointF pt_corner(point_radius, point_radius);
            QRectF rect(_ref_line.at(i).pos - pt_corner, _ref_line.at(i).pos + pt_corner);
            point_item->setRect(rect);
        }
    }
    emit emitSelectedPointsChanged();
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
        point_item->setData(IndexRole, i);
        float point_radius = _ref_line.at(i).is_selected ? 0.08 : 0.04;
        QPointF pt_corner(point_radius, point_radius);
        QRectF rect(_ref_line.at(i).pos - pt_corner, _ref_line.at(i).pos + pt_corner);
        point_item->setRect(rect);
        point_item->setVisible(true);
    }
    _points_group->update();
}

void RefLineManager::clearRefLine()
{
    _ref_line.clear();
    _path_item->setVisible(false);
    QList<QGraphicsItem* > item_list = _points_group->childItems();
    for (int i = 0; i < item_list.size(); ++i) {
        delete item_list[i];
    }
}
