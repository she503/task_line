#include "map_manager.h"
#include <common/file.hpp>

#include <QPen>
#include <QBrush>
#include <QMessageBox>
#include <QGraphicsScene>

MapManager::MapManager(QObject *parent) : QObject(parent)
{
    _map_item = new QGraphicsItemGroup();
    _map_item->setZValue(0);
    _edge_item = new QGraphicsItemGroup(_map_item);
    _edge_item->setZValue(5);
}

bool MapManager::loadMap(const std::string &map_dir)
{
    _map_dir = map_dir;
    std::vector<std::string> area_paths;
    tergeo::common::file_system::GetFiles(map_dir, area_paths, "area");
    if (area_paths.empty()) {
        QMessageBox::warning(nullptr, "Warning", "Has no area file in map dir, please check!");
        return false;
    }
    if (!_map_area.load(area_paths[0])) {
        QMessageBox::warning(nullptr, "Warning", "Can not load area file, please check!");
        return false;
    }
    if (_map_area.getAllRoads(_map_roads) <= 0) {
        QMessageBox::warning(nullptr, "Warning", "Map selected has no road, please check!");
        return false;
    }
    this->mapToGraphicsItem();
    this->updateSceneSize();
    return true;
}

std::string MapManager::getMapDir() const
{
    return _map_dir;
}

std::string MapManager::getMapName() const
{
    return _map_area.getName();
}

QGraphicsItemGroup *MapManager::getMapItemGroup()
{
    return _map_item;
}

void MapManager::generateEdge(const float edge_dist)
{
    if (_cur_edge_dist == edge_dist) return;
    _cur_edge_dist = edge_dist;
    this->updateRoadEdge();
    this->updateSceneSize();
}

void MapManager::calEdgeDistInfo(RefPoint &ref_point)
{
    if (!ref_point.edge_dist_info.is_edge_wise) {
        return;
    }
    float min_dist = std::numeric_limits<float>::max();
    for (int i = 0; i < _map_roads.size(); ++i) {
        const std::vector<tergeo::common::math::Polyline2d>& include_polygon =
                _map_roads[i]->getIncludePolygons();
        const std::vector<tergeo::common::math::Polyline2d>& exclude_polygon =
                _map_roads[i]->getExcludePolygons();
        for (int j = 0; j < include_polygon.size(); ++j) {
            const tergeo::common::math::Polyline2d& polyline = include_polygon[j];
            for (int k = 0; k < polyline.size(); ++k) {
                QPointF cur_pt(polyline[k].x, polyline[k].y);
                float dist = std::hypot(ref_point.pos.x() - cur_pt.x(),
                                        ref_point.pos.y() - cur_pt.y());
                if (dist < min_dist) {
                    min_dist = dist;
                    int pre_index = k - 1;
                    int lat_index = k + 1;
                    if (pre_index < 0) pre_index = polyline.size() - 1;
                    if (lat_index >= polyline.size()) lat_index = 0;
                    QPointF pre_pt(polyline[pre_index].x, polyline[pre_index].y);
                    QPointF lat_pt(polyline[lat_index].x, polyline[lat_index].y);
                    this->calEdgeDistAndEdgeDir(ref_point, pre_pt, cur_pt, lat_pt);
                }
            }
        }
        for (int j = 0; j < exclude_polygon.size(); ++j) {
            const tergeo::common::math::Polyline2d& polyline = exclude_polygon[j];
            for (int k = 0; k < polyline.size(); ++k) {
                QPointF cur_pt(polyline[k].x, polyline[k].y);
                float dist = std::hypot(ref_point.pos.x() - cur_pt.x(),
                                        ref_point.pos.y() - cur_pt.y());
                if (dist < min_dist) {
                    min_dist = dist;
                    int pre_index = k - 1;
                    int lat_index = k + 1;
                    if (pre_index < 0) pre_index = polyline.size() - 1;
                    if (lat_index >= polyline.size()) lat_index = 0;
                    QPointF pre_pt(polyline[pre_index].x, polyline[pre_index].y);
                    QPointF lat_pt(polyline[lat_index].x, polyline[lat_index].y);
                    this->calEdgeDistAndEdgeDir(ref_point, pre_pt, cur_pt, lat_pt);
                }
            }
        }
    }
}

void MapManager::updateSceneSize()
{
    int border = 20;
    QRectF scene_rect = _map_item->childrenBoundingRect();
    scene_rect.setTop(scene_rect.top() - border);
    scene_rect.setBottom(scene_rect.bottom() + border);
    scene_rect.setLeft(scene_rect.left() - border);
    scene_rect.setRight(scene_rect.right() + border);
    _map_item->scene()->setSceneRect(scene_rect);
}

void MapManager::mapToGraphicsItem()
{
    QGraphicsItemGroup *signals_item = new QGraphicsItemGroup(_map_item);
    QGraphicsItemGroup *trees_item = new QGraphicsItemGroup(_map_item);
    QGraphicsItemGroup *stop_signs_item = new QGraphicsItemGroup(_map_item);
    QGraphicsItemGroup *speed_bumps_item = new QGraphicsItemGroup(_map_item);
    QGraphicsItemGroup *road_edges_item = new QGraphicsItemGroup(_map_item);
    QGraphicsItemGroup *lane_lines_item = new QGraphicsItemGroup(_map_item);
    QGraphicsItemGroup *cross_walks_item = new QGraphicsItemGroup(_map_item);
    QGraphicsItemGroup *junctions_item = new QGraphicsItemGroup(_map_item);
    QGraphicsItemGroup *clear_areas_item = new QGraphicsItemGroup(_map_item);
    QGraphicsItemGroup *parking_spaces_item = new QGraphicsItemGroup(_map_item);
    QGraphicsItemGroup *roads_item = new QGraphicsItemGroup(_map_item);

    signals_item->setZValue(3);
    trees_item->setZValue(3);
    stop_signs_item->setZValue(4);
    speed_bumps_item->setZValue(2);
    road_edges_item->setZValue(2);
    lane_lines_item->setZValue(3);
    cross_walks_item->setZValue(2);
    junctions_item->setZValue(2);
    clear_areas_item->setZValue(2);
    parking_spaces_item->setZValue(2);

    tergeo::common::math::Point2d point(0, 0);
    float search_radius = std::numeric_limits<float>::max();

    std::vector<const tergeo::hdmap::Signal *> road_signals;
    if (_map_area.getSignals(point, search_radius, road_signals) > 0) {
        this->signalsToGraphicsItem(road_signals, signals_item);
    }
    std::vector<const tergeo::hdmap::Tree*> trees;
    if (_map_area.getTrees(point, search_radius, trees) > 0) {
        this->treesToGraphicsItem(trees, trees_item);
    }
    std::vector<const tergeo::hdmap::StopSign *> stop_signs;
    if (_map_area.getStopSigns(point, search_radius, stop_signs) > 0) {
        this->stopSignsToGraphicsItem(stop_signs, stop_signs_item);
    }
    std::vector<const tergeo::hdmap::SpeedBump *> speed_bumps;
    if (_map_area.getSpeedBumps(point, search_radius, speed_bumps) > 0) {
        this->speedBumpsToGraphicsItem(speed_bumps, speed_bumps_item);
    }
    std::vector<const tergeo::hdmap::RoadEdge *> road_edges;
    if (_map_area.getRoadEdges(point, search_radius, road_edges) > 0) {
        this->roadEdgesToGraphicsItem(road_edges, road_edges_item);
    }
    std::vector<const tergeo::hdmap::LaneLine *> lane_lines;
    if (_map_area.getLaneLines(point, search_radius, lane_lines) > 0) {
        this->laneLinesToGraphicsItem(lane_lines, lane_lines_item);
    }
    std::vector<const tergeo::hdmap::Crosswalk *> crosswalks;
    if (_map_area.getCrosswalks(point, search_radius, crosswalks) > 0) {
        this->crosswalksToGraphicsItem(crosswalks, cross_walks_item);
    }
    std::vector<const tergeo::hdmap::Junction *> junctions;
    if (_map_area.getJunctions(point, search_radius, junctions) > 0) {
        this->junctionsToGraphicsItem(junctions, junctions_item);
    }
    std::vector<const tergeo::hdmap::ClearArea *> clear_areas;
    if (_map_area.getClearAreas(point, search_radius, clear_areas) > 0) {
        this->clearAreasToGraphicsItem(clear_areas, clear_areas_item);
    }
    std::vector<const tergeo::hdmap::ParkingSpace *> parking_spaces;
    if (_map_area.getParkingSpaces(point, search_radius, parking_spaces) > 0) {
        this->parkingSpacesToGraphicsItem(parking_spaces, parking_spaces_item);
    }
    std::vector<const tergeo::hdmap::Road *> roads;
    if (_map_area.getRoads(point, search_radius, roads) > 0) {
        this->roadsToGraphicsItem(roads, roads_item);
    }
}

bool MapManager::signalsToGraphicsItem(std::vector<const tergeo::hdmap::Signal *> &road_signals,
                                   QGraphicsItemGroup *signals_item) {
    QPen signal_pen(QColor(255, 100, 0));
    QBrush signal_brush(QColor(0, 255, 0));
    signal_pen.setWidthF(0.2);
    for (int i = 0; i < road_signals.size(); ++i) {
        QGraphicsEllipseItem* signal_item = new QGraphicsEllipseItem(signals_item);
//        signal_item->setData(MapItemRole, "map_item");
        signal_item->setPen(signal_pen);
        signal_item->setBrush(signal_brush);
        QPointF signal_pt(road_signals[i]->getPoint().x,
                          road_signals[i]->getPoint().y);
        QRectF rect(signal_pt - QPointF(0.3, 0.3),
                    signal_pt + QPointF(0.3, 0.3));
        signal_item->setRect(rect);
        signal_item->setZValue(3);
        signal_item->setVisible(true);
    }
    return true;
}

bool MapManager::treesToGraphicsItem(std::vector<const tergeo::hdmap::Tree *> &trees,
                                 QGraphicsItemGroup *trees_item) {
    QPen tree_pen(QColor(0, 255, 0));
    tree_pen.setWidthF(0.2);
    QBrush tree_brush(QColor(0, 0, 0));
    for (int i = 0; i < trees.size(); ++i) {
        QGraphicsEllipseItem* tree_item = new QGraphicsEllipseItem(trees_item);
//        tree_item->setData(MapItemRole, "map_item");
        tree_item->setPen(tree_pen);
        tree_item->setBrush(tree_brush);
        QPointF signal_pt(trees[i]->getPoint().x,
                          trees[i]->getPoint().y);
        QRectF rect(signal_pt - QPointF(0.15, 0.15),
                    signal_pt + QPointF(0.15, 0.15));
        tree_item->setRect(rect);
        tree_item->setZValue(3);
        tree_item->setVisible(true);
    }
    return true;
}

bool MapManager::stopSignsToGraphicsItem(std::vector<const tergeo::hdmap::StopSign *> &stop_signs,
                                     QGraphicsItemGroup *stop_signs_item) {
    QPen stopSign_pen(Qt::darkRed);
    stopSign_pen.setWidthF(0.2);
    for (int i = 0; i < stop_signs.size(); ++i) {
        QGraphicsLineItem* stopSign_item = new QGraphicsLineItem(stop_signs_item);
//        stopSign_item->setData(MapItemRole, "map_item");
        stopSign_item->setPen(stopSign_pen);
        stopSign_item->setOpacity(0.6);
        QPointF stopSign_pt_start(stop_signs[i]->getStartPoint().x,
                             stop_signs[i]->getStartPoint().y);
        QPointF stopSign_pt_end(stop_signs[i]->getEndPoint().x,
                             stop_signs[i]->getEndPoint().y);
        stopSign_item->setLine(QLineF(stopSign_pt_start, stopSign_pt_end));
        stopSign_item->setZValue(3);
        stopSign_item->setVisible(true);
    }
    return true;
}

bool MapManager::speedBumpsToGraphicsItem(std::vector<const tergeo::hdmap::SpeedBump *> &speed_bumps,
                                      QGraphicsItemGroup *speed_bumps_item) {
    QPen speedBump_pen(QColor("#FFFF00"));
    speedBump_pen.setWidthF(1.5);
    for (int i = 0; i < speed_bumps.size(); ++i) {
        QGraphicsLineItem* speedBump_item = new QGraphicsLineItem(speed_bumps_item);
//        speedBump_item->setData(MapItemRole, "map_item");
        speedBump_item->setPen(speedBump_pen);
        speedBump_item->setOpacity(0.6);
        QPointF speedBump_pt_start(speed_bumps[i]->getStartPoint().x,
                             speed_bumps[i]->getStartPoint().y);
        QPointF speedBump_pt_end(speed_bumps[i]->getEndPoint().x,
                             speed_bumps[i]->getEndPoint().y);
        speedBump_item->setLine(QLineF(speedBump_pt_start, speedBump_pt_end));
        speedBump_item->setZValue(3);
        speedBump_item->setVisible(true);
    }
    return true;
}

bool MapManager::roadEdgesToGraphicsItem(std::vector<const tergeo::hdmap::RoadEdge *> &road_edges,
                                     QGraphicsItemGroup *road_edges_item) {
    QPen roadEdge_pen(QColor(Qt::black));
    roadEdge_pen.setWidthF(0.2);
    for (int i = 0; i < road_edges.size(); ++i) {
        const std::vector<tergeo::common::math::Point2d>& roadEdeg =
                road_edges[i]->getPolyline();
        for (int j = 0; j < roadEdeg.size(); ++j) {
            QGraphicsPathItem* roadEdeg_item = new QGraphicsPathItem(road_edges_item);
            roadEdeg_item->setPen(roadEdge_pen);
//            roadEdeg_item->setData(MapItemRole, "map_item");
            roadEdeg_item->setZValue(3);

            QPainterPath path;
            path.moveTo(roadEdeg[0].x, roadEdeg[0].y);
            for (int k = 1; k < roadEdeg.size(); ++k) {
                path.lineTo(QPointF(roadEdeg[k].x, roadEdeg[k].y));
            }
            roadEdeg_item->setPath(path);
            roadEdeg_item->update();
        }
    }
    return true;
}

bool MapManager::laneLinesToGraphicsItem(std::vector<const tergeo::hdmap::LaneLine *> &lane_lines,
                                     QGraphicsItemGroup *lane_lines_item) {
    QPen laneLine_pen(QColor("#C6E2FF"));
    laneLine_pen.setWidthF(0.2);
    for (int i = 0; i < lane_lines.size(); ++i) {
        const std::vector<tergeo::common::math::Point2d>& laneLine =
                lane_lines[i]->getPolyline();

        QVector<qreal> dashes;
        qreal space = 3;
        for (int i = 1; i < lane_lines.size(); ++i) {
            i % 2 == 0 ? dashes << space : dashes << i;
        }
        laneLine_pen.setDashPattern(dashes);

        for (int j = 0; j < laneLine.size(); ++j) {
            QGraphicsPathItem* laneLine_item = new QGraphicsPathItem(lane_lines_item);
            laneLine_item->setPen(laneLine_pen);
//            laneLine_item->setData(MapItemRole, "map_item");
            laneLine_item->setZValue(3);

            QPainterPath path;
            path.moveTo(laneLine[0].x, laneLine[0].y);
            for (int k = 1; k < laneLine.size(); ++k) {
                path.lineTo(QPointF(laneLine[k].x, laneLine[k].y));
            }
            laneLine_item->setPath(path);
            laneLine_item->update();
        }
        dashes.clear();
    }
    return true;
}

bool MapManager::crosswalksToGraphicsItem(std::vector<const tergeo::hdmap::Crosswalk *> &crosswalks,
                                      QGraphicsItemGroup *cross_walks_item) {
    QPen crosswalk_pen(QColor(Qt::transparent));
    crosswalk_pen.setWidthF(0.2);
    for (int i = 0; i < crosswalks.size(); ++i) {
        const std::vector<tergeo::common::math::Polyline2d>& include_crosswalk =
                crosswalks[i]->getIncludePolygons();
        for (int j = 0; j < include_crosswalk.size(); ++j) {
            QGraphicsPolygonItem* in_polygon_item = new QGraphicsPolygonItem(cross_walks_item);
            in_polygon_item->setPen(crosswalk_pen);
//            in_polygon_item->setData(MapItemRole, "map_item");
            in_polygon_item->setZValue(1);
            in_polygon_item->setOpacity(0.6);
            in_polygon_item->setBrush(QColor(Qt::cyan));
            QPolygonF in_polygon;
            for (int k = 0; k < include_crosswalk[j].size(); ++k) {
                in_polygon.push_back(QPointF(include_crosswalk[j][k].x, include_crosswalk[j][k].y));
            }
            in_polygon_item->setPolygon(in_polygon);
            in_polygon_item->update();
        }

        const std::vector<tergeo::common::math::Polyline2d>& exclude_crosswalk =
                crosswalks[i]->getExcludePolygons();
        for (int j = 0; j < exclude_crosswalk.size(); ++j) {
            QGraphicsPolygonItem* ex_polygon_item = new QGraphicsPolygonItem(cross_walks_item);
            ex_polygon_item->setPen(crosswalk_pen);
//            ex_polygon_item->setData(MapItemRole, "map_item");
            ex_polygon_item->setZValue(2);
            ex_polygon_item->setBrush(QColor(Qt::gray));
            QPolygonF ex_polygon;
            for (int k = 0; k < exclude_crosswalk[j].size(); ++k) {
                ex_polygon.push_back(QPointF(exclude_crosswalk[j][k].x, exclude_crosswalk[j][k].y));
            }
            ex_polygon_item->setPolygon(ex_polygon);
            ex_polygon_item->update();
        }
    }
    return true;
}

bool MapManager::junctionsToGraphicsItem(std::vector<const tergeo::hdmap::Junction *> &junctions,
                                     QGraphicsItemGroup *junctions_item) {
    QPen junction_pen(QColor(Qt::transparent));
    QLinearGradient junction_brush;
    junction_brush.setColorAt(0, QColor("#FFD700"));
    junction_brush.setColorAt(0.5, QColor("#C0FF3E"));
    junction_brush.setColorAt(1, QColor("#F08080"));

    for (int i = 0; i < junctions.size(); ++i) {
        const std::vector<tergeo::common::math::Polyline2d>& include_junction =
                junctions[i]->getIncludePolygons();
        for (int j = 0; j < include_junction.size(); ++j) {
            QGraphicsPolygonItem* in_polygon_item = new QGraphicsPolygonItem(junctions_item);
            in_polygon_item->setPen(junction_pen);
//            in_polygon_item->setData(MapItemRole, "map_item");
            in_polygon_item->setZValue(1);
            in_polygon_item->setBrush(junction_brush);
            in_polygon_item->setOpacity(0.3);
            QPolygonF in_polygon;
            for (int k = 0; k < include_junction[j].size(); ++k) {
                in_polygon.push_back(QPointF(include_junction[j][k].x, include_junction[j][k].y));
            }
            in_polygon_item->setPolygon(in_polygon);
            in_polygon_item->update();
        }

        const std::vector<tergeo::common::math::Polyline2d>& exclude_junction =
                junctions[i]->getExcludePolygons();
        for (int j = 0; j < exclude_junction.size(); ++j) {
            QGraphicsPolygonItem* ex_polygon_item = new QGraphicsPolygonItem(junctions_item);
            ex_polygon_item->setPen(junction_pen);
//            ex_polygon_item->setData(MapItemRole, "map_item");
            ex_polygon_item->setZValue(2);
            ex_polygon_item->setBrush(junction_brush);
            ex_polygon_item->setOpacity(0.2);
            QPolygonF ex_polygon;
            for (int k = 0; k < exclude_junction[j].size(); ++k) {
                ex_polygon.push_back(QPointF(exclude_junction[j][k].x, exclude_junction[j][k].y));
            }
            ex_polygon_item->setPolygon(ex_polygon);
            ex_polygon_item->update();
        }
    }
    return true;
}

bool MapManager::clearAreasToGraphicsItem(std::vector<const tergeo::hdmap::ClearArea *> &clear_areas,
                                      QGraphicsItemGroup *clear_areas_item) {
    QPen clearArea_pen(QColor(Qt::transparent));
    clearArea_pen.setWidthF(0.2);
    for (int i = 0; i < clear_areas.size(); ++i) {
        const std::vector<tergeo::common::math::Polyline2d>& include_clearArea =
                clear_areas[i]->getIncludePolygons();
        for (int j = 0; j < include_clearArea.size(); ++j) {
            QGraphicsPolygonItem* in_polygon_item = new QGraphicsPolygonItem(clear_areas_item);
            in_polygon_item->setPen(clearArea_pen);
//            in_polygon_item->setData(MapItemRole, "map_item");
            in_polygon_item->setZValue(1);
            in_polygon_item->setBrush(QColor(Qt::red));
            in_polygon_item->setOpacity(0.6);
            QPolygonF in_polygon;
            for (int k = 0; k <include_clearArea[j].size(); ++k) {
                in_polygon.push_back(QPointF(include_clearArea[j][k].x, include_clearArea[j][k].y));
            }
            in_polygon_item->setPolygon(in_polygon);
            in_polygon_item->update();
        }

        const std::vector<tergeo::common::math::Polyline2d>& exclude_clearArea =
                clear_areas[i]->getExcludePolygons();
        for (int j = 0; j < exclude_clearArea.size(); ++j) {
            QGraphicsPolygonItem* ex_polygon_item = new QGraphicsPolygonItem(clear_areas_item);
            ex_polygon_item->setPen(clearArea_pen);
//            ex_polygon_item->setData(MapItemRole, "map_item");
            ex_polygon_item->setZValue(2);
            ex_polygon_item->setBrush(QColor(Qt::green));
            ex_polygon_item->setOpacity(0.6);
            QPolygonF ex_polygon;
            for (int k = 0; k < exclude_clearArea[j].size(); ++k) {
                ex_polygon.push_back(QPointF(exclude_clearArea[j][k].x, exclude_clearArea[j][k].y));
            }
            ex_polygon_item->setPolygon(ex_polygon);
            ex_polygon_item->update();
        }
    }
    return true;
}

bool MapManager::parkingSpacesToGraphicsItem(std::vector<const tergeo::hdmap::ParkingSpace *> &parking_spaces,
                                         QGraphicsItemGroup *parking_spaces_item) {
    QPen parkingSpace_pen(QColor(Qt::transparent));
    parkingSpace_pen.setWidthF(0.2);
    for (int i = 0; i < parking_spaces.size(); ++i) {
        const std::vector<tergeo::common::math::Polyline2d>& include_parkingSpace =
                parking_spaces[i]->getIncludePolygons();
        for (int j = 0; j < include_parkingSpace.size(); ++j) {
            QGraphicsPolygonItem* in_polygon_item = new QGraphicsPolygonItem(parking_spaces_item);
            in_polygon_item->setPen(parkingSpace_pen);
//            in_polygon_item->setData(MapItemRole, "map_item");
            in_polygon_item->setZValue(1);
            in_polygon_item->setBrush(QColor(Qt::green));
            in_polygon_item->setOpacity(0.6);
            QPolygonF in_polygon;
            for (int k = 0; k < include_parkingSpace[j].size(); ++k) {
                in_polygon.push_back(QPointF(include_parkingSpace[j][k].x, include_parkingSpace[j][k].y));
            }
            in_polygon_item->setPolygon(in_polygon);
            in_polygon_item->update();
        }

        const std::vector<tergeo::common::math::Polyline2d>& exclude_parkingSpace =
                parking_spaces[i]->getExcludePolygons();
        for (int j = 0; j < exclude_parkingSpace.size(); ++j) {
            QGraphicsPolygonItem* ex_polygon_item = new QGraphicsPolygonItem(parking_spaces_item);
            ex_polygon_item->setPen(parkingSpace_pen);
//            ex_polygon_item->setData(MapItemRole, "map_item");
            ex_polygon_item->setZValue(2);
            ex_polygon_item->setBrush(Qt::red);
            ex_polygon_item->setOpacity(0.6);
            QPolygonF ex_polygon;
            for (int k = 0; k < exclude_parkingSpace[j].size(); ++k) {
                ex_polygon.push_back(QPointF(exclude_parkingSpace[j][k].x, exclude_parkingSpace[j][k].y));
            }
            ex_polygon_item->setPolygon(ex_polygon);
            ex_polygon_item->update();
        }
    }
    return true;
}

bool MapManager::roadsToGraphicsItem(std::vector<const tergeo::hdmap::Road *> &roads,
                                 QGraphicsItemGroup *roads_item) {
    QPen road_pen(QColor(0, 0, 0));
    road_pen.setWidthF(0.04);
    for (int i = 0; i < roads.size(); ++i) {
        const std::vector<tergeo::common::math::Polyline2d>& include_road =
                roads[i]->getIncludePolygons();
        for (int j = 0; j < include_road.size(); ++j) {
            QGraphicsPolygonItem* in_polygon_item = new QGraphicsPolygonItem(roads_item);
            in_polygon_item->setPen(road_pen);
//            in_polygon_item->setData(MapItemRole, "map_item");
            in_polygon_item->setZValue(1);
            in_polygon_item->setBrush(QColor(193,255,193, 80));
            QPolygonF in_polygon;
            for (int k = 0; k < include_road[j].size(); ++k) {
                in_polygon.push_back(QPointF(include_road[j][k].x, include_road[j][k].y));
            }
            in_polygon_item->setPolygon(in_polygon);
            in_polygon_item->update();
        }

        const std::vector<tergeo::common::math::Polyline2d>& exclude_road =
                roads[i]->getExcludePolygons();
        for (int j = 0; j < exclude_road.size(); ++j) {
            QGraphicsPolygonItem* ex_polygon_item = new QGraphicsPolygonItem(roads_item);
            ex_polygon_item->setPen(road_pen);
//            ex_polygon_item->setData(MapItemRole, "map_item");
            ex_polygon_item->setZValue(2);
            ex_polygon_item->setBrush(QColor(Qt::gray));
            QPolygonF ex_polygon;
            for (int k = 0; k < exclude_road[j].size(); ++k) {
                ex_polygon.push_back(QPointF(exclude_road[j][k].x, exclude_road[j][k].y));
            }
            ex_polygon_item->setPolygon(ex_polygon);
            ex_polygon_item->update();
        }
    }
    return true;
}

bool MapManager::updateRoadEdge()
{
    QList<QGraphicsItem*> child_items = _edge_item->childItems();
    for (QGraphicsItem* item : child_items) {
        delete item;
    }
    QPen edge_pen(QColor("red"));
    edge_pen.setWidthF(0.02);
    for (const tergeo::hdmap::Road* road : _map_roads) {
        const std::vector<tergeo::common::math::Polyline2d>& include_polygon =
                road->getIncludePolygons();
        const std::vector<tergeo::common::math::Polyline2d>& exclude_polygon =
                road->getExcludePolygons();
        for (int k = 0; k < include_polygon.size(); ++k) {
            int pt_num = include_polygon[k].size();
            if (pt_num <= 2) {
                continue;
            }
            QGraphicsPathItem* edge_item = new QGraphicsPathItem(_edge_item);
            edge_item->setPen(edge_pen);
            QPainterPath edge_path; QPointF start_pt;
            for (int i = 0; i < pt_num; ++i) {
                QPointF edge_point;
                if (i == 0) {
                    edge_point = this->calEdgePoint(include_polygon[k][pt_num-1],
                        include_polygon[k][i], include_polygon[k][i+1], _cur_edge_dist);
                    edge_path.moveTo(edge_point);
                    start_pt = edge_point;
                } else if (i == pt_num - 1) {
                    edge_point = this->calEdgePoint(include_polygon[k][i-1],
                        include_polygon[k][i], include_polygon[k][0], _cur_edge_dist);
                    edge_path.lineTo(edge_point);
                } else {
                    edge_point = this->calEdgePoint(include_polygon[k][i-1],
                        include_polygon[k][i], include_polygon[k][i+1], _cur_edge_dist);
                    edge_path.lineTo(edge_point);
                }
            }
            edge_path.lineTo(start_pt);
            edge_item->setPath(edge_path);
            edge_item->update();
        }
        for (int k = 0; k < exclude_polygon.size(); ++k) {
            int pt_num = exclude_polygon[k].size();
            if (pt_num <= 2) {
                continue;
            }
            QGraphicsPathItem* edge_item = new QGraphicsPathItem(_edge_item);
            edge_item->setPen(edge_pen);
            QPainterPath edge_path; QPointF start_pt;
            for (int i = 0; i < pt_num; ++i) {
                QPointF edge_point;
                if (i == 0) {
                    edge_point = this->calEdgePoint(exclude_polygon[k][pt_num-1],
                        exclude_polygon[k][i], exclude_polygon[k][i+1], -_cur_edge_dist);
                    edge_path.moveTo(edge_point);
                    start_pt = edge_point;
                } else if (i == pt_num - 1) {
                    edge_point = this->calEdgePoint(exclude_polygon[k][i-1],
                        exclude_polygon[k][i], exclude_polygon[k][0], -_cur_edge_dist);
                    edge_path.lineTo(edge_point);
                } else {
                    edge_point = this->calEdgePoint(exclude_polygon[k][i-1],
                        exclude_polygon[k][i], exclude_polygon[k][i+1], -_cur_edge_dist);
                    edge_path.lineTo(edge_point);
                }
            }
            edge_path.lineTo(start_pt);
            edge_item->setPath(edge_path);
            edge_item->update();
        }
    }
}

QPointF MapManager::calEdgePoint(const tergeo::common::math::Point2d &pt_1,
                                 const tergeo::common::math::Point2d &pt_2,
                                 const tergeo::common::math::Point2d &pt_3,
                                 const float dist)
{
    tergeo::common::math::Vector2d dir_1 = pt_2 - pt_1;
    tergeo::common::math::Vector2d dir_2 = pt_2 - pt_3;
    dir_1.normalize(); dir_2.normalize();
    double sin_value = dir_1.cross(dir_2);
    tergeo::common::math::Vector2d dir_cross = dir_1 + dir_2;
    double ratio = dist / sin_value;
    tergeo::common::math::Vector2d pt_cross = pt_2 + dir_cross * ratio;
    return QPointF(pt_cross.x, pt_cross.y);
}

void MapManager::calEdgeDistAndEdgeDir(RefPoint &ref_point, const QPointF &pre_pt,
                                       const QPointF &cur_pt, const QPointF &lat_pt)
{
    QVector2D pt_cur_dir(ref_point.pos - cur_pt);
    QVector2D pre_cur_dir(pre_pt - cur_pt); pre_cur_dir.normalize();
    QVector2D lat_cur_dir(lat_pt - cur_pt); lat_cur_dir.normalize();
    float pt_cur_dir_length = pt_cur_dir.length();
    float theta_pre = std::acos(QVector2D::dotProduct(pt_cur_dir, pre_cur_dir) / pt_cur_dir_length);
    float theta_lat = std::acos(QVector2D::dotProduct(pt_cur_dir, lat_cur_dir) / pt_cur_dir_length);
    if (theta_pre <= theta_lat) {
        ref_point.edge_dist_info.map_pt_1 = pre_pt;
        ref_point.edge_dist_info.map_pt_2 = cur_pt;
        float cros_value = std::fabs(pt_cur_dir.x() * pre_cur_dir.y() -
                                     pt_cur_dir.y() * pre_cur_dir.x());
        ref_point.edge_dist_info.edge_dist = cros_value;
        QVector2D cross_dir(-pre_cur_dir.y(), pre_cur_dir.x());
        if (QVector2D::dotProduct(pt_cur_dir, cross_dir) >= 0) {
            ref_point.edge_dist_info.edge_dir = cross_dir;
        } else {
            ref_point.edge_dist_info.edge_dir = -cross_dir;
        }
    } else {
        ref_point.edge_dist_info.map_pt_1 = cur_pt;
        ref_point.edge_dist_info.map_pt_2 = lat_pt;
        float cros_value = std::fabs(pt_cur_dir.x() * lat_cur_dir.y() -
                                     pt_cur_dir.y() * lat_cur_dir.x());
        ref_point.edge_dist_info.edge_dist = cros_value;
        QVector2D cross_dir(-lat_cur_dir.y(), lat_cur_dir.x());
        if (QVector2D::dotProduct(pt_cur_dir, cross_dir) >= 0) {
            ref_point.edge_dist_info.edge_dir = cross_dir;
        } else {
            ref_point.edge_dist_info.edge_dir = -cross_dir;
        }
    }
}
