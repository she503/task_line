#ifndef TASK_REFLINE_EDITOR_EDITOR_VIEWER_H
#define TASK_REFLINE_EDITOR_EDITOR_VIEWER_H

#include <QMenu>
#include <QGraphicsView>
#include <QWheelEvent>
#include <QContextMenuEvent>
#include <common/math.hpp>
#include "data_manager.h"
#include "vehicle_item.h"

class EditorViewer : public QGraphicsView
{
    Q_OBJECT
public:
    EditorViewer(DataManager* data_manager, QWidget* parent = nullptr);
    ~EditorViewer();

    enum ViewerMode {
        MODE_NORMAL = 0,
        MODE_RECORD_REFLINE = 1,
        MODE_DRAW_REFLINE = 2,
        MODE_EDIT_REFLINE = 3
    };

signals:
    void emitAppendRefPoint(const float x, const float y);
    void emitPopRefPoint();
    void emitClearRefPoint();

public slots:
    void updateLocalization(const float x, const float y, const float theta);
    void startRecordSlot();
    void stopRecordSlot();
    void startDrawRefLineSlot();
    void stopDrawRefLineSlot();

private:
    void setViewerMode(ViewerMode mode);

    void wheelEvent(QWheelEvent *event) override;
    void mousePressEvent(QMouseEvent *event) override;
    void mouseMoveEvent(QMouseEvent* event) override;
    void mouseReleaseEvent(QMouseEvent* event) override;
    void contextMenuEvent(QContextMenuEvent* event) override;

private:
    DataManager* _data_manager;
    VehicleItem* _vehicle_item;
    ViewerMode _viewer_mode = MODE_NORMAL;

    QMenu* _draw_menu;
};

#endif // RECORD_VIEWER_H
