#include <QDialog>
#include <QSettings>
#include <QCheckBox>
#include <QLineEdit>
#include "trie.h"

class QComboBox;
class QSlider;
class QLabel;

class SettingsDialog : public QDialog {
    Q_OBJECT
public:
    explicit SettingsDialog(Trie*, QWidget *parent = nullptr);
    void loadSettings();
    
signals:
    void settingsChanged(bool bfs, int maxSuggestions, bool useFreq);

private slots:
    void onSaveClicked();
    void onResetClicked();
    void onSliderMoved(int value);
    void onAddClicked();
    void onDeleteClicked();

private:
    void setupUI();
    Trie* trie;
    QLineEdit* wordInput;
    QPushButton* addButton;
    QPushButton* deleteButton;
    QComboBox *searchMethodCombo;
    QSlider *maxSuggestionsSlider;
    QLabel *suggestionCountLabel;
    QCheckBox *freq;
    QSettings settings;
};
