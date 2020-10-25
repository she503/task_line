#ifndef TASK_REFLINE_EDITOR_EDITOR_VIEWER_H
#define TASK_REFLINE_EDITOR_EDITOR_VIEWER_H

#include <QMenu>
#include <QWheelEvent>
#include <QGraphicsView>
#include <QContextMenuEvent>
#include <QGraphicsEllipseItem>
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
    void startEditRefLineSlot();
    void stopEditRefLineSlot();

private:
    void setViewerMode(ViewerMode mode);

    void wheelEvent(QWheelEvent *event) override;
    void mousePressEvent(QMouseEvent *event) override;
    void mouseMoveEvent(QMouseEvent* event) override;
    void mouseReleaseEvent(QMouseEvent* event) override;
    void mouseDoubleClickEvent(QMouseEvent* event) override;
    void contextMenuEvent(QContextMenuEvent* event) override;

private:
    DataManager* _data_manager;
    RefLineManager* _refline_manager;

    ViewerMode _viewer_mode = MODE_NORMAL;
    Qt::MouseButton _last_pressed_button = Qt::NoButton;
//    QGraphicsEllipseItem* _pressed_pt_item = nullptr;
    QPointF _last_mouse_pos;
    bool _can_move = false;
    bool _has_moved = false;
    int _inserted_index = -1;

    QMenu* _draw_menu;
    QMenu* _edit_menu;

    VehicleItem* _vehicle_item;
};

#endif // RECORD_VIEWER_H
