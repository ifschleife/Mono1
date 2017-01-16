

#include "Renderer.h"
#include "IEntity.h"
#include "ICamera.h"
#include "IWindow.h"
#include "Math/Vector.h"
#include "Math/Quad.h"
#include "Math/Matrix.h"
#include "Math/MathFunctions.h"
#include "RenderUtils.h"
#include "Text/TextFunctions.h"

#include "Shader/IShaderFactory.h"
#include "Shader/ShaderFunctions.h"
#include "Shader/ITextureShader.h"
#include "Shader/IColorShader.h"
#include "Shader/IMorphingShader.h"
#include "Shader/IPointSpriteShader.h"

#include "Sprite/ISprite.h"
#include "Texture/ITexture.h"


using namespace mono;

Renderer::Renderer(ICameraPtr camera, IWindowPtr window)
    : mCamera(camera),
      mWindow(window),
      m_currentShaderId(-1),
      m_currentTextureId(-1)
{
    mColorShader = GetShaderFactory()->CreateColorShader();
    mTextureShader = GetShaderFactory()->CreateTextureShader();
    m_morphShader = GetShaderFactory()->CreateMorphingShader();
    m_pointSpriteShader = GetShaderFactory()->CreatePointSpriteShader();
}

void Renderer::PrepareDraw()
{
    const math::Quad& viewport = mCamera->GetViewport();
    mProjectionMatrix = math::Ortho(0.0f, viewport.mB.x, 0.0f, viewport.mB.y, -10.0f, 10.0f);

    math::Identity(mModelView);
    math::Translate(mModelView, -viewport.mA);

    mWindow->MakeCurrent();
}

void Renderer::EndDraw()
{
    mWindow->SwapBuffers();

    // Clear all the stuff once the frame has been drawn
    mDrawables.clear();
    mUpdatables.clear();
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

void Renderer::Update(unsigned int milliseconds)
{
    mCamera->Update(milliseconds);
    
    for(auto& updatable : mUpdatables)
        updatable->doUpdate(milliseconds);
}

void Renderer::AddDrawable(const IDrawablePtr& drawable)
{
    mDrawables.push_back(drawable);
}

void Renderer::AddUpdatable(const IUpdatablePtr& updatable)
{
    mUpdatables.push_back(updatable);
}

void Renderer::DrawText(int font_id, const char* text, const math::Vector& pos, bool center, const mono::Color::RGBA& color)
{
    const mono::ITexturePtr& texture = mono::GetFontTexture(font_id);
    if(!texture)
        return;

    TextDefinition def = mono::GenerateVertexDataFromString(font_id, text, pos, center);
    def.color = color;

    UseTexture(texture);
    UseShader(mTextureShader);

    ::DrawText(def, mTextureShader);
}

void Renderer::DrawSprite(const ISprite& sprite) const
{
    UseTexture(sprite.GetTexture());
    UseShader(mTextureShader);
    ::DrawSprite(sprite, mTextureShader);
}

void Renderer::DrawPoints(const std::vector<math::Vector>& points, const mono::Color::RGBA& color, float size) const
{
    UseShader(mColorShader);
    ::DrawPoints(points, color, size, mColorShader);
}

void Renderer::DrawLines(const std::vector<math::Vector>& linePoints, const mono::Color::RGBA& color, float width) const
{
    UseShader(mColorShader);
    ::DrawLines(linePoints, color, width, mColorShader);
}

void Renderer::DrawPolyline(const std::vector<math::Vector>& linePoints, const mono::Color::RGBA& color, float width) const
{
    UseShader(mColorShader);
    ::DrawLine(linePoints, color, width, mColorShader);
}

void Renderer::DrawClosedPolyline(const std::vector<math::Vector>& linePoints, const mono::Color::RGBA& color, float width) const
{
    UseShader(mColorShader);
    ::DrawClosedLine(linePoints, color, width, mColorShader);
}

void Renderer::DrawQuad(const math::Quad& quad, const mono::Color::RGBA& color, float width) const
{
    UseShader(mColorShader);
    ::DrawQuad(quad, color, width, mColorShader);
}

void Renderer::DrawCircle(const math::Vector& pos, float radie, int segments, float lineWidth, const mono::Color::RGBA& color) const
{
    UseShader(mColorShader);
    ::DrawCircle(pos, radie, segments, lineWidth, color, mColorShader);
}

void Renderer::DrawShape(const std::vector<math::Vector>& shape1, const std::vector<math::Vector>& shape2, float morphGrade, const mono::Color::RGBA& color)
{
    UseShader(m_morphShader);

    m_morphShader->SetMorphGrade(morphGrade);
    ::DrawShape(shape1, shape2, color, m_morphShader);
}

void Renderer::DrawGeometry(const std::vector<math::Vector>& vertices, const std::vector<math::Vector>& texture_coordinates, const std::vector<unsigned short>& indices, const ITexturePtr& texture)
{
    UseTexture(texture);
    UseShader(mTextureShader);

    ::DrawTexturedGeometry(vertices, texture_coordinates, indices, mTextureShader);
}

void Renderer::DrawPoints(  const std::vector<math::Vector>& points,
                            const std::vector<mono::Color::RGBA>& colors,
                            size_t count,
                            float point_size,
                            const ITexturePtr& texture) const
{
    UseTexture(texture);
    UseShader(m_pointSpriteShader);
    m_pointSpriteShader->SetPointSize(point_size);

    ::DrawParticlePoints(points, colors, count, m_pointSpriteShader);
}

void Renderer::UseShader(const IShaderPtr& shader) const
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

