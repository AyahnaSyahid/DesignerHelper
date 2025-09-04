#include "polaroidmaker.h"
#include "polaroid.h"
#include "updater.h"
#include <QColor>
#include <QPdfWriter>
#include <QMargins>
#include <QtMath>

QVariantMap PolaroidMaker::defaultSettings {
    {"background-color", QColor(Qt::white)},
    {"enable-cropmarks", true},
    {"cropmarks-color", QColor(Qt::gray)},
    {"cropmarks-line-width", 0.076},
    {"cropmarks-width", 2.5},
    {"render-picture-mode", Polaroid::ScaleMode::Expanded}
};

QPair<int, int> PolaroidMaker::getLayoutDimension()
{
    if(p_polsize.width() > pageSize.width()) return {0, 0};
    if(p_polsize.height() > pageSize.height()) return {0, 0};
    int w = pageSize.width() / p_polsize.width();
    int h = pageSize.height()/ p_polsize.height();
    if (w && h)
        return {w, h};
    return {0, 0};
}

void PolaroidMaker::notifyUpdate()
{
    if(this->updateCB)
        updateCB();
}

void PolaroidMaker::progressUpdate(const int &page, const int &polIndex)
{
    if(progressUpdateCB)
        progressUpdateCB(page, polIndex);
}

void PolaroidMaker::setUpdateCB(void (*ucb)())
{
   updateCB = ucb;
}

void PolaroidMaker::setProgressUpdateCB(void (*pucb)(const int &, const int &))
{
    progressUpdateCB = pucb;
}

void PolaroidMaker::createConnection(QProgressBar* polp, QProgressBar* pagep)
{
    updater->connect(updater, &Updater::polaroidUpdate, polp, &QProgressBar::setValue);
    updater->connect(updater, &Updater::pageUpdate, pagep, &QProgressBar::setValue);
}

PolaroidMaker::PolaroidMaker():
    updater(new Updater), settings(defaultSettings)
{}

PolaroidMaker::~PolaroidMaker()
{
    delete updater;
}

int PolaroidMaker::pageToCreate()
{
    if(p_pols.isEmpty())
        return 0;
    auto ld = getLayoutDimension();
    return qCeil(1.0 / (ld.first * ld.second) * p_pols.count());
}

void PolaroidMaker::setGamma(const float &gm)
{
    if(gamma != gm) {
        gamma = gm;
    }
}

void PolaroidMaker::setPageSize(const QSizeF &sf)
{
    if(pageSize != sf) {
        pageSize = sf;
        notifyUpdate();
    }

}

void PolaroidMaker::setPolaroidSize(const QSizeF &ps)
{
    if(p_polsize != ps) {
        p_polsize = ps;
        notifyUpdate();
    }
}

void PolaroidMaker::setDpi(int dpi)
{
    if(p_dpi != dpi) {
        p_dpi = dpi;
        notifyUpdate();
    }
}

void PolaroidMaker::addPolaroid(const QString &pic)
{
    p_pols.append(pic);
    notifyUpdate();
}

void PolaroidMaker::removePolaroid(const int &ind)
{
    if(!p_pols.count())
        return ;
    if(ind >= 0 && ind < p_pols.count()) {
        p_pols.removeAt(ind);
        notifyUpdate();
    }
}

bool PolaroidMaker::writeToFile(const QString &fname)
{
    m_failReason.clear();
    auto ly = getLayoutDimension();
    if(ly.first * ly.second < 1) {
        // Calculation Error
        m_failReason = QString("Invalid Layout %1 x %2").arg(ly.first).arg(ly.second);
        return false;
    }
    QPdfWriter wrt(fname);
    wrt.setPageSize(QPageSize(pageSize, QPageSize::Millimeter, QString("Custom %1x%2").arg(pageSize.width()).arg(pageSize.height()), QPageSize::ExactMatch));
    wrt.setResolution(p_dpi);
    QMarginsF mrgs {0, 0, 0, 0};
    wrt.setPageMargins(mrgs, QPageLayout::Millimeter);
    auto mpi = wrt.logicalDpiX() / 25.4;
    QPainter ptr(&wrt);
    QSizeF drawSize(ly.first * p_polsize.width(),
                    ly.second * p_polsize.height());

    // QPointF fpost((wrt.pageSizeMM().width() - drawSize.width()) / 2,
                  // (wrt.pageSizeMM().height() - drawSize.height()) / 2);

    QPointF fpost((wrt.pageLayout().pageSize().definitionSize().width() - drawSize.width()) / 2,
                  (wrt.pageLayout().pageSize().definitionSize().height() - drawSize.height()) / 2);

    QRectF drawRect(fpost, drawSize);
    QRectF pageNumberRect, pageMarkRect;
    QSizeF markSize(15.0 * mpi, 4.0 * mpi);
    pageNumberRect.setSize(markSize);
    pageMarkRect.setSize(markSize);
    pageMarkRect.moveLeft((drawRect.left() + 5) * mpi);
    pageMarkRect.moveBottom((drawRect.bottom() - 5) * mpi);
    pageNumberRect.moveRight((drawRect.right() - 5) * mpi);
    pageNumberRect.moveBottom((drawRect.bottom() - 5) * mpi);
    auto itr = p_pols.begin();
    QPointF nextPos;
    int curPage = 1;
    int curPol = 0;
    while(itr != p_pols.end()) {
        for(int r=0; r < ly.second; ++r) {
            for(int c=0; c < ly.first; ++c) {
                if(itr != p_pols.end()) {
                    nextPos.setX((p_polsize.width() * c) + fpost.x());
                    nextPos.setY((p_polsize.height() * r) + fpost.y());
                    (*itr).setScaleMode(Polaroid::Expanded);
                    // (*itr).setScaleMode(Polaroid::Blured);
                    (*itr).setSize(p_polsize);
                    (*itr).render(&ptr, nextPos);
                    progressUpdate(curPage, ++curPol);
                    updater->emitPageUpdate(curPage);
                    updater->emitPolaroidUpdate(curPol);
                } else {
                    break;
                }
                ++itr;
            }
        }
        { // Marking Section
            QTextOption topt;
            QFont uft = pageMarkingFont;
            QPen pen = ptr.pen();
            topt.setAlignment(Qt::AlignHCenter | Qt::AlignVCenter);
            uft.setPointSizeF(10.5);
            pen.setColor(pageMarkingColor);

            ptr.save();
            ptr.setFont(uft);
            ptr.setPen(pen);

            if(pageNumbering) {
                auto fnm = ptr.fontMetrics();
                auto pn = QString("%1/%2").arg(curPage).arg(pageToCreate());
                auto tw = fnm.boundingRect(pageNumberRect.toRect(), Qt::AlignVCenter | Qt::AlignHCenter, pn);
                tw.moveRight(pageNumberRect.right());
                ptr.drawText(tw, pn, topt);
                //ptr.fillRect(pageNumberRect, Qt::yellow);
            }
            if(pageMarking) {
                auto fnm = ptr.fontMetrics();
                auto tw = fnm.boundingRect(pageMarkRect.toRect(), Qt::AlignHCenter | Qt::AlignVCenter, pageMarkingText);
                tw.moveLeft(pageMarkRect.left());
                ptr.drawText(tw, pageMarkingText, topt);
                //ptr.fillRect(pageMarkRect, Qt::yellow);
            }
        } // End Marking Section
        ptr.restore();
        if(itr == p_pols.end()) {
            break;
        }
        else
        {
            wrt.newPage();
            ++curPage;
        }
    }
    return true;
}

QAbstractItemModel *PolaroidMaker::polaroidModel()
{
    return new PolaroidListModel(nullptr, &p_pols);
}
