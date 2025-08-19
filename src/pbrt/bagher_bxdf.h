#ifndef PBRT_BAGHER_BXDF
#define PBRT_BAGHER_BXDF

#include <pbrt/pbrt.h>

#include <pbrt/base/bxdf.h>
#include <pbrt/interaction.h>
#include <pbrt/media.h>
#include <pbrt/options.h>
#include <pbrt/util/math.h>
#include <pbrt/util/memory.h>
#include <pbrt/util/pstd.h>
#include <pbrt/util/scattering.h>
#include <pbrt/util/spectrum.h>
#include <pbrt/util/taggedptr.h>
#include <pbrt/util/vecmath.h>

#include <string>

namespace pbrt {
  // BagherBxDF Declarations
  class BagherBxDF {
  public:
      // BagherBxDF Public Methods
      BagherBxDF() = default;

      PBRT_CPU_GPU
      BagherBxDF(
        const Float &R,
        const BagherDistribution &distribution,
        const Float &F0,
        const Float &F1
      ) :
      R(R),
      distribution(distribution),
      F0(F0),
      F1(F1) {}

      PBRT_CPU_GPU
      BxDFFlags Flags() const {
          return BxDFFlags::Reflection | BxDFFlags::Glossy;
      }

      PBRT_CPU_GPU
      SampledSpectrum f(
        Vector3f wo,
        Vector3f wi,
        TransportMode mode
      ) const;

      PBRT_CPU_GPU
      pstd::optional<BSDFSample> Sample_f(
        Vector3f wo,
        Float uc,
        Point2f u,
        TransportMode mode,
        BxDFReflTransFlags sampleFlags
      ) const;

      PBRT_CPU_GPU
      Float PDF(Vector3f wo, Vector3f wi, TransportMode mode,
                BxDFReflTransFlags sampleFlags) const;

      std::string ToString() const;

      PBRT_CPU_GPU void Regularize() {}

  private:
      // BagherBxDF Private Methods
      Float FresnelTerm(Float cosTheta) const;

      // BagherBxDF Private Members
      Float R;
      BagherDistribution distribution;
      Float F0, F1;
  };

}
#endif
