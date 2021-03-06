#include "editor_viewer.h"

#include <QDebug>

#include <QAction>
#include <QScrollBar>
#include <QContextMenuEvent>

EditorViewer::EditorViewer(DataManager *data_manager, QWidget *parent) :
        QGraphicsView(parent), _data_manager(data_manager)
{
    this->setFrameShape(QFrame::NoFrame);
    this->setTransformationAnchor(AnchorUnderMouse);
    this->setDragMode(QGraphicsView::ScrollHandDrag);
    this->setSizePolicy(QSizePolicy::Preferred, QSizePolicy::Preferred);
    this->setHorizontalScrollBarPolicy(Qt::ScrollBarAsNeeded);
    this->setVerticalScrollBarPolicy(Qt::ScrollBarAsNeeded);
    this->setRenderHint(QPainter::Antialiasing);
    this->setContextMenuPolicy(Qt::DefaultContextMenu);

    _refline_manager = _data_manager->getRefLineManager();

    QGraphicsScene* scene = new QGraphicsScene(this);
    scene->addItem(_data_manager->getMapManager()->getMapItemGroup());
    scene->addItem(_data_manager->getTaskManager()->getTaskItemGroup());
    scene->addItem(_data_manager->getRefLineManager()->getEditRefLineGroup());
    _vehicle_item = new VehicleItem(this);
    scene->addItem(_vehicle_item->getVehicleGroup());
    this->setScene(scene);
    QTransform transform(10, 0, 0, -10, 0, 0);
    this->setTransform(transform);

    _draw_menu = new QMenu(this);
    _draw_menu->addAction(tr("pop"), this, SIGNAL(emitPopRefPoint()));
    _draw_menu->addAction(tr("clear"), this, SIGNAL(emitClearRefPoint()));

    connect(this, SIGNAL(emitAppendRefPoint(float,float)),
            _data_manager->getTaskManager(), SLOT(appendRefPoint(float,float)));
    connect(this, SIGNAL(emitPopRefPoint()),
            _data_manager->getTaskManager(), SLOT(popRefPoint()));
    connect(this, SIGNAL(emitClearRefPoint()),
            _data_manager->getTaskManager(), SLOT(clearRefLine()));
}

EditorViewer::~EditorViewer()
{

}

void EditorViewer::startRecordSlot()
{
    this->setViewerMode(MODE_RECORD_REFLINE);
    _vehicle_item->setVisible(true);
}

void EditorViewer::stopRecordSlot()
{
    this->setViewerMode(MODE_NORMAL);
    _vehicle_item->setVisible(false);
}

void EditorViewer::startDrawRefLineSlot()
{
    this->setViewerMode(MODE_DRAW_REFLINE);
}

void EditorViewer::stopDrawRefLineSlot()
{
    this->setViewerMode(MODE_NORMAL);
}

void EditorViewer::startEditRefLineSlot()
{
    this->setViewerMode(MODE_EDIT_REFLINE);
}

void EditorViewer::stopEditRefLineSlot()
{
    this->setViewerMode(MODE_NORMAL);
}

void EditorViewer::setViewerMode(EditorViewer::ViewerMode mode)
{
    _viewer_mode = mode;
    switch (_viewer_mode) {
    case MODE_NORMAL:
        this->setDragMode(QGraphicsView::ScrollHandDrag);
        break;
    case MODE_RECORD_REFLINE:
        this->setDragMode(QGraphicsView::ScrollHandDrag);
        break;
    case MODE_DRAW_REFLINE:
        this->setDragMode(QGraphicsView::NoDrag);
        this->setCursor(QCursor(Qt::CrossCursor));
        break;
    case MODE_EDIT_REFLINE:
        this->setDragMode(QGraphicsView::RubberBandDrag);
        this->setCursor(QCursor(Qt::ArrowCursor));
        break;
    default:
        break;
    }
    _last_pressed_button = Qt::NoButton;
}

void EditorViewer::wheelEvent(QWheelEvent *event)
{
    qreal s = event->delta() > 0 ? 1.25 : 0.8;
    this->scale(s, s);
}

void EditorViewer::mousePressEvent(QMouseEvent *event)
{
    _last_pressed_button = event->button();
    _last_mouse_pos = event->pos();
    if (event->button() == Qt::LeftButton) {
        this->processLeftMousePressEvent(event);
    } else if (event->button() == Qt::RightButton) {
        this->processRightMousePressEvent(event);
    }/* else {
        QGraphicsView::mousePressEvent(event);
    }*/
}

void EditorViewer::mouseMoveEvent(QMouseEvent *event)
{
    if (_last_pressed_button == Qt::LeftButton) {
        this->processLeftMouseMoveEvent(event);
    } else if (_last_pressed_button == Qt::RightButton) {
        this->processRightMouseMoveEvent(event);
    } else {
        QGraphicsView::mouseMoveEvent(event);
    }
}

void EditorViewer::mouseReleaseEvent(QMouseEvent *event)
{
    _last_pressed_button = Qt::NoButton;
    if (event->button() == Qt::LeftButton) {
        this->processLeftMouseReleaseEvent(event);
    } else if (event->button() == Qt::RightButton) {
        this->processRightMouseReleaseEvent(event);
    }/* else {
        QGraphicsView::mouseReleaseEvent(event);
    }*/
}

void EditorViewer::mouseDoubleClickEvent(QMouseEvent *event)
{
    _refline_manager->updateSelectedPointIndex(-1);
    QGraphicsView::mouseDoubleClickEvent(event);
}

void EditorViewer::contextMenuEvent(QContextMenuEvent *event)
{
    if (_viewer_mode == MODE_DRAW_REFLINE) {
        _draw_menu->exec(QCursor::pos());
    } else {
        QGraphicsView::contextMenuEvent(event);
    }
}

void EditorViewer::processLeftMousePressEvent(QMouseEvent *event)
{
    _has_moved = false;
    _can_move = false;
    if (_viewer_mode == MODE_EDIT_REFLINE) {
        QList<QGraphicsItem*> items = this->items(event->pos());
        if (!items.empty()) {
            for (const auto& item : items) {
                if (!item->data(IndexRole).isNull()) {
                    _can_move = true;
                    break;
                }
            }
        }
    }
    QGraphicsView::mousePressEvent(event);
    return;
}

void EditorViewer::processRightMousePressEvent(QMouseEvent *event)
{
    if (_viewer_mode == MODE_EDIT_REFLINE) {
        this->setCursor(QCursor(Qt::ClosedHandCursor));
    }
    QGraphicsView::mousePressEvent(event);
}

void EditorViewer::processLeftMouseMoveEvent(QMouseEvent *event)
{
    if (_viewer_mode == MODE_EDIT_REFLINE && _can_move) {
        QPointF cur_pos = this->mapToScene(event->pos());
        QPointF move_pos = cur_pos - this->mapToScene(_last_mouse_pos);
        _refline_manager->updateSelectedPointsPos(move_pos);
        _has_moved = true;
        _last_mouse_pos = event->pos();
    } else {
        QGraphicsView::mouseMoveEvent(event);
    }
}

void EditorViewer::processRightMouseMoveEvent(QMouseEvent *event)
{
    if (_viewer_mode == MODE_EDIT_REFLINE) {
        QScrollBar *hBar = horizontalScrollBar();
        QScrollBar *vBar = verticalScrollBar();
        QPoint delta = event->pos() - _last_mouse_pos;
        hBar->setValue(hBar->value() + (isRightToLeft() ? delta.x() : -delta.x()));
        vBar->setValue(vBar->value() - delta.y());
        _last_mouse_pos = event->pos();
    } else {
        QGraphicsView::mouseMoveEvent(event);
    }
}

void EditorViewer::processLeftMouseReleaseEvent(QMouseEvent *event)
{
    QPointF pt = this->mapToScene(event->pos());
    if (_viewer_mode == MODE_DRAW_REFLINE) {
        emit emitAppendRefPoint(pt.x(), pt.y());
    } else if (_viewer_mode == MODE_EDIT_REFLINE) {
        if (!_has_moved) {
            int nearest_index = _refline_manager->searchNearestPtIndex(pt);
            if (nearest_index >= 0) {
                _refline_manager->updateSelectedPointIndex(nearest_index);
            }
        }
        QRectF rubber_rect = this->rubberBandRect();
        QRectF mapped_rect;
        if (rubber_rect.width() != 0) {
            QPolygonF rect_pts = this->mapToScene(rubber_rect.x(), rubber_rect.y(),
                                                  rubber_rect.width(), rubber_rect.height());
            mapped_rect.setTopLeft(rect_pts.at(0));
            mapped_rect.setBottomRight(rect_pts.at(2));
            _refline_manager->setSelectedPointRect(mapped_rect);
        }
    }
    QGraphicsView::mouseReleaseEvent(event);
}

void EditorViewer::processRightMouseReleaseEvent(QMouseEvent *event)
{
    if (_viewer_mode == MODE_EDIT_REFLINE) {
        this->setCursor(QCursor(Qt::ArrowCursor));
    }
    QGraphicsView::mouseReleaseEvent(event);
}

void EditorViewer::updateLocalization(const float x, const float y, const float theta)
{
    if (_viewer_mode != MODE_RECORD_REFLINE) {
        return;
    }
    float sin_value = std::sin(theta);
    float cos_value = std::cos(theta);
    QTransform transform(cos_value, sin_value, -sin_value, cos_value, x, y);
    _vehicle_item->update(transform);
    this->centerOn(x, y);
}


