#include "mainwindow.h"
#include "ui_mainwindow.h"
#include <QMessageBox>
#include <QFile>
#include <QFileDialog>
#include <QLineEdit>
#include <QValidator>
#include <QLabel>

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    QIntValidator *validator = new QIntValidator;
    validator->setRange(1, 1000);
    auto label = new QLabel;
    label->setText(tr("load times:"));
    label->setMargin(5);
    ui->toolBar->addWidget(label);
    edit = new QLineEdit;
    edit->setFixedWidth(50);
    edit->setValidator(validator);
    edit->setStyleSheet("color:black;background-color:white");
    ui->toolBar->addWidget(edit);
}

MainWindow::~MainWindow()
{
    delete edit;
    delete ui;
}

void MainWindow::showAboutDialog()
{
  QMessageBox::information(this, "About", "By Martijn Koopman.\nSource code available under Apache License 2.0.");
}

void MainWindow::showOpenFileDialog()
{
  QString fileName = QFileDialog::getOpenFileName(this, tr("Open file"), "", "Model Files (*.obj)");
  QString filename = fileName.toLower();
  if (filename.endsWith(tr(".obj"))) {
      openFile(fileName);
  }
}

void MainWindow::openFile(const QString &fileName)
{
    auto text = edit->text();
    int times = text.toInt();
    ui->sceneWidget->loadObj(fileName, times);
}
