#include "model.h"
#include <QFile>
#include <QDebug>
#include <nlohmann/json.hpp>

using json = nlohmann::json;
Model::Model(){}

void Model::readJson(const QString &fileName)
{
    QFile file(fileName);
    if (file.open(QIODevice::ReadOnly | QIODevice::Text)) {
        qDebug() << "Opened file:" << fileName;
    }

    if (!file.isOpen()) {
        qCritical() << fileName << " couldn't be opened!";
        return;
    }
    try {
        json jsonData = json::parse(file.readAll().toStdString());
        for (auto &[word, frequency] : jsonData.items()) {
            trie->insert(word, frequency);
        }
    } catch (json::exception &e) {
        qCritical() << "Error happen when parseing " << e.what();
    }
}

void Model::saveJson(const QString &fileName) {
    json data;
    trie->makeJson(data);

    QString backUpName = fileName + ".backup";

    // Create backup
    if (QFile::exists(fileName)) {
        QFile::remove(backUpName);
        QFile::copy(fileName, backUpName);
    }

    QFile file(fileName);
    if (file.open(QIODevice::WriteOnly | QIODevice::Truncate)) {
        std::string jsonStr = data.dump(4); // Pretty print with 4 spaces
        file.write(jsonStr.c_str(), jsonStr.size());
        file.close();
    }
}

void Model::loadTrie(Trie *t)
{
    trie = t;
}
