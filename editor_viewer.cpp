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
    _vehicle_item = new VehicleItem(this);
    scene->addItem(_vehicle_item->getVehicleGroup());
    this->setScene(scene);
    QTransform transform(10, 0, 0, -10, 0, 0);
    this->setTransform(transform);

    connect(_data_manager, SIGNAL(emitStartRosSpin()), this, SLOT(startRecordSlot()));
    connect(_data_manager, SIGNAL(emitStopRosSpin()), this, SLOT(stopRecordSlot()));
    connect(_data_manager, SIGNAL(emitDrawReflineFlag(bool)),
            this, SLOT(drawReflineFlagSlot(bool)));

    _draw_menu = new QMenu(this);
    _draw_menu->addAction(tr("pop"), this, SIGNAL(emitPopRefPoint()));
    _draw_menu->addAction(tr("finish"), this, SIGNAL(emitFinishDrawRefline()));
    _draw_menu->addAction(tr("clear"), this, SIGNAL(emitClearRefPoint()));

    connect(this, SIGNAL(emitAppendRefPoint(float,float)),
            _data_manager, SLOT(appendRefPoint(float,float)));
    connect(this, SIGNAL(emitPopRefPoint()), _data_manager, SLOT(popRefPoint()));
    connect(this, SIGNAL(emitFinishDrawRefline()), _data_manager, SLOT(finishDrawRefline()));
    connect(this, SIGNAL(emitClearRefPoint()), _data_manager, SLOT(clearRefPoint()));
}

EditorViewer::~EditorViewer()
{

}

void EditorViewer::startRecordSlot()
{
    _vehicle_item->setVisible(true);
}

void EditorViewer::stopRecordSlot()
{
    _vehicle_item->setVisible(false);
}

void EditorViewer::drawReflineFlagSlot(const bool flag)
{
    if (flag) {
        this->setViewerMode(ViewerMode::MODE_DRAW_REFLINE);
    } else {
        this->setViewerMode(ViewerMode::MODE_NORMAL);
    }
}

void EditorViewer::setViewerMode(EditorViewer::ViewerMode mode)
{
    _viewer_mode = mode;
    switch (_viewer_mode) {
    case MODE_NORMAL:
        this->setDragMode(QGraphicsView::ScrollHandDrag);
        break;
    case MODE_DRAW_REFLINE:
        this->setDragMode(QGraphicsView::NoDrag);
        this->setCursor(QCursor(Qt::CrossCursor));
        break;
    case MODE_SELECT_POINT:
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
    case MODE_SELECT_POINT:
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
    case MODE_SELECT_POINT:
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
    case MODE_SELECT_POINT:
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
    float sin_value = std::sin(theta);
    float cos_value = std::cos(theta);
    QTransform transform(cos_value, sin_value, -sin_value, cos_value, x, y);
    _vehicle_item->update(transform);
    this->centerOn(x, y);
}
