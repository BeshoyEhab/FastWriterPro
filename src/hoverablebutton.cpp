#include "hoverablebutton.h"
#include <QEvent>
#include <QEnterEvent>

HoverableButton::HoverableButton(const QString &text, QWidget *parent)
    : QPushButton(text, parent)
{
    setMouseTracking(true);
}

void HoverableButton::enterEvent(QEnterEvent *event)
{
    QPropertyAnimation *moveUp = new QPropertyAnimation(this, "geometry");
    moveUp->setDuration(100);
    QRect currentGeom = geometry();
    moveUp->setStartValue(currentGeom);
    moveUp->setEndValue(QRect(currentGeom.x(), currentGeom.y() - 2,
                             currentGeom.width(), currentGeom.height()));
    moveUp->start(QAbstractAnimation::DeleteWhenStopped);
    QPushButton::enterEvent(event);
}

void HoverableButton::leaveEvent(QEvent *event)
{
    if (!property("selected").toBool()) {
        QPropertyAnimation *moveDown = new QPropertyAnimation(this, "geometry");
        moveDown->setDuration(100);
        QRect currentGeom = geometry();
        moveDown->setStartValue(currentGeom);
        moveDown->setEndValue(QRect(currentGeom.x(), currentGeom.y() + 2,
                                   currentGeom.width(), currentGeom.height()));
        moveDown->start(QAbstractAnimation::DeleteWhenStopped);
    }
    QPushButton::leaveEvent(event);
} 