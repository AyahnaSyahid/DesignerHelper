#include "nomoratorscene.h"

#include <QGraphicsItem>
#include <QGraphicsRectItem>
#include <QPainter>
#include <QPen>

NomoratorScene::NomoratorScene(QObject *parent)
    : QGraphicsScene(parent), m_pageSize(325, 485), bgImage(nullptr) {
  setBackgroundBrush(QColor(200, 200, 200));
  setItemIndexMethod(NoIndex);
  setSceneRect(-1000, -1000, 2500, 2500);
}

void NomoratorScene::drawBackground(QPainter *painter, const QRectF &rect) {
  QGraphicsScene::drawBackground(painter, rect);
  painter->save();
  QBrush brush(Qt::gray);
  painter->setPen(Qt::NoPen);
  painter->setBrush(brush);
  painter->drawRect(5, 5, m_pageSize.width(), m_pageSize.height());
  QPen pen(Qt::gray, 0);
  painter->setPen(pen);
  painter->setBrush(Qt::white);
  if (bgImage) {
    painter->drawPixmap(QRectF(0, 0, m_pageSize.width(), m_pageSize.height()),
                        *bgImage, bgImage->rect());
  } else {
    painter->drawRect(0, 0, m_pageSize.width(), m_pageSize.height());
  }
  painter->restore();
}

const QSizeF &NomoratorScene::pageSize() const { return m_pageSize; }

void NomoratorScene::setPageSize(const QSizeF &nsize) {
  if (nsize == m_pageSize) return;
  m_pageSize = nsize;
  qDebug() << "scene page size changed";
  update();
}

void NomoratorScene::setBgImage(const QString &fileName) {
  if (bgImage) delete bgImage;
  if (fileName.isEmpty()) {
    bgImage = nullptr;
  } else {
    bgImage = new QPixmap(fileName);
    if (bgImage->isNull()) {
      delete bgImage;
      bgImage = nullptr;
    }
  }
}
