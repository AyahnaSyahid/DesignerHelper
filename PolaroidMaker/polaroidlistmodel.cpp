#include "polaroidlistmodel.h"
#include <QIcon>
#include <QPixmapCache>

PolaroidListModel::PolaroidListModel(QObject *parent, QList<Polaroid> *pl)
    : QAbstractListModel(parent), pols(pl)
{
    QPixmapCache::setCacheLimit(QPixmapCache::cacheLimit()*5);
}

QVariant PolaroidListModel::headerData(int section, Qt::Orientation orientation, int role) const
{
    if(role == Qt::DisplayRole) {
        if(orientation == Qt::Horizontal) {
            if(section == 0) {
                return "Gambar Polaroid";
            }
        }
    }
    return QAbstractItemModel::headerData(section, orientation, role);
}

int PolaroidListModel::rowCount(const QModelIndex &parent) const
{
    // For list models only the root node (an invalid parent) should return the list's size. For all
    // other (valid) parents, rowCount() should return 0 so that it does not become a tree model.
    if (parent.isValid())
        return 0;
    if(pols)
        return pols->count();
    // FIXME: Implement me!
    return 0;
}

QVariant PolaroidListModel::data(const QModelIndex &index, int role) const
{
    if (!index.isValid())
        return QVariant();

    auto &cpol = (*pols)[index.row()];
    
    switch (role) {
    case Qt::DisplayRole:
        return cpol.imageName();
    case Qt::EditRole:
        return cpol.imageFile();
    case Qt::DecorationRole:
        QString cKey("plm@%1");
        cKey = cKey.arg(cpol.imageFile());
        QPixmap cached;
        if(QPixmapCache::find(cKey, &cached)) return QIcon(cached);
        QImage img =  cpol.getImage().scaled(128, 128, Qt::KeepAspectRatio);
        auto pxm = QPixmap::fromImage(img);
        QPixmapCache::insert(cKey, pxm);
        return QIcon(pxm);
    }
    return QVariant();
}

bool PolaroidListModel::setData(const QModelIndex &index, const QVariant &value, int role)
{
    if (data(index, role) != value) {
        auto &p = (*pols)[index.row()];
        p.setImage(value.toString());
        emit dataChanged(index, index, QVector<int>() << role);
        return true;
    }
    return false;
}

Qt::ItemFlags PolaroidListModel::flags(const QModelIndex &index) const
{
    if (!index.isValid())
        return Qt::NoItemFlags;

    return Qt::ItemIsSelectable | Qt::ItemIsEnabled;
}

bool PolaroidListModel::insertRows(int row, int count, const QModelIndex &parent)
{
    auto last = row + count - 1;
    beginInsertRows(parent, row, last);
    for(auto i = row; i <= last ; ++i) {
        pols->insert(row, Polaroid());
    }
    endInsertRows();
    return true;
}

bool PolaroidListModel::removeRows(int r, int count, const QModelIndex &parent)
{
    auto last = r + count - 1;
    beginRemoveRows(parent, r, last);
    for(auto n = r; n <= last; ++n) {
        pols->removeAt(r);
    }
    endRemoveRows();
    return true;
}

void PolaroidListModel::insertImages(const QStringList &imgs)
{
    beginResetModel();
    for(auto &s : imgs) {
        pols->append(s);
    }
    endResetModel();
}

void PolaroidListModel::clearData()
{
    beginResetModel();
    pols->clear();
    endResetModel();
}

Polaroid &PolaroidListModel::getPolaroid(int n)
{
    QList<Polaroid> &p = *pols;
    return p[n];
}

void PolaroidListModel::setGamma(double gm)
{
    if(!pols || pols->isEmpty())
        return;
    for(auto &pl : *pols)
    {
        pl.setGamma(gm);
    }
    QPixmapCache::clear();
}
