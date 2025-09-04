#ifndef MINMAXSPINBOX_H
#define MINMAXSPINBOX_H

#include <QSpinBox>
#include <QtDebug>

class MinMaxSpinBox : public QSpinBox
{
    QSpinBox *m_min;
    QSpinBox *m_max;
public:
    MinMaxSpinBox(QWidget *parent=nullptr) : QSpinBox(parent) {}
    MinMaxSpinBox(QSpinBox *min=nullptr, QSpinBox *max=nullptr, QWidget *parent=nullptr);
    virtual QValidator::State validate(QString &input, int &pos) const override;
    void setMin(QSpinBox *val);
    void setMax(QSpinBox *val);
    virtual void wheelEvent(QWheelEvent *event) override;
};



#endif // MINMAXSPINBOX_H
