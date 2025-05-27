#pragma once
#include <QMainWindow>
#include <QStringList>
#include <QPushButton>
#include <QPropertyAnimation>
#include <QGraphicsOpacityEffect>
#include <QParallelAnimationGroup>
#include <QTimer>
#include "../data_model/model.h"

class InputField;
class QLabel;

class AutoCompleteApp : public QMainWindow {
    Q_OBJECT

public:
    explicit AutoCompleteApp(Model *m, QWidget *parent = nullptr);

signals:
    void suggestionsVisibilityChanged(bool visible);

protected:
    void keyPressEvent(QKeyEvent *event) override;
    void keyReleaseEvent(QKeyEvent *event) override;
    void closeEvent(QCloseEvent *event) override;

private:
    bool useBFS = true;
    int maxSuggestions = 4;
    bool isBackspace;
    bool useFreq = true;
    bool isDeletingText;
    QTimer *debounceTimer;
    QTimer *throttleTimer;
    bool isThrottling;
    Model *model;
    InputField *inputField;
    QWidget *suggestionContainer;
    QList<QPushButton *> suggestionButtons;
    int selectedIndex;
    Trie *trie;
    QLabel *titleLabel;
    QPropertyAnimation *slideAnimation;
    QGraphicsOpacityEffect *opacityEffect;
    QParallelAnimationGroup *currentAnimGroup;

    void setupUI();
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
    void onSettingsChanged(bool bfs, int maxSug, bool useFreq);
};
