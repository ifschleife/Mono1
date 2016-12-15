//
//  MathFunctions.h
//  Mono1
//
//  Created by Niblit on 2012-07-27.
//  Copyright 2012 __MyCompanyName__. All rights reserved.
//

#pragma once

#include "MathFwd.h"
#include <vector>

namespace math
{    
    bool PointInsideQuad(const math::Vector& point, const math::Quad& quad);
    bool QuadOverlaps(const math::Quad& left, const math::Quad& right);
    void ResizeQuad(math::Quad& quad, float value, float aspect = 1.0f);

    // Return angle in radians
    float AngleBetweenPoints(const math::Vector& first, const math::Vector& second);

    // Pass in at least 3 points, else you will get "nan nan" back.
    math::Vector CentroidOfPolygon(const std::vector<math::Vector>& points);

    // Check if a polygon is clockwise oriented or not
    bool IsPolygonClockwise(const std::vector<math::Vector>& points);

    math::Vector MapVectorInQuad(const math::Vector& point, const math::Quad& quad);

    constexpr float PI()
    {
        return 3.14159265359f;
    }

    constexpr float PI_2()
    {
        return 1.57079632679f;
    }

    constexpr float ToRadians(float degrees)
    {
        return degrees * PI() / 180.0f;
    }

    constexpr float ToDegrees(float radians)
    {
        return radians * 180.0f / PI();
    }
}
