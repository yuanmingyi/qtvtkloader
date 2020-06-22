#ifndef UTIL_H
#define UTIL_H

#include <vtkSmartPointer.h>
#include <vtkRenderWindow.h>
#include <vtkRenderer.h>

namespace util
{
    /**
     * Setup the rendering environment for depth peeling (general depth peeling
     * support is requested).
     * @see IsDepthPeelingSupported()
     * @param renderWindow a valid openGL-supporting render window
     * @param renderer a valid renderer instance
     * @param maxNoOfPeels maximum number of depth peels (multi-pass rendering)
     * @param occulusionRation the occlusion ration (0.0 means a perfect image,
     * >0.0 means a non-perfect image which in general results in faster rendering)
     * @return TRUE if depth peeling could be set up
     */
    static inline bool SetupEnvironmentForDepthPeeling(
            vtkSmartPointer<vtkRenderWindow> renderWindow,
            vtkSmartPointer<vtkRenderer> renderer, int maxNoOfPeels,
            double occlusionRatio)
    {
        if (!renderWindow || !renderer)
            return false;

        // 1. Use a render window with alpha bits (as initial value is 0 (false)):
        renderWindow->SetAlphaBitPlanes(true);

        // 2. Force to not pick a framebuffer with a multisample buffer
        // (as initial value is 8):
        renderWindow->SetMultiSamples(0);

        // 3. Choose to use depth peeling (if supported) (initial value is 0 (false)):
        renderer->SetUseDepthPeeling(true);

        // 4. Set depth peeling parameters
        // - Set the maximum number of rendering passes (initial value is 4):
        renderer->SetMaximumNumberOfPeels(maxNoOfPeels);
        // - Set the occlusion ratio (initial value is 0.0, exact image):
        renderer->SetOcclusionRatio(occlusionRatio);

        return true;
    }


    /**
     * Find out whether this box supports depth peeling. Depth peeling requires
     * a variety of openGL extensions and appropriate drivers.
     * @param renderWindow a valid openGL-supporting render window
     * @param renderer a valid renderer instance
     * @param doItOffscreen do the test off screen which means that nothing is
     * rendered to screen (this requires the box to support off screen rendering)
     * @return TRUE if depth peeling is supported, FALSE otherwise (which means
     * that another strategy must be used for correct rendering of translucent
     * geometry, e.g. CPU-based depth sorting)
     */
    static inline bool IsDepthPeelingSupported(vtkSmartPointer<vtkRenderWindow> renderWindow,
                                               vtkSmartPointer<vtkRenderer> renderer,
                                               bool doItOffScreen)
    {
        if (!renderWindow || !renderer)
        {
            return false;
        }

        bool success = true;

        // Save original renderer / render window state
        bool origOffScreenRendering = renderWindow->GetOffScreenRendering() == 1;
        bool origAlphaBitPlanes = renderWindow->GetAlphaBitPlanes() == 1;
        int origMultiSamples = renderWindow->GetMultiSamples();
        bool origUseDepthPeeling = renderer->GetUseDepthPeeling() == 1;
        int origMaxPeels = renderer->GetMaximumNumberOfPeels();
        double origOcclusionRatio = renderer->GetOcclusionRatio();

        // Activate off screen rendering on demand
        renderWindow->SetOffScreenRendering(doItOffScreen);

        // Setup environment for depth peeling (with some default parametrization)
        success = success && SetupEnvironmentForDepthPeeling(renderWindow, renderer,
                                                             100, 0.1);

        // Do a test render
        renderWindow->Render();

        // Check whether depth peeling was used
        success = success && renderer->GetLastRenderingUsedDepthPeeling();

        // recover original state
        renderWindow->SetOffScreenRendering(origOffScreenRendering);
        renderWindow->SetAlphaBitPlanes(origAlphaBitPlanes);
        renderWindow->SetMultiSamples(origMultiSamples);
        renderer->SetUseDepthPeeling(origUseDepthPeeling);
        renderer->SetMaximumNumberOfPeels(origMaxPeels);
        renderer->SetOcclusionRatio(origOcclusionRatio);

        return success;
    }
}

#endif // UTIL_H
