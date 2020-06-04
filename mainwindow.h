#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QLineEdit>
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

protected:
  //! Open a file
  /*!
    \param[in] fileName The name of the file including the path
  */
  void openFile(const QString &fileName);

private:
    Ui::MainWindow *ui;
    QLineEdit *edit;
};

#endif // MAINWINDOW_H
