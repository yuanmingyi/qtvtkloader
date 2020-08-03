#ifndef SCENEWIDGET_H
#define SCENEWIDGET_H

#include <QObject>
#include <QSurfaceFormat>
#include <QVTKOpenGLNativeWidget.h>
#include <QThread>
#include "timerutil.h"
#include "dongfengvis.h"
#include "cameraanimation.h"
#include "importworker.h"

/*!
 * \brief The SceneWidget class
 * 将QT窗口提升为SceneWidget，使用ImportObj加载模型文件，然后通过成员函数控制模型的显示和动画
 * \remark 需要在Main.cpp中增加以下语句：QSurfaceFormat::setDefaultFormat(QVTKOpenGLNativeWidget::defaultFormat());
 */
class SceneWidget : public QVTKOpenGLNativeWidget
{
    Q_OBJECT
public:
    static void RendererEventHandler(vtkObject* caller, unsigned long eventId, void* clientData, void* callData);

    explicit SceneWidget(QWidget *parent = nullptr);
    ~SceneWidget();

    /*!
     * \brief InsideSelectedOpacity 选择高亮内部部件时，表面部件的透明度，0为完全透明，1为完全不透明，默认0.3
     */
    double InsideSelectedOpacity;

    /*!
     * \brief CameraAnimationSpeed 相机动画移动速度，单位像素/秒，默认为1
     */
    double CameraAnimationSpeed;

    /*!
     * \brief HighlightAnimationTime 高亮动画播放时间，单位秒，默认为1
     */
    double HighlightAnimationTime;

    /*!
     * \brief ModuleAnimationSpeed 部件动画速度，默认为1，表示当前部件完成完整动作花费1秒，速度越快时间越短
     */
    double GetModuleAnimationSpeed() const { return _moduleAnimationSpeed; }
    void SetModuleAnimationSpeed(double speed);

    /*!
     * \fn void SceneWidget::ImportObj(const std::string& fileName, bool loadTexture = false)
     * \brief 从文件中导入模型
     * \param fileName: 模型文件名(.obj)
     * \param loadTexture: \c true同时导入贴图文件，\c false不导入贴图文件（贴图文件为jpg或者png在纹理文件(.mtl)中指定，必须放在模型文件同一目录下
    */
    void ImportObj(const std::string& fileName, bool loadTexture = false);

    /*!
     * \brief ImportObjAsync 异步导入模型文件，导入完成将触发importComplete信号
     * \param fileName 文件路径
     * \param loadTexture 是否同时导入贴图文件
     */
    void ImportObjAsync(const QString& fileName, bool loadTexture = false);

    /*!
     * \fn const std::vector<std::string>& SceneWidget::GetPickableItems() const
     * \brief 获取所有可选部件名称
     * \return 返回可选部件名称列表，可选部件名称可以作为参数用于PickModule和AnimateHighlight方法的参数来将模型中相应部件高亮显示
     */
    const std::vector<std::string>& GetPickableItems() const { return _dongfeng->GetModuleNames(); }

    /*!
     * \fn bool SceneWidget::IsImported() const
     * \brief 模型是否已经加载
     * \return 返回 \c true 表示模型已完全加载。\c false 表示模型没有成功加载
     */
    bool IsImported() const { return _isFullImported; }

    /*!
     * \fn void SceneWidget::SetOpacity(double opacity)
     * \brief 设置模型的透明度
     * \param opacity: 范围0-1，0表示完全透明，1表示完全不透明，只对没有高亮和动画效果的部件进行设置
     */
    void SetOpacity(double opacity);

    /*!
     * \fn void SceneWidget::PickModule(const std::string& moduleName)
     * \brief 选取指定名称的部件，使用设置的高亮颜色渲染该部件
     * \param moduleName: 要选取的部件名称，参考GetPickableItems()
     */
    void PickModule(const std::string& moduleName);

    /*! \fn void SceneWidget::SetLightIntensity(double intensity)
     * \brief 设置光强度，对场景中所有物体有效
     * \param intensity: 光强度，0时光线最弱，数值越大光线约亮
     */
    void SetLightIntensity(double intensity);

    /// 显示/隐藏坐标轴
    /// show: true时显示xyz坐标轴，false隐藏坐标轴
    void ShowAxes(bool show) { _axes->SetVisibility(show); }

    /// 是否显示坐标轴
    /// 返回：true显示坐标轴，false隐藏坐标轴
    bool IsAxesShown() { return _axes->GetVisibility(); }

    /// 设置高亮颜色
    /// color: 3维数组，color[0]指定红色分量，color[1]指定绿色分量，color[2]指定蓝色分量，取值范围都是0-1
    void SetHighlightColor(const double* color);

    /// 设置高亮颜色
    /// r: 红色分量（0-1）
    /// g: 绿色分量（0-1）
    /// b: 蓝色分量（0-1）
    void SetHighlightColor(double r, double g, double b);

    /// 设置模型颜色（仅针对没有高亮和动画的部件）
    /// color: 3维数组，color[0]指定红色分量，color[1]指定绿色分量，color[2]指定蓝色分量，取值范围都是0-1
    void SetModelColor(const double* color);

    /// 设置模型颜色（仅针对没有高亮和动画的部件）
    /// r: 红色分量（0-1）
    /// g: 绿色分量（0-1）
    /// b: 蓝色分量（0-1）
    void SetModelColor(double r, double g, double b);

    /// 设置背景颜色
    /// color: 3维数组，color[0]指定红色分量，color[1]指定绿色分量，color[2]指定蓝色分量，取值范围都是0-1
    void SetBackgroundColor(const double* color);

    /// 设置背景颜色
    /// r: 红色分量（0-1）
    /// g: 绿色分量（0-1）
    /// b: 蓝色分量（0-1）
    void SetBackgroundColor(double r, double g, double b);

    /// 获取当前高亮颜色
    /// 返回：当前高亮颜色数组，[0]是红色分量，[1]是绿色分量，[2]是蓝色分量
    const double* GetHighlightColor() const { return _highlightColor; }

    /// 获取当前模型颜色
    /// 返回：当前模型颜色数组，[0]是红色分量，[1]是绿色分量，[2]是蓝色分量
    const double* GetModelColor() const { return _modelColor; }

    /// 获取当前背景颜色
    /// 返回：当前背景颜色数组，[0]是红色分量，[1]是绿色分量，[2]是蓝色分量
    const double* GetBackgroundColor() const { return _backgroundColor; }

    ////// 演示动画
    /// 动画高亮指定的部件
    /// moduleName: 要高亮的部件名称，可以通过GetPickableItems获取所有可用的名称
    void AnimateHighlight(const std::string& moduleName);

    /// 动画打开或收起倒伏
    /// open: true为打开倒伏，false为收起倒伏
    void AnimateDaofu(bool open = true) { open ? AnimateDaofu(0, 1) : AnimateDaofu(1, 0); }

    /// 动画打开或者收起鞭天线
    /// open: true为打开鞭天线，false为收起鞭天线
    void AnimateBiantianxian(bool open = true) { open ? AnimateBiantianxian(0, 1) : AnimateBiantianxian(1, 0); }

    /// 动画展开或缩回升降杆
    /// open: true为全部展开升降杆，false为全部缩回升降杆
    void AnimateShengjianggan(bool open = true) { open ? AnimateShengjianggan(0, 1) : AnimateShengjianggan(1, 0); }

    /// 动画演示左侧板天线水平旋转
    /// open: true为从默认位置向外侧旋转90度，false为从外侧90度旋转回默认位置
    void AnimateZuobanHorizontal(bool open = true) { open ? AnimateZuobanHorizontal(0, 1) : AnimateZuobanHorizontal(1, 0); }

    /// 动画演示左侧板天线俯仰旋转
    void AnimateZuobanVertical() { _dongfeng->AnimateZuobanVertical(); }

    /// 动画演示右侧板天线水平旋转
    /// open: true为从默认位置向外侧旋转90度，false为从外侧90度旋转回默认位置
    void AnimateYoubanHorizontal(bool open = true) { open ? AnimateYoubanHorizontal(0, 1) : AnimateYoubanHorizontal(1, 0); }

    /// 动画演示右侧板天线俯仰旋转
    void AnimateYoubanVertical() { _dongfeng->AnimateYoubanVertical(); }

    ////// 以下方法为指定初始和结束位置的动画
    /// 系统按照线性插值算法根据给定的初始和结束位置计算出每一帧模型出现的位置并以动画形式播放出来
    /// start: 动画开始位置（具体数值各方法解释）
    /// end: 动画结束位置（具体数值各方法解释）

    /// 左倒伏动画。0表示水平位置，1表示垂直立起位置
    void AnimateZuodaofu(double start = 0, double end = 1, bool updateCamera = true) {
        _resetCameraInAnimation[DongfengVis::Zuodaofu] = updateCamera;
        _dongfeng->AnimateZuodaofu(start, end);
        _zuodaofu = end;
    }

    /// 右倒伏动画。0表示水平位置，1表示垂直立起位置
    void AnimateYoudaofu(double start = 0, double end = 1, bool updateCamera = true) {
        _resetCameraInAnimation[DongfengVis::Youdaofu] = updateCamera;
        _dongfeng->AnimateYoudaofu(start, end);
        _youdaofu = end;
    }

    /// 倒伏动画。0表示水平位置，1表示垂直立起位置
    void AnimateDaofu(double start = 0, double end = 1, bool updateCamera = true) {
        _resetCameraInAnimation[DongfengVis::Daofu] = updateCamera;
        _dongfeng->AnimateDaofu(start, end);
        _daofu = end;
    }

    /// 鞭天线动画。0表示水平位置，1表示两侧完全打开，并且垂直立起位置
    void AnimateBiantianxian(double start = 0, double end = 1, bool updateCamera = true) {
        _resetCameraInAnimation[DongfengVis::Biantianxian] = updateCamera;
        _dongfeng->AnimateBiantianxian(start, end);
        _biantianxian = end;
    }

    /// 升降杆动画。0表示全部缩回位置，1表示全部展开的位置
    void AnimateShengjianggan(double start = 0, double end = 1, bool updateCamera = true) {
        _resetCameraInAnimation[DongfengVis::Shengjianggan] = updateCamera;
        _dongfeng->AnimateShengjianggan(start, end);
        _shengjianggan = end;
    }

    /// （车）左侧板天线水平旋转动画。0表示默认位置，1表示向外侧旋转90度位置
    void AnimateZuobanHorizontal(double start = 0, double end = 1, bool updateCamera = false) {
        _resetCameraInAnimation[DongfengVis::ZuobanHorizontal] = updateCamera;
        _dongfeng->AnimateZuobanHorizontal(start, end);
        _zuobanHorizontal = end;
    }

    /// （车）左侧板天线垂直旋转动画。0表示默认位置，-0.5表示上仰45度位置，0.5表示下俯45度位置
    void AnimateZuobanVertical(double start, double end, bool updateCamera = false) {
        _resetCameraInAnimation[DongfengVis::ZuobanVertical] = updateCamera;
        _dongfeng->AnimateZuobanVertical(start, end);
        _zuobanVertical = end;
    }

    /// （车）右侧板天线水平旋转动画。0表示默认位置，1表示向外侧旋转90度位置
    void AnimateYoubanHorizontal(double start = 0, double end = 1, bool updateCamera = false) {
        _resetCameraInAnimation[DongfengVis::YoubanHorizontal] = updateCamera;
        _dongfeng->AnimateYoubanHorizontal(start, end);
        _youbanHorizontal = end;
    }

    /// （车）右侧板天线垂直旋转动画。0表示默认位置，-0.5表示上仰45度位置，0.5表示下俯45度位置
    void AnimateYoubanVertical(double start, double end, bool updateCamera = false) {
        _resetCameraInAnimation[DongfengVis::YoubanVertical] = updateCamera;
        _dongfeng->AnimateYoubanVertical(start, end);
        _youbanVertical = end;
    }

    /// 左倒伏动画。0表示水平位置，90表示垂直立起位置
    void AnimateZuodaofuD(double start = 0, double end = 90, bool updateCamera = true) { AnimateZuodaofu(start / 90, end / 90, updateCamera); }

    /// 右倒伏动画。0表示水平位置，90表示垂直立起位置
    void AnimateYoudaofuD(double start = 0, double end = 90, bool updateCamera = true) { AnimateYoudaofu(start / 90, end / 90, updateCamera); }

    /// 倒伏动画。0表示水平位置，90表示垂直立起位置
    void AnimateDaofuD(double start = 0, double end = 90, bool updateCamera = true) { AnimateDaofu(start / 90, end / 90, updateCamera); }

    /// 鞭天线动画。0表示水平位置，90表示两侧完全打开，并且垂直立起位置
    void AnimateBiantianxianD(double start = 0, double end = 90, bool updateCamera = true) { AnimateBiantianxian(start / 90, end / 90, updateCamera); }

    /// 升降杆动画。0表示全部缩回位置，8表示全部展开的位置(8米）
    void AnimateShengjiangganM(double start = 0, double end = 8, bool updateCamera = true) { AnimateShengjianggan(start / 8, end / 8, updateCamera); }

    /// （车）左侧板天线水平旋转动画。0表示默认位置，90表示向外侧旋转90度位置
    void AnimateZuobanHorizontalD(double start = 0, double end = 90, bool updateCamera = false) { AnimateZuobanHorizontal(start / 90, end / 90, updateCamera); }

    /// （车）左侧板天线垂直旋转动画。0表示默认位置，-45表示上仰45度位置，45表示下俯45度位置
    void AnimateZuobanVerticalD(double start, double end, bool updateCamera = false) { AnimateZuobanVertical(start / 90, end / 90, updateCamera); }

    /// （车）右侧板天线水平旋转动画。0表示默认位置，90表示向外侧旋转90度位置
    void AnimateYoubanHorizontalD(double start = 0, double end = 90, bool updateCamera = false) { AnimateYoubanHorizontal(start / 90, end / 90, updateCamera); }

    /// （车）右侧板天线垂直旋转动画。0表示默认位置，-45表示上仰45度位置，45表示下俯45度位置
    void AnimateYoubanVerticalD(double start, double end, bool updateCamera = false) { AnimateYoubanVertical(start / 90, end / 90, updateCamera); }

    ///
    /// \brief MoveZuodaofuTo 旋转左倒伏到目标位置
    /// \param goal 要旋转到的位置，范围0-1，0表示0度位置，1表示90度位置
    /// \param updateCamera 为true时，动画过程实时调整相机视角使模型保持在相机可视范围
    ///
    void MoveZuodaofuTo(double goal, bool updateCamera = true) { AnimateZuodaofu(_zuodaofu, goal, updateCamera); }

    ///
    /// \brief MoveYoudaofuTo 旋转右倒伏到目标位置
    /// \param goal 要旋转到的位置，范围0-1，0表示0度位置，1表示90度位置
    /// \param updateCamera 为true时，动画过程实时调整相机视角使模型保持在相机可视范围
    ///
    void MoveYoudaofuTo(double goal, bool updateCamera = true) { AnimateYoudaofu(_youdaofu, goal, updateCamera); }

    void MoveDaofuTo(double goal, bool updateCamera = true) { AnimateDaofu(_daofu, goal, updateCamera); }

    void MoveBiantianxianTo(double goal, bool updateCamera = true) { AnimateBiantianxian(_biantianxian, goal, updateCamera); }

    void MoveShengjiangganTo(double goal, bool updateCamera = true) { AnimateShengjianggan(_shengjianggan, goal, updateCamera); }

    void MoveZuobanVerticalTo(double goal, bool updateCamera = false) { AnimateZuobanVertical(_zuobanVertical, goal, updateCamera); }

    void MoveZuobanHorizontalTo(double goal, bool updateCamera = false) { AnimateZuobanHorizontal(_zuobanHorizontal, goal, updateCamera); }

    void MoveYoubanVerticalTo(double goal, bool updateCamera = false) { AnimateYoubanVertical(_youbanVertical, goal, updateCamera); }

    void MoveYoubanHorizontalTo(double goal, bool updateCamera = false) { AnimateYoubanHorizontal(_youbanHorizontal, goal, updateCamera); }

    ///
    /// \brief MoveZuodaofu 旋转左倒伏指定角度
    /// \param goal 要旋转的距离，范围-1到1，-1表示-90度，1表示90度
    /// \param updateCamera 为true时，动画过程实时调整相机视角使模型保持在相机可视范围
    ///
    void MoveZuodaofu(double offset, bool updateCamera = true) { MoveZuodaofuTo(_zuodaofu + offset, updateCamera); }

    ///
    /// \brief MoveYoudaofu 旋转右倒伏指定角度
    /// \param goal 要旋转的距离，范围-1到1，-1表示-90度，1表示90度
    /// \param updateCamera 为true时，动画过程实时调整相机视角使模型保持在相机可视范围
    ///
    void MoveYoudaofu(double offset, bool updateCamera = true) { MoveYoudaofuTo(_youdaofu + offset, updateCamera); }

    void MoveDaofu(double offset, bool updateCamera = true) { MoveDaofuTo(_daofu + offset, updateCamera); }

    void MoveBiantianxian(double offset, bool updateCamera = true) { MoveBiantianxianTo(_biantianxian + offset, updateCamera); }

    void MoveShengjianggan(double offset, bool updateCamera = true) { MoveShengjiangganTo(_shengjianggan + offset, updateCamera); }

    void MoveZuobanVertical(double offset, bool updateCamera = false) { MoveZuobanVerticalTo(_zuobanVertical + offset, updateCamera); }

    void MoveZuobanHorizontal(double offset, bool updateCamera = false) { MoveZuobanHorizontalTo(_zuobanHorizontal + offset, updateCamera); }

    void MoveYoubanVertical(double offset, bool updateCamera = false) { MoveYoubanVerticalTo(_youbanVertical + offset, updateCamera); }

    void MoveYoubanHorizontal(double offset, bool updateCamera = false) { MoveYoubanHorizontalTo(_youbanHorizontal + offset, updateCamera); }

    ///
    /// \brief MoveZuodaofuD 旋转左倒伏指定角度
    /// \param goal 要旋转的角度，范围-90到90，单位角度
    /// \param updateCamera 为true时，动画过程实时调整相机视角使模型保持在相机可视范围
    ///
    void MoveZuodaofuD(double offset, bool updateCamera = true) { MoveZuodaofu(offset / 90, updateCamera); }

    ///
    /// \brief MoveYoudaofu 旋转右倒伏指定角度
    /// \param goal 要旋转的角度，范围-90到90，单位角度
    /// \param updateCamera 为true时，动画过程实时调整相机视角使模型保持在相机可视范围
    ///
    void MoveYoudaofuD(double offset, bool updateCamera = true) { MoveYoudaofu(offset / 90, updateCamera); }

    void MoveDaofuD(double offset, bool updateCamera = true) { MoveDaofu(offset / 90, updateCamera); }

    void MoveBiantianxianD(double offset, bool updateCamera = true) { MoveBiantianxian(offset / 90, updateCamera); }

    void MoveShengjiangganM(double offset, bool updateCamera = true) { MoveShengjianggan(offset / 8, updateCamera); }

    void MoveZuobanVerticalD(double offset, bool updateCamera = false) { MoveZuobanVertical(offset / 90, updateCamera); }

    void MoveZuobanHorizontalD(double offset, bool updateCamera = false) { MoveZuobanHorizontal(offset / 90, updateCamera); }

    void MoveYoubanVerticalD(double offset, bool updateCamera = false) { MoveYoubanVertical(offset / 90, updateCamera); }

    void MoveYoubanHorizontalD(double offset, bool updateCamera = false) { MoveYoubanHorizontal(offset / 90, updateCamera); }

    ///
    /// \brief MoveZuodaofuToD 旋转左倒伏到目标位置
    /// \param goal 要旋转到的角度位置，范围0-90，单位角度
    /// \param updateCamera 为true时，动画过程实时调整相机视角使模型保持在相机可视范围
    ///
    void MoveZuodaofuToD(double goal, bool updateCamera = true) { MoveZuodaofuTo(goal / 90, updateCamera); }

    ///
    /// \brief MoveYoudaofuTo 旋转右倒伏到目标位置
    /// \param goal 要旋转到的角度位置，范围0-90，单位角度
    /// \param updateCamera 为true时，动画过程实时调整相机视角使模型保持在相机可视范围
    ///
    void MoveYoudaofuToD(double goal, bool updateCamera = true) { MoveYoudaofuTo(goal / 90, updateCamera); }

    void MoveDaofuToD(double goal, bool updateCamera = true) { MoveDaofuTo(goal / 90, updateCamera); }

    void MoveBiantianxianToD(double goal, bool updateCamera = true) { MoveBiantianxianTo(goal / 90, updateCamera); }

    void MoveShengjiangganToM(double goal, bool updateCamera = true) { MoveShengjiangganTo(goal / 8, updateCamera); }

    void MoveZuobanVerticalToD(double goal, bool updateCamera = false) { MoveZuobanVerticalTo(goal / 90, updateCamera); }

    void MoveZuobanHorizontalToD(double goal, bool updateCamera = false) { MoveZuobanHorizontalTo(goal / 90, updateCamera); }

    void MoveYoubanVerticalToD(double goal, bool updateCamera = false) { MoveYoubanVerticalTo(goal / 90, updateCamera); }

    void MoveYoubanHorizontalToD(double goal, bool updateCamera = false) { MoveYoubanHorizontalTo(goal / 90, updateCamera); }

    ////// 摄像机相关方法
    /// 保存当前摄像机位置和角度
    /// cameraName: 保存名称，如果有重复的名字将覆盖之前的保存
    void SaveCurrentCamera(const std::string& cameraName);

    /// 恢复之前保存的指定名称的摄像机的位置和角度
    /// cameraName: 保存名称，在SaveCurrentCamera中指定
    /// animate: 是否以动画形式播放。设为true时，摄像机从当前位置以动画形式光滑的移动到目标位置；设为false时，摄像机直接出现在目标位置
    void RestoreCamera(const std::string& cameraName, bool animate = false);

    /// 是否保存了给定名称的摄像机位置
    /// cameraName: 要查询的摄像机保存名称
    /// 返回：true表示存在，可以通过RestoreCamera恢复改保存的位置，false表示不存在
    bool HasCamera(const std::string& cameraName) const { return _cameras.find(cameraName) != _cameras.end(); }

    /// 获取所有保存的摄像机位置的名称
    /// 返回：所有保存的摄像机位置名称列表
    const std::vector<std::string>& GetCameras() const { return _cameraNames; }

    /// 从文件中读取保存的摄像机位置
    /// filepath: 摄像机位置文件路径
    void LoadCameras(const std::string& filepath);

    /// 将当前保存的摄像机位置写入到指定文件中
    /// filepath: 要保存的文件路径。文件内容将被覆盖
    void SaveCameras(const std::string& filepath);

    ///
    /// \brief ResetCamera: 重置相机位置和角度来完整的展示车辆
    ///
    void ResetCamera();

signals:
    ///
    /// \brief importComplete 加载模型完成
    /// \param success \c true表示加载成功，\c false表示加载失败
    /// \param filepath 加载模型的文件路径
    /// \param loadTexture 是否加载纹理贴图
    ///
    void importComplete(bool success, QString filepath, bool loadTexture);

    ///
    /// \brief startImport 发送异步加载模型信号
    /// \param filepath 模型文件路径
    /// \param loadTexture 是否加载纹理贴图
    ///
    void startImport(QString filepath, bool loadTexture);

public slots:
    void importDone(bool success, QString filepath, bool loadTexture);

private:
    ImportWorker *_worker;
    QThread _workThread;
    bool _isImporting;

    void StartTimer();
    void EndTimer(const std::string& context = "time: ");
    void UpdateDepthRendering();

    CameraInfo GetCameraInfo();
    void ApplyCamaraInfo(const CameraInfo& cameraInfo);

    DongfengVis* _dongfeng;
    vtkNew<vtkRenderer> _renderer;
    vtkNew<vtkLight> _light;
    vtkNew<vtkAxesActor> _axes;
    timerutil _tm;
    bool _isTiming;
    bool _isFullImported;
    double _highlightColor[3];
    double _modelColor[3];
    double _backgroundColor[3];
    bool _useDepthPeeling;
    int _maxPeels;
    double _occulusionRatio;
    std::vector<std::string> _cameraNames;
    std::map<std::string, CameraInfo> _cameras;

    double _opacity;
    double _moduleAnimationSpeed;

    /// current module states
    double _zuodaofu;
    double _youdaofu;
    double _daofu;
    double _biantianxian;
    double _shengjianggan;
    double _zuobanVertical;
    double _zuobanHorizontal;
    double _youbanVertical;
    double _youbanHorizontal;

    std::map<std::string, bool> _resetCameraInAnimation;
};

#endif // SCENEWIDGET_H
