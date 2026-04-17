


#include <atomic>
#include <chrono>
#include <cmath>
#include <functional>
#include <vector>
#include <cstdint>

struct Timeline {
    Timeline(uint64_t duration = 0, uint64_t fps = 60, bool looping = false) : duration_v(duration), fps(fps), looping(looping) { keys.push_back({duration,1});} 
    
    Timeline(const Timeline&) = delete;
    Timeline& operator=(const Timeline&) = delete;

    Timeline(Timeline&&) = default;
    Timeline& operator=(Timeline&&) = default;
    
    uint64_t duration_v; // ms
    
    uint64_t current = 0; // ms
    
    uint64_t fps;
    
    bool looping;

    uint64_t offset = 0; // ms

    void duration(uint64_t val) { 
        
        auto &lk = keys.back();
        if (lk.value == 1 && lk.position == duration_v)
            lk.position = val;

        duration_v = val; 
        
    }
    
    struct Key {
        
        uint64_t position; // ms
        
        float value; // 0 to 1

        enum Type { LINEAR, SINUS, TRIANGLE, SQUARE };
        
        Type type = LINEAR;
        
        std::function<void()> cb = nullptr;
        
        bool triggered = false; 

    };

    std::vector<Key> keys = {{0,0}};

    void reset() {
        current = 0;
        for (auto &k : keys)
            k.triggered = false;
    }

    std::function<void(Timeline&)> cb = nullptr;

    void resetTriggers() {

        for (auto &k : keys)
            k.triggered = false;

    }

    void run(uint64_t deltaMs) {

        if (duration_v == 0) return;

        auto prev = current;
        current += deltaMs;

        bool didLoop = looping && current >= duration_v;
        if (didLoop)
        {
            current %= duration_v;
            resetTriggers();
        }
        else if (!looping && current > duration_v)
        {
            current = duration_v;
        }

        if (cb) cb(*this);

        for (auto &k : keys)
        {
            if (!k.triggered && prev < k.position && current >= k.position)
            {
                k.triggered = true;
                if (k.cb) k.cb();
            }
        }
    }

    bool finished() const {
        return current >= duration_v;
    }

    float ease(float t, Key::Type type) const
    {
        switch (type)
        {
            case Key::LINEAR:
                return t;

            case Key::SINUS:
            
            return 0.5f - 0.5f * std::cos(t * 3.1415926f);
            case Key::TRIANGLE:
                return (t < 0.5f) ? (2.0f * t) : (2.0f * (1.0f - t));

            case Key::SQUARE:
                return (t < 0.5f) ? 0.0f : 1.0f;
        }
        return t;
    }

    float sample(uint64_t timeMs) const
    {
        if (keys.empty())
            return 0.0f;

        if (timeMs <= keys.front().position)
            return keys.front().value;

        if (timeMs >= keys.back().position)
            return keys.back().value;

        for (size_t i = 0; i < keys.size() - 1; i++)
        {
            const Key &k0 = keys[i];
            const Key &k1 = keys[i + 1];

            if (timeMs >= k0.position && timeMs <= k1.position)
            {
                float t = float(timeMs - k0.position) /
                        float(k1.position - k0.position);

                t = ease(t, k0.type);

                return k0.value + (k1.value - k0.value) * t;
            }
        }

        return 0.0f;
    }

    float value() const
    {
        return sample(current);
    }

    void sortKeys()
    {
        std::sort(keys.begin(), keys.end(),
                [](const Key &a, const Key &b)
                {
                    return a.position < b.position;
                });
    }
};

std::vector<Timeline>& getTimelines();