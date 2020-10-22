#ifndef TASK_REFLINE_EDITOR_VEHICLE_ITEM_H
#define TASK_REFLINE_EDITOR_VEHICLE_ITEM_H

#include <QObject>
#include <QGraphicsItemGroup>
#include <QGraphicsPathItem>
#include <QGraphicsLineItem>
#include <QGraphicsSimpleTextItem>

class VehicleItem : public QObject
{
    Q_OBJECT
public:
    VehicleItem(QObject* parent);
    ~VehicleItem();

    QGraphicsItemGroup* getVehicleGroup();
    void setVisible(bool flag);
    void update(const QTransform& transform);

private:
    void init();

private:
    QPointF _cnt_point;
    QPointF _top_left;
    QPointF _top_right;
    QPointF _bot_left;
    QPointF _bot_right;
    QGraphicsItemGroup* _group_root;
    QGraphicsPolygonItem* _polygon_item;
    QGraphicsLineItem* _line_1_item;
    QGraphicsLineItem* _line_2_item;
    QGraphicsEllipseItem* _ellipse_item;
};

#endif // VEHICLE_ITEM_H
