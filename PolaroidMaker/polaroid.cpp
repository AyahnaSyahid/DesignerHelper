#include "polaroid.h"

#include <QGraphicsBlurEffect>
#include <QGraphicsItem>
#include <QGraphicsPixmapItem>
#include <QGraphicsScene>
#include <QIcon>
#include <QImageReader>
#include <QPixmapCache>
#include <QtMath>

QImage _readImage(const QString &s) {
  QImageReader reader(s);
  reader.setAutoTransform(true);
  return reader.read();
}

QSize Polaroid::pixelSize(int dpi) {
  auto dpmm = 1 / 25.4 * dpi;
  auto sf = m_size * dpmm;
  return sf.toSize();
}

QPoint Polaroid::pixelPosition(int dpi) {
  auto dpmm = dpi / 25.4;
  auto x = m_position.x() * dpmm;
  auto y = m_position.y() * dpmm;
  QPoint ret(x, y);
  return ret;
}

void Polaroid::applyGamma(QImage *tg, const float &gamma) {
  auto ctable = tg->colorTable();
  if (!ctable.isEmpty()) {
    for (auto &color : ctable) {
      const auto r = qRed(color) / 255.0;
      const auto g = qGreen(color) / 255.0;
      const auto b = qBlue(color) / 255.0;
      const auto a = qAlpha(color) / 255.0;
      auto col = qRgba(255 * pow(r, gamma), 255 * pow(g, gamma),
                       255 * pow(b, gamma), 255 * pow(a, gamma));
      color = col;
    }
    tg->setColorTable(ctable);
    return;
  }
  for (int w = 0; w < tg->width(); ++w) {
    for (int h = 0; h < tg->height(); ++h) {
      const auto px = tg->pixel(w, h);
      const auto r = qRed(px) / 255.0;
      const auto g = qGreen(px) / 255.0;
      const auto b = qBlue(px) / 255.0;
      const auto a = qAlpha(px) / 255.0;
      auto col = qRgba(255 * pow(r, gamma), 255 * pow(g, gamma),
                       255 * pow(b, gamma), 255 * pow(a, gamma));
      tg->setPixel(w, h, col);
    }
  }
}

Polaroid::Polaroid(const QString &image, const QSizeF &size,
                   const QPointF &position, bool e_cropmark,
                   qreal cropmark_size, qreal cropmark_weight,
                   const QColor &cropmark_color, const QColor &background_color,
                   Polaroid::ScaleMode scale)
    : m_image(image),
      m_size(size),
      m_position(position),
      e_cropmark(e_cropmark),
      cropmark_size(std::move(cropmark_size)),
      cropmark_weight(std::move(cropmark_weight)),
      cropmark_color(cropmark_color),
      background_color(background_color),
      m_scale(scale),
      r_gamma(1.0f) {}

QImage Polaroid::getImage() {
  QImage m = _readImage(m_image);
  if (m.isNull()) {
    QImage ret(256, 256, QImage::Format_ARGB32);
    ret.fill(Qt::black);
  }
  // max image is 1200
  if (m.height() > 1200 || m.width() > 1200) {
    m = m.scaled(QSize(1200, 1200), Qt::KeepAspectRatio,
                 Qt::SmoothTransformation);
  }
  if (r_gamma == 1.0f) {
    return m;
  }
  applyGamma(&m, r_gamma);
  return m;
}

QIcon Polaroid::getIcon(const QSize &sz) {
  if (!m_image.isEmpty()) {
    auto i = QIcon(QPixmap::fromImage(getImage()));
    return i;
  }
  QPixmap px(sz);
  px.fill(Qt::transparent);
  return QIcon(px);
}

void Polaroid::setSize(const QSizeF &sz) {
  if (sz == m_size) return;
  m_size = sz;
}

void Polaroid::setPoint(const QPointF &ps) {
  if (ps == m_position) return;
  m_position = ps;
}

void Polaroid::setImage(const QString &fl) {
  if (fl == m_image) return;
  m_image = fl;
}

void Polaroid::enableCropmark(bool c) {
  if (e_cropmark != c) e_cropmark = c;
}

void Polaroid::setScaleMode(ScaleMode sm) {
  if (m_scale == sm) return;
  m_scale = sm;
}

void Polaroid::render(QPainter *ptr, const QPointF &pos) {
  auto lastpos = m_position;
  m_position = pos;
  auto ldpi = ptr->device()->logicalDpiX();
  auto psize = pixelSize(ldpi);
  auto ppos = pixelPosition(ldpi);

  auto fRect = QRect(ppos, psize);
  auto cRect = contentsRect(fRect);
  ptr->fillRect(fRect, background_color);
  QImage imf = getImage();
  if (!imf.isNull()) {
    auto scaled = imf.scaled(cRect.size(), Qt::KeepAspectRatioByExpanding,
                             Qt::SmoothTransformation);
    if (m_scale == Expanded) {
      auto srcRect = cRect;
      srcRect.moveCenter(scaled.rect().center());
      ptr->drawImage(cRect, scaled, srcRect);
    } else {
      QGraphicsScene scene;
      QGraphicsPixmapItem pixi(QPixmap::fromImage(scaled));
      QGraphicsPixmapItem full(QPixmap::fromImage(imf.scaled(
          cRect.size(), Qt::KeepAspectRatio, Qt::SmoothTransformation)));
      QGraphicsBlurEffect be;
      scene.addItem(&pixi);
      scene.addItem(&full);
      be.setBlurHints(be.QualityHint);
      be.setBlurRadius(ldpi / 5);
      pixi.setGraphicsEffect(&be);
      full.setX((cRect.width() - full.boundingRect().width()) / 2);
      full.setY(pixi.boundingRect().center().y() -
                full.boundingRect().center().y());
      auto srcRect = cRect;
      srcRect.moveCenter(pixi.boundingRect().center().toPoint());
      scene.render(ptr, cRect, srcRect, Qt::IgnoreAspectRatio);
    }
  } else {
    ptr->fillRect(cRect, QColor(Qt::gray).darker(250));
  }
  if (e_cropmark) {
    uint cpstep = cropmark_size / 25.4 * ldpi;
    uint cpweight = cropmark_weight / 25.4 * ldpi;
    ptr->save();
    auto pen = ptr->pen();
    pen.setWidth(cpweight);
    pen.setColor(cropmark_color);
    pen.setJoinStyle(Qt::MiterJoin);
    ptr->setPen(pen);
    QPainterPath pp;
    pp.moveTo(fRect.left() + cpstep, fRect.top());
    pp.lineTo(fRect.topLeft());
    pp.lineTo(fRect.left(), fRect.top() + cpstep);
    pp.moveTo(fRect.left(), fRect.bottom() - cpstep);
    pp.lineTo(fRect.bottomLeft());
    pp.lineTo(fRect.left() + cpstep, fRect.bottom());
    pp.moveTo(fRect.right() - cpstep, fRect.bottom());
    pp.lineTo(fRect.bottomRight());
    pp.lineTo(fRect.right(), fRect.bottom() - cpstep);
    pp.moveTo(fRect.right(), fRect.top() + cpstep);
    pp.lineTo(fRect.topRight());
    pp.lineTo(fRect.right() - cpstep, fRect.top());
    ptr->drawPath(pp);
    ptr->restore();
  }
  m_position = lastpos;
}

void Polaroid::setGamma(const float &gm) {
  if (!qFuzzyCompare(gm, r_gamma)) r_gamma = gm;
  return;
}

QRect Polaroid::contentsRect(const QRect &f) const {
  QRect n(f);
  n.setWidth(f.width() * 0.86);
  n.setHeight(f.height() * 0.734);
  n.adjust((f.width() - n.width()) / 2, (f.height() - n.height()) / 5,
           (f.width() - n.width()) / 2, (f.height() - n.height()) / 5);
  return n;
}

QPixmap Polaroid::getPixmap() const {
  QImage im = _readImage(m_image);
  if (!im.isNull()) {
    QPixmap fallback(100, 100);
    fallback.fill(Qt::white);
    return fallback;
  }
  if (r_gamma != 1.0f) Polaroid::applyGamma(&im, r_gamma);
  return QPixmap::fromImage(im);
}
