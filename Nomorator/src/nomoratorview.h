#ifndef NOMORATORVIEW_H
#define NOMORATORVIEW_H

#include <QtDebug>
#include <QGraphicsView>
#include <QRubberBand>

class NomoratorScene;
class NomoratorView : public QGraphicsView {

    QPoint last_press;
    NomoratorScene *nscene;
    Q_OBJECT

public:
    NomoratorView(QWidget *parent = nullptr);
    QGraphicsItem *fv;

public slots:
    void setPageSize(const QSizeF& newSize);
    void setBgImage(const QString& file);
    void clearBg();

protected:
    void mousePressEvent(QMouseEvent *event) override;
    void mouseReleaseEvent(QMouseEvent *event) override;
    void mouseMoveEvent(QMouseEvent *event) override;
    void wheelEvent(QWheelEvent *event) override;

signals:
    void startPosMoved(const QPointF&);
};
#endif
