#include "ncontroller.h"
#include <QGraphicsTextItem>

const QSizeF &Controller::pageSize() const
{
    return m_pageSize;
}

void Controller::setPageSize(const QSizeF &newPageSize)
{
    if (m_pageSize == newPageSize)
        return;
    m_pageSize = newPageSize;
    nview.setPageSize(m_pageSize);
    emit pageSizeChanged();
}

const QSizeF &Controller::dupOffset() const
{
    return m_dupOffset;
}

void Controller::setDupOffset(const QSizeF &newDupOffset)
{
    if (m_dupOffset == newDupOffset)
        return;
    m_dupOffset = newDupOffset;
    recreateTemplate();
    emit dupOffsetChanged();
}

QPointF Controller::startPos() const
{
    return m_startPos;
}

void Controller::setStartPos(QPointF newStartPos)
{
    if (m_startPos == newStartPos)
        return;
    m_startPos = newStartPos;
    emit startPosChanged();
    updateView();
}

int Controller::displayPage() const
{
    return m_displayPage;
}

void Controller::setDisplayPage(int newDisplayPage)
{
    if (m_displayPage == newDisplayPage)
        return;
    m_displayPage = newDisplayPage;
    emit displayPageChanged();
    updateView();
}

void Controller::resetDisplayPage()
{
    setDisplayPage(0); // TODO: Adapt to use your actual default value
}

const QSizeF &Controller::offsets() const
{
    return m_offsets;
}

void Controller::setOffsets(const QSizeF &newOffsets)
{
    if (m_offsets == newOffsets)
        return;
    m_offsets = newOffsets;
    qDebug() << m_offsets;
    updateView();
    emit offsetsChanged();
}

const QFont &Controller::font() const
{
    return m_font;
}

void Controller::setFont(const QFont &newFont)
{
    if (m_font == newFont)
        return;
    m_font = newFont;
    emit fontChanged();
    updateView();
}

Controller::Controller(QObject *parent) : QObject(parent),
    nomorator(), nview(), m_dupOffset(0, 0), m_offsets(100, 100), m_startPos(0, 0), m_displayPage(0)
{
    recreateTemplate();
}

void Controller::setStart(const int &val)
{
    if(start() != val) {
        int cpage = totalPage();
        nomorator.setStart(val);
        if(totalPage() != cpage)
            emit parameterChanged("totalPage", totalPage());
    }
    updateView();
}

void Controller::setStop(const int &val)
{
    if(stop() != val) {
        int cpage = totalPage();
        nomorator.setStop(val);
        if(totalPage() != cpage)
            emit parameterChanged("totalPage", totalPage());
    }
    updateView();
}

void Controller::setRow(const int &val)
{
    if(row() != val) {
        nomorator.setRow(val);
        auto pc = totalPage();
        recreateTemplate();
        emit parameterChanged("pageDimension", pageDimension());
        if(pc != totalPage())
            emit parameterChanged("totalPage", totalPage());
        updateView();
    }
}

void Controller::setColumn(const int &val)
{
    if(column() != val) {
        auto pc = totalPage();
        nomorator.setColumn(val);
        recreateTemplate();
        emit parameterChanged("pageDimension", pageDimension());
        if(pc != totalPage()) {
            emit parameterChanged("totalPage", totalPage());
        }
        updateView();
    }
}

void Controller::setBind(const int &val)
{
    if(bind() != val) {
        nomorator.setBind(val);
        emit parameterChanged("bindCount", bind());
    }
    updateView();
}

void Controller::setPad(const int &val)
{
    if(padCount() != val) {
        nomorator.setPadCount(val);
        emit parameterChanged("padCount", padCount());
    }
    updateView();
}

void Controller::setPad(const char &val)
{
    if(padChar() != val) {
        nomorator.setPadChar(val);
        emit parameterChanged("padChar", padChar());
    }
    updateView();
}

void Controller::setPad(const QString &val)
{
    if(val.isEmpty()) return;
    setPad(val.toStdString().at(0));
}

void Controller::setReverse(const bool &val)
{
    if(isReversed() != val) {
        nomorator.setReverse(val);
        emit parameterChanged("pageOrder", isReversed());
    }
}

void Controller::setPrefix(const QString &val)
{
    if(prefix() != val.toStdString()) {
        nomorator.setPrefix(val.toStdString());
        emit parameterChanged("prefixStr", val);
    }
    updateView();
}

void Controller::setSufix(const QString &val)
{
    if(sufix() != val.toStdString()) {
        nomorator.setSufix(val.toStdString());
        emit parameterChanged("suffixStr", val);
    }
    updateView();
}

void Controller::updateView()
{
    return recreateTemplate();
    using gti = QGraphicsSimpleTextItem;
    auto cpage = nomorator.getPage(m_displayPage);
    if(cpage.data.size() < 1) return;
    auto items = nview.scene()->items(Qt::AscendingOrder);
    if(items.size() < 1) {
        recreateTemplate();
        items = nview.scene()->items(Qt::AscendingOrder);
        return;
    }
    int dim[2] {nomorator.row(), nomorator.column()};
    int idx = 0;
    gti *current_item;
    for(int r=0; r<dim[0]; ++r) {
        for(int c=0; c<dim[1]; ++c) {
            idx = r * dim[1] + c;
            current_item = qgraphicsitem_cast<gti*>(items.at(idx));
            if(idx == 0) {
                continue;
            }
            if(c % 2) {
                if(!m_dupOffset.isNull()) {
                    current_item->setText(cpage.data[r][c].c_str());
                    current_item->setPos(m_startPos + QPointF(m_dupOffset.width() * c, m_dupOffset.height() * r));
                    continue;
                }
            }
            current_item->setText(cpage.data[r][c].c_str());
            current_item->setPos(r * m_offsets.width(), c * m_offsets.height());
        }
    }
}

void Controller::recreateTemplate()
{
    m_first = nullptr;
    using gti = QGraphicsSimpleTextItem;
    auto scene = nview.scene();
    scene->clear();
    auto cpage = nomorator.getPage(0);
    QString cstr(cpage.data[0][0].c_str());
    auto first = scene->addSimpleText(cstr, m_font);
    first->setPos(m_startPos);
    first->setFlags(gti::ItemIsSelectable | gti::ItemIsMovable);
    m_first = first;
    m_first->setData(1001, "first");
    gti *dupit = nullptr;
    if(!m_dupOffset.isNull()) {
        dupit = new gti(cstr, first);
        dupit->moveBy(m_dupOffset.width(), m_dupOffset.height());
        dupit->setFlags(gti::ItemIsSelectable | gti::ItemIsMovable);
    }
    for(int r = 0; r<row(); ++r) {
        for(int c = 0; c<column(); ++c) {
            if(r == 0 && c == 0) continue;
            const QString p(cpage.data[r][c].c_str());
            auto n = new gti(p, first);
            n->moveBy(m_offsets.width() * c, m_offsets.height() * r);
            n->setFlags(gti::ItemIsSelectable | gti::ItemIsMovable);
            if(dupit) {
                auto d = new gti(p, dupit);
                d->moveBy(m_offsets.width() * c, m_offsets.height() * r);
                d->setFlags(gti::ItemIsSelectable | gti::ItemIsMovable);
            }
        }
    }

}

