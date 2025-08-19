#include <pbrt/bxdfs.h>

#include <pbrt/bssrdf.h>
#include <pbrt/interaction.h>
#include <pbrt/media.h>
#include <pbrt/options.h>
#include <pbrt/util/check.h>
#include <pbrt/util/color.h>
#include <pbrt/util/colorspace.h>
#include <pbrt/util/error.h>
#include <pbrt/util/file.h>
#include <pbrt/util/float.h>
#include <pbrt/util/hash.h>
#include <pbrt/util/log.h>
#include <pbrt/util/math.h>
#include <pbrt/util/memory.h>
#include <pbrt/util/print.h>
#include <pbrt/util/sampling.h>
#include <pbrt/util/stats.h>

#include "base/bxdf.h"
#include "pbrt/util/spectrum.h"

namespace pbrt {
  // BagherBxDF Methods
  Float BagherBxDF::FresnelTerm(Float cosTheta) const {
      // Fresnel modificado: F(θd) = F0 + (1-F0)(1-cosθd)^5 - F1*cosθd
      Float cos5 = std::pow(1 - cosTheta, 5);
      return Float(F0 + (1.0f - F0) * cos5 - F1 * cosTheta);
  }

  SampledSpectrum BagherBxDF::f(
    Vector3f wo,
    Vector3f wi,
    TransportMode mode
  ) const {

      Float cosThetaO = AbsCosTheta(wo);
      Float cosThetaI = AbsCosTheta(wi);

      if (cosThetaI == 0 || cosThetaO == 0) return SampledSpectrum(0);

      Vector3f wh = wi + wo;
      if (wh.x == 0 && wh.y == 0 && wh.z == 0) return SampledSpectrum(0);
      wh = Normalize(wh);

      Float cosThetaH = CosTheta(wh);
      if (cosThetaH <= 0) return SampledSpectrum(0);

      Float cosThetaD = Dot(wi, wh);

      Float D = distribution.D(wh);
      Float G = distribution.G(cosThetaI, cosThetaO);
      Float F = FresnelTerm(cosThetaD);

      SampledSpectrum specular = SampledSpectrum(R * D * G * F / (4 * cosThetaI * cosThetaO));

      return specular;
  }

  pstd::optional<BSDFSample> BagherBxDF::Sample_f(
    Vector3f wo,
    Float uc,
    Point2f u,
    TransportMode mode,
    BxDFReflTransFlags sampleFlags
  ) const {
      if (!(sampleFlags & BxDFReflTransFlags::Reflection)) return {};

      Vector3f wh = distribution.Sample_wh(u);
      if (Dot(wo, wh) < 0) wh = -wh;

      Vector3f wi = Reflect(wo, wh);
      if (!SameHemisphere(wo, wi)) return {};

      Float pdf = PDF(wo, wi, mode, sampleFlags);
      if (pdf == 0) return {};

      SampledSpectrum f_val = f(wo, wi, mode);

      return BSDFSample(f_val, wi, pdf, BxDFFlags::Glossy | BxDFFlags::Reflection);
  }

  // Bagher BxDF Methods Definitions
  Float BagherBxDF::PDF(
    Vector3f wo,
    Vector3f wi,
    TransportMode mode,
    BxDFReflTransFlags sampleFlags
  ) const {
      if (!(sampleFlags & BxDFReflTransFlags::Reflection)) return 0;
      if (!SameHemisphere(wo, wi)) return 0;

      Vector3f wh = Normalize(wo + wi);
      return distribution.Pdf(wh) / (4 * AbsDot(wo, wh));
  }

  std::string BagherBxDF::ToString() const {
      return StringPrintf(
        "[ BagherBxDF R: %d distribution: %s F0: %d F1: %d ]",
        R,
        distribution.ToString().c_str(),
        F0,
        F1
      );
  }
}
