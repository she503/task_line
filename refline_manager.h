#ifndef TASK_REFLINE_EDITOR_REFLINE_MANAGER_H
#define TASK_REFLINE_EDITOR_REFLINE_MANAGER_H

#include <QObject>
#include <QGraphicsPathItem>
#include <QGraphicsItemGroup>
#include "utils.h"

#define IndexRole Qt::UserRole + 10

class RefLineManager : public QObject
{
    Q_OBJECT
public:
    explicit RefLineManager(QObject *parent = nullptr);

    RefLine &getRefLine();
    const RefLine &getRefLine() const;
    void setUnEditMode();

    void updateRefLine();
    void updateSelectedPointIndex(const int index);
    void setSelectedPointIndex(const int index);
    void setSelectedPointRect(const QRectF& rect);


    void setSelectedPointsType(const tergeo::task::ReferencePoint::Type& type);
    void updateSelectedPointsPos(const QPointF& changed_pos);
    void deleteSelectedPoints();
    void resampleSelectedPoints();
    void smoothSelectedPoints();

    int searchNearestPtIndex(const QPointF& point);

    QGraphicsItemGroup *getEditRefLineGroup();

signals:
    void emitRefLineUpdate();
    void emitSelectedPointsChanged();
    void emitPointsTypeChanged();

public slots:

private:
    void updateSelectedPoints(const int first, const int last);

    void updatePathItem();
    void updateRefPointGroup();

    void clearRefLine();

private:
    RefLine _ref_line;

    QGraphicsItemGroup* _refline_group;
    QGraphicsItemGroup* _points_group;
    QGraphicsPathItem* _path_item;
};

#endif // REFLINE_MANAGER_H
