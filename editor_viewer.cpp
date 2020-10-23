#include "editor_viewer.h"

#include <QDebug>

#include <QAction>
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
        this->setDragMode(QGraphicsView::NoDrag);
        this->setCursor(QCursor(Qt::ArrowCursor));
        break;
    default:
        break;
    }
}

void EditorViewer::wheelEvent(QWheelEvent *event)
{
    qreal s = event->delta() > 0 ? 1.25 : 0.8;
    this->scale(s, s);
}

void EditorViewer::mousePressEvent(QMouseEvent *event)
{
    if (event->button() != Qt::LeftButton) {
        QGraphicsView::mousePressEvent(event);
        return;
    }
    switch (_viewer_mode) {
    case MODE_NORMAL:
        break;
    case MODE_DRAW_REFLINE:
        break;
    case MODE_EDIT_REFLINE:
        break;
    default:
        break;
    }

    QGraphicsView::mousePressEvent(event);
}

void EditorViewer::mouseMoveEvent(QMouseEvent *event)
{
    if (event->button() != Qt::LeftButton) {
        QGraphicsView::mouseMoveEvent(event);
        return;
    }

    switch (_viewer_mode) {
    case MODE_NORMAL:
        break;
    case MODE_DRAW_REFLINE:
        break;
    case MODE_EDIT_REFLINE:
        break;
    default:
        break;
    }

    QGraphicsView::mouseMoveEvent(event);
}

void EditorViewer::mouseReleaseEvent(QMouseEvent *event)
{
    if (event->button() != Qt::LeftButton) {
        QGraphicsView::mouseReleaseEvent(event);
        return;
    }

    QPointF pt = this->mapToScene(event->pos());
    switch (_viewer_mode) {
    case MODE_NORMAL:
        break;
    case MODE_DRAW_REFLINE:
        emit emitAppendRefPoint(pt.x(), pt.y());
        break;
    case MODE_EDIT_REFLINE:
        break;
    default:
        break;
    }

    QGraphicsView::mouseReleaseEvent(event);
}

void EditorViewer::contextMenuEvent(QContextMenuEvent *event)
{
    if (_viewer_mode == MODE_DRAW_REFLINE) {
        _draw_menu->exec(QCursor::pos());
    } else {
        QGraphicsView::contextMenuEvent(event);
    }
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


