#include "mainwindow.h"
#include "ui_mainwindow.h"

#include <QMessageBox>
#include <QFile>
#include <QFileDialog>
#include <QLineEdit>
#include <QValidator>
#include <QLabel>
#include <QComboBox>

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);

    QIntValidator *validator = new QIntValidator;
    validator->setRange(1, 1000);
    auto label = new QLabel;
    label->setText(tr("actors:"));
    label->setMargin(5);
    ui->toolBar->addWidget(label);
    actorsComboBox = new QComboBox;
    actorsComboBox->setFixedWidth(50);
    actorsComboBox->setValidator(validator);
    actorsComboBox->setStyleSheet("color:black;background-color:white");
    ui->toolBar->addWidget(actorsComboBox);
    QObject::connect(actorsComboBox, SIGNAL(currentTextChanged(QString)), this, SLOT(currentActorChanged(QString)));
    QObject::connect(ui->sceneWidget, SIGNAL(pickedActorChanged(int)), this, SLOT(currentActorChanged(int)));
}

MainWindow::~MainWindow()
{
    delete actorsComboBox;
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

void MainWindow::currentActorChanged(QString actorText)
{
    if (actorText == "None") {
        // cancel selection
        ui->sceneWidget->PickActor(-1);
    } else {
        auto idx = actorText.toInt();
        ui->sceneWidget->PickActor(idx);
    }
}

void MainWindow::currentActorChanged(int actorIndex)
{
    QString actorText = "None";
    if (actorIndex >= 0) {
        actorText = QString::asprintf("%d", actorIndex);
    }
    actorsComboBox->setCurrentText(actorText);
}

void MainWindow::openFile(const QString &fileName)
{
    ui->sceneWidget->ImportObj(fileName);
    actorsComboBox->clear();
    actorsComboBox->addItem("None");
    const std::vector<vtkActor*>& actors = ui->sceneWidget->GetActors();
    for (size_t i = 0; i < actors.size(); i++) {
        actorsComboBox->addItem(QString::asprintf("%lu", i));
    }
}

