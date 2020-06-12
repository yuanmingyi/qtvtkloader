#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QLineEdit>
#include <QComboBox>
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

public slots:
    //! Show the 'About this application' dialog
    void showAboutDialog();

    //! Show the 'Open file...' dialog
    void showOpenFileDialog();

    void currentModuleChanged(QString);
    void currentModuleChanged(std::string);

    void sliderValueChanged(int);
    void lightIntensityChanged(QString);
    void showAxesChanged(int);

protected:
    //! Open a file
    /*!
    \param[in] fileName The name of the file including the path
  */
    void openFile(const QString &fileName);

private:
    Ui::MainWindow *ui;
    QComboBox* actorsComboBox;
    QSlider* lightIntensitySlider;
    QLineEdit* lightIntensityEdit;
    QLabel* statusLabel;
    QCheckBox* showAxesCheckBox;

    void AddActorComboBox();
    void AddLightIntensityControl();
    void AddStatusbarLabel();
    void AddShowAxesCheckBox();
};

#endif // MAINWINDOW_H
