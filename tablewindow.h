#ifndef TABLEWINDOW_H
#define TABLEWINDOW_H

#include <QMainWindow>
#include <QDir>
#include <QDebug>
#include <QMenu>
#include <QSettings>
#include "qexlabel.h"

namespace Ui {
    class TableWindow;
}

class TableWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit TableWindow(QWidget *parent = 0);
    ~TableWindow();

private slots:
    void on_pushButton_clicked();
    void cardclicked(QObject *sender);
    void on_pushButton_2_clicked();
    void on_pushButton_3_clicked();
    void contextMenuRequest(const QPoint p);

    void on_pushButton_6_clicked();

    void on_plainTextEdit_textChanged();

    void on_pushButton_7_clicked();

private:
    Ui::TableWindow *ui;
    int i;
    int who;
    int what;
    QString generated;
    QSettings *settings;
};

#endif // TABLEWINDOW_H
