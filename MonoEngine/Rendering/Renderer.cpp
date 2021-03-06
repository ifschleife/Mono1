
#include "Renderer.h"
#include "IDrawable.h"
#include "ICamera.h"
#include "RenderUtils.h"
#include "Text/TextFunctions.h"
#include "Texture/ITexture.h"

#include "Shader/IShaderFactory.h"
#include "Shader/ShaderFunctions.h"
#include "Shader/ITextureShader.h"
#include "Shader/IColorShader.h"
#include "Shader/IMorphingShader.h"
#include "Shader/IPointSpriteShader.h"

#include "Sprite/ISprite.h"

#include "Math/Vector.h"
#include "Math/Quad.h"
#include "Math/Matrix.h"
#include "Math/MathFunctions.h"


using namespace mono;

Renderer::Renderer(ICameraPtr camera)
    : mCamera(camera)
{
    m_color_shader = GetShaderFactory()->CreateColorShader();
    m_texture_shader = GetShaderFactory()->CreateTextureShader();
    m_morph_shader = GetShaderFactory()->CreateMorphingShader();
    m_point_sprite_shader = GetShaderFactory()->CreatePointSpriteShader();
}

Renderer::~Renderer()
{ }

void Renderer::PrepareDraw()
{
    const math::Quad& viewport = mCamera->GetViewport();
    mProjectionMatrix = math::Ortho(0.0f, viewport.mB.x, 0.0f, viewport.mB.y, -10.0f, 10.0f);

    math::Identity(mModelView);
    math::Translate(mModelView, -viewport.mA);
}

void Renderer::EndDraw()
{
    // Clear all the stuff once the frame has been drawn
    mDrawables.clear();
}

void Renderer::DrawFrame()
{
    PrepareDraw();

    unsigned int draw_count = 0;

    const math::Quad& viewport = mCamera->GetViewport();
    const math::Quad camera_quad(viewport.mA, viewport.mA + viewport.mB);

    for(const auto& drawable : mDrawables)
    {
        m_current_transform = mModelView;
        m_current_projection = mProjectionMatrix;

        const math::Quad& bounds = drawable->BoundingBox();

        // Make sure the entity is visible
        const bool visible = math::QuadOverlaps(camera_quad, bounds);
        if(visible)
            drawable->doDraw(*this);

        draw_count += visible;
    }

    // Restore the matrices
    m_current_transform = mModelView;
    m_current_projection = mProjectionMatrix;

    EndDraw();
}

void Renderer::AddDrawable(const IDrawablePtr& drawable)
{
    mDrawables.push_back(drawable);
}

void Renderer::DrawText(int font_id, const char* text, const math::Vector& pos, bool center, const mono::Color::RGBA& color) const
{
    const mono::ITexturePtr& texture = mono::GetFontTexture(font_id);
    if(!texture)
        return;

    TextDefinition def = mono::GenerateVertexDataFromString(font_id, text, pos, center);
    def.color = color;

    UseTexture(texture);
    UseShader(m_texture_shader.get());

    ::DrawText(def, m_texture_shader.get());
}

void Renderer::DrawSprite(const ISprite& sprite) const
{
    UseTexture(sprite.GetTexture());
    UseShader(m_texture_shader.get());
    ::DrawSprite(sprite, m_texture_shader.get());
}

void Renderer::DrawPoints(const std::vector<math::Vector>& points, const mono::Color::RGBA& color, float size) const
{
    UseShader(m_color_shader.get());
    ::DrawPoints(points, color, size, m_color_shader.get());
}

void Renderer::DrawLines(const std::vector<math::Vector>& linePoints, const mono::Color::RGBA& color, float width) const
{
    UseShader(m_color_shader.get());
    ::DrawLines(linePoints, color, width, m_color_shader.get());
}

void Renderer::DrawPolyline(const std::vector<math::Vector>& linePoints, const mono::Color::RGBA& color, float width) const
{
    UseShader(m_color_shader.get());
    ::DrawLine(linePoints, color, width, m_color_shader.get());
}

void Renderer::DrawClosedPolyline(const std::vector<math::Vector>& linePoints, const mono::Color::RGBA& color, float width) const
{
    UseShader(m_color_shader.get());
    ::DrawClosedLine(linePoints, color, width, m_color_shader.get());
}

void Renderer::DrawQuad(const math::Quad& quad, const mono::Color::RGBA& color, float width) const
{
    UseShader(m_color_shader.get());
    ::DrawQuad(quad, color, width, m_color_shader.get());
}

void Renderer::DrawFilledQuad(const math::Quad& quad, const mono::Color::RGBA& color) const
{
    UseShader(m_color_shader.get());
    ::DrawFilledQuad(quad, color, m_color_shader.get());
}

void Renderer::DrawCircle(const math::Vector& pos, float radie, int segments, float lineWidth, const mono::Color::RGBA& color) const
{
    UseShader(m_color_shader.get());
    ::DrawCircle(pos, radie, segments, lineWidth, color, m_color_shader.get());
}

void Renderer::DrawShape(const std::vector<math::Vector>& shape1, const std::vector<math::Vector>& shape2, float morphGrade, const mono::Color::RGBA& color)
{
    UseShader(m_morph_shader.get());

    m_morph_shader->SetMorphGrade(morphGrade);
    ::DrawShape(shape1, shape2, color, m_morph_shader.get());
}

void Renderer::DrawGeometry(const std::vector<math::Vector>& vertices, const std::vector<math::Vector>& texture_coordinates, const std::vector<unsigned short>& indices, const ITexturePtr& texture)
{
    UseTexture(texture);
    UseShader(m_texture_shader.get());

    ::DrawTexturedGeometry(vertices, texture_coordinates, indices, m_texture_shader.get());
}

void Renderer::DrawGeometry(const IRenderBuffer* vertices,
                            const IRenderBuffer* texture_coordinates,
                            size_t offset,
                            size_t count,
                            const ITexturePtr& texture)
{
    UseTexture(texture);
    UseShader(m_texture_shader.get());

    ::DrawTexturedGeometry(vertices, texture_coordinates, offset, count, m_texture_shader.get());
}

void Renderer::DrawParticlePoints(const IRenderBuffer* position,
                                  const IRenderBuffer* color,
                                  float point_size,
                                  const ITexturePtr& texture,
                                  size_t count)
{
    UseTexture(texture);
    UseShader(m_point_sprite_shader.get());
    m_point_sprite_shader->SetPointSize(point_size);
    ::DrawParticlePoints(position, color, count, m_point_sprite_shader.get());
}

void Renderer::DrawPolyline(
    const IRenderBuffer* vertices, const IRenderBuffer* colors, size_t offset, size_t count)
{
    UseShader(m_color_shader.get());
    ::DrawPolyline(vertices, colors, offset, count, 2.0f, m_color_shader.get());
}

void Renderer::UseShader(IShader* shader) const
{
    const unsigned int id = shader->GetShaderId();
    if(id != m_currentShaderId)
    {
        shader->Use();
        m_currentShaderId = id;
    }

    shader->LoadModelViewMatrix(m_current_transform);
    shader->LoadProjectionMatrix(m_current_projection);
}

void Renderer::UseTexture(const ITexturePtr& texture) const
{
    const unsigned int id = texture->Id();
    if(id != m_currentTextureId)
    {
        texture->Use();
        m_currentTextureId = id;
    }
}

void Renderer::ClearTexture()
{
    m_currentTextureId = -1;
    mono::ClearTexture();
}

void Renderer::PushGlobalTransform()
{
    m_current_transform = mModelView;
}

void Renderer::PushNewTransform(const math::Matrix& transform)
{
    m_current_transform = transform;
}

const math::Matrix& Renderer::GetCurrentTransform() const
{
    return m_current_transform;
}

void Renderer::PushNewProjection(const math::Matrix& projection)
{
    m_current_projection = projection;
}

const math::Matrix& Renderer::GetCurrentProjection() const
{
    return m_current_projection;
}

