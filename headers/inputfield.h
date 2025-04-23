#include <QTextEdit>
#include <QKeyEvent>

class InputField : public QTextEdit {
    Q_OBJECT
public:
    explicit InputField(QWidget *parent = nullptr);

signals:
    void navigationKeyPressed(QKeyEvent *event);

protected:
    void keyPressEvent(QKeyEvent *event) override;

private slots:
    void adjustHeight();
};
