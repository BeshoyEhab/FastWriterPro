#include <QApplication>
#include "../headers/autocompleteapp.h"

int main(int argc, char *argv[])
{
    QApplication app(argc, argv);
    AutoCompleteApp window;
    window.show();
    window.setFocus();
    return app.exec();
}
