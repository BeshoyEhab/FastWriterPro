#include "inputfield.h"
#include <QFontMetrics>
#include <QTextDocument>

InputField::InputField(QWidget *parent) : QTextEdit(parent)
{
    setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    setLineWrapMode(QTextEdit::WidgetWidth);
    setAcceptRichText(false);
    setWordWrapMode(QTextOption::WrapAtWordBoundaryOrAnywhere);

    // Make it start as a single line
    QFontMetrics fm(font());
    setMinimumHeight(fm.height() + 30); // Initial height for single line + padding
    setMaximumHeight(fm.height() + 30);

    // Connect to text change to handle auto-resize
    connect(this, &QTextEdit::textChanged, this, &InputField::adjustHeight);
}

void InputField::keyPressEvent(QKeyEvent *event)
{
    if (event->key() == Qt::Key_Return || event->key() == Qt::Key_Enter) {
        // Prevent new lines, treat Enter/Return as navigation
        emit navigationKeyPressed(event);
        if (!event->isAccepted()) {
            QTextEdit::keyPressEvent(event);
        }
        return;
    }

    if (event->key() == Qt::Key_Tab) {
        emit navigationKeyPressed(event);
        if (!event->isAccepted()) {
            QTextEdit::keyPressEvent(event);
        }
        return;
    }

    if (event->key() == Qt::Key_Space)
    {
        emit navigationKeyPressed(event);
    }

    QTextEdit::keyPressEvent(event);
}

void InputField::adjustHeight()
{
    // Get the content height
    int docHeight = document()->size().height();
    QFontMetrics fm(font());
    int contentHeight = docHeight + 30; // Add padding

    // Get the window height
    QWidget* window = this->window();
    int maxHeight = window ? int(window->height() * 0.7) : 400;

    // Set height between minimum (single line) and maximum (70% of window)
    int newHeight = qMin(maxHeight, qMax(fm.height() + 30, contentHeight));
    setMaximumHeight(newHeight);
} 
