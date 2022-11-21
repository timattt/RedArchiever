#include "mainwindow.h"
#include "ui_mainwindow.h"
#include <QFileDialog>
#include <QMessageBox>
#include "../RedArchiver.hpp"

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);

}

MainWindow::~MainWindow()
{
    delete ui;
}


void MainWindow::on_pushButton_2_clicked()
{
    QStringList files = QFileDialog::getOpenFileNames(this,"Select one or more files","D:/","");
    if (files.size() == 0) {
        return;
    }
    QString result = {};
    int index = 0;
    for (; index < files.length()-1; index++){
       result += files.at(index) + QString("\n");
    }
    result += files.at(index);
    ui -> textEdit -> setText(result);
}


void MainWindow::on_pushButton_clicked()
{
    if (ui->textEdit->toPlainText().size() == 0){
        QMessageBox::information(NULL,QObject::tr("Info"),tr("Choose files to compress at first!"));
        return;
    }
    QString file = QFileDialog::getSaveFileName(this, tr("Save File"), "D:/untitled.cli");
    char argv_buf[128][256] = {0};
    memset(argv_buf, 0, 128*256);
    int argc = 0;
    foreach (const QString &str, ui->textEdit->toPlainText().split(QLatin1Char('\n'))){
        strcpy(argv_buf[argc++], str.toLocal8Bit().data());
    }
    //argc--;//есть пустая строчка в конце изза того что plainText завершается /n
    strcpy(argv_buf[argc++], ui->lineEdit->text().toLocal8Bit().data()); //Key
    strcpy(argv_buf[argc++], file.toLocal8Bit().data());

    /*for(int i = 0; i < argc; i++){
        printf("%s\n", argv[i]);
    }
    printf("%d\n", argc);*/
    char* argv[argc];
    for(int i =0; i < argc; i++){
        argv[i]= argv_buf[i];
        printf("[%s]\n", argv_buf[i]);
    }
    fflush(stdout);
    encrypt(argc, argv);
    QMessageBox::information(NULL, QObject::tr("Info"),tr("Complete!"));
    ui -> textEdit -> setText(file);
}


void MainWindow::on_pushButton_3_clicked()
{
    if (ui->textEdit->toPlainText().size() == 0){
        QMessageBox::information(NULL,QObject::tr("Info"),tr("Choose files to unarchive at first!"));
        return;
    }
    QString file = QFileDialog::getExistingDirectory(this);
    if (file.size() == 0) {
        return;
    }
    char argv_buf[3][256] = {};
    memset(argv_buf, 0, 3*256);
    int argc = 0;
    strcpy(argv_buf[argc++], ui->textEdit->toPlainText().toLocal8Bit().data());
    strcpy(argv_buf[argc++], ui->lineEdit->text().toLocal8Bit().data()); //Key
    strcpy(argv_buf[argc++], file.toLocal8Bit().data());

    char* argv[argc];
    for(int i =0; i < argc; i++){
        argv[i]= argv_buf[i];
    }
    for(int i =0; i < argc; i++){
        argv[i]= argv_buf[i];
        printf("[%s]\n", argv_buf[i]);
    }
    fflush(stdout);
    decrypt(argc, argv);
    QMessageBox::information(NULL,QObject::tr("Info"), tr("Complete!"));
    ui -> textEdit -> setText("");
}

