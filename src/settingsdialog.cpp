#include "settingsdialog.h"
#include <QHBoxLayout>
#include <QComboBox>
#include <QSlider>
#include <QLabel>
#include <QPushButton>
#include <QMessageBox>
#include <QFile>
#include <QDir>
#include <QCoreApplication>

SettingsDialog::SettingsDialog(Trie* t,QWidget *parent)
    : QDialog(parent), trie(t)
{
    setWindowTitle("Preferences");

    QString baseDir = QCoreApplication::applicationDirPath();
    QString srcPath = QDir(baseDir + "/../../src").absolutePath();
    QFile styleFile(srcPath+"/Style.css");
    if (styleFile.open(QIODevice::ReadOnly | QIODevice::Text)) {
        QString styleSheet = QLatin1String(styleFile.readAll());
        setStyleSheet(styleSheet);
        styleFile.close();
    }
    this->setObjectName("settingsDialog");

    setupUI();
    loadSettings();
}

void SettingsDialog::setupUI() {
    QVBoxLayout *mainLayout = new QVBoxLayout(this);

           // Search Method Selection
    QHBoxLayout *methodLayout = new QHBoxLayout();
    QLabel *methodLabel = new QLabel("Search Method:");
    searchMethodCombo = new QComboBox();
    searchMethodCombo->addItem("DFS (Depth-First Search)", QVariant(false));
    searchMethodCombo->addItem("BFS (Breadth-First Search)", QVariant(true));
    searchMethodCombo->setToolTip("DFS: Suggests words alphabetically\nBFS: Suggests words by popularity");
    freq = new QCheckBox("Use Frequency Sorting");
    freq->setToolTip("Prioritize suggestions based on word usage frequency");

    methodLayout->addWidget(methodLabel);
    methodLayout->addWidget(searchMethodCombo);
    mainLayout->addLayout(methodLayout);
    mainLayout->addWidget(freq);

    QHBoxLayout *sliderLayout = new QHBoxLayout();
    QLabel *sliderLabel = new QLabel("Max Suggestions:");
    sliderLabel->setProperty("sliderLabel", true);
    maxSuggestionsSlider = new QSlider(Qt::Horizontal);
    maxSuggestionsSlider->setRange(1, 10);
    maxSuggestionsSlider->setValue(4);
    maxSuggestionsSlider->setTickInterval(1);
    maxSuggestionsSlider->setTickPosition(QSlider::TicksBelow);

    suggestionCountLabel = new QLabel("4");
    suggestionCountLabel->setFixedWidth(30);

    sliderLayout->addWidget(sliderLabel);
    sliderLayout->addWidget(maxSuggestionsSlider);
    sliderLayout->addWidget(suggestionCountLabel);
    mainLayout->addLayout(sliderLayout);

    QHBoxLayout* wordLayout = new QHBoxLayout();
    wordInput = new QLineEdit();
    wordInput->setPlaceholderText("Enter word");

    addButton = new QPushButton("Add Word");
    addButton->setObjectName("AddButton");
    deleteButton = new QPushButton("Delete Word");
    deleteButton->setObjectName("deleteButton");

    wordLayout->addWidget(wordInput);
    wordLayout->addWidget(addButton);
    wordLayout->addWidget(deleteButton);

    mainLayout->insertLayout(3, wordLayout);

    QHBoxLayout *buttonLayout = new QHBoxLayout();
    QPushButton *saveButton = new QPushButton("Save");
    QPushButton *resetButton = new QPushButton("Reset to Defaults");

    connect(maxSuggestionsSlider, &QSlider::valueChanged, this, &SettingsDialog::onSliderMoved);
    connect(saveButton, &QPushButton::clicked, this, &SettingsDialog::onSaveClicked);
    connect(resetButton, &QPushButton::clicked, this, &SettingsDialog::onResetClicked);
    connect(addButton, &QPushButton::clicked, this, &SettingsDialog::onAddClicked);
    connect(deleteButton, &QPushButton::clicked, this, &SettingsDialog::onDeleteClicked);

    buttonLayout->addWidget(resetButton);
    buttonLayout->addWidget(saveButton);
    mainLayout->addLayout(buttonLayout);
}

void SettingsDialog::loadSettings() {
    bool useFrequency = settings.value("Search/UseFrequency", true).toBool();
    bool bfs = settings.value("Search/BFS", false).toBool();
    int maxSuggestions = settings.value("Suggestions/Max", 4).toInt();

    freq->setChecked(useFrequency);
    searchMethodCombo->setCurrentIndex(bfs ? 1 : 0);
    maxSuggestionsSlider->setValue(maxSuggestions);
    suggestionCountLabel->setText(QString::number(maxSuggestions));
}

void SettingsDialog::onSaveClicked() {
    settings.setValue("Search/BFS", searchMethodCombo->currentData().toBool());
    settings.setValue("Suggestions/Max", maxSuggestionsSlider->value());
    settings.setValue("Search/UseFrequency", freq->isChecked());
    emit settingsChanged(searchMethodCombo->currentData().toBool(),
                         maxSuggestionsSlider->value(),
                         freq->isChecked());
    accept();
}

void SettingsDialog::onResetClicked() {
    freq->setChecked(true);
    searchMethodCombo->setCurrentIndex(0);
    maxSuggestionsSlider->setValue(4);
}

void SettingsDialog::onSliderMoved(int value) {
    suggestionCountLabel->setText(QString::number(value));
}

void SettingsDialog::onAddClicked() {
    QString word = wordInput->text().trimmed().toLower();
    if(word.isEmpty() || word.contains(' ')) {
        QMessageBox::warning(this, "Invalid Input", "Please enter a valid word");
        return;
    }

    trie->insert(word.toStdString());
    wordInput->clear();
}

void SettingsDialog::onDeleteClicked() {
    QString word = wordInput->text().trimmed().toLower();
    if(word.isEmpty() || word.contains(' ')) {
        QMessageBox::warning(this, "Invalid Input", "Please enter a valid word");
        return;
    }

    if(!trie->remove(word.toStdString())) {
        QMessageBox::information(this, "Not Found", "Word not found in dictionary");
    }
    wordInput->clear();
}
