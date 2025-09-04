#ifndef NCONTROLLER_H
#define NCONTROLLER_H
#include "nomorator.h"
#include "nomoratorview.h"
#include "nomoratorscene.h"

#include <QObject>
#include <QString>
#include <QVariant>

#define INLINE_AUTO(T, FN) \
    inline T FN() const { return nomorator.FN(); }

class Controller : public QObject
{
    Q_OBJECT
    Nomorator nomorator;
    NomoratorView nview;

    QSizeF m_pageSize;
    QSizeF m_dupOffset;
    QSizeF m_offsets;
    QPointF m_startPos;
    int m_displayPage;
    QFont m_font;
    Q_PROPERTY(QSizeF pageSize READ pageSize WRITE setPageSize NOTIFY pageSizeChanged)
    Q_PROPERTY(QSizeF dupOffset READ dupOffset WRITE setDupOffset NOTIFY dupOffsetChanged)
    Q_PROPERTY(QPointF startPos READ startPos WRITE setStartPos NOTIFY startPosChanged)
    Q_PROPERTY(int displayPage READ displayPage WRITE setDisplayPage RESET resetDisplayPage NOTIFY displayPageChanged)
    Q_PROPERTY(QSizeF offsets READ offsets WRITE setOffsets NOTIFY offsetsChanged)
    Q_PROPERTY(QFont font READ font WRITE setFont NOTIFY fontChanged)

public:
    Controller(QObject *parent = nullptr);
    inline Nomorator *getNomorator() { return &nomorator; }
    inline NomoratorView *getView() { return &nview; }

    // BEGIN_NOMORATOR_STUFF
public:
    INLINE_AUTO(int, start)
    INLINE_AUTO(int, stop)
    INLINE_AUTO(int, row)
    INLINE_AUTO(int, column)
    INLINE_AUTO(int, bind)
    INLINE_AUTO(int, padCount)
    INLINE_AUTO(int, pageDimension)
    INLINE_AUTO(int, totalPage)
    INLINE_AUTO(int, upper)
    INLINE_AUTO(string, prefix)
    INLINE_AUTO(string, sufix)
    INLINE_AUTO(bool, isReversed)
    INLINE_AUTO(char, padChar)

    const QSizeF &pageSize() const;
    void setPageSize(const QSizeF &newPageSize);
    const QSizeF &dupOffset() const;
    void setDupOffset(const QSizeF &newDupOffset);
    QPointF startPos() const;
    void setStartPos(QPointF newStartPos);
    int displayPage() const;
    void setDisplayPage(int newDisplayPage);
    void resetDisplayPage();

    const QSizeF &offsets() const;
    void setOffsets(const QSizeF &newOffsets);

    const QFont &font() const;
    void setFont(const QFont &newFont);

public slots:
    void setStart(const int& val);
    void setStop(const int& val);
    void setRow(const int& val);
    void setColumn(const int& val);
    void setBind(const int& val);
    void setPad(const int& val);
    void setPad(const char& val);
    void setPad(const QString& val);
    void setReverse(const bool& val);
    void setPrefix(const QString& val);
    void setSufix(const QString& val);
    void updateView();

signals:
    void parameterChanged(const QString& key, const QVariant& var=QVariant());
    // END_NOMORATOR_STUFF
    void pageWidthChanged();
    void pageSizeChanged();
    void dupOffsetChanged();
    void startPosChanged(const QPointF& p=QPointF());
    void displayPageChanged();
    void offsetsChanged();
    void fontChanged();

private:
    QGraphicsSimpleTextItem *m_first;
    void recreateTemplate();
};
#undef INLINE_AUTO
#endif // NCONTROLLER_H
