#ifndef NOMORATORSCENE_H
#define NOMORATORSCENE_H

#include <QGraphicsScene>
#include <QObject>
#include <QtDebug>

class NomoratorScene : public QGraphicsScene
{
    Q_OBJECT
    Q_PROPERTY(QSizeF pageSize READ pageSize WRITE setPageSize NOTIFY pageSizeChanged)

    QSizeF m_pageSize;
    QPixmap *bgImage;

public:
    NomoratorScene(QObject *parent = nullptr);
    const QSizeF &pageSize() const;
    QRectF sceneRect() const;

public slots:
    void setPageSize(const QSizeF& nsize);
    void setBgImage(const QString& fileName);

signals:
    void pageSizeChanged();
    void bgImageChanged();

protected:
    virtual void drawBackground(QPainter *painter, const QRectF &rect) override;

};

#endif // NOMORATORSCENE_H
