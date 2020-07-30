#ifndef SCENEWIDGET_H
#define SCENEWIDGET_H

#ifdef WIN32
#pragma comment(lib, "user32.lib")
#pragma comment(lib, "gdi32.lib")
#pragma comment(lib, "Advapi32.lib")
#pragma comment(lib, "glu32.lib")
#pragma comment(lib, "opengl32.lib")
#pragma comment(lib, "dbghelp.lib")
#endif

#include <QObject>
#include <QSurfaceFormat>
#include <QVTKOpenGLNativeWidget.h>
#include "timerutil.h"
#include "dongfengvis.h"
#include "cameraanimation.h"

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
     * \fn void SceneWidget::ImportObj(const std::string& fileName, bool loadTexture = false)
     * \brief 从文件中导入模型
     * \param fileName: 模型文件名(.obj)
     * \param loadTexture: \c true同时导入贴图文件，\c false不导入贴图文件（贴图文件为jpg或者png在纹理文件(.mtl)中指定，必须放在模型文件同一目录下
    */
    void ImportObj(const std::string& fileName, bool loadTexture = false);

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
    void AnimateDaofu(bool open = true) { open ? _dongfeng->AnimateDaofu(0, 1) : _dongfeng->AnimateDaofu(1, 0); }

    /// 动画打开或者收起鞭天线
    /// open: true为打开鞭天线，false为收起鞭天线
    void AnimateBiantianxian(bool open = true) { open ? _dongfeng->AnimateBiantianxian(0, 1) : _dongfeng->AnimateBiantianxian(1, 0); }

    /// 动画展开或缩回升降杆
    /// open: true为全部展开升降杆，false为全部缩回升降杆
    void AnimateShengjianggan(bool open = true) { open ? _dongfeng->AnimateShengjianggan(0, 1) : _dongfeng->AnimateShengjianggan(1, 0); }

    /// 动画演示左侧板天线水平旋转
    /// open: true为从默认位置向外侧旋转90度，false为从外侧90度旋转回默认位置
    void AnimateZuobanHorizontal(bool open = true) { open ? _dongfeng->AnimateZuobanHorizontal(0, 1) : _dongfeng->AnimateZuobanHorizontal(1, 0); }

    /// 动画演示左侧板天线俯仰旋转
    void AnimateZuobanVertical() { _dongfeng->AnimateZuobanVertical(); }

    /// 动画演示右侧板天线水平旋转
    /// open: true为从默认位置向外侧旋转90度，false为从外侧90度旋转回默认位置
    void AnimateYoubanHorizontal(bool open = true) { open ? _dongfeng->AnimateYoubanHorizontal(0, 1) : _dongfeng->AnimateYoubanHorizontal(1, 0); }

    /// 动画演示右侧板天线俯仰旋转
    void AnimateYoubanVertical() { _dongfeng->AnimateYoubanVertical(); }

    ////// 以下方法为指定初始和结束位置的动画
    /// 系统按照线性插值算法根据给定的初始和结束位置计算出每一帧模型出现的位置并以动画形式播放出来
    /// start: 动画开始位置（具体数值各方法解释）
    /// end: 动画结束位置（具体数值各方法解释）

    /// 倒伏动画。0表示水平位置，1表示垂直立起位置
    void AnimateDaofu(double start = 0, double end = 1) { _dongfeng->AnimateDaofu(start, end); }

    /// 鞭天线动画。0表示水平位置，1表示两侧完全打开，并且垂直立起位置
    void AnimateBiantianxian(double start = 0, double end = 1) { _dongfeng->AnimateBiantianxian(start, end); }

    /// 升降杆动画。0表示全部缩回位置，1表示全部展开的位置
    void AnimateShengjianggan(double start = 0, double end = 1) { _dongfeng->AnimateShengjianggan(start, end); }

    /// （车）左侧板天线水平旋转动画。0表示默认位置，1表示向外侧旋转90度位置
    void AnimateZuobanHorizontal(double start = 0, double end = 1) { _dongfeng->AnimateZuobanHorizontal(start, end); }

    /// （车）左侧板天线垂直旋转动画。0表示默认位置，-0.5表示上仰45度位置，0.5表示下俯45度位置
    void AnimateZuobanVertical(double start, double end) { _dongfeng->AnimateZuobanVertical(start, end); }

    /// （车）右侧板天线水平旋转动画。0表示默认位置，1表示向外侧旋转90度位置
    void AnimateYoubanHorizontal(double start = 0, double end = 1) { _dongfeng->AnimateYoubanHorizontal(start, end); }

    /// （车）右侧板天线垂直旋转动画。0表示默认位置，-0.5表示上仰45度位置，0.5表示下俯45度位置
    void AnimateYoubanVertical(double start, double end) { _dongfeng->AnimateYoubanVertical(start, end); }

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

public slots:
    //! Zoom to the extent of the data set in the scene
    void zoomToExtent();

signals:
    void pickedModuleChanged(const std::string& moduleName);

private:
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
};

#endif // SCENEWIDGET_H
