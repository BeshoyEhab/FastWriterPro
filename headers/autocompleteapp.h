#ifndef AUTOCOMPLETEAPP_H
#define AUTOCOMPLETEAPP_H

#include <QMainWindow>
#include <QMap>
#include <QStringList>
#include <QPushButton>
#include <QPropertyAnimation>
#include <QGraphicsOpacityEffect>
#include <QParallelAnimationGroup>
#include "trie.h"

class InputField;
class QLabel;

class AutoCompleteApp : public QMainWindow {
    Q_OBJECT

public:
    explicit AutoCompleteApp(QWidget *parent = nullptr);

signals:
    void suggestionsVisibilityChanged(bool visible);

protected:
    void keyPressEvent(QKeyEvent *event) override;
    void closeEvent(QCloseEvent *event) override;

private:
    InputField *inputField;
    QWidget *suggestionContainer;
    QList<QPushButton *> suggestionButtons;
    int selectedIndex;
    Trie trie;
    QLabel *titleLabel;
    QPropertyAnimation *slideAnimation;
    QGraphicsOpacityEffect *opacityEffect;
    QParallelAnimationGroup *currentAnimGroup;

    void setupUI();
    void setupAutocomplete();
    void updateInputHeight();
    QString getCurrentWord();
    void clearSelection();
    void selectNext();
    void selectPrevious();
    void updateSelection();
    void activateSelected();
    void updateUI();
    void showSuggestions();
    void hideSuggestions();
    void updateSuggestions();
    void replaceCurrentWord(const QString &replacement);
    void loadDictionary(const QString& filename);
    void saveJson();

private slots:
    void handleNavigationKeys(QKeyEvent *event);
};

#endif // AUTOCOMPLETEAPP_H 
