#ifndef TASK_REFLINE_EDITOR_UTILS_H
#define TASK_REFLINE_EDITOR_UTILS_H

#include <QMap>
#include <QList>
#include <QColor>
#include <QString>
#include <task/task.hpp>

const float ResampleDist = 0.2;
const float SearchRadius = 1.0;

static QMap<tergeo::task::ReferencePoint::Type, QString> PointTypeStringMap {
    {tergeo::task::ReferencePoint::Type::TYPE_NULL, "NULL"},
    {tergeo::task::ReferencePoint::Type::TYPE_NORMAL, "NORMAL"},
    {tergeo::task::ReferencePoint::Type::TYPE_GO_STRAIGHT, "GO_STRAIGHT"},
    {tergeo::task::ReferencePoint::Type::TYPE_GO_STRAIGHT_CURVE, "GO_STRAIGHT_CURVE"},
    {tergeo::task::ReferencePoint::Type::TYPE_TURN_LEFT, "TURN_LEFT"},
    {tergeo::task::ReferencePoint::Type::TYPE_TURN_RIGHT, "TURN_RIGHT"},
    {tergeo::task::ReferencePoint::Type::TYPE_INNER_TURN_LEFT, "INNER_TURN_LEFT"},
    {tergeo::task::ReferencePoint::Type::TYPE_INNER_TURN_RIGHT, "INNER_TURN_RIGHT"},
    {tergeo::task::ReferencePoint::Type::TYPE_OUTER_TURN_LEFT, "OUTER_TURN_LEFT"},
    {tergeo::task::ReferencePoint::Type::TYPE_OUTER_TURN_RIGHT, "OUTER_TURN_RIGHT"},
    {tergeo::task::ReferencePoint::Type::TYPE_U_TURN_LEFT, "U_TURN_LEFT"},
    {tergeo::task::ReferencePoint::Type::TYPE_U_TURN_RIGHT, "U_TURN_RIGHT"},
    {tergeo::task::ReferencePoint::Type::TYPE_INNER_U_TURN_LEFT, "INNER_U_TURN_LEFT"},
    {tergeo::task::ReferencePoint::Type::TYPE_INNER_U_TURN_RIGHT, "INNER_U_TURN_RIGHT"},
    {tergeo::task::ReferencePoint::Type::TYPE_OUTER_U_TURN_LEFT, "OUTER_U_TURN_LEFT"},
    {tergeo::task::ReferencePoint::Type::TYPE_OUTER_U_TURN_RIGHT, "OUTER_U_TURN_RIGHT"},
    {tergeo::task::ReferencePoint::Type::TYPE_EDGE_WISE_LINE, "EDGE_WISE_LINE"},
    {tergeo::task::ReferencePoint::Type::TYPE_EDGE_WISE_CURVE, "EDGE_WISE_CURVE"}
};

static QMap<tergeo::task::ReferencePoint::Type, QColor> PointTypeColorMap {
    {tergeo::task::ReferencePoint::Type::TYPE_NULL, QColor("#000000")},
    {tergeo::task::ReferencePoint::Type::TYPE_NORMAL, QColor("#808080")},
    {tergeo::task::ReferencePoint::Type::TYPE_GO_STRAIGHT, QColor("#32CD32")},
    {tergeo::task::ReferencePoint::Type::TYPE_GO_STRAIGHT_CURVE, QColor("#ADFF2F")},
    {tergeo::task::ReferencePoint::Type::TYPE_TURN_LEFT, QColor("#FFFF00")},
    {tergeo::task::ReferencePoint::Type::TYPE_TURN_RIGHT, QColor("#F0E68C")},
    {tergeo::task::ReferencePoint::Type::TYPE_INNER_TURN_LEFT, QColor("#FFFF00")},
    {tergeo::task::ReferencePoint::Type::TYPE_INNER_TURN_RIGHT, QColor("#F0E68C")},
    {tergeo::task::ReferencePoint::Type::TYPE_OUTER_TURN_LEFT, QColor("#FFFF00")},
    {tergeo::task::ReferencePoint::Type::TYPE_OUTER_TURN_RIGHT, QColor("#F0E68C")},
    {tergeo::task::ReferencePoint::Type::TYPE_U_TURN_LEFT, QColor("#FF8C00")},
    {tergeo::task::ReferencePoint::Type::TYPE_U_TURN_RIGHT, QColor("#FA8072")},
    {tergeo::task::ReferencePoint::Type::TYPE_INNER_U_TURN_LEFT, QColor("#FF8C00")},
    {tergeo::task::ReferencePoint::Type::TYPE_INNER_U_TURN_RIGHT, QColor("#FA8072")},
    {tergeo::task::ReferencePoint::Type::TYPE_OUTER_U_TURN_LEFT, QColor("#FF8C00")},
    {tergeo::task::ReferencePoint::Type::TYPE_OUTER_U_TURN_RIGHT, QColor("#FA8072")},
    {tergeo::task::ReferencePoint::Type::TYPE_EDGE_WISE_LINE, QColor("#FF0000")},
    {tergeo::task::ReferencePoint::Type::TYPE_EDGE_WISE_CURVE, QColor("#B22222")}
};

struct RefPoint {
    QPointF pos;
    tergeo::task::ReferencePoint::Type type;
    bool is_edge_wise = false;
    float edge_dist;
    QVector2D edge_dir;
    bool is_selected = false;
};

using RefLine = QList<RefPoint>;

#endif // UTILS_H
