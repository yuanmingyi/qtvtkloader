#include "importworker.h"
#include "dongfengvis.h"
#include <QDebug>

ImportWorker::ImportWorker(QObject *parent) : QObject(parent)
{
    _dongfeng = nullptr;
    _isWorking = false;
}

void ImportWorker::start(QString filepath, bool loadTexture)
{
    _isWorking = true;
    bool success = true;
    try {
        _dongfeng->ImportObj(filepath.toStdString(), loadTexture);
    } catch (std::exception ex) {
        success = false;
        qDebug() << "import failed: " << ex.what() << endl;
    }
    _isWorking = false;
    done(success, filepath, loadTexture);
}
