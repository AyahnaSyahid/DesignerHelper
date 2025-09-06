#include "nomoratorview.h"

#include <QGraphicsItem>
#include <QMouseEvent>
#include <QScrollBar>
#include <QtMath>

#include "nomoratorscene.h"

NomoratorView::NomoratorView(QWidget *parent)
    : QGraphicsView(parent), nscene(new NomoratorScene(this)) {
  setScene(nscene);
  setCursor(Qt::CursorShape::CrossCursor);
  setDragMode(RubberBandDrag);
  setViewportUpdateMode(BoundingRectViewportUpdate);
  setRenderHint(QPainter::HighQualityAntialiasing);
  // LIEUR
  setTransformationAnchor(NoAnchor);
}

void NomoratorView::setPageSize(const QSizeF &newSize) {
  nscene->setPageSize(newSize);
  qDebug() << "view pagesize changed";
}

void NomoratorView::setBgImage(const QString &file) {
  nscene->setBgImage(file);
}

void NomoratorView::clearBg() {
  nscene->setBgImage("");
  nscene->update();
}

void NomoratorView::mousePressEvent(QMouseEvent *event) {
  last_press = event->pos();
  if (event->button() == Qt::MidButton) {
    setCursor(Qt::ClosedHandCursor);
    event->accept();
    return;
  }
  if (event->button() == Qt::LeftButton) {
    QGraphicsView::mousePressEvent(event);
  }
}

void NomoratorView::mouseReleaseEvent(QMouseEvent *event) {
  qDebug() << "Mouse Released" << event->button();
  if (event->button() == Qt::MidButton) {
    setCursor(Qt::ArrowCursor);
    event->accept();
    return;
  } else if (event->button() == Qt::LeftButton) {
    fv = nullptr;
    QGraphicsView::mouseReleaseEvent(event);
  }
}

void NomoratorView::mouseMoveEvent(QMouseEvent *event) {
  if (event->buttons() == Qt::MidButton) {
    QPoint delta = last_press - event->pos();
    QScrollBar *ver = verticalScrollBar(), *hor = horizontalScrollBar();
    if (ver) ver->setValue(ver->value() + delta.y());
    if (hor) hor->setValue(hor->value() + delta.x());
    last_press = event->pos();
    event->accept();
    return;
  } else if (event->buttons() == Qt::LeftButton) {
    //        fv = scene()->itemAt(event->pos(), transform());
    //        if(fv && fv->data(1001).toString() == "first") {
    //            auto delta = mapToScene(last_press) -
    //            mapToScene(event->pos()); QPointF fvpos = fv->scenePos() -
    //            delta; emit startPosMoved(fvpos);
    //        }
    QGraphicsView::mouseMoveEvent(event);
    last_press = event->pos();
  }
}

void NomoratorView::wheelEvent(QWheelEvent *event) {
  auto yamount = event->angleDelta().y();
  if (event->modifiers() == Qt::NoModifier) {
    auto oldpos = mapToScene(event->position().toPoint());
    double factor = qPow(1.0015, yamount);
    scale(factor, factor);
    auto curpos = mapToScene(event->position().toPoint());
    auto delta = (curpos - oldpos);
    qDebug() << delta;
    this->translate(delta.x(), delta.y());
    event->accept();
  } else if (event->modifiers() == Qt::SHIFT) {
    rotate(event->angleDelta().y() * 1 / 8);
    event->accept();
  } else {
    QGraphicsView::wheelEvent(event);
  }
}
