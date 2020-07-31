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
#include <QGroupBox>
#include <float.h>

/***
 * 摄像机状态文件保存在运行目录下的cameras.txt，可以用此demo保存几个合适的摄像机状态，然后将cameras.txt拷贝到别的项目中使用
 * */
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
    QObject::connect(ui->sceneWidget, SIGNAL(importComplete(bool, QString, bool)), this, SLOT(fileOpened(bool, QString, bool)));

    InitProperties();
    AddStatusbarLabel();
    AddCamerasComboBox();
    AddActorComboBox();
    AddLightIntensityControl();
    AddOpacityControl();
    AddOpacity1Control();
    AddCameraAnimationSpeedControl();
    AddModuleAnimationSpeedControl();
    AddHighlightAnimationTimeControl();
    AddPushButtons();
}

MainWindow::~MainWindow()
{
    delete camerasComboBox;
    delete actorsComboBox;
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

/***
 * 状态栏
 * */
void MainWindow::AddStatusbarLabel()
{
    statusLabel = new QLabel;
    statusLabel->setTextInteractionFlags(Qt::TextSelectableByMouse);
    ui->statusbar->addWidget(statusLabel);
}

/***
 * 摄像机选项下拉框
 * 选择保存的摄像机位置，场景会切换到相应的位置和角度
 * 摄像机位置选项通过SceneWidget::LoadCameras方法从文件中读取
 * */
void MainWindow::AddCamerasComboBox()
{
    auto group = new QGroupBox(tr("Cameras"));
    ui->toolBar->addWidget(group);

    camerasComboBox = new QComboBox;
    camerasComboBox->setStyleSheet("color:black;background-color:white;padding:2 10");

    auto layout = new QVBoxLayout;
    layout->addWidget(camerasComboBox);
    group->setLayout(layout);

    QObject::connect(camerasComboBox, SIGNAL(currentTextChanged(QString)), this, SLOT(currentCameraChanged(QString)));
}

/***
 * 部件选择下拉框
 * 通过选择不同选项可以高亮（包括以动画方式）相应的部件
 * */
void MainWindow::AddActorComboBox()
{
    auto group = new QGroupBox(tr("Highlight"));
    ui->toolBar->addWidget(group);

    actorsComboBox = new QComboBox;
    actorsComboBox->setStyleSheet("color:black;background-color:white;padding:2 10");

    auto layout = new QVBoxLayout;
    layout->addWidget(actorsComboBox);
    group->setLayout(layout);

    QObject::connect(actorsComboBox, SIGNAL(currentTextChanged(QString)), this, SLOT(currentModuleChanged(QString)));
}

/***
 * 亮度控制滑动条
 * 用来控制场景的光照强度
 * */
void MainWindow::AddLightIntensityControl()
{
    auto group = new QGroupBox(tr("Light Intensity"));
    ui->toolBar->addWidget(group);
    auto layout = new QVBoxLayout;
    group->setLayout(layout);

    auto slider = new QSlider(Qt::Horizontal);
    slider->setMaximum(100);
    slider->setMinimum(1);
    layout->addWidget(slider);

    auto label = new QLabel;
    label->setMargin(5);
    layout->addWidget(label);

    QObject::connect(slider, SIGNAL(valueChanged(int)), this, SLOT(lightSliderValueChanged(int)));
    QObject::connect(this, SIGNAL(lightValueChanged(double)), label, SLOT(setNum(double)));

    slider->setValue(10);
}

/***
 * 透明度控制滑动条
 * 用来控制物体的透明度
 * */
void MainWindow::AddOpacityControl()
{
    auto group = new QGroupBox(tr("Opacity"));
    ui->toolBar->addWidget(group);
    auto layout = new QVBoxLayout;
    group->setLayout(layout);

    auto slider = new QSlider(Qt::Horizontal);
    slider->setMaximum(100);
    slider->setMinimum(0);
    layout->addWidget(slider);

    auto label = new QLabel;
    label->setMargin(5);
    layout->addWidget(label);

    QObject::connect(slider, SIGNAL(valueChanged(int)), this, SLOT(opacitySliderValueChanged(int)));
    QObject::connect(this, SIGNAL(opacityValueChanged(double)), label, SLOT(setNum(double)));

    slider->setValue(100);
}

/***
 * 透明度控制滑动条
 * 用来控制高亮内部部件时，外部部件的透明度
 * */
void MainWindow::AddOpacity1Control()
{
    auto group = new QGroupBox(tr("Opacity1"));
    ui->toolBar->addWidget(group);
    auto layout = new QVBoxLayout;
    group->setLayout(layout);

    auto slider = new QSlider(Qt::Horizontal);
    slider->setMaximum(100);
    slider->setMinimum(0);
    layout->addWidget(slider);

    auto label = new QLabel;
    label->setMargin(5);
    layout->addWidget(label);

    QObject::connect(slider, SIGNAL(valueChanged(int)), this, SLOT(opacity1SliderValueChanged(int)));
    QObject::connect(this, SIGNAL(opacity1ValueChanged(double)), label, SLOT(setNum(double)));

    slider->setValue(100);
}

/***
 * 相机动画速度控制滑动条
 * 用来控制相机动画的播放速度
 * */
void MainWindow::AddCameraAnimationSpeedControl()
{
    auto group = new QGroupBox(tr("Camera Speed"));
    ui->toolBar->addWidget(group);
    auto layout = new QVBoxLayout;
    group->setLayout(layout);

    auto slider = new QSlider(Qt::Horizontal);
    slider->setMaximum(100);
    slider->setMinimum(1);
    layout->addWidget(slider);

    auto label = new QLabel;
    label->setMargin(5);
    layout->addWidget(label);

    QObject::connect(slider, SIGNAL(valueChanged(int)), this, SLOT(cameraAnimationSpeedSliderValueChanged(int)));
    QObject::connect(this, SIGNAL(cameraAnimationSpeedValueChanged(double)), label, SLOT(setNum(double)));

    slider->setValue(10);
}

/***
 * 部件动画速度控制滑动条
 * 用来控制部件动画的移动速度
 * */
void MainWindow::AddModuleAnimationSpeedControl()
{
    auto group = new QGroupBox(tr("Animation Speed"));
    ui->toolBar->addWidget(group);
    auto layout = new QVBoxLayout;
    group->setLayout(layout);

    auto slider = new QSlider(Qt::Horizontal);
    slider->setMaximum(100);
    slider->setMinimum(1);
    layout->addWidget(slider);

    auto label = new QLabel;
    label->setMargin(5);
    layout->addWidget(label);

    QObject::connect(slider, SIGNAL(valueChanged(int)), this, SLOT(moduleAnimationSpeedSliderValueChanged(int)));
    QObject::connect(this, SIGNAL(moduleAnimationSpeedValueChanged(double)), label, SLOT(setNum(double)));

    slider->setValue(10);
}

/***
 * 高亮动画时间控制滑动条
 * 用来控制高亮动画播放时间
 * */
void MainWindow::AddHighlightAnimationTimeControl()
{
    auto group = new QGroupBox(tr("Highlight Time"));
    ui->toolBar->addWidget(group);
    auto layout = new QVBoxLayout;
    group->setLayout(layout);

    auto slider = new QSlider(Qt::Horizontal);
    slider->setMaximum(100);
    slider->setMinimum(1);
    layout->addWidget(slider);

    auto label = new QLabel;
    label->setMargin(5);
    layout->addWidget(label);

    QObject::connect(slider, SIGNAL(valueChanged(int)), this, SLOT(highlightAnimationTimeSliderValueChanged(int)));
    QObject::connect(this, SIGNAL(highlightAnimationTimeValueChanged(double)), label, SLOT(setNum(double)));

    slider->setValue(10);
}

/***
 * 设置功能按键，包括各部件的动画按钮，以及颜色设置按钮
 * */
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

    animateDaofuButton->setStyleSheet("padding:10;margin:5");
    animateBiantianxianButton->setStyleSheet("padding:10;margin:5");
    animateShengjiangganButton->setStyleSheet("padding:10;margin:5");
    animateYoubanVerticalButton->setStyleSheet("padding:10;margin:5");
    animateZuobanVerticalButton->setStyleSheet("padding:10;margin:5");
    animateYoubanHorizontalButton->setStyleSheet("padding:10;margin:5");
    animateZuobanHorizontalButton->setStyleSheet("padding:10;margin:5");
    setModelColorButton->setStyleSheet("padding:10;margin:5");
    setHighlightColorButton->setStyleSheet("padding:10;margin:5");
    setBackgroundColorButton->setStyleSheet("padding:10;margin:5");

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

/***
 * 保存当前摄像机状态
 * 保存当前场景中的摄像机位置和角度，并添加到选项下拉框中以供后续选择
 * */
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

/***
 * 窗口关闭事件
 * 主窗口关闭时，保存摄像机状态到文件中
 * */
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

/***
 * 用户在摄像机下拉框选择了不同的选项
 * 恢复场景中的摄像机状态到所选状态
 * 当动画选取部件选项勾选时，摄像机切换具有动画效果
 * */
void MainWindow::currentCameraChanged(QString name)
{
    std::string stdName = name.toStdString();
    qDebug() << "restore camera: " << name << endl;
    ui->sceneWidget->RestoreCamera(stdName, isAnimatePick);
}

/***
 * 勾选/取消勾选显示坐标轴选项
 * */
void MainWindow::showAxesChanged()
{
    ui->sceneWidget->ShowAxes(ui->actionShow_Axes->isChecked());
}

/***
 * 勾选/取消勾选动画选取部件选项
 * */
void MainWindow::animateHighlightChanged()
{
    isAnimatePick = ui->actionPick_with_Animation->isChecked();
}

/***
 * 勾选/取消勾选加载纹理贴图选项
 * 勾选时，再次加载模型将同时加载纹理贴图，否则不加载
 * */
void MainWindow::loadTextureChanged()
{
    loadTexture = ui->actionLoad_Texture->isChecked();
}

/***
 * 用户在模型部件下拉框选择了不同的选项
 * 以高亮显示选中的部件
 * 当动画选取部件选项勾选时，高亮过程有动画效果
 * */
void MainWindow::currentModuleChanged(QString moduleName)
{
    qDebug() << "currentActorChanged(QString): " << moduleName << endl;
    if (isAnimatePick) {
        ui->sceneWidget->AnimateHighlight(moduleName.toStdString());
    } else {
        ui->sceneWidget->PickModule(moduleName.toStdString());
    }
}

/***
 * 光照强度信号槽
 * */
void MainWindow::lightSliderValueChanged(int value)
{
    qDebug() << "lightSliderValueChanged()" << endl;
    double intensity = sliderValueToIntensity(value, 10.0);
    ui->sceneWidget->SetLightIntensity(intensity);
    lightValueChanged(intensity);
}

/***
 * 透明度信号槽
 * */
void MainWindow::opacitySliderValueChanged(int value)
{
    qDebug() << "opacitySliderValueChanged()" << endl;
    double opacity = sliderValueToIntensity(value, 100.0);
    ui->sceneWidget->SetOpacity(opacity);
    opacityValueChanged(opacity);
}

/***
 * 高亮内部部件透明度信号槽
 * */
void MainWindow::opacity1SliderValueChanged(int value)
{
    qDebug() << "opacity1SliderValueChanged()" << endl;
    double opacity = sliderValueToIntensity(value, 100.0);
    ui->sceneWidget->InsideSelectedOpacity = opacity;
    opacity1ValueChanged(opacity);
}

/***
 * 相机动画速度信号槽
 * */
void MainWindow::cameraAnimationSpeedSliderValueChanged(int value)
{
    qDebug() << "cameraAnimationSpeedSliderValueChanged()" << endl;
    double speed = sliderValueToIntensity(value, 10.0);
    ui->sceneWidget->CameraAnimationSpeed = speed;
    cameraAnimationSpeedValueChanged(speed);
}

/***
 * 部件动画速度信号槽
 * */
void MainWindow::moduleAnimationSpeedSliderValueChanged(int value)
{
    qDebug() << "moduleAnimationSpeedSliderValueChanged()" << endl;
    double speed = sliderValueToIntensity(value, 10.0);
    ui->sceneWidget->SetModuleAnimationSpeed(speed);
    moduleAnimationSpeedValueChanged(speed);
}

/***
 * 高亮动画时间信号槽
 * */
void MainWindow::highlightAnimationTimeSliderValueChanged(int value)
{
    qDebug() << "highlightAnimationTimeSliderValueChanged()" << endl;
    double time = sliderValueToIntensity(value, 10.0);
    ui->sceneWidget->HighlightAnimationTime = time;
    highlightAnimationTimeValueChanged(time);
}

/***
 * 倒伏动画
 * */
void MainWindow::animateDaofu()
{
    isDaofuOpen = !isDaofuOpen;
    ui->sceneWidget->AnimateDaofu(isDaofuOpen);
}

/***
 * 鞭天线动画
 * */
void MainWindow::animateBiantianxian()
{
    isBiantianxianOpen = !isBiantianxianOpen;
    ui->sceneWidget->AnimateBiantianxian(isBiantianxianOpen);
}

/***
 * 升降杆动画
 * */
void MainWindow::animateShengjianggan()
{
    isShengjiangganOpen = !isShengjiangganOpen;
    ui->sceneWidget->AnimateShengjianggan(isShengjiangganOpen);
}

/***
 * 左侧板天线水平动画
 * */
void MainWindow::animateZuobanHorizontal()
{
    isZuobanHorizontalOpen = !isZuobanHorizontalOpen;
    ui->sceneWidget->AnimateZuobanHorizontal(isZuobanHorizontalOpen);
}

/***
 * 左侧板天线垂直动画
 * */
void MainWindow::animateZuobanVertical()
{
    ui->sceneWidget->AnimateZuobanVertical();
}

/***
 * 右侧板天线水平动画
 * */
void MainWindow::animateYoubanHorizontal()
{
    isYoubanHorizontalOpen = !isYoubanHorizontalOpen;
    ui->sceneWidget->AnimateYoubanHorizontal(isYoubanHorizontalOpen);
}

/***
 * 右侧板天线垂直动画
 * */
void MainWindow::animateYoubanVertical()
{
    ui->sceneWidget->AnimateYoubanVertical();
}

/***
 * 设置模型颜色
 * */
void MainWindow::setModelColor()
{
    auto color = ui->sceneWidget->GetModelColor();
    QColor qColor = QColorDialog::getColor(colorToQColor(color), this, tr("select model color"), QColorDialog::ColorDialogOptions());
    if (qColor.isValid()) {
        ui->sceneWidget->SetModelColor(qColor.redF(), qColor.greenF(), qColor.blueF());
        auto pixmap = QPixmap(40, 40);
        pixmap.fill(qColor);
        setModelColorButton->setIcon(QIcon(pixmap));
    }
}

/***
 * 设置模型高亮颜色
 * */
void MainWindow::setHighlightColor()
{
    auto color = ui->sceneWidget->GetHighlightColor();
    QColor qColor = QColorDialog::getColor(colorToQColor(color), this, tr("select highlight color"), QColorDialog::ColorDialogOptions());
    if (qColor.isValid()) {
        ui->sceneWidget->SetHighlightColor(qColor.redF(), qColor.greenF(), qColor.blueF());
        auto pixmap = QPixmap(40, 40);
        pixmap.fill(qColor);
        setHighlightColorButton->setIcon(QIcon(pixmap));
    }
}

/***
 * 设置场景背景颜色
 * */
void MainWindow::setBackgroundColor()
{
    auto color = ui->sceneWidget->GetBackgroundColor();
    QColor qColor = QColorDialog::getColor(colorToQColor(color), this, tr("select background color"), QColorDialog::ColorDialogOptions());
    if (qColor.isValid()) {
        ui->sceneWidget->SetBackgroundColor(qColor.redF(), qColor.greenF(), qColor.blueF());
        auto pixmap = QPixmap(40, 40);
        pixmap.fill(qColor);
        setBackgroundColorButton->setIcon(QIcon(pixmap));
    }
}

/***
 * 模型加载完毕信号槽
 * 模型加载完成时，读取模型相机参数并重置按钮状态
 * */
void MainWindow::fileOpened(bool success, QString filepath, bool loadTexture)
{
    ui->actionOpenFile->setEnabled(true);

    if (!success) {
        statusLabel->setText(tr("Importing model failed"));
        QMessageBox::warning(this, tr("ERROR"), tr("open model file failed!"));
        return;
    }

    QFileInfo info(filepath);
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

    statusLabel->setText(tr("Model imported"));
}

/***
 * 打开模型文件
 * 打开后缀为.obj的模型文件（暂时只支持wavefront公司的obj格式的模型文件）
 * 异步加载，加载完成后将调用fileOpened信号槽
 * */
void MainWindow::openFile(const QString &fileName)
{
    if (ui->sceneWidget->IsImported())
    {
        qDebug() << "save cameras to file: " << camerasFilePath << endl;
        ui->sceneWidget->SaveCameras(camerasFilePath.toStdString());
    }

    statusLabel->setText(tr("Start importing model..."));
    ui->sceneWidget->ImportObjAsync(fileName, loadTexture);
    ui->actionOpenFile->setEnabled(false);
    setModelColorButton->setEnabled(false);
    animateDaofuButton->setEnabled(false);
    animateBiantianxianButton->setEnabled(false);
    animateShengjiangganButton->setEnabled(false);
    animateZuobanVerticalButton->setEnabled(false);
    animateZuobanHorizontalButton->setEnabled(false);
    animateYoubanVerticalButton->setEnabled(false);
    animateYoubanHorizontalButton->setEnabled(false);
}
