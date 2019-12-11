#include "mainwindow.h"
#include "ui_mainwindow.h"
#include <QMessageBox>
#include <QFile>
#include <QFileDialog>

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);
}

MainWindow::~MainWindow()
{
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
    ui->sceneWidget->loadObj(fileName);
}
