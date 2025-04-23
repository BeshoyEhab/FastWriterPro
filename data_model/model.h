#include <QString>
#include "../headers/trie.h"

class Model
{
private:
    Trie* trie;

public:
    Model();
    void readJson(const QString &fileName);
    void saveJson(const QString &fileName);
    void loadTrie(Trie *t);
};
