


#include <atomic>
#include <chrono>
#include <cmath>
#include <functional>

using Clock = std::chrono::steady_clock;

// ==============================
// EASING FUNCTIONS
// ==============================

namespace Easing {
    inline float linear(float t) {
        return t;
    }

    inline float easeInOut(float t) {
        return t * t * (3.0f - 2.0f * t); // smoothstep
    }

    inline float easeIn(float t) {
        return t * t;
    }

    inline float easeOut(float t) {
        return 1.0f - (1.0f - t) * (1.0f - t);
    }
}

// ==============================
// CUSTOM CURVE (interpolation)
// ==============================

class Curve {
public:
    std::vector<std::pair<float, float>> points;

    float sample(float t) const {
        if (points.empty()) return t;

        for (size_t i = 1; i < points.size(); ++i) {
            if (t <= points[i].first) {
                auto [t0, v0] = points[i - 1];
                auto [t1, v1] = points[i];

                float alpha = (t - t0) / (t1 - t0);
                return v0 + alpha * (v1 - v0);
            }
        }
        return points.back().second;
    }
};

// ==============================
// OSCILLATOR (LOCK-FREE)
// ==============================

class Oscillator {
public:
    // paramètres atomiques (lock-free)
    std::atomic<float> amplitude {50.0f};
    std::atomic<float> offset {50.0f};      // centre
    std::atomic<float> frequency {1.0f};    // cycles/sec
    std::atomic<float> phaseOffset {0.0f};  // décalage

    std::function<float(float)> easing = Easing::easeInOut;
    Curve curve;

    Oscillator() {
        startTime = Clock::now();
    }

    float getValue() const {
        auto now = Clock::now();
        float timeSec = std::chrono::duration<float>(now - startTime).count();

        float freq = frequency.load(std::memory_order_relaxed);
        float phase = phaseOffset.load(std::memory_order_relaxed);

        float t = std::fmod(timeSec * freq + phase, 1.0f);

        // appliquer easing ou courbe custom
        float shaped = curve.points.empty() ? easing(t) : curve.sample(t);

        float amp = amplitude.load(std::memory_order_relaxed);
        float off = offset.load(std::memory_order_relaxed);

        return off + (shaped * 2.0f - 1.0f) * amp;
    }

    float sample(float tGlobal) const {
        float f = frequency.load(std::memory_order_relaxed);
        float p = phaseOffset.load(std::memory_order_relaxed);

        float t = std::fmod(tGlobal * f + p, 1.0f);

        float v = 0.5f + 0.5f * std::sin(t * 2.0f * M_PI);

        float a = amplitude.load(std::memory_order_relaxed);
        float o = offset.load(std::memory_order_relaxed);

        return o + (v * 2.0f - 1.0f) * a;
    }
private:
    Clock::time_point startTime;
};