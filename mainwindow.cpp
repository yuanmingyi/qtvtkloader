#include "mainwindow.h"
#include "ui_mainwindow.h"

#include <QMessageBox>
#include <QFile>
#include <QFileDialog>
#include <QLineEdit>
#include <QDoubleValidator>
#include <QValidator>
#include <QLabel>
#include <QDebug>
#include <QComboBox>
#include <float.h>


double sliderValueToIntensity(int sliderValue)
{
    // value range: 1 - 100
    return sliderValue / 10.0;
}

int intensityToSliderValue(double intensity)
{
    return static_cast<int>(intensity * 10);
}

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    AddStatusbarLabel();
    AddActorComboBox();
    AddLightIntensityControl();
    AddShowAxesCheckBox();
}

MainWindow::~MainWindow()
{
    delete actorsComboBox;
    delete lightIntensitySlider;
    delete lightIntensityEdit;
    delete statusLabel;
    delete ui;
}

void MainWindow::AddStatusbarLabel()
{
    statusLabel = new QLabel;
    statusLabel->setTextInteractionFlags(Qt::TextSelectableByMouse);
    ui->statusbar->addWidget(statusLabel);
}

void MainWindow::AddShowAxesCheckBox()
{
    showAxesCheckBox = new QCheckBox;
    showAxesCheckBox->setFixedWidth(100);
    showAxesCheckBox->setText(tr("Axes"));
    ui->toolBar->addWidget(showAxesCheckBox);

    QObject::connect(showAxesCheckBox, SIGNAL(stateChanged(int)), this, SLOT(showAxesChanged(int)));
}

void MainWindow::AddActorComboBox()
{
    auto label = new QLabel;
    label->setText(tr("actors:"));
    label->setMargin(5);
    ui->toolBar->addWidget(label);

    actorsComboBox = new QComboBox;
    actorsComboBox->setFixedWidth(100);
    actorsComboBox->setStyleSheet("color:black;background-color:white");
    ui->toolBar->addWidget(actorsComboBox);

    QObject::connect(actorsComboBox, SIGNAL(currentTextChanged(QString)), this, SLOT(currentActorChanged(QString)));
    QObject::connect(ui->sceneWidget, SIGNAL(pickedActorChanged(int)), this, SLOT(currentActorChanged(int)));
}

void MainWindow::AddLightIntensityControl()
{
    auto validator = new QDoubleValidator;
    validator->setRange(0.1, 10);

    auto label = new QLabel;
    label->setText(tr("light:"));
    label->setMargin(5);
    ui->toolBar->addWidget(label);

    lightIntensitySlider = new QSlider(Qt::Horizontal);
    lightIntensitySlider->setFixedWidth(200);
    lightIntensitySlider->setMaximum(100);
    lightIntensitySlider->setMinimum(1);
    lightIntensitySlider->setValue(5);
    ui->toolBar->addWidget(lightIntensitySlider);

    lightIntensityEdit = new QLineEdit();
    lightIntensityEdit->setFixedWidth(50);
    lightIntensityEdit->setValidator(validator);
    lightIntensityEdit->setText(QString::number(sliderValueToIntensity(lightIntensitySlider->value())));
    ui->toolBar->addWidget(lightIntensityEdit);

    QObject::connect(lightIntensitySlider, SIGNAL(valueChanged(int)), this, SLOT(sliderValueChanged(int)));
    QObject::connect(lightIntensityEdit, SIGNAL(textChanged(QString)), this, SLOT(lightIntensityChanged(QString)));
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

void MainWindow::showAxesChanged(int state)
{
    ui->sceneWidget->ShowAxes(state == Qt::Checked);
}

void MainWindow::currentActorChanged(QString actorText)
{
    std::cout << "currentActorChanged(QString): " << actorText.toStdString() << std::endl;
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
    qDebug() << "currentActorChanged(int): " << actorIndex << endl;
    QString actorText = "None";
    if (actorIndex >= 0) {
        actorText = QString::number(actorIndex);
    }

    actorsComboBox->setCurrentText(actorText);

    // update the status label
    QString status("");
    double bounds[6];
    double* center = ui->sceneWidget->GetActorCenterBounds(actorIndex, bounds);
    if (center != nullptr) {
        status = QString("Center: (%1,%2,%3), MinX: %4, MaxX: %5, MinY: %6, MaxY: %7, MinZ: %8, MaxZ: %9")
            .arg(center[0]).arg(center[1]).arg(center[2])
            .arg(bounds[0]).arg(bounds[1]).arg(bounds[2])
            .arg(bounds[3]).arg(bounds[4]).arg(bounds[5]);
    }
    statusLabel->setText(status);
}

void MainWindow::sliderValueChanged(int value)
{
    qDebug() << "sliderValueChanged()" << endl;
    double intensity = sliderValueToIntensity(value);
    auto text = QString::number(intensity);
    lightIntensityEdit->setText(text);
    ui->sceneWidget->SetLightIntensity(intensity);
}

void MainWindow::lightIntensityChanged(QString text)
{
    qDebug() << "lightIntensityChanged()" << endl;
    int value = intensityToSliderValue(text.toDouble());
    lightIntensitySlider->setValue(value);
}

void MainWindow::openFile(const QString &fileName)
{
    ui->sceneWidget->ImportObj(fileName);
    actorsComboBox->clear();
    actorsComboBox->addItem("None");
    const std::vector<vtkActor*>& actors = ui->sceneWidget->GetActors();
    for (size_t i = 0; i < actors.size(); i++) {
        actorsComboBox->addItem(QString::number(i));
    }
}

