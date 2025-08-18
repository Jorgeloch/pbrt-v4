// pbrt is Copyright(c) 1998-2020 Matt Pharr, Wenzel Jakob, and Greg Humphreys.
// The pbrt source code is licensed under the Apache License, Version 2.0.
// SPDX: Apache-2.0

#include <pbrt/util/scattering.h>

namespace pbrt {

// BSSRDF Utility Functions
Float FresnelMoment1(Float eta) {
    Float eta2 = eta * eta, eta3 = eta2 * eta, eta4 = eta3 * eta, eta5 = eta4 * eta;
    if (eta < 1)
        return 0.45966f - 1.73965f * eta + 3.37668f * eta2 - 3.904945 * eta3 +
               2.49277f * eta4 - 0.68441f * eta5;
    else
        return -4.61686f + 11.1136f * eta - 10.4646f * eta2 + 5.11455f * eta3 -
               1.27198f * eta4 + 0.12746f * eta5;
}

Float FresnelMoment2(Float eta) {
    Float eta2 = eta * eta, eta3 = eta2 * eta, eta4 = eta3 * eta, eta5 = eta4 * eta;
    if (eta < 1) {
        return 0.27614f - 0.87350f * eta + 1.12077f * eta2 - 0.65095f * eta3 +
               0.07883f * eta4 + 0.04860f * eta5;
    } else {
        Float r_eta = 1 / eta, r_eta2 = r_eta * r_eta, r_eta3 = r_eta2 * r_eta;
        return -547.033f + 45.3087f * r_eta3 - 218.725f * r_eta2 + 458.843f * r_eta +
               404.557f * eta - 189.519f * eta2 + 54.9327f * eta3 - 9.00603f * eta4 +
               0.63942f * eta5;
    }
}

std::string TrowbridgeReitzDistribution::ToString() const {
    return StringPrintf("[ TrowbridgeReitzDistribution alpha_x: %f alpha_y: %f ]",
                        alpha_x, alpha_y);
}

// Bagher distribution methods definition
Float BagherDistribution::D(Float cosTheta) const {
    if (cosTheta <= 0) return 0;

    Float tanTheta2 = (1 - cosTheta * cosTheta) / (cosTheta * cosTheta);
    Float cos4Theta = cosTheta * cosTheta * cosTheta * cosTheta;

    Float alphaTerm = alpha * alpha + tanTheta2;
    Float expTerm = std::exp(-(alphaTerm / alpha));
    Float powerTerm = std::pow(alphaTerm, p);

    return (InvPi / cos4Theta) * (Kap * expTerm / powerTerm);
}

Float BagherDistribution::Lambda(Float cosTheta) const {
    if (cosTheta <= 0) return 0;

    Float theta = std::acos(std::clamp(cosTheta, Float(0), Float(1)));

    Float tanTheta = std::sqrt((1 - cosTheta * cosTheta) / (cosTheta * cosTheta));
    Float a = 1 / (alpha * tanTheta);

    // Aproximação similar à usada para outras distribuições microfacetas
    if (a < 1.6f) {
        return (1 - 1.259f * a + 0.396f * a * a) / (3.535f * a + 2.181f * a * a);
    } else {
        return 0;
    }
}

Vector3f BagherDistribution::Sample_wh(Point2f u) const {
    // Como não temos forma fechada para o inverso da CDF,
    // usamos importance sampling baseado em GGX como aproximação
    Float cosTheta = std::sqrt((1 - u[0]) / (1 + (alpha * alpha - 1) * u[0]));
    Float sinTheta = std::sqrt(std::max(Float(0), 1 - cosTheta * cosTheta));
    Float phi = 2 * Pi * u[1];

    return SphericalDirection(sinTheta, cosTheta, phi);
}

Float BagherDistribution::Pdf(Vector3f wh) const {
    return D(wh) * AbsCosTheta(wh);
}

std::string BagherDistribution::ToString() const {
    return StringPrintf(
      "[ BagherDistribution alpha: %f p: %f Kap: %f ]",
      alpha,
      p,
      Kap
    );
}
// Bagher distribution methods definition end
}  // namespace pbrt
