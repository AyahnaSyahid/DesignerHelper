#include "sizetemplatemodel.h"

#include <QFile>
#include <QSizeF>

SizeTemplateModel::SizeTemplateModel(QObject *parent,
                                     PolaroidMakerNS::SizeFListsTable *ls)
    : QAbstractTableModel(parent), s_table(ls) {
  colHeader << "Jenis"
            << "Nama"
            << "Lebar"
            << "Tinggi";
}

SizeTemplateModel::~SizeTemplateModel() {
  if (!m_dataFile.isEmpty()) s_table->saveToFile(m_dataFile);
}

QVariant SizeTemplateModel::headerData(int section, Qt::Orientation orientation,
                                       int role) const {
  if (orientation == Qt::Horizontal) {
    if (section >= colHeader.count()) return QVariant();
    if (role == Qt::DisplayRole) {
      return colHeader[section];
    }
  }
  if (orientation == Qt::Vertical && role == Qt::DisplayRole) {
    return section + 1;
  }
  return QVariant();
}

int SizeTemplateModel::rowCount(const QModelIndex &parent) const {
  if (parent.isValid()) return 0;
  if (!s_table) return 0;
  return s_table->count();
}

int SizeTemplateModel::columnCount(const QModelIndex &parent) const {
  if (parent.isValid()) return 0;
  return colHeader.count();
}

QVariant SizeTemplateModel::data(const QModelIndex &index, int role) const {
  if (!index.isValid()) return QVariant();

  auto &st = *s_table;
  int row = index.row();
  int col = index.column();

  if (role == Qt::EditRole || role == Qt::DisplayRole) {
    switch (col) {
      case 0:
        return st[row].jenis;
      case 1:
        return st[row].nama;
      case 2:
        return st[row].ukuran.width();
      case 3:
        return st[row].ukuran.height();
      default:
        return QVariant();
    }
  }
  return QVariant();
}

bool SizeTemplateModel::insertRows(int row, int count,
                                   const QModelIndex &parent) {
  auto much = row + count - 1;
  beginInsertRows(parent, row, much);
  for (auto a = 0; a < count; ++a) {
    s_table->insert(a, PolaroidMakerNS::SizeFLists());
  }
  endInsertRows();
  return true;
}

bool SizeTemplateModel::removeRows(int row, int count,
                                   const QModelIndex &parent) {
  beginRemoveRows(parent, row, row + count - 1);
  auto much = count;
  for (int r = 0; r < much; ++r) {
    s_table->remove(row);
  }
  endRemoveRows();
  return true;
}

bool SizeTemplateModel::setData(const QModelIndex &index, const QVariant &value,
                                int role) {
  if (!index.isValid()) {
    return false;
  }
  auto &st = *s_table;
  auto r = index.row();
  auto c = index.column();
  if (role == Qt::EditRole) {
    auto &ref = st[r];
    switch (c) {
      case 0:
        ref.jenis = value.toString().trimmed();
        break;
      case 1:
        ref.nama = value.toString().trimmed();
        break;
      case 2:
        ref.ukuran.setWidth(value.toDouble());
        break;
      case 3:
        ref.ukuran.setHeight(value.toDouble());
        break;
    }
    emit dataChanged(index, index);
    return true;
  }
  //    return QAbstractTableModel::setData(index, value, role);
  return false;
}

Qt::ItemFlags SizeTemplateModel::flags(const QModelIndex &index) const {
  if (!index.isValid()) return Qt::NoItemFlags;
  return Qt::ItemIsSelectable | Qt::ItemIsEditable | Qt::ItemIsEnabled;
}

void SizeTemplateModel::setDataFile(const QString &df) {
  beginResetModel();
  m_dataFile = df;
  if (QFile::exists(df))
    if (s_table) s_table->loadFromFile(df);
  endResetModel();
}
