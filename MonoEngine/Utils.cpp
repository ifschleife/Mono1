
#include "Utils.h"
#include "System/SysTime.h"
#include <cmath>
#include <random>

using namespace mono;

FPSCounter::FPSCounter()
    : mTime(0),
      mFrames(0),
      mTotalFrames(0),
      mFps(0)
{ }

void FPSCounter::operator ++ (int)
{
    ++mFrames;
    ++mTotalFrames;
    
    const unsigned int currentTime = Time::GetMilliseconds();
    const unsigned int delta = currentTime - mTime;
    if(delta >= 1000)
    {
        mTime = currentTime;
        mFps = (unsigned int)std::floor(float(mFrames) / 1.0f);
        mFrames = 0;
    }    
}

unsigned int FPSCounter::Fps() const
{
    return mFps;
}

unsigned int FPSCounter::Frames() const
{
    return mTotalFrames;
}


float mono::Random()
{
    constexpr int seed = 666;
    static std::default_random_engine engine(seed);
    static std::uniform_real_distribution<float> distribution(0.0f, 1.0f);
    
    return distribution(engine);
}
