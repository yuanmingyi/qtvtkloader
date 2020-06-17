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


double sliderValueToIntensity(int sliderValue, double scale)
{
    // value range: 1 - 100
    return sliderValue / scale;
}

int intensityToSliderValue(double intensity, double scale)
{
    return static_cast<int>(intensity * scale);
}

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    AddStatusbarLabel();
    AddActorComboBox();
    AddLightIntensityControl();
    AddOpacityControl();
    AddShowAxesCheckBox();
    AddPushButtons();
}

MainWindow::~MainWindow()
{
    delete actorsComboBox;
    delete lightIntensitySlider;
    delete lightIntensityEdit;
    delete opacitySlider;
    delete opacityEdit;
    delete statusLabel;
    delete animateDaofuButton;
    delete animateBiantianxianButton;
    delete animateShengjiangganButton;
    delete animateZuobanHorizontalButton;
    delete animateZuobanVerticalButton;
    delete animateYoubanHorizontalButton;
    delete animateYoubanVerticalButton;
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
    showAxesCheckBox->setFixedWidth(80);
    showAxesCheckBox->setText(tr("Axes"));
    ui->toolBar->addWidget(showAxesCheckBox);

    QObject::connect(showAxesCheckBox, SIGNAL(stateChanged(int)), this, SLOT(showAxesChanged(int)));
}

void MainWindow::AddActorComboBox()
{
    auto label = new QLabel;
    label->setText(tr("highlight:"));
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
    lightIntensitySlider->setFixedWidth(100);
    lightIntensitySlider->setMaximum(100);
    lightIntensitySlider->setMinimum(1);
    lightIntensitySlider->setValue(10);
    ui->toolBar->addWidget(lightIntensitySlider);

    lightIntensityEdit = new QLineEdit();
    lightIntensityEdit->setFixedWidth(50);
    lightIntensityEdit->setValidator(validator);
    lightIntensityEdit->setText(QString::number(sliderValueToIntensity(lightIntensitySlider->value(), 10.0)));
    ui->toolBar->addWidget(lightIntensityEdit);

    QObject::connect(lightIntensitySlider, SIGNAL(valueChanged(int)), this, SLOT(lightSliderValueChanged(int)));
    QObject::connect(lightIntensityEdit, SIGNAL(textChanged(QString)), this, SLOT(lightIntensityChanged(QString)));
}

void MainWindow::AddOpacityControl()
{
    auto validator = new QDoubleValidator;
    validator->setRange(0, 1);

    auto label = new QLabel;
    label->setText(tr("opacity:"));
    label->setMargin(5);
    ui->toolBar->addWidget(label);

    opacitySlider = new QSlider(Qt::Horizontal);
    opacitySlider->setFixedWidth(50);
    opacitySlider->setMaximum(100);
    opacitySlider->setMinimum(0);
    opacitySlider->setValue(100);
    ui->toolBar->addWidget(opacitySlider);

    opacityEdit = new QLineEdit();
    opacityEdit->setFixedWidth(50);
    opacityEdit->setValidator(validator);
    opacityEdit->setText(QString::number(sliderValueToIntensity(opacitySlider->value(), 100.0)));
    ui->toolBar->addWidget(opacityEdit);

    QObject::connect(opacitySlider, SIGNAL(valueChanged(int)), this, SLOT(opacitySliderValueChanged(int)));
    QObject::connect(opacityEdit, SIGNAL(textChanged(QString)), this, SLOT(opacityEditTextChanged(QString)));
}

void MainWindow::AddPushButtons()
{
    animateDaofuButton = new QPushButton(tr("daofu"));
    animateBiantianxianButton = new QPushButton(tr("bian"));
    animateShengjiangganButton = new QPushButton(tr("gan"));
    animateZuobanHorizontalButton = new QPushButton(tr("zuo-h"));
    animateZuobanVerticalButton = new QPushButton(tr("zuo-v"));
    animateYoubanHorizontalButton = new QPushButton(tr("you-h"));
    animateYoubanVerticalButton = new QPushButton(tr("you-v"));
    ui->toolBar->addWidget(animateDaofuButton);
    ui->toolBar->addWidget(animateBiantianxianButton);
    ui->toolBar->addWidget(animateShengjiangganButton);
    ui->toolBar->addWidget(animateZuobanHorizontalButton);
    ui->toolBar->addWidget(animateZuobanVerticalButton);
    ui->toolBar->addWidget(animateYoubanHorizontalButton);
    ui->toolBar->addWidget(animateYoubanVerticalButton);

    QObject::connect(animateDaofuButton, SIGNAL(clicked()), this, SLOT(animateDaofu()));
    QObject::connect(animateBiantianxianButton, SIGNAL(clicked()), this, SLOT(animateBiantianxian()));
    QObject::connect(animateShengjiangganButton, SIGNAL(clicked()), this, SLOT(animateShengjianggan()));
    QObject::connect(animateZuobanHorizontalButton, SIGNAL(clicked()), this, SLOT(animateZuobanHorizontal()));
    QObject::connect(animateZuobanVerticalButton, SIGNAL(clicked()), this, SLOT(animateZuobanVertical()));
    QObject::connect(animateYoubanHorizontalButton, SIGNAL(clicked()), this, SLOT(animateYoubanHorizontal()));
    QObject::connect(animateYoubanVerticalButton, SIGNAL(clicked()), this, SLOT(animateYoubanVertical()));
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

void MainWindow::lightSliderValueChanged(int value)
{
    qDebug() << "lightSliderValueChanged()" << endl;
    double intensity = sliderValueToIntensity(value, 10.0);
    auto text = QString::number(intensity);
    lightIntensityEdit->setText(text);
    ui->sceneWidget->SetLightIntensity(intensity);
}

void MainWindow::lightIntensityChanged(QString text)
{
    qDebug() << "lightIntensityChanged()" << endl;
    int value = intensityToSliderValue(text.toDouble(), 10.0);
    lightIntensitySlider->setValue(value);
}

void MainWindow::opacitySliderValueChanged(int value)
{
    qDebug() << "opacitySliderValueChanged()" << endl;
    double opacity = sliderValueToIntensity(value, 100.0);
    auto text = QString::number(opacity);
    opacityEdit->setText(text);
    ui->sceneWidget->SetOpacity(opacity);
}

void MainWindow::opacityEditTextChanged(QString text)
{
    qDebug() << "opacityEditTextChanged()" << endl;
    int value = intensityToSliderValue(text.toDouble(), 100.0);
    opacitySlider->setValue(value);
}

void MainWindow::animateDaofu()
{
    isDaofuOpen = !isDaofuOpen;
    ui->sceneWidget->AnimateDaofu(isDaofuOpen);
}

void MainWindow::animateBiantianxian()
{
    isBiantianxianOpen = !isBiantianxianOpen;
    ui->sceneWidget->AnimateBiantianxian(isBiantianxianOpen);
}

void MainWindow::animateShengjianggan()
{
    isShengjiangganOpen = !isShengjiangganOpen;
    ui->sceneWidget->AnimateShengjianggan(isShengjiangganOpen);
}

void MainWindow::animateZuobanHorizontal()
{
    isZuobanHorizontalOpen = !isZuobanHorizontalOpen;
    ui->sceneWidget->AnimateZuobanHorizontal(isZuobanHorizontalOpen);
}

void MainWindow::animateZuobanVertical()
{
    ui->sceneWidget->AnimateZuobanVertical();
}

void MainWindow::animateYoubanHorizontal()
{
    isYoubanHorizontalOpen = !isYoubanHorizontalOpen;
    ui->sceneWidget->AnimateYoubanHorizontal(isYoubanHorizontalOpen);
}

void MainWindow::animateYoubanVertical()
{
    ui->sceneWidget->AnimateYoubanVertical();
}

void MainWindow::openFile(const QString &fileName)
{
    ui->sceneWidget->ImportObj(fileName);
    actorsComboBox->clear();
    const std::vector<std::string>& items = ui->sceneWidget->GetPickableItems();
    for (size_t i = 0; i < items.size(); i++) {
        actorsComboBox->addItem(QString::fromStdString(items[i]));
    }
    isDaofuOpen = false;
    isBiantianxianOpen = false;
    isShengjiangganOpen = false;
    isZuobanHorizontalOpen = false;
    isYoubanHorizontalOpen = false;
}

