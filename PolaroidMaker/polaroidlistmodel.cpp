#include "polaroidlistmodel.h"

#include <QIcon>
#include <QImageReader>
#include <QPixmapCache>

const QString iconCacheKey("pml%1_100");

PolaroidListModel::PolaroidListModel(QObject *parent)
    : gamma(1.0f), pols(), QAbstractListModel(parent) {
  QPixmapCache::setCacheLimit(QPixmapCache::cacheLimit() * 20);  // 10240 x 20
}

QVariant PolaroidListModel::headerData(int section, Qt::Orientation orientation,
                                       int role) const {
  if (role == Qt::DisplayRole) {
    if (orientation == Qt::Horizontal) {
      if (section == 0) {
        return "Gambar Polaroid";
      }
    }
  }
  return QAbstractItemModel::headerData(section, orientation, role);
}

int PolaroidListModel::rowCount(const QModelIndex &parent) const {
  // For list models only the root node (an invalid parent) should return the
  // list's size. For all other (valid) parents, rowCount() should return 0 so
  // that it does not become a tree model.
  if (parent.isValid()) return 0;
  return pols.count();
  // FIXME: Implement me!
  return 0;
}

QVariant PolaroidListModel::data(const QModelIndex &index, int role) const {
  if (!index.isValid()) return QVariant();

  auto cpol = pols.at(index.row());

  switch (role) {
    case Qt::DisplayRole:
      return cpol.imageName();
    case Qt::EditRole:
      return cpol.imageFile();
    case Qt::DecorationRole:
      auto key = iconCacheKey.arg(cpol.imageFile());
      QPixmap pix(160, 160);
      pix.fill(Qt::red);
      if (QPixmapCache::find(key, &pix)) {
        qDebug() << "cache found" << key;
      }
      return pix;
  }
  return QVariant();
}

bool PolaroidListModel::setData(const QModelIndex &index, const QVariant &value,
                                int role) {
  if (data(index, role) != value) {
    pols[index.row()].setImage(value.toString());
    emit dataChanged(index, index, QVector<int>() << role);
    return true;
  }
  return false;
}

Qt::ItemFlags PolaroidListModel::flags(const QModelIndex &index) const {
  if (!index.isValid()) return Qt::NoItemFlags;

  return Qt::ItemIsSelectable | Qt::ItemIsEnabled;
}

bool PolaroidListModel::insertRows(int row, int count,
                                   const QModelIndex &parent) {
  auto last = row + count - 1;
  beginInsertRows(parent, row, last);
  for (auto i = row; i <= last; ++i) {
    pols.insert(row, Polaroid());
  }
  endInsertRows();
  return true;
}

bool PolaroidListModel::removeRows(int r, int count,
                                   const QModelIndex &parent) {
  auto last = r + count - 1;
  beginRemoveRows(parent, r, last);
  for (auto n = r; n <= last; ++n) {
    pols.removeAt(r);
  }
  endRemoveRows();
  return true;
}

void PolaroidListModel::insertImages(const QStringList &imgs) {
  beginResetModel();
  for (auto &s : imgs) {
    Polaroid pl(s);
    pl.setGamma(gamma);
    auto key = iconCacheKey.arg(s);
    QPixmap pix(160, 160);
    if (!QPixmapCache::find(key, &pix)) {  // pixmap cache belum dibuat
      QImage img(160, 160, QImage::Format_ARGB32);
      img.fill(Qt::transparent);
      QImageReader reader(s);
      reader.setAutoTransform(true);
      if (reader.read(&img)) {  // image bisa dibaca
        QImage expanded = img.scaled(160, 160, Qt::KeepAspectRatioByExpanding,
                                     Qt::SmoothTransformation);
        QRect dest(0, 0, 160, 160);
        qDebug() << dest;
        dest.moveCenter(expanded.rect().center());
        qDebug() << dest;
        expanded = expanded.copy(dest);
        if (pl.r_gamma != 0.0f) {
          Polaroid::applyGamma(&expanded, pl.r_gamma);
          qDebug() << "Gamma applied";
        }
        pix = QPixmap::fromImage(expanded);
        QPixmapCache::insert(key, pix);
      } else {  // image tidak terbaca
        QPixmapCache::insert(key, pix);
      }
    }  // pixmap cache sudah tersedia
    pols.append(pl);
  }
  endResetModel();
}

void PolaroidListModel::clearData() {
  beginResetModel();
  pols.clear();
  endResetModel();
}

Polaroid &PolaroidListModel::getPolaroid(int n) { return pols[n]; }

void PolaroidListModel::setGamma(double gm) {
  if (gm == gamma) return;
  gamma = gm;
  QStringList imageFileList;
  for (auto p : pols) {
    imageFileList << p.imageFile();
  }
  pols.clear();
  QPixmapCache::clear();
  insertImages(imageFileList);
}
