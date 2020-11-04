
#include "nvimage.h"
#include "FloatImage.h" // For FloatImage::WrapMode


namespace nv
{
    class FloatImage;

    float rmsColorError(const FloatImage * ref, const FloatImage * img, bool alphaWeight);
    float rmsAlphaError(const FloatImage * ref, const FloatImage * img);

    float averageColorError(const FloatImage * ref, const FloatImage * img, bool alphaWeight);
    float averageAlphaError(const FloatImage * ref, const FloatImage * img);

    float rmsBilinearColorError(const FloatImage * ref, const FloatImage * img, FloatImage::WrapMode wm, bool alphaWeight);

    float cieLabError(const FloatImage * ref, const FloatImage * img);
    float cieLab94Error(const FloatImage * ref, const FloatImage * img);
    float spatialCieLabError(const FloatImage * ref, const FloatImage * img);

    float averageAngularError(const FloatImage * img0, const FloatImage * img1);
    float rmsAngularError(const FloatImage * img0, const FloatImage * img1);

} // nv namespace
