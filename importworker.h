#ifndef IMPORTWORKER_H
#define IMPORTWORKER_H

#include <QObject>

class DongfengVis;
class ImportWorker : public QObject
{
    Q_OBJECT
public:
    explicit ImportWorker(QObject *parent = nullptr);

    void SetDongfeng(DongfengVis* dongfeng) { _dongfeng = dongfeng; }
    bool IsWorking() const { return _isWorking; }

signals:
    void done(bool success, QString filename, bool loadTexture);

public slots:
    void start(QString filename, bool loadTexture);

private:
    DongfengVis* _dongfeng;
    bool _isWorking;
};

#endif // WORKER_H
