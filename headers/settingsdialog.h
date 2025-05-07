#include <QDialog>
#include <QSettings>
#include <QCheckBox>

class QComboBox;
class QSlider;
class QLabel;

class SettingsDialog : public QDialog {
    Q_OBJECT
public:
    explicit SettingsDialog(QWidget *parent = nullptr);
    void loadSettings();
    
signals:
    void settingsChanged(bool bfs, int maxSuggestions, bool useFreq);

private slots:
    void onSaveClicked();
    void onResetClicked();
    void onSliderMoved(int value);

private:
    void setupUI();
    QComboBox *searchMethodCombo;
    QSlider *maxSuggestionsSlider;
    QLabel *suggestionCountLabel;
    QCheckBox *freq;
    QSettings settings;
};
