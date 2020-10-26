#include "refline_manager.h"

#include <QPen>
#include <QBrush>

#include <QDebug>

RefLineManager::RefLineManager(MapManager *map_manager, QObject *parent) :
        _map_manager(map_manager), QObject(parent)
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

void RefLineManager::updateEdgeDistAndEdgeDir()
{
    for (int i = 0; i < _ref_line.size(); ++i) {
        if (!_ref_line.at(i).edge_dist_info.is_edge_wise) {
            continue;
        }
        _map_manager->calEdgeDistInfo(_ref_line[i]);
    }
}

void RefLineManager::updateSelectedPointIndex(const int index)
{
    int first_index = _ref_line.size();
    for (int i = 0; i < _ref_line.size(); ++i) {
        if (_ref_line.at(i).is_selected) {
            first_index = i;
            break;
        }
    }
    if (first_index >= index) {
        this->updateSelectedPoints(index, index);
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
            _ref_line[i].edge_dist_info.is_edge_wise = (
                        type == tergeo::task::ReferencePoint::Type::TYPE_EDGE_WISE_LINE ||
                        type == tergeo::task::ReferencePoint::Type::TYPE_EDGE_WISE_CURVE);
            if (_ref_line.at(i).edge_dist_info.is_edge_wise) {
                _map_manager->calEdgeDistInfo(_ref_line[i]);
            }
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
    bool has_change_edge_wise_point = false;
    for (int i = 0; i < item_list.size(); ++i) {
        if (i >= _ref_line.size()) {
            break;
        }
        if (!_ref_line.at(i).is_selected) {
            continue;
        }
        _ref_line[i].pos += changed_pos;
        if (_ref_line.at(i).edge_dist_info.is_edge_wise) {
            _map_manager->calEdgeDistInfo(_ref_line[i]);
            has_change_edge_wise_point = true;
        }
        QGraphicsEllipseItem* point_item =
                dynamic_cast<QGraphicsEllipseItem*>(item_list[i]);
        float point_radius = 0.08;
        QPointF pt_corner(point_radius, point_radius);
        QRectF rect(_ref_line.at(i).pos - pt_corner, _ref_line.at(i).pos + pt_corner);
        point_item->setRect(rect);
    }
    emit emitEdgeDistChanged();
    this->updatePathItem();
}

void RefLineManager::setSelectedEdgeDist(const float &edge_dist)
{
    QList<QGraphicsItem* > item_list = _points_group->childItems();
    for (int i = 0; i < item_list.size(); ++i) {
        if (i >= _ref_line.size()) {
            break;
        }
        if (!_ref_line.at(i).is_selected || !_ref_line.at(i).edge_dist_info.is_edge_wise) {
            continue;
        }
        float changed_edge_dist = edge_dist - _ref_line.at(i).edge_dist_info.edge_dist;
        this->adjustEdgeDist(_ref_line[i], changed_edge_dist);
        QGraphicsEllipseItem* point_item =
                dynamic_cast<QGraphicsEllipseItem*>(item_list[i]);
        float point_radius = 0.08;
        QPointF pt_corner(point_radius, point_radius);
        QRectF rect(_ref_line.at(i).pos - pt_corner, _ref_line.at(i).pos + pt_corner);
        point_item->setRect(rect);
    }
    emit emitEdgeDistChanged();
    this->updatePathItem();
}

void RefLineManager::updateSelectedEdgeDist(const float &changed_dist)
{
    QList<QGraphicsItem* > item_list = _points_group->childItems();
    for (int i = 0; i < item_list.size(); ++i) {
        if (i >= _ref_line.size()) {
            break;
        }
        if (!_ref_line.at(i).is_selected || !_ref_line.at(i).edge_dist_info.is_edge_wise) {
            continue;
        }
        this->adjustEdgeDist(_ref_line[i], changed_dist);
        QGraphicsEllipseItem* point_item =
                dynamic_cast<QGraphicsEllipseItem*>(item_list[i]);
        float point_radius = 0.08;
        QPointF pt_corner(point_radius, point_radius);
        QRectF rect(_ref_line.at(i).pos - pt_corner, _ref_line.at(i).pos + pt_corner);
        point_item->setRect(rect);
    }
    emit emitEdgeDistChanged();
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
    this->resampleSelectedPointsPrivate();
    this->updateRefLine();
}

void RefLineManager::smoothSelectedPoints()
{
    this->resampleSelectedPointsPrivate();

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

    int smooth_neighbor_num = 4;
    for (int i = 0; i < ref_line_tmp.size(); ++i) {
        if (i <= first_index || i >= last_index) {
            _ref_line.append(ref_line_tmp.at(i));
            continue;
        }
        QList<QPointF> points;
        int cur_index = 0;
        for (int j = -smooth_neighbor_num; j <= smooth_neighbor_num; ++j) {
            int idx = i + j;
            if (idx < 0 || idx >= ref_line_tmp.size()) {
                continue;
            }
            points.append(ref_line_tmp.at(idx).pos);
            if (idx == i) {
                cur_index = points.size() - 1;
            }
        }
        if (points.size() < smooth_neighbor_num) {
            _ref_line.append(ref_line_tmp.at(i));
            continue;
        }
        Eigen::Vector3d line_coeff;
        this->fitLine(points, line_coeff);
        double theta = atan2(line_coeff(1), line_coeff(0));
        if (theta < 0) {
            theta = M_PI / 2 + theta;
        } else {
            theta = theta - M_PI / 2;
        }
        RefPoint ref_pt_tmp = ref_line_tmp.at(i);
        this->fitQuadricCurve(points, theta, cur_index, ref_pt_tmp.pos);
        _ref_line.append(ref_pt_tmp);
    }
    this->resampleSelectedPointsPrivate();
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

void RefLineManager::resampleSelectedPointsPrivate()
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
            if (_ref_line.last().edge_dist_info.is_edge_wise) {
                _map_manager->calEdgeDistInfo(_ref_line.last());
            }
            --i;
        }
    }
}

void RefLineManager::adjustEdgeDist(RefPoint &ref_point, const float adjust_dist)
{
    ref_point.edge_dist_info.edge_dist += adjust_dist;
    QPointF pt_tmp(ref_point.edge_dist_info.edge_dir.x(),
                   ref_point.edge_dist_info.edge_dir.y());
    ref_point.pos += pt_tmp * adjust_dist;
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

void RefLineManager::fitLine(const QList<QPointF> &points, Eigen::Vector3d &line_coeff)
{
    Eigen::Matrix<double, 2, Eigen::Dynamic> mat(2, points.size());
    for (int i = 0; i < points.size(); ++i) {
        mat.col(i) << points.at(i).x(), points.at(i).y();
    }
    Eigen::Matrix<double, 2, 1> cnt_pt;
    cnt_pt = mat.rowwise().mean();
    Eigen::Matrix<double, 2, Eigen::Dynamic> mat_unit = mat.colwise() - cnt_pt;
    Eigen::Matrix<double, 2, 2> H = mat_unit * mat_unit.transpose();
    Eigen::JacobiSVD<Eigen::Matrix<double, 2, 2> > svd(
                H, Eigen::ComputeFullU | Eigen::ComputeFullV);
    Eigen::Matrix<double, 2, 2> svd_v = svd.matrixV();
    line_coeff.block<2, 1>(0, 0) = svd_v.col(1);
    line_coeff(2) = -svd_v.col(1).transpose() * cnt_pt;
}

void RefLineManager::fitQuadricCurve(const QList<QPointF> &points,
                                     Eigen::Vector2d &cnt_pt,
                                     Eigen::Vector3d &curve_coeff)
{
    Eigen::Matrix<double, 2, Eigen::Dynamic> pts_mat(2, points.size());
    for (int i = 0; i < points.size(); ++i) {
        pts_mat(0, i) = points.at(i).x();
        pts_mat(1, i) = points.at(i).y();
    }
    cnt_pt = pts_mat.rowwise().mean();
    Eigen::Matrix<double, 2, Eigen::Dynamic> pts_in = pts_mat.colwise() - cnt_pt;
    Eigen::Matrix<double, 3, Eigen::Dynamic> pt_coeff(3, pts_in.cols());
    for (int i = 0; i < pts_in.cols(); ++i) {
        pt_coeff(2, i) = 1;
        pt_coeff(1, i) = pts_in(0, i);
        pt_coeff(0, i) = pts_in(0, i) * pts_in(0, i);
    }
    Eigen::Matrix<double, Eigen::Dynamic, 1> l(pts_in.cols(), 1);
    int iter = 0;
    while(iter++ < 5) {
        l = pts_in.row(1).transpose() - (curve_coeff.transpose() * pt_coeff).transpose();
        Eigen::Matrix<double, 3, 1> delta = (pt_coeff * pt_coeff.transpose()).inverse() * pt_coeff * l;
        curve_coeff += delta;
        if (delta.norm() < 1e-3) {
            break;
        }
    }
    return;
}

void RefLineManager::fitQuadricCurve(const QList<QPointF> &points,
                                     double rot_angle, int index, QPointF &pt_dst)
{
    Eigen::Matrix<double, 2, Eigen::Dynamic> pts_mat(2, points.size());
    for (int i = 0; i < points.size(); ++i) {
        pts_mat(0, i) = points.at(i).x();
        pts_mat(1, i) = points.at(i).y();
    }
    Eigen::Vector2d cnt_pt = pts_mat.rowwise().mean();
    Eigen::Matrix<double, 2, Eigen::Dynamic> pts_cnt = pts_mat.colwise() - cnt_pt;

    Eigen::Matrix2d rot_mat;
    rot_mat << cos(rot_angle), -sin(rot_angle),
               sin(rot_angle), cos(rot_angle);

    Eigen::Matrix<double, 2, Eigen::Dynamic> pts_in(2, points.size());
    for (int i = 0; i < pts_cnt.cols(); ++i) {
        Eigen::Vector2d pt_ori = pts_cnt.col(i).transpose();
        Eigen::Vector2d pt_tmp = rot_mat * pt_ori;
        pts_in(0, i) = pt_tmp(0);
        pts_in(1, i) = pt_tmp(1);
    }

    Eigen::Matrix<double, 3, Eigen::Dynamic> pt_coeff(3, pts_in.cols());
    for (int i = 0; i < pts_in.cols(); ++i) {
        pt_coeff(2, i) = 1;
        pt_coeff(1, i) = pts_in(0, i);
        pt_coeff(0, i) = pts_in(0, i) * pts_in(0, i);
    }
    Eigen::Matrix<double, Eigen::Dynamic, 1> l(pts_in.cols(), 1);
    int iter = 0;
    Eigen::Vector3d curve_coeff(0, 0, 0);
    while(iter++ < 5) {
        l = pts_in.row(1).transpose() - (curve_coeff.transpose() * pt_coeff).transpose();
        Eigen::Matrix<double, 3, 1> delta = (pt_coeff * pt_coeff.transpose()).inverse() * pt_coeff * l;
        curve_coeff += delta;
        if (delta.norm() < 1e-3) {
            break;
        }
    }
    double tmp_x = pts_in(0, index);
    double tmp_y = tmp_x * tmp_x * curve_coeff(0) + tmp_x * curve_coeff(1) + curve_coeff(2);
    Eigen::Vector2d tmp_pt(tmp_x, tmp_y);
    Eigen::Vector2d tmp_pt2 = rot_mat.inverse() * tmp_pt + cnt_pt;
    pt_dst.setX(tmp_pt2(0));
    pt_dst.setY(tmp_pt2(1));
}
