#include "mainwindow.h"
#include "ui_mainwindow.h"

#include <QMessageBox>
#include <QCloseEvent>
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

static std::string camerasFilePath = "./cameras.txt";
static size_t cameraIndex = 0;

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
    QObject::connect(ui->actionShow_Axes, SIGNAL(triggered()), this, SLOT(showAxesChanged()));
    QObject::connect(ui->actionLoad_Texture, SIGNAL(triggered()), this, SLOT(loadTextureChanged()));
    QObject::connect(ui->actionPick_with_Animation, SIGNAL(triggered()), this, SLOT(animateHighlightChanged()));
    QObject::connect(ui->actionSaveCamera, SIGNAL(triggered()), this, SLOT(addCamera()));

    InitProperties();
    AddStatusbarLabel();
    AddCamerasComboBox();
    AddActorComboBox();
    AddLightIntensityControl();
    AddOpacityControl();
    AddPushButtons();
}

MainWindow::~MainWindow()
{
    delete camerasComboBox;
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
    delete setModelColorButton;
    delete setHighlightColorButton;
    delete setBackgroundColorButton;
    delete ui;
}

void MainWindow::InitProperties()
{
    loadTexture = false;
    isAnimatePick = false;
}

void MainWindow::AddStatusbarLabel()
{
    statusLabel = new QLabel;
    statusLabel->setTextInteractionFlags(Qt::TextSelectableByMouse);
    ui->statusbar->addWidget(statusLabel);
}

void MainWindow::AddCamerasComboBox()
{
    auto label = new QLabel;
    label->setText(tr("cameras:"));
    label->setMargin(5);
    ui->toolBar->addWidget(label);

    camerasComboBox = new QComboBox;
    camerasComboBox->setStyleSheet("color:black;background-color:white;margin:10");
    ui->toolBar->addWidget(camerasComboBox);

    QObject::connect(camerasComboBox, SIGNAL(currentTextChanged(QString)), this, SLOT(currentCameraChanged(QString)));
}

void MainWindow::AddActorComboBox()
{
    auto label = new QLabel;
    label->setText(tr("highlight:"));
    label->setMargin(5);
    ui->toolBar->addWidget(label);

    actorsComboBox = new QComboBox;
    actorsComboBox->setStyleSheet("color:black;background-color:white;margin:10");
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
    opacitySlider->setFixedWidth(100);
    opacitySlider->setMaximum(100);
    opacitySlider->setMinimum(0);
    opacitySlider->setValue(100);
    ui->toolBar->addWidget(opacitySlider);

    auto value = sliderValueToIntensity(opacitySlider->value(), 100.0);
    opacityEdit = new QLineEdit();
    opacityEdit->setValidator(validator);
    opacityEdit->setText(QString::number(value));
    ui->toolBar->addWidget(opacityEdit);
    ui->sceneWidget->SetOpacity(value);

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
    QMessageBox::information(this, "About", "By millan@IQR.\nSource code available under Apache License 2.0.");
}

void MainWindow::showOpenFileDialog()
{
    QString fileName = QFileDialog::getOpenFileName(this, tr("Open file"), "", "Model Files (*.obj)");
    QString filename = fileName.toLower();
    if (filename.endsWith(".obj")) {
        openFile(fileName);
    }
}

void MainWindow::addCamera()
{
    if (ui->sceneWidget->IsImported())
    {
        qDebug() << "add new camera: " << cameraIndex << endl;
        auto camera = QString::number(cameraIndex++);
        ui->sceneWidget->SaveCurrentCamera(camera.toStdString());
        camerasComboBox->addItem(camera);
    }
}

void MainWindow::closeEvent(QCloseEvent *event)
{
    // save the cameras
    if (ui->sceneWidget->IsImported())
    {
        qDebug() << "Save cameras to file: " << camerasFilePath << endl;
        ui->sceneWidget->SaveCameras(camerasFilePath.toStdString());
    }
    QMainWindow::closeEvent(event);
}

void MainWindow::currentCameraChanged(QString name)
{
    std::string stdName = name.toStdString();
    qDebug() << "restore camera: " << name << endl;
    ui->sceneWidget->RestoreCamera(stdName, isAnimatePick);
}

void MainWindow::showAxesChanged()
{
    ui->sceneWidget->ShowAxes(ui->actionShow_Axes->isChecked());
}

void MainWindow::animateHighlightChanged()
{
    isAnimatePick = ui->actionPick_with_Animation->isChecked();
}

void MainWindow::loadTextureChanged()
{
    loadTexture = ui->actionLoad_Texture->isChecked();
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
    if (ui->sceneWidget->IsImported())
    {
        qDebug() << "save cameras to file: " << camerasFilePath << endl;
        ui->sceneWidget->SaveCameras(camerasFilePath.toStdString());
    }

    ui->sceneWidget->ImportObj(fileName.toStdString(), loadTexture);

    QFileInfo info(fileName);
    camerasFilePath = info.dir().filePath("cameras.txt");
    QFileInfo camerasFileInfo(camerasFilePath);
    if (camerasFileInfo.exists())
    {
        qDebug() << "load cameras from file: " << camerasFilePath << endl;
        ui->sceneWidget->LoadCameras(camerasFilePath.toStdString());
    }

    camerasComboBox->clear();
    const std::vector<std::string>& cameras = ui->sceneWidget->GetCameras();
    for (size_t i = 0; i < cameras.size(); i++) {
        camerasComboBox->addItem(QString::fromStdString(cameras[i]));
    }
    cameraIndex = cameras.size();

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

