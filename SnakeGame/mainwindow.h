#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include "gamewindow.h"

namespace Ui {
class MainWindow;
}

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

    QString getPlayerName() const { return playerName; }

protected:
    void closeEvent(QCloseEvent *event) override;

private slots:
    void on_enterButton_clicked();
    void on_exitButton_clicked();
    void on_nameEdit_textChanged();

private:
    Ui::MainWindow *ui;
    GameWindow *gameWindowUI;
    QString playerName;
};

#endif // MAINWINDOW_H
