#ifndef TFORMPLAINTEXT_H
#define TFORMPLAINTEXT_H

#include <QWidget>
#include <QFile>
#include <QFileDialog>
#include <QFontDialog>
#include <QFont>
#include <QMessageBox>

namespace Ui {
class TFormPlainText;
}

class TFormPlainText : public QWidget
{
    Q_OBJECT

public:
    explicit TFormPlainText(QWidget *parent = nullptr);
    ~TFormPlainText();

public:
    void load_file();
    void save();
    void cut();
    void copy();
    void paste();
    void font();
private:
    QString file_path;

signals:
    void text_selected();
    void text_changed();

private slots:
    void on_plainTextEdit_selectionChanged();

    void on_plainTextEdit_modificationChanged(bool arg1);

private:
    Ui::TFormPlainText *ui;
};

#endif // TFORMPLAINTEXT_H
