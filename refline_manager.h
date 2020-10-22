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

    void updateRefLine();
    QGraphicsItemGroup *getEditRefLineGroup();

signals:
    void emitRefLineUpdate();

public slots:

private:
    void updateRefLineGroup();
    void updatePathItem();
    void updateRefPointGroup();

private:
    RefLine _ref_line;

    QGraphicsItemGroup* _refline_group;
    QGraphicsItemGroup* _points_group;
    QGraphicsPathItem* _path_item;
};

#endif // REFLINE_MANAGER_H
