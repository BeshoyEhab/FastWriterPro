#ifndef HOVERABLEBUTTON_H
#define HOVERABLEBUTTON_H

#include <QPushButton>
#include <QPropertyAnimation>
#include <QEnterEvent>

class HoverableButton : public QPushButton {
    Q_OBJECT
public:
    explicit HoverableButton(const QString &text, QWidget *parent = nullptr);

protected:
    void enterEvent(QEnterEvent *event) override;
    void leaveEvent(QEvent *event) override;
};

#endif // HOVERABLEBUTTON_H 