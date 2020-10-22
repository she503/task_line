#include "vehicle_item.h"
#include <QBrush>
#include <QPen>
#include <iostream>

#include <vehicle/vehicle.hpp>

VehicleItem::VehicleItem(QObject *parent) : QObject(parent) {
    this->init();
}

VehicleItem::~VehicleItem() { }

QGraphicsItemGroup *VehicleItem::getVehicleGroup()
{
    return _group_root;
}


void VehicleItem::setVisible(bool flag)
{
    _group_root->setVisible(flag);
}

void VehicleItem::update(const QTransform &transform)
{
    _group_root->setTransform(transform);
}

void VehicleItem::init()
{
    _group_root = new QGraphicsItemGroup();
    _group_root->setZValue(30);

    _polygon_item = new QGraphicsPolygonItem(_group_root);
    _polygon_item->setBrush(QBrush(QColor(0, 255, 0)));
    _polygon_item->setOpacity(0.9);
    QPen pen(QColor(255, 255, 0));
    pen.setWidthF(0.1);
    _polygon_item->setPen(pen);

    _line_1_item = new QGraphicsLineItem(_group_root);
    pen.setColor(QColor(255, 0, 0));
    _line_1_item->setPen(pen);
    _line_1_item->setOpacity(0.5);
    _line_2_item = new QGraphicsLineItem(_group_root);
    _line_2_item->setPen(pen);
    _line_2_item->setOpacity(0.5);

    _ellipse_item = new QGraphicsEllipseItem(_group_root);
    pen.setColor(QColor(255, 255, 0));
    _ellipse_item->setPen(pen);
    _ellipse_item->setBrush(QBrush(QColor(255, 255, 0)));
    _ellipse_item->setOpacity(0.6);

    tergeo::vehicle::Vehicle::Load("/tergeo/conf/vehicle.conf");
    const tergeo::common::math::Polyline2f& vehicle_contour =
                tergeo::vehicle::Vehicle::GetVehicleContour();
    QPolygonF vehicle;
    for (int i = 0; i < vehicle_contour.size(); ++i) {
        vehicle.push_back(QPointF(vehicle_contour[i].x, vehicle_contour[i].y));
    }

    QPointF cnt_pt(0,0);
    _ellipse_item->setRect(QRectF(cnt_pt - QPointF(0.2, 0.2), cnt_pt + QPointF(0.2, 0.2)));
    _line_1_item->setLine(QLineF(cnt_pt, vehicle[0]));
    _line_2_item->setLine(QLineF(cnt_pt, vehicle[1]));
    _polygon_item->setPolygon(vehicle);

    _group_root->setVisible(false);
}
