#include <QApplication>
#include <QScreen>
#include "autocompleteapp.h"

int main(int argc, char *argv[])
{
    QApplication app(argc, argv);
    AutoCompleteApp window;

    QScreen *screen = QGuiApplication::primaryScreen();
    int screenWidth = screen->size().width();
    window.setMinimumWidth((screenWidth + 300) / 2);
    window.setMinimumHeight(300);
    window.show();
    window.setFocus();

    return app.exec();
}
