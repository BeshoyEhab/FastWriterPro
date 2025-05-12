#include "autocompleteapp.h"
#include "settingsdialog.h"
#include <QMenuBar>
#include "inputfield.h"
#include <QHBoxLayout>
#include <QLabel>
#include <QRegularExpression>
#include <QStyle>
#include <QSizePolicy>
#include <QPropertyAnimation>
#include <QEasingCurve>
#include <QGraphicsOpacityEffect>
#include <QMessageBox>
#include <QDir>
#include <math.h>
using namespace std;

AutoCompleteApp::AutoCompleteApp(Model *m, QWidget *parent)
    : QMainWindow(parent)
    , selectedIndex(-1)
    , model(m)
    , isBackspace(false)
    , useBFS(true)
    , maxSuggestions(4)
    , useFreq(true)
{
    QString baseDir = QCoreApplication::applicationDirPath();
    QString assetPath = QDir(baseDir + "/../assets").absolutePath();
    QString assetsPath = QDir(assetPath + "/../../assets").absolutePath();

    QFile styleFile;
    if (QFile::exists(assetPath + "/Style.css")) {
        styleFile.setFileName(assetPath + "/Style.css");
    } else {
        styleFile.setFileName(assetsPath + "/Style.css");
    }

    if (styleFile.open(QIODevice::ReadOnly | QIODevice::Text)) {
        QString styleSheet = QLatin1String(styleFile.readAll());
        setStyleSheet(styleSheet);
        styleFile.close();
    }
    trie = new Trie;
    model->loadTrie(trie);

    setupUI();
    resize(800, 600);
    setWindowTitle("Fast Writer Pro");
}

void AutoCompleteApp::keyPressEvent(QKeyEvent *event) {
    if (event->key() == Qt::Key_Backspace) {
        isBackspace = true;
    } else {
        handleNavigationKeys(event);
    }
    QMainWindow::keyPressEvent(event);
}

void AutoCompleteApp::keyReleaseEvent(QKeyEvent *event) {
    if (event->key() == Qt::Key_Backspace) {
        isBackspace = false;
        updateUI();
    }
    QMainWindow::keyReleaseEvent(event);
}

void AutoCompleteApp::setupUI()
{
    QWidget *centralWidget = new QWidget();
    QVBoxLayout *mainLayout = new QVBoxLayout(centralWidget);
    mainLayout->setContentsMargins(0, 0, 0, 0);
    mainLayout->setSpacing(0);

    // Add stretch to push content to vertical center
    mainLayout->addStretch(1);

    // Main content container (input + suggestions)
    QWidget *contentWidget = new QWidget();
    contentWidget->setObjectName("inputContainer");
    QVBoxLayout *contentLayout = new QVBoxLayout(contentWidget);
    contentLayout->setContentsMargins(40, 0, 40, 0);
    contentLayout->setSpacing(0);

    // Title Label
    titleLabel = new QLabel("let me help you to write fast");
    titleLabel->setAlignment(Qt::AlignCenter);
    contentLayout->addWidget(titleLabel);
    contentLayout->addSpacing(20);

    // Create a container for suggestions that's always present
    QWidget *suggestionsWrapper = new QWidget();
    QVBoxLayout *suggestionsWrapperLayout = new QVBoxLayout(suggestionsWrapper);
    suggestionsWrapperLayout->setContentsMargins(0, 0, 0, 0);
    suggestionsWrapperLayout->setSpacing(0);

    // Create a fixed-size spacer widget to reserve space for suggestions
    QWidget *suggestionsSpacer = new QWidget();
    suggestionsSpacer->setFixedHeight(36); // Same height as suggestion container
    suggestionsSpacer->setVisible(false);
    suggestionsWrapperLayout->addWidget(suggestionsSpacer);

    // Suggestions Container with animation setup
    suggestionContainer = new QWidget();
    suggestionContainer->setObjectName("suggestionContainer");
    suggestionContainer->setSizePolicy(QSizePolicy::Preferred, QSizePolicy::MinimumExpanding);

    QGridLayout* suggestionsLayout = new QGridLayout(suggestionContainer);
    suggestionsLayout->setContentsMargins(10, 10, 10, 10);
    suggestionsLayout->setHorizontalSpacing(8);
    suggestionsLayout->setVerticalSpacing(8);
    suggestionsLayout->setAlignment(Qt::AlignTop | Qt::AlignHCenter);

    contentLayout->addWidget(suggestionContainer);

    // Setup opacity effect
    opacityEffect = new QGraphicsOpacityEffect(suggestionContainer);
    opacityEffect->setOpacity(0.0);
    suggestionContainer->setGraphicsEffect(opacityEffect);

    // Setup slide animation
    slideAnimation = new QPropertyAnimation(suggestionContainer, "pos");
    slideAnimation->setDuration(150);

    // Initialize animation group pointer
    currentAnimGroup = nullptr;

    // Add suggestion container to wrapper
    suggestionsWrapperLayout->addWidget(suggestionContainer);

    // Input Field
    inputField = new InputField();
    inputField->setObjectName("inputField");
    inputField->setPlaceholderText("Start typing Here");

    // Create the main content stack in correct order
    contentLayout->addWidget(suggestionsWrapper);
    contentLayout->addWidget(inputField);

    // Hide suggestions initially
    suggestionContainer->hide();

    // Center the content widget horizontally
    QHBoxLayout *horizontalWrapper = new QHBoxLayout();
    horizontalWrapper->addStretch();
    horizontalWrapper->addWidget(contentWidget, 8);
    horizontalWrapper->addStretch();

    mainLayout->addLayout(horizontalWrapper);

    // Add stretch after content for vertical centering
    mainLayout->addStretch(1);

    connect(this, &AutoCompleteApp::suggestionsVisibilityChanged, 
            suggestionsSpacer, &QWidget::setVisible);

    setCentralWidget(centralWidget);
    connect(inputField, &InputField::navigationKeyPressed,
            this, &AutoCompleteApp::handleNavigationKeys);
    connect(inputField, &QTextEdit::textChanged,
            this, &AutoCompleteApp::updateUI);

    QMenuBar *menuBar = new QMenuBar();
    QMenu *settingsMenu = menuBar->addMenu("Settings");
    QAction *prefsAction = settingsMenu->addAction("Preferences...");
    connect(prefsAction, &QAction::triggered, [this]() {
        SettingsDialog dlg(trie, this);
        connect(&dlg, &SettingsDialog::settingsChanged,
                this, &AutoCompleteApp::onSettingsChanged);
        dlg.exec();
    });
    this->setMenuBar(menuBar);
}

void AutoCompleteApp::onSettingsChanged(bool bfs, int maxSug, bool usefreq)
{
    useBFS = bfs;
    maxSuggestions = maxSug;
    useFreq = usefreq;
    updateSuggestions();
}

void AutoCompleteApp::updateInputHeight()
{
    int docHeight = inputField->document()->size().height();
    inputField->setMinimumHeight(qMin(qMax(60, int(docHeight) + 30), int(this->height() * 0.7)));
}

QString AutoCompleteApp::getCurrentWord()
{
    QTextCursor cursor = inputField->textCursor();
    QString text = inputField->toPlainText().left(cursor.position());
    QString trimmed = text.replace(QRegularExpression("\\s+$"), "");
    QString lastWord = trimmed.section(' ', -1, -1, QString::SectionSkipEmpty);
    return lastWord;
}

void AutoCompleteApp::clearSelection()
{
    selectedIndex = -1;
    for(QPushButton *btn : suggestionButtons) {
        btn->setProperty("selected", false);
        btn->style()->polish(btn);
    }
}

void AutoCompleteApp::selectNext()
{
    if(suggestionButtons.isEmpty()) return;
    selectedIndex = (selectedIndex + 1) % suggestionButtons.size();
    updateSelection();
}

void AutoCompleteApp::selectPrevious()
{
    if(suggestionButtons.isEmpty()) return;
    selectedIndex = (selectedIndex - 1 + suggestionButtons.size()) % suggestionButtons.size();
    updateSelection();
}

void AutoCompleteApp::updateSelection()
{
    for(int i = 0; i < suggestionButtons.size(); i++) {
        suggestionButtons[i]->setStyleSheet((i == selectedIndex) ?
            "background-color: #9d9d9d; color: #ffffff; border-radius: 20px;" :
            "background-color: #21262d; color: rgba(255, 255, 255, 0.9); border-radius: 20px;");
    }
}

void AutoCompleteApp::activateSelected()
{
    if(selectedIndex >= 0 && selectedIndex < suggestionButtons.size()) {
        suggestionButtons[selectedIndex]->click();
    }
}

void AutoCompleteApp::updateUI()
{
    updateInputHeight();
    if (!isBackspace)
        updateSuggestions();
}

void AutoCompleteApp::showSuggestions()
{
    if (!suggestionContainer->isVisible()) {
        suggestionContainer->show();
        emit suggestionsVisibilityChanged(true);
        opacityEffect->setOpacity(0.0);

        QPropertyAnimation *fadeAnimation = new QPropertyAnimation(opacityEffect, "opacity", this);
        fadeAnimation->setDuration(1000);
        fadeAnimation->setStartValue(0.0);
        fadeAnimation->setEndValue(1.150);
        fadeAnimation->setEasingCurve(QEasingCurve::OutCubic);
        connect(fadeAnimation, &QPropertyAnimation::finished, fadeAnimation, &QPropertyAnimation::deleteLater);
        fadeAnimation->start();
    }
}

void AutoCompleteApp::hideSuggestions()
{
    if (suggestionContainer->isVisible()) {
        QPropertyAnimation *fadeAnimation = new QPropertyAnimation(opacityEffect, "opacity", this);
        fadeAnimation->setDuration(200);
        fadeAnimation->setStartValue(opacityEffect->opacity());
        fadeAnimation->setEndValue(0.0);
        fadeAnimation->setEasingCurve(QEasingCurve::InCubic);
        
        connect(fadeAnimation, &QPropertyAnimation::finished, [this, fadeAnimation]() {
            suggestionContainer->hide();
            emit suggestionsVisibilityChanged(false);
            fadeAnimation->deleteLater();
        });

        fadeAnimation->start();
    }
}

void AutoCompleteApp::updateSuggestions()
{
    clearSelection();
    suggestionButtons.clear();
    QLayoutItem* child;
    QGridLayout* layout = qobject_cast<QGridLayout*>(suggestionContainer->layout());
    while ((child = layout->takeAt(0)) != nullptr) {
        delete child->widget();
        delete child;
    }

    if (inputField->toPlainText().endsWith(' ')) {
        return;
    }
    if (inputField->toPlainText().isEmpty()) {
        hideSuggestions();
        return;
    }

    QString currentWord = getCurrentWord();
    QString baseWord = currentWord.toLower();
    bool capitalize = currentWord.length() > 0 && currentWord[0].isUpper();
    bool allCaps = currentWord == currentWord.toUpper();

    std::vector<std::string> suggestions = trie->autoComplete(
        baseWord.toStdString(),
        useBFS,
        useFreq,
        maxSuggestions);

    // Calculate optimal grid layout
    int availableWidth = suggestionContainer->width() - 20; // Account for container margins
    int maxButtonsPerRow = max(5, maxSuggestions/2); // Default value

           // Calculate maximum number of buttons to fit in a row
    const int avgButtonWidth = 120; // Average button width estimation

    int row = 0;
    int col = 0;

    for (const auto &suggestion : suggestions) {
        QString QSug = QString::fromStdString(suggestion);
        QString displayText = QSug;

        if(capitalize) {
            displayText = QSug.left(1).toUpper() + QSug.mid(1).toLower();
        } else if(allCaps) {
            displayText = QSug.toUpper();
        } else {
            displayText = QSug.toLower();
        }

        QPushButton *btn = new QPushButton(displayText);
        btn->setCursor(Qt::PointingHandCursor);
        btn->setSizePolicy(QSizePolicy::Preferred, QSizePolicy::Fixed);
        btn->setMinimumHeight(26);

        // Calculate the width needed for the text
        QFontMetrics fm(btn->font());
        int textWidth = fm.horizontalAdvance(displayText);
        // Add padding (16px on each side from the stylesheet + 5px extra on each side)
        int totalWidth = textWidth + 42;  // 16px + 5px padding on each side
        btn->setMinimumWidth(totalWidth);

        connect(btn, &QPushButton::clicked, [this, displayText]() {
            replaceCurrentWord(displayText);
        });

        layout->addWidget(btn, row, col);
        suggestionButtons.append(btn);

        if (++col >= maxButtonsPerRow)
        {
            col = 0;
            row++;
        }
    }

    if(!suggestionButtons.isEmpty()) {
        selectedIndex = 0;
        updateSelection();
        showSuggestions();
    }
}

void AutoCompleteApp::replaceCurrentWord(const QString &replacement)
{
    QTextCursor cursor = inputField->textCursor();
    cursor.select(QTextCursor::WordUnderCursor);
    cursor.insertText(replacement + " ");
    inputField->setFocus();
    trie->insert(replacement.toLower().toStdString());
}

void AutoCompleteApp::handleNavigationKeys(QKeyEvent *event)
{
    if (suggestionButtons.isEmpty())
    {
        if (event->key() == Qt::Key_Space && !getCurrentWord().isEmpty())
        {
            trie->addNew(getCurrentWord().toLower().toStdString());
            event->accept();
        } else
            event->ignore();
        return;
    }

    switch(event->key()) {
    case Qt::Key_Tab:
        if(event->modifiers() & Qt::ShiftModifier) {
            selectPrevious();
        } else {
            selectNext();
        }
        event->accept();
        break;
    case Qt::Key_Return:
    case Qt::Key_Enter:
        activateSelected();
        event->accept();
        break;
    case Qt::Key_Space:
        trie->addNew(getCurrentWord().toLower().toStdString());
    default:
        event->ignore();
    }
}

void AutoCompleteApp::closeEvent(QCloseEvent *event) {
    // Create a message box with custom buttons
    QSettings settings;
    if (!trie->changed) {
        event->accept();
        settings.clear();
        return;
    }

    QMessageBox msgBox(this);
    msgBox.setWindowTitle("Exit Confirmation");
    msgBox.setText("You have unsaved changes. What would you like to do?");

    // Add custom buttons
    QPushButton *saveButton = msgBox.addButton("Save", QMessageBox::AcceptRole);
    QPushButton *discardButton = msgBox.addButton("Discard", QMessageBox::DestructiveRole);
    QPushButton *cancelButton = msgBox.addButton("Cancel", QMessageBox::RejectRole);

    msgBox.exec();  // Show the dialog

    // Determine which button was clicked
    if (msgBox.clickedButton() == saveButton) {
        saveJson();
        event->accept();  // Close the window
        settings.clear();
    } else if (msgBox.clickedButton() == discardButton) {
        event->accept();  // Close without saving
        settings.clear();
    } else if (msgBox.clickedButton() == cancelButton) {
        event->ignore();   // Cancel closing
    }
}

void AutoCompleteApp::saveJson()
{
    QString baseDir = QCoreApplication::applicationDirPath();
    QString assetPath = QDir(baseDir + "/../../assets").absolutePath();

    QString fileName = assetPath + "/words_dictionary.json";
    model->saveJson(fileName);
}
