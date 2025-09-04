#ifndef POLAROID_H
#define POLAROID_H
#include "PolaroidMaker_global.h"

#include <QRectF>
#include <QImage>
#include <QColor>
#include <QFile>
#include <QFileInfo>
#include <QPainter>


class POLAROIDMAKER_EXPORT Polaroid
{
public:
    enum ScaleMode {
        Blured,
        Expanded
    };

private:
    QString m_image;
    QSizeF m_size;
    QPointF m_position;

    // cropmarks
    bool e_cropmark;
    qreal cropmark_size;
    qreal cropmark_weight;
    QColor cropmark_color;

    // background
    QColor background_color;

    // Scaling
    ScaleMode m_scale;

    // GammaAddjustment
    float r_gamma = 1;

public:
    static void applyGamma(QImage *, const float& gg);
    Polaroid(const QString &image = QString(),
             const QSizeF &size  = {63.6, 95},
             const QPointF& position = {0, 0},
             bool e_cropmark = true,
             qreal cropmark_size = 2,
             qreal cropmark_weight = 0.1,
             const QColor &cropmark_color = Qt::gray,
             const QColor &background_color = Qt::transparent,
             ScaleMode scale = Expanded);

    virtual ~Polaroid(){};
    QImage getImage();
    inline QString imageName() noexcept {return m_image.isEmpty() ? "" : QFileInfo(m_image).baseName();}
    inline const QString imageFile() const noexcept {return m_image;}
    QIcon getIcon(const QSize &sz = QSize(128, 128));
    // translator
    QSize pixelSize(int dpi);
    QPoint pixelPosition(int dpi);

    void setSize(const QSizeF& sz);
    inline const QSizeF& size() {return m_size;}
    inline const QPointF& pos() {return m_position;}
    void setPoint(const QPointF& ps);
    void setImage(const QString& fl);
    void enableCropmark(bool c = true);
    void setScaleMode(ScaleMode sm);
    void render(QPainter *ptr, const QPointF& pos);

    inline const QColor &backgroundColor() const {return background_color;};
    inline void setCropmarkColor(const QColor& ccl) {cropmark_color = ccl;}
    inline void setBackgroudColor(const QColor& bcl) {background_color = bcl;}
    inline const float &gammaValue() const {return r_gamma;}
    void setGamma(const float& gm);
    void setCropmarkSize(const qreal& sz) {cropmark_size = sz;}
    void setCropmarkWeight(const qreal& wgh) {cropmark_weight = wgh;}
    virtual QRect contentsRect(const QRect &f) const;
};

#endif // POLAROID_H
