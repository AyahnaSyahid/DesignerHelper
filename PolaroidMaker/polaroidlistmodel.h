#ifndef POLAROIDLISTMODEL_H
#define POLAROIDLISTMODEL_H

#include "polaroid.h"
#include <QAbstractListModel>

class PolaroidListModel : public QAbstractListModel
{
    Q_OBJECT

public:
    explicit PolaroidListModel(QObject *parent = nullptr, QList<Polaroid> *lpol=nullptr);

    // Header:
    QVariant headerData(int section, Qt::Orientation orientation, int role = Qt::DisplayRole) const override;
    // Basic functionality:
    int rowCount(const QModelIndex &parent = QModelIndex()) const override;

    QVariant data(const QModelIndex &index, int role = Qt::DisplayRole) const override;

    // Editable:
    bool setData(const QModelIndex &index, const QVariant &value,
                 int role = Qt::EditRole) override;

    Qt::ItemFlags flags(const QModelIndex& index) const override;

    // Add data:
    bool insertRows(int row, int count, const QModelIndex &parent = QModelIndex()) override;
    bool removeRows(int r, int count, const QModelIndex &parent = QModelIndex()) override;

    void insertImages(const QStringList &imgs);
    void clearData();

    Polaroid& getPolaroid(int n);
    void setGamma(double gm);

private:
    QList<Polaroid> *pols;
};

#endif // POLAROIDLISTMODEL_H
