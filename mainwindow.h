#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QLineEdit>
#include <QComboBox>
#include <QPushButton>
#include <QSlider>
#include <QLabel>
#include <QCheckBox>
#include <QObject>

namespace Ui {
class MainWindow;
}

class MainWindow : public QMainWindow
{
    Q_OBJECT
public:
    explicit MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

signals:
    void lightValueChanged(double);
    void opacityValueChanged(double);
    void opacity1ValueChanged(double);
    void cameraAnimationSpeedValueChanged(double);
    void moduleAnimationSpeedValueChanged(double);
    void highlightAnimationTimeValueChanged(double);

public slots:
    //! Show the 'About this application' dialog
    void showAboutDialog();

    //! Show the 'Open file...' dialog
    void showOpenFileDialog();

    void addCamera();
    void currentCameraChanged(QString);
    void currentModuleChanged(QString);

    void lightSliderValueChanged(int);
    void opacitySliderValueChanged(int);
    void opacity1SliderValueChanged(int);
    void cameraAnimationSpeedSliderValueChanged(int);
    void moduleAnimationSpeedSliderValueChanged(int);
    void highlightAnimationTimeSliderValueChanged(int);

    void showAxesChanged();
    void animateHighlightChanged();
    void loadTextureChanged();

    void animateDaofu();
    void animateBiantianxian();
    void animateShengjianggan();
    void animateZuobanHorizontal();
    void animateZuobanVertical();
    void animateYoubanHorizontal();
    void animateYoubanVertical();

    void setModelColor();
    void setBackgroundColor();
    void setHighlightColor();

    void fileOpened(bool, QString, bool);

protected:
    //! Open a file
    /*!
    \param[in] fileName The name of the file including the path
  */
    void openFile(const QString &fileName);
    void loadCameras(const QString &fileName);
    void saveCameras(const QString &fileName);
    void closeEvent(QCloseEvent *event);

private:
    Ui::MainWindow *ui;
    QComboBox* actorsComboBox;
    QComboBox* camerasComboBox;
    QLabel* statusLabel;
    QString camerasFilePath;

    QPushButton* setModelColorButton;
    QPushButton* setHighlightColorButton;
    QPushButton* setBackgroundColorButton;

    QPushButton* animateDaofuButton;
    QPushButton* animateBiantianxianButton;
    QPushButton* animateShengjiangganButton;
    QPushButton* animateZuobanHorizontalButton;
    QPushButton* animateZuobanVerticalButton;
    QPushButton* animateYoubanHorizontalButton;
    QPushButton* animateYoubanVerticalButton;

    bool isDaofuOpen;
    bool isBiantianxianOpen;
    bool isShengjiangganOpen;
    bool isZuobanHorizontalOpen;
    bool isYoubanHorizontalOpen;
    bool isAnimatePick;
    bool loadTexture;

    void InitProperties();
    void AddActorComboBox();
    void AddLightIntensityControl();
    void AddOpacityControl();
    void AddOpacity1Control();
    void AddCameraAnimationSpeedControl();
    void AddModuleAnimationSpeedControl();
    void AddHighlightAnimationTimeControl();
    void AddStatusbarLabel();
    void AddPushButtons();
    void AddCamerasComboBox();
};

#endif // MAINWINDOW_H
