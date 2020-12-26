#ifndef TASK_REFLINE_EDITOR_REFLINE_MANAGER_H
#define TASK_REFLINE_EDITOR_REFLINE_MANAGER_H

#include <QObject>
#include <QGraphicsPathItem>
#include <QGraphicsItemGroup>
#include <Eigen/Core>
#include <Eigen/Dense>
#include "utils.h"
#include "map_manager.h"

#define IndexRole Qt::UserRole + 10

class RefLineManager : public QObject
{
    Q_OBJECT
public:
    explicit RefLineManager(MapManager* map_manager, QObject *parent = nullptr);

    RefLine &getRefLine();
    const RefLine &getRefLine() const;
    void setUnEditMode();

    void updateRefLine();
    void updateEdgeDistAndEdgeDir();

    void updateSelectedPointIndex(const int index);
    void setSelectedPointIndex(const int index);
    void setSelectedPointRect(const QRectF& rect);

    void setSelectedPointsType(const tergeo::task::ReferencePoint::Type& type);
    void updateSelectedPointsPos(const QPointF& changed_pos);
    void setSelectedEdgeDist(const float& edge_dist);
    void updateSelectedEdgeDist(const float& changed_dist);
    void deleteSelectedPoints();
    void resampleSelectedPoints();
    void smoothSelectedPoints();

    int searchNearestPtIndex(const QPointF& point);

    QGraphicsItemGroup *getEditRefLineGroup();

signals:
    void emitRefLineUpdate();
    void emitSelectedPointsChanged();
    void emitPointsTypeChanged();
    void emitEdgeDistChanged();

private:
    void updateSelectedPoints(const int first, const int last);
    void resampleSelectedPointsPrivate();
    void adjustEdgeDist(RefPoint& ref_point, const float adjust_dist);

    void updatePathItem();
    void updateRefPointGroup();

    void clearRefLine();

private:
    void fitLine(const QList<QPointF>& points,
                 Eigen::Vector3d& line_coeff);
    void fitQuadricCurve(const QList<QPointF>& points,
                         Eigen::Vector2d& cnt_pt,
                         Eigen::Vector3d& curve_coeff);
    void fitQuadricCurve(const QList<QPointF>& points,
                         double rot_angle, int index, QPointF& pt_dst);

private:
    RefLine _ref_line;
    MapManager* _map_manager;

    QGraphicsItemGroup* _refline_group;
    QGraphicsItemGroup* _points_group;
    QGraphicsPathItem* _path_item;
};

#endif // REFLINE_MANAGER_H
