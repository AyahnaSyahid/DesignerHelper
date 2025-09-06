#ifndef POLAROIDMAKERNS_H
#define POLAROIDMAKERNS_H

#include <QFile>
#include <QSizeF>
#include <QString>
#include <QTextStream>
#include <QVariant>
#include <QVector>

namespace PolaroidMakerNS {
struct SizeFLists {
  QString jenis, nama;
  QSizeF ukuran;
  const QVariant operator[](int x) const {
    switch (x) {
      case 0:
        return jenis;
      case 1:
        return nama;
      case 2:
        return ukuran.width();
      case 3:
        return ukuran.height();
      default:
        return QVariant();
    }
  }
  SizeFLists(const QString &j = "Custom", const QString &n = "Custom",
             double w = 0, double h = 0)
      : jenis(j), nama(n), ukuran(w, h) {}
  ~SizeFLists(){};
};

class SizeFListsTable : public QVector<SizeFLists> {
 public:
  SizeFListsTable() : QVector<SizeFLists>(){};
  QSizeF get(const QString &jenis, const QString &nama) {
    if (availableJenis().contains(jenis)) {
      if (availableNama().contains(nama)) {
        for (auto sz = begin(); sz < end(); ++sz) {
          if (sz->jenis == jenis && sz->nama == nama) return sz->ukuran;
        }
      }
    }
    return QSizeF();
  }
  QStringList availableJenis() {
    QStringList ret;
    for (auto j = begin(); j < end(); ++j) {
      ret << j->jenis;
    }
    return ret;
  }
  QStringList availableNama() {
    QStringList ret;
    for (auto j = begin(); j < end(); ++j) {
      ret << j->nama;
    }
    return ret;
  }
  void saveToFile(const QString &fn) {
    QFile inf(fn);
    inf.open(QFile::WriteOnly | QFile::Text);
    QString fmt("\"%1\",\"%2\",\"%3\",\"%4\"\n");
    QString cf;
    for (auto j = begin(); j != end(); ++j) {
      cf = fmt.arg(j->jenis, j->nama)
               .arg(j->ukuran.width())
               .arg(j->ukuran.height());
      inf.write(cf.toLocal8Bit());
    }
    inf.close();
  }
  void loadFromFile(const QString &fn) {
    clear();
    QFile inf(fn);
    QTextStream txs(&inf);
    inf.open(QFile::ReadOnly | QFile::Text);
    while (!txs.atEnd()) {
      QString line = txs.readLine();
      line = line.replace("\",", "<>");
      line = line.remove("\"");
      auto stlist = line.split("<>");
      try {
        SizeFLists got(stlist[0], stlist[1], stlist[2].toDouble(),
                       stlist[3].toDouble());
        append(got);
      } catch (...) {
        continue;
      }
    }
  }
};

}  // namespace PolaroidMakerNS

#endif  // POLAROIDMAKERNS_H
