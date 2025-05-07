#include "settingsdialog.h"
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QComboBox>
#include <QSlider>
#include <QLabel>
#include <QPushButton>
#include <QToolTip>

SettingsDialog::SettingsDialog(QWidget *parent)
    : QDialog(parent)
{
    setupUI();
    searchMethodCombo->setCurrentIndex(0);
    maxSuggestionsSlider->setValue(4);
    freq->setChecked(true);
    suggestionCountLabel->setText("4");
}

void SettingsDialog::setupUI() {
    QVBoxLayout *mainLayout = new QVBoxLayout(this);

    // Search Method Selection
    QHBoxLayout *methodLayout = new QHBoxLayout();
    QLabel *methodLabel = new QLabel("Search Method:");
    searchMethodCombo = new QComboBox();
    searchMethodCombo->addItem("BFS (Breadth-First Search)", QVariant(true));
    searchMethodCombo->addItem("DFS (Depth-First Search)", QVariant(false));
    searchMethodCombo->setToolTip("BFS: Suggests words by popularity\nDFS: Suggests words alphabetically");
    freq = new QCheckBox("Use Frequency Sorting");
    freq->setToolTip("Prioritize suggestions based on word usage frequency");
    
    methodLayout->addWidget(methodLabel);
    methodLayout->addWidget(searchMethodCombo);
    mainLayout->addLayout(methodLayout);
    mainLayout->addWidget(freq);

    // Suggestions Slider
    QHBoxLayout *sliderLayout = new QHBoxLayout();
    QLabel *sliderLabel = new QLabel("Max Suggestions:");
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


    // Buttons
    QHBoxLayout *buttonLayout = new QHBoxLayout();
    QPushButton *saveButton = new QPushButton("Save");
    QPushButton *resetButton = new QPushButton("Reset to Defaults");
    
    connect(maxSuggestionsSlider, &QSlider::valueChanged, this, &SettingsDialog::onSliderMoved);
    connect(saveButton, &QPushButton::clicked, this, &SettingsDialog::onSaveClicked);
    connect(resetButton, &QPushButton::clicked, this, &SettingsDialog::onResetClicked);

    buttonLayout->addWidget(resetButton);
    buttonLayout->addWidget(saveButton);
    mainLayout->addLayout(buttonLayout);
}

void SettingsDialog::loadSettings() {
    bool useFrequency = settings.value("Search/UseFrequency", true).toBool();
    freq->setChecked(useFrequency);
    bool bfs = settings.value("Search/BFS", true).toBool();
    int maxSuggestions = settings.value("Suggestions/Max", 4).toInt();
    
    searchMethodCombo->setCurrentIndex(bfs ? 0 : 1);
    maxSuggestionsSlider->setValue(maxSuggestions);
}

void SettingsDialog::onSaveClicked() {
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
