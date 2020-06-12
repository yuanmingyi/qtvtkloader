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

    QObject::connect(actorsComboBox, SIGNAL(currentTextChanged(QString)), this, SLOT(currentModuleChanged(QString)));
    QObject::connect(ui->sceneWidget, SIGNAL(pickedModuleChanged(std::string)), this, SLOT(currentModuleChanged(std::string)));
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

void MainWindow::currentModuleChanged(QString moduleName)
{
    qDebug() << "currentActorChanged(QString): " << moduleName << endl;
    ui->sceneWidget->PickModule(moduleName.toStdString());
}

void MainWindow::currentModuleChanged(std::string name)
{
    qDebug() << "currentActorChanged(std::string): " << QString::fromStdString(name) << endl;
    actorsComboBox->setCurrentText(QString::fromStdString(name));
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
    const std::vector<std::string>& items = ui->sceneWidget->GetPickableItems();
    for (size_t i = 0; i < items.size(); i++) {
        actorsComboBox->addItem(QString::fromStdString(items[i]));
    }
}

