#ifndef TASK_REFLINE_EDITOR_MAP_MANAGER_H
#define TASK_REFLINE_EDITOR_MAP_MANAGER_H

#include <QGraphicsItemGroup>
#include <common/math.hpp>
#include <hdmap/area.hpp>
#include <QObject>
#include "utils.h"

class MapManager : public QObject
{
    Q_OBJECT
public:
    explicit MapManager(QObject* parent = nullptr);

    bool loadMap(const std::string& map_dir);
    std::string getMapDir() const;
    std::string getMapName() const;
    QGraphicsItemGroup *getMapItemGroup();
    void generateEdge(const float edge_dist);

    void calEdgeDistInfo(RefPoint& ref_point);

private:
    void updateSceneSize();

    void mapToGraphicsItem();

    bool signalsToGraphicsItem(std::vector<const tergeo::hdmap::Signal*>& road_signals,
            QGraphicsItemGroup* signals_item);
    bool treesToGraphicsItem(std::vector<const tergeo::hdmap::Tree*>& trees,
            QGraphicsItemGroup* trees_item);
    bool stopSignsToGraphicsItem(std::vector<const tergeo::hdmap::StopSign*>& stop_signs,
            QGraphicsItemGroup* stop_signs_item);
    bool speedBumpsToGraphicsItem(std::vector<const tergeo::hdmap::SpeedBump*>& speed_bumps,
            QGraphicsItemGroup* speed_bumps_item);
    bool roadEdgesToGraphicsItem(std::vector<const tergeo::hdmap::RoadEdge*>& road_edges,
            QGraphicsItemGroup* road_edges_item);
    bool laneLinesToGraphicsItem(std::vector<const tergeo::hdmap::LaneLine*>& lane_lines,
            QGraphicsItemGroup* lane_lines_item);
    bool crosswalksToGraphicsItem(std::vector<const tergeo::hdmap::Crosswalk*>& crosswalks,
            QGraphicsItemGroup* cross_walks_item);
    bool junctionsToGraphicsItem(std::vector<const tergeo::hdmap::Junction*>& junctions,
            QGraphicsItemGroup* junctions_item);
    bool clearAreasToGraphicsItem(std::vector<const tergeo::hdmap::ClearArea*>& clear_areas,
            QGraphicsItemGroup* clear_areas_item);
    bool parkingSpacesToGraphicsItem(std::vector<const tergeo::hdmap::ParkingSpace*>& parking_spaces,
            QGraphicsItemGroup* parking_spaces_item);
    bool roadsToGraphicsItem(std::vector<const tergeo::hdmap::Road*>& roads,
            QGraphicsItemGroup* roads_item);

    bool updateRoadEdge();
    QPointF calEdgePoint(const tergeo::common::math::Point2d &pt_1,
                         const tergeo::common::math::Point2d &pt_2,
                         const tergeo::common::math::Point2d &pt_3,
                         const float dist);
    float calPtEdgeDist(const RefPoint& ref_point,
                        const QPointF& cur_pt, const QPointF& lat_pt);
    void updateEdgeDistInfo(RefPoint& ref_point, const float dist,
                            const QPointF& cur_pt, const QPointF& lat_pt);
    void calEdgeDistAndEdgeDir(RefPoint& ref_point, const QPointF& pre_pt,
                               const QPointF& cur_pt, const QPointF& lat_pt);
private:
    std::string _map_dir = "";
    tergeo::hdmap::Area _map_area;
    std::vector<const tergeo::hdmap::Road*> _map_roads;

    QGraphicsItemGroup* _map_item;
    QGraphicsItemGroup* _edge_item;
    float _cur_edge_dist = 0;
};

#endif // GEO_MAP_H
