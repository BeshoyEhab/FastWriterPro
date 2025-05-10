#include <QApplication>
#include <QScreen>
#include <QDir>
#include "autocompleteapp.h"

int main(int argc, char *argv[])
{
    QApplication app(argc, argv);
    Model *model = new Model;
    AutoCompleteApp window(model);
    QString baseDir = QCoreApplication::applicationDirPath();
    QString assetPath = QDir(baseDir + "/../assets").absolutePath();
    QString assetsPath = QDir(baseDir + "/../../assets").absolutePath();
    if (!QFile::exists(assetPath+"/words_dictionary.json"))
        model->readJson(assetsPath+"/words_dictionary.json");
    else
        model->readJson(assetPath+"/words_dictionary.json");

    QScreen *screen = QGuiApplication::primaryScreen();
    int screenWidth = screen->size().width();
    window.setMinimumWidth((screenWidth + 300) / 2);
    window.setMinimumHeight(300);
    window.show();
    window.setFocus();

    return app.exec();
}
