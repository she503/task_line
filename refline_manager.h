#ifndef TASK_REFLINE_EDITOR_REFLINE_MANAGER_H
#define TASK_REFLINE_EDITOR_REFLINE_MANAGER_H

#include <QObject>
#include <QGraphicsPathItem>
#include <QGraphicsItemGroup>
#include "utils.h"

class RefLineManager : public QObject
{
    Q_OBJECT
public:
    explicit RefLineManager(QObject *parent = nullptr);

    RefLine &getRefLine();
    const RefLine &getRefLine() const;
    void setUnEditMode();

    void updateRefLine();
    void updateSelectedPoints(const int first, const int last);

    void setSelectedPointsType(const tergeo::task::ReferencePoint::Type& type);


    QGraphicsItemGroup *getEditRefLineGroup();

signals:
    void emitRefLineUpdate();
    void emitSelectedPointsChanged();
    void emitPointsTypeChanged();

public slots:

private:
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
