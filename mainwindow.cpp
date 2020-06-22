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
#include <QColor>
#include <QColorDialog>
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

QColor colorToQColor(const double* color)
{
    return QColor(static_cast<int>(color[0] * 255), static_cast<int>(color[1] * 255), static_cast<int>(color[2] * 255));
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
    //AddShowAxesCheckBox();
    AddAnimateHighlightCheckBox();
    AddLoadTextureCheckBox();
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
    //delete showAxesCheckBox;
    delete animateHighlightCheckBox;
    delete loadTextureCheckBox;
    delete animateDaofuButton;
    delete animateBiantianxianButton;
    delete animateShengjiangganButton;
    delete animateZuobanHorizontalButton;
    delete animateZuobanVerticalButton;
    delete animateYoubanHorizontalButton;
    delete animateYoubanVerticalButton;
    delete setModelColorButton;
    delete setHighlightColorButton;
    delete setBackgroundColorButton;
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
    showAxesCheckBox->setFixedWidth(60);
    showAxesCheckBox->setText(tr("Axes"));
    ui->toolBar->addWidget(showAxesCheckBox);

    QObject::connect(showAxesCheckBox, SIGNAL(stateChanged(int)), this, SLOT(showAxesChanged(int)));
}

void MainWindow::AddAnimateHighlightCheckBox()
{
    isAnimatePick = false;
    animateHighlightCheckBox = new QCheckBox;
    animateHighlightCheckBox->setFixedWidth(80);
    animateHighlightCheckBox->setText(tr("Animate"));
    ui->toolBar->addWidget(animateHighlightCheckBox);

    QObject::connect(animateHighlightCheckBox, SIGNAL(stateChanged(int)), this, SLOT(animateHighlightChanged(int)));
}

void MainWindow::AddLoadTextureCheckBox()
{
    loadTexture = false;
    loadTextureCheckBox = new QCheckBox;
    loadTextureCheckBox->setFixedWidth(80);
    loadTextureCheckBox->setText(tr("load Tex"));
    ui->toolBar->addWidget(loadTextureCheckBox);

    QObject::connect(loadTextureCheckBox, SIGNAL(stateChanged(int)), this, SLOT(loadTextureChanged(int)));
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
    lightIntensitySlider->setFixedWidth(60);
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
    setModelColorButton = new QPushButton(QIcon(QPixmap(40, 40)), tr("model"));
    setHighlightColorButton = new QPushButton(QIcon(QPixmap(40, 40)), tr("picked"));
    setBackgroundColorButton = new QPushButton(QIcon(QPixmap(40, 40)), tr("bkg"));

    ui->toolBar->addWidget(animateDaofuButton);
    ui->toolBar->addWidget(animateBiantianxianButton);
    ui->toolBar->addWidget(animateShengjiangganButton);
    ui->toolBar->addWidget(animateZuobanHorizontalButton);
    ui->toolBar->addWidget(animateZuobanVerticalButton);
    ui->toolBar->addWidget(animateYoubanHorizontalButton);
    ui->toolBar->addWidget(animateYoubanVerticalButton);
    ui->toolBar->addWidget(setModelColorButton);
    ui->toolBar->addWidget(setHighlightColorButton);
    ui->toolBar->addWidget(setBackgroundColorButton);


    setModelColorButton->setEnabled(false);
    animateDaofuButton->setEnabled(false);
    animateBiantianxianButton->setEnabled(false);
    animateShengjiangganButton->setEnabled(false);
    animateZuobanVerticalButton->setEnabled(false);
    animateZuobanHorizontalButton->setEnabled(false);
    animateYoubanVerticalButton->setEnabled(false);
    animateYoubanHorizontalButton->setEnabled(false);

    QObject::connect(animateDaofuButton, SIGNAL(clicked()), this, SLOT(animateDaofu()));
    QObject::connect(animateBiantianxianButton, SIGNAL(clicked()), this, SLOT(animateBiantianxian()));
    QObject::connect(animateShengjiangganButton, SIGNAL(clicked()), this, SLOT(animateShengjianggan()));
    QObject::connect(animateZuobanHorizontalButton, SIGNAL(clicked()), this, SLOT(animateZuobanHorizontal()));
    QObject::connect(animateZuobanVerticalButton, SIGNAL(clicked()), this, SLOT(animateZuobanVertical()));
    QObject::connect(animateYoubanHorizontalButton, SIGNAL(clicked()), this, SLOT(animateYoubanHorizontal()));
    QObject::connect(animateYoubanVerticalButton, SIGNAL(clicked()), this, SLOT(animateYoubanVertical()));
    QObject::connect(setModelColorButton, SIGNAL(clicked()), this, SLOT(setModelColor()));
    QObject::connect(setHighlightColorButton, SIGNAL(clicked()), this, SLOT(setHighlightColor()));
    QObject::connect(setBackgroundColorButton, SIGNAL(clicked()), this, SLOT(setBackgroundColor()));
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

void MainWindow::animateHighlightChanged(int state)
{
    isAnimatePick = state == Qt::Checked;
}

void MainWindow::loadTextureChanged(int state)
{
    loadTexture = state == Qt::Checked;
}

void MainWindow::currentModuleChanged(QString moduleName)
{
    qDebug() << "currentActorChanged(QString): " << moduleName << endl;
    if (isAnimatePick) {
        ui->sceneWidget->AnimateHighlight(moduleName.toStdString());
    } else {
        ui->sceneWidget->PickModule(moduleName.toStdString());
    }
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

void MainWindow::setModelColor()
{
    auto color = ui->sceneWidget->GetModelColor();
    QColor qColor = QColorDialog::getColor(colorToQColor(color), nullptr, tr("select model color"), QColorDialog::ColorDialogOptions());
    ui->sceneWidget->SetModelColor(qColor.redF(), qColor.greenF(), qColor.blueF());
    auto pixmap = QPixmap(40, 40);
    pixmap.fill(qColor);
    setModelColorButton->setIcon(QIcon(pixmap));
}

void MainWindow::setHighlightColor()
{
    auto color = ui->sceneWidget->GetHighlightColor();
    QColor qColor = QColorDialog::getColor(colorToQColor(color), nullptr, tr("select highlight color"), QColorDialog::ColorDialogOptions());
    ui->sceneWidget->SetHighlightColor(qColor.redF(), qColor.greenF(), qColor.blueF());
    auto pixmap = QPixmap(40, 40);
    pixmap.fill(qColor);
    setHighlightColorButton->setIcon(QIcon(pixmap));
}

void MainWindow::setBackgroundColor()
{
    auto color = ui->sceneWidget->GetBackgroundColor();
    QColor qColor = QColorDialog::getColor(colorToQColor(color), nullptr, tr("select background color"), QColorDialog::ColorDialogOptions());
    ui->sceneWidget->SetBackgroundColor(qColor.redF(), qColor.greenF(), qColor.blueF());
    auto pixmap = QPixmap(40, 40);
    pixmap.fill(qColor);
    setBackgroundColorButton->setIcon(QIcon(pixmap));
}

void MainWindow::openFile(const QString &fileName)
{
    ui->sceneWidget->ImportObj(fileName, loadTexture);
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
    setModelColorButton->setEnabled(true);
    animateDaofuButton->setEnabled(true);
    animateBiantianxianButton->setEnabled(true);
    animateShengjiangganButton->setEnabled(true);
    animateZuobanVerticalButton->setEnabled(true);
    animateZuobanHorizontalButton->setEnabled(true);
    animateYoubanVerticalButton->setEnabled(true);
    animateYoubanHorizontalButton->setEnabled(true);
    auto pixmap = QPixmap(40, 40);
    pixmap.fill(colorToQColor(ui->sceneWidget->GetModelColor()));
    setModelColorButton->setIcon(QIcon(pixmap));
    pixmap.fill(colorToQColor(ui->sceneWidget->GetHighlightColor()));
    setHighlightColorButton->setIcon(QIcon(pixmap));
    pixmap.fill(colorToQColor(ui->sceneWidget->GetBackgroundColor()));
    setBackgroundColorButton->setIcon(QIcon(pixmap));
}

