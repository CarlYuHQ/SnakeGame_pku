#include "mainwindow.h"
#include "ui_mainwindow.h"
#include <QMessageBox>
#include <QCloseEvent>

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow),
    gameWindowUI(nullptr)
{
    ui->setupUi(this);

    ui->enterButton->setEnabled(false);
}

MainWindow::~MainWindow()
{
    delete ui;
    if (gameWindowUI) {
        delete gameWindowUI;
    }
}

void MainWindow::on_enterButton_clicked()
{
    if(gameWindowUI) {
        gameWindowUI->deleteLater();
    }
    gameWindowUI = new GameWindow(this);
    gameWindowUI -> show();
    this->hide();
}

void MainWindow::on_exitButton_clicked()
{
    this->close();
}

void MainWindow::closeEvent(QCloseEvent *event)
{
    QMessageBox::StandardButton reply;
    reply = QMessageBox::question(this, "退出游戏", "确定要退出吗？", QMessageBox::Yes | QMessageBox::No);

    if (reply == QMessageBox::Yes) {
        if (gameWindowUI) {
            gameWindowUI->close();
        }
        event->accept();
    } else {
        event->ignore();
    }
}

void MainWindow::on_nameEdit_textChanged()
{
    playerName = ui->nameEdit->text().trimmed();
    ui->enterButton->setEnabled(!playerName.isEmpty());
}
