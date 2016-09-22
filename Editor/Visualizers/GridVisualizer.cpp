//
//  GridVisualizer.cpp
//  MonoiOS
//
//  Created by Niklas Damberg on 12/08/16.
//
//

#include "GridVisualizer.h"
#include "IRenderer.h"
#include "ICamera.h"
#include "Color.h"
#include "Math/Quad.h"
#include "Math/Matrix.h"

#include <vector>
#include <cstdio>

namespace
{
    std::vector<math::Vector2f> BuildGridVertices(const math::Quad& bounds)
    {
        constexpr float cellSize = 50.0f;

        const float width = bounds.mB.x - bounds.mA.x;
        const float height = bounds.mB.y - bounds.mA.y;

        const float cellsWidth = width / cellSize;
        const float cellsHeight = height / cellSize;

        std::vector<math::Vector2f> vertices;

        for(int index = 0; index < cellsWidth; ++index)
        {
            const float x = bounds.mA.x + index * cellSize;

            vertices.push_back(math::Vector2f(x, bounds.mA.y));
            vertices.push_back(math::Vector2f(x, bounds.mB.y));
        }

        for(int index = 0; index < cellsHeight; ++index)
        {
            const float y = bounds.mA.y + index * cellSize;

            vertices.push_back(math::Vector2f(bounds.mA.x, y));
            vertices.push_back(math::Vector2f(bounds.mB.x, y));
        }
        
        return vertices;
    }
}

using namespace editor;

GridVisualizer::GridVisualizer(const mono::ICameraPtr& camera)
    : m_camera(camera)
{
    m_gridVertices = BuildGridVertices(math::Quad(0, 0, 1200, 800));
}

void GridVisualizer::doDraw(mono::IRenderer& renderer) const
{
    const math::Matrix& projection = math::Ortho(0.0f, 1200, 0.0f, 800, -10.0f, 10.0f);
    constexpr math::Matrix transform;

    constexpr mono::Color::RGBA gray_color(1.0f, 1.0f, 1.0f, 0.2f);
    constexpr mono::Color::RGBA black_color(0.0f, 0.0f, 0.0f, 0.5f);

    const math::Quad& viewport = m_camera->GetViewport();
    const float scale = viewport.mB.x / 1200.0f * 200.0f;

    char text[100];
    std::sprintf(text, "%3.2f m", scale);

    constexpr math::Vector2f position(1050, 15);
    const std::vector<math::Vector2f> points = { math::Vector2f(950, 25), math::Vector2f(1150, 25) };

    renderer.PushNewProjection(projection);
    renderer.PushNewTransform(transform);

    renderer.DrawLines(m_gridVertices, gray_color, 1.0f);
    renderer.DrawLines(points, black_color, 2.0f);
    renderer.DrawText(text, position, true, black_color);
}

math::Quad GridVisualizer::BoundingBox() const
{
    return math::Quad(-math::INF, -math::INF, math::INF, math::INF);
}
