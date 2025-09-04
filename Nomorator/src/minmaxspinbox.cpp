#include "minmaxspinbox.h"
#include <QWheelEvent>

MinMaxSpinBox::MinMaxSpinBox(QSpinBox *min, QSpinBox *max, QWidget *parent)
    :  QSpinBox(parent), m_min(min), m_max(max)
{}

QValidator::State MinMaxSpinBox::validate(QString &input, int &pos) const
{
    auto nv = input.toInt();
    qDebug() << nv;
    if(m_min) return nv > m_min->value() ? QValidator::State::Acceptable : QValidator::State::Invalid;
    if(m_max) return nv < m_max->value() ? QValidator::State::Acceptable : QValidator::State::Invalid;
    return QSpinBox::validate(input, pos);
}

void MinMaxSpinBox::setMin(QSpinBox *val){m_min = val;}

void MinMaxSpinBox::setMax(QSpinBox *val){m_max = val;}

void MinMaxSpinBox::wheelEvent(QWheelEvent *event)
{
    auto df = event->angleDelta().y();
    if(m_min && df < 0) {
        if(value() <= m_min->value() + 1) {
            event->accept();
            return ;
        }
    }
    if(m_max && df > 0) {
        if(value() >= m_max->value() - 1) {
            event->accept();
            return;
        }
    }
    QSpinBox::wheelEvent(event);
}
