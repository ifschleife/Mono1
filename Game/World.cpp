//
//  World.cpp
//  MonoiOS
//
//  Created by Niklas Damberg on 26/07/16.
//
//

#include "World.h"
#include "PhysicsEntityBase.h"

#include "CMIBody.h"
#include "CMIShape.h"
#include "CMFactory.h"

#include "IRenderer.h"
#include "IPhysicsZone.h"
#include "RenderLayers.h"

#include "Color.h"
#include "Quad.h"

namespace
{
    class TerrainBlock : public mono::PhysicsEntityBase
    {
    public:

        TerrainBlock(const world::PolygonData& polygon)
            : m_vertices(polygon.vertices)
        {
            mPhysicsObject.body = mono::PhysicsFactory::CreateStaticBody();
            mPhysicsObject.shapes.push_back(mono::PhysicsFactory::CreateShape(mPhysicsObject.body, polygon.vertices, math::zeroVec));
        }

        virtual void Draw(mono::IRenderer& renderer) const
        {
            constexpr mono::Color::RGBA color(1.0f, 0.0f, 1.0f, 1.0f);
            renderer.DrawClosedPolyline(m_vertices, color, 2.0f);
        }

        virtual math::Quad BoundingBox() const
        {
            return math::Quad(-math::INF, -math::INF, math::INF, math::INF);
        }

        virtual void Update(unsigned int delta)
        { }

        const std::vector<math::Vector2f> m_vertices;
    };
}

void game::LoadWorld(mono::IPhysicsZone* zone, const std::vector<world::PolygonData>& polygons)
{
    for(const world::PolygonData& polygon : polygons)
    {
        auto terrain = std::make_shared<TerrainBlock>(polygon);
        zone->AddPhysicsEntity(terrain, BACKGROUND);
    }
}
