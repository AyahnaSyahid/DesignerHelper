#ifndef SIZETEMPLATEMODEL_H
#define SIZETEMPLATEMODEL_H

#include <QAbstractTableModel>
#include "polaroidmakerns.h"


class SizeTemplateModel : public QAbstractTableModel
{
    Q_OBJECT
public:
    explicit SizeTemplateModel(QObject *parent = nullptr, PolaroidMakerNS::SizeFListsTable *ls = nullptr);
    ~SizeTemplateModel();
    // Header:
    QVariant headerData(int section, Qt::Orientation orientation, int role = Qt::DisplayRole) const override;

    // Basic functionality:
    int rowCount(const QModelIndex &parent = QModelIndex()) const override;
    int columnCount(const QModelIndex &parent = QModelIndex()) const override;

    QVariant data(const QModelIndex &index, int role = Qt::DisplayRole) const override;

    // Add data:
    bool insertRows(int row, int count, const QModelIndex &parent = QModelIndex()) override;

    // Remove data:
    bool removeRows(int row, int count, const QModelIndex &parent = QModelIndex()) override;

    bool setData(const QModelIndex &index, const QVariant &value, int role) override;
    Qt::ItemFlags flags(const QModelIndex &index) const override;

    void setDataFile(const QString &df);

private:
    QString m_dataFile;
    PolaroidMakerNS::SizeFListsTable *s_table;
    QStringList colHeader;

};

#endif // SIZETEMPLATEMODEL_H
