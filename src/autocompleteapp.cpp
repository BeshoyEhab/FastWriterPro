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
    , isDeletingText(false)
    , isThrottling(false)
{
    // Initialize debounce timer
    debounceTimer = new QTimer(this);
    debounceTimer->setSingleShot(true);
    debounceTimer->setInterval(100); // 100ms debounce delay for better responsiveness
    connect(debounceTimer, &QTimer::timeout, [this]() {
        isDeletingText = false;
        updateSuggestions();
    });
    
    // Initialize throttle timer for held keys
    throttleTimer = new QTimer(this);
    throttleTimer->setSingleShot(false);
    throttleTimer->setInterval(150); // 150ms throttle interval for held keys
    connect(throttleTimer, &QTimer::timeout, [this]() {
        if (isDeletingText) {
            updateSuggestions();
        }
    });
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
    if (event->key() == Qt::Key_Backspace || event->key() == Qt::Key_Delete) {
        isBackspace = (event->key() == Qt::Key_Backspace);
        isDeletingText = true;
        
        if (event->isAutoRepeat()) {
            // For held keys, use throttling instead of debouncing
            if (!isThrottling) {
                isThrottling = true;
                throttleTimer->start();
                updateSuggestions(); // First update immediately
            }
            debounceTimer->stop();
        } else {
            // For single presses, stop throttling and use debouncing
            throttleTimer->stop();
            isThrottling = false;
            debounceTimer->stop();
            debounceTimer->setInterval(100);
        }
    } else {
        handleNavigationKeys(event);
    }
    QMainWindow::keyPressEvent(event);
}

void AutoCompleteApp::keyReleaseEvent(QKeyEvent *event) {
    if (event->key() == Qt::Key_Backspace || event->key() == Qt::Key_Delete) {
        isBackspace = false;
        
        // Stop throttling when key is released
        if (isThrottling) {
            throttleTimer->stop();
            isThrottling = false;
        }
        
        // Start debounce timer for final update
        if (!event->isAutoRepeat()) {
            debounceTimer->start();
        }
        
        updateInputHeight(); // Still update height immediately
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

    suggestionsWrapperLayout->addWidget(suggestionContainer);

    inputField = new InputField();
    inputField->setObjectName("inputField");
    inputField->setPlaceholderText("Start typing Here");

    contentLayout->addWidget(suggestionsWrapper);
    contentLayout->addWidget(inputField);

    suggestionContainer->hide();

    QHBoxLayout *horizontalWrapper = new QHBoxLayout();
    horizontalWrapper->addStretch();
    horizontalWrapper->addWidget(contentWidget, 8);
    horizontalWrapper->addStretch();

    mainLayout->addLayout(horizontalWrapper);

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
                                                "background-color:#e1e1e1; color:rgb(27, 27, 27) ; border-radius: 10px;":
                                                "background-color:#262626; color: rgba(255, 255, 255, 0.9); border-radius: 10px;");
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
    if (!isDeletingText)
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

    int maxButtonsPerRow = max(5, maxSuggestions/2);

    const int avgButtonWidth = 120;

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

        QFontMetrics fm(btn->font());
        int textWidth = fm.horizontalAdvance(displayText);
        int totalWidth = textWidth + 42;
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
