
#include "RenderUtils.h"
#include "IRenderBuffer.h"
#include "Shader/IColorShader.h"
#include "Shader/ITextureShader.h"
#include "Shader/IMorphingShader.h"
#include "Shader/IPointSpriteShader.h"
#include "Sprite/ISprite.h"
#include "Text/TextDefinition.h"
#include "Texture/ITexture.h"

#include "Math/MathFunctions.h"
#include "Math/Quad.h"

#include "System/OpenGL.h"

#include <cmath>
#include <cassert>
#include <cstdio>

void mono::DrawQuad(const math::Quad& quad,
                    const mono::Color::RGBA& color,
                    float width,
                    IColorShader* shader)
{
    const std::vector<math::Vector> vertices = {
        math::Vector(quad.mA.x, quad.mA.y),
        math::Vector(quad.mB.x, quad.mA.y),
        math::Vector(quad.mB.x, quad.mB.y),
        math::Vector(quad.mA.x, quad.mB.y)
    };

    DrawClosedLine(vertices, color, width, shader);
}

void mono::DrawFilledQuad(const math::Quad& quad, const mono::Color::RGBA& color, IColorShader* shader)
{
    const float vertices[] = {
        quad.mA.x, quad.mA.y,
        quad.mB.x, quad.mA.y,
        quad.mB.x, quad.mB.y,
        quad.mA.x, quad.mB.y
    };

    const float colors[] = {
        color.red, color.green, color.blue, color.alpha,        
        color.red, color.green, color.blue, color.alpha,        
        color.red, color.green, color.blue, color.alpha,        
        color.red, color.green, color.blue, color.alpha
    };

    glEnableVertexAttribArray(0);
    glEnableVertexAttribArray(1);

    glVertexAttribPointer(shader->GetPositionAttributeLocation(), 2, GL_FLOAT, GL_FALSE, 0, vertices);
    glVertexAttribPointer(shader->GetColorAttributeLocation(), 4, GL_FLOAT, GL_FALSE, 0, colors);

    glDrawArrays(GL_QUADS, 0, 4);

    glDisableVertexAttribArray(0);
    glDisableVertexAttribArray(1);
}

void mono::DrawCircle(const math::Vector& position,
                      float radie,
                      unsigned int segments,
                      float lineWidth,
                      const mono::Color::RGBA& color,
                      IColorShader* shader)
{
    std::vector<math::Vector> vertices;
    vertices.reserve(segments);

    const float coef = 2.0f * math::PI() / float(segments);
    
	for(unsigned int index = 0; index < segments; ++index)
	{
		const float radians = index * coef;
		const float x = radie * std::cos(radians) + position.x;
		const float y = radie * std::sin(radians) + position.y;

        vertices.emplace_back(x, y);
	}

    DrawClosedLine(vertices, color, lineWidth, shader);
}

void mono::DrawSprite(const mono::ISprite& sprite, ITextureShader* shader)
{
    // The sprite can return zeroQuad as texture coordinates when the animation is finished
    const math::Quad& quad = sprite.GetTextureCoords();
    if(quad == math::zeroQuad)
        return;

    constexpr float vertices[] = { -0.5f, -0.5f,
                                   -0.5f,  0.5f,
                                    0.5f,  0.5f,
                                    0.5f, -0.5f };
    constexpr unsigned short indices[] = { 0, 1, 2, 0, 2, 3 };

    const float coords[] = { quad.mA.x, quad.mA.y,
                             quad.mA.x, quad.mB.y,
                             quad.mB.x, quad.mB.y,
                             quad.mB.x, quad.mA.y };
    
    shader->SetShade(sprite.GetShade());
    shader->SetAlphaTexture(false);

    glEnableVertexAttribArray(0);
    glEnableVertexAttribArray(1);

    glVertexAttribPointer(shader->GetPositionAttributeLocation(), 2, GL_FLOAT, GL_FALSE, 0, vertices);
    glVertexAttribPointer(shader->GetTextureAttributeLocation(), 2, GL_FLOAT, GL_FALSE, 0, coords);

    glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_SHORT, indices);

    glDisableVertexAttribArray(0);
    glDisableVertexAttribArray(1);
}

void mono::DrawText(const TextDefinition& text, ITextureShader* shader)
{
    shader->SetAlphaTexture(true);

    glEnableVertexAttribArray(0);
    glEnableVertexAttribArray(1);

    shader->SetShade(text.color);

    glVertexAttribPointer(shader->GetPositionAttributeLocation(), 2, GL_FLOAT, GL_FALSE, 0, text.vertices.data());
    glVertexAttribPointer(shader->GetTextureAttributeLocation(), 2, GL_FLOAT, GL_FALSE, 0, text.texcoords.data());

    // Number of chars in the text, times 3 since each triangle contains 3 vertices,
    // times 2 since each char containts two triangles.
    const int verticesToDraw = (int)text.chars * 3 * 2;

    glDrawArrays(GL_TRIANGLES, 0, verticesToDraw);

    glDisableVertexAttribArray(0);
    glDisableVertexAttribArray(1);
}

void mono::DrawLine(const std::vector<math::Vector>& vertices,
                    const mono::Color::RGBA& color,
                    float width,
                    IColorShader* shader)
{
    std::vector<mono::Color::RGBA> colors(vertices.size());
    std::fill(colors.begin(), colors.end(), color);

    glLineWidth(width);

    glEnableVertexAttribArray(0);
    glEnableVertexAttribArray(1);

    glVertexAttribPointer(shader->GetPositionAttributeLocation(), 2, GL_FLOAT, GL_FALSE, 0, vertices.data());
    glVertexAttribPointer(shader->GetColorAttributeLocation(), 4, GL_FLOAT, GL_FALSE, 0, colors.data());

    glDrawArrays(GL_LINE_STRIP, 0, static_cast<int>(vertices.size()));

    glDisableVertexAttribArray(0);
    glDisableVertexAttribArray(1);
}

void mono::DrawClosedLine(const std::vector<math::Vector>& vertices,
                          const mono::Color::RGBA& color,
                          float width,
                          IColorShader* shader)
{
    std::vector<mono::Color::RGBA> colors(vertices.size());
    std::fill(colors.begin(), colors.end(), color);

    glLineWidth(width);

    glEnableVertexAttribArray(0);
    glEnableVertexAttribArray(1);

    glVertexAttribPointer(shader->GetPositionAttributeLocation(), 2, GL_FLOAT, GL_FALSE, 0, vertices.data());
    glVertexAttribPointer(shader->GetColorAttributeLocation(), 4, GL_FLOAT, GL_FALSE, 0, colors.data());

    glDrawArrays(GL_LINE_LOOP, 0, static_cast<int>(vertices.size()));

    glDisableVertexAttribArray(0);
    glDisableVertexAttribArray(1);
}

void mono::DrawLines(const std::vector<math::Vector>& vertices,
                     const mono::Color::RGBA& color,
                     float width,
                     IColorShader* shader)
{
    std::vector<mono::Color::RGBA> colors(vertices.size());
    std::fill(colors.begin(), colors.end(), color);

    glLineWidth(width);

    glEnableVertexAttribArray(0);
    glEnableVertexAttribArray(1);

    glVertexAttribPointer(shader->GetPositionAttributeLocation(), 2, GL_FLOAT, GL_FALSE, 0, vertices.data());
    glVertexAttribPointer(shader->GetColorAttributeLocation(), 4, GL_FLOAT, GL_FALSE, 0, colors.data());

    glDrawArrays(GL_LINES, 0, static_cast<int>(vertices.size()));

    glDisableVertexAttribArray(0);
    glDisableVertexAttribArray(1);
}

void mono::DrawPoints(const std::vector<math::Vector>& vertices,
                      const mono::Color::RGBA& color,
                      float size,
                      IColorShader* shader)
{
    std::vector<mono::Color::RGBA> colors(vertices.size());
    std::fill(colors.begin(), colors.end(), color);

    shader->SetPointSize(size);

    glEnableVertexAttribArray(0);
    glEnableVertexAttribArray(1);

    glVertexAttribPointer(shader->GetPositionAttributeLocation(), 2, GL_FLOAT, GL_FALSE, 0, vertices.data());
    glVertexAttribPointer(shader->GetColorAttributeLocation(), 4, GL_FLOAT, GL_FALSE, 0, colors.data());

    glDrawArrays(GL_POINTS, 0, static_cast<GLsizei>(vertices.size()));

    glDisableVertexAttribArray(0);
    glDisableVertexAttribArray(1);
}

void mono::DrawShape(const std::vector<math::Vector>& shape1,
                     const std::vector<math::Vector>& shape2,
                     const mono::Color::RGBA& color,
                     IMorphingShader* shader)
{
    if(shape1.size() != shape2.size())
        return;

    std::vector<mono::Color::RGBA> colors(shape1.size());
    std::fill(colors.begin(), colors.end(), color);

    glEnableVertexAttribArray(0);
    glEnableVertexAttribArray(1);
    glEnableVertexAttribArray(2);

    glVertexAttribPointer(shader->GetPositionLocation(), 2, GL_FLOAT, GL_FALSE, 0, shape1.data());
    glVertexAttribPointer(shader->GetMorphPositionLocation(), 2, GL_FLOAT, GL_FALSE, 0, shape2.data());
    glVertexAttribPointer(shader->GetColorLocation(), 4, GL_FLOAT, GL_FALSE, 0, colors.data());

    glDrawArrays(GL_LINE_STRIP, 0, static_cast<GLsizei>(shape1.size()));

    glDisableVertexAttribArray(0);
    glDisableVertexAttribArray(1);
    glDisableVertexAttribArray(2);
}

void mono::DrawTexturedGeometry(const std::vector<math::Vector>& vertices,
                                const std::vector<math::Vector>& texture_coordinates,
                                const std::vector<unsigned short>& indices,
                                ITextureShader* shader)
{
    shader->SetShade(mono::Color::RGBA());
    shader->SetAlphaTexture(false);

    glEnableVertexAttribArray(0);
    glEnableVertexAttribArray(1);

    glVertexAttribPointer(shader->GetPositionAttributeLocation(), 2, GL_FLOAT, GL_FALSE, 0, vertices.data());
    glVertexAttribPointer(shader->GetTextureAttributeLocation(), 2, GL_FLOAT, GL_FALSE, 0, texture_coordinates.data());

    glDrawElements(GL_TRIANGLE_FAN, static_cast<GLsizei>(indices.size()), GL_UNSIGNED_SHORT, indices.data());

    glDisableVertexAttribArray(0);
    glDisableVertexAttribArray(1);
}

void mono::DrawTexturedGeometry(const mono::IRenderBuffer* vertices,
                                const mono::IRenderBuffer* texture_coordinates,
                                size_t offset,
                                size_t count,
                                ITextureShader* shader)
{
    shader->SetShade(mono::Color::RGBA());
    shader->SetAlphaTexture(false);

    glEnableVertexAttribArray(0);
    glEnableVertexAttribArray(1);

    vertices->Use();
    glVertexAttribPointer(shader->GetPositionAttributeLocation(), 2, GL_FLOAT, GL_FALSE, 0, 0);

    texture_coordinates->Use();
    glVertexAttribPointer(shader->GetTextureAttributeLocation(), 2, GL_FLOAT, GL_FALSE, 0, 0);

    glDrawArrays(GL_TRIANGLE_FAN, offset, count);

    glDisableVertexAttribArray(0);
    glDisableVertexAttribArray(1);

    ClearRenderBuffer();
}

void mono::DrawParticlePoints(const IRenderBuffer* position,
                              const IRenderBuffer* color,
                              size_t count,
                              IPointSpriteShader* shader)
{
    glEnable(GL_POINT_SPRITE);
    glDepthMask(GL_FALSE);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE);

    glEnableVertexAttribArray(0);
    glEnableVertexAttribArray(1);

    position->Use();
    glVertexAttribPointer(shader->GetPositionAttributeLocation(), 2, GL_FLOAT, GL_FALSE, 0, 0);

    color->Use();
    glVertexAttribPointer(shader->GetColorAttributeLocation(), 4, GL_FLOAT, GL_FALSE, 0, 0);

    glDrawArrays(GL_POINTS, 0, count);

    glDisableVertexAttribArray(0);
    glDisableVertexAttribArray(1);

    glDisable(GL_POINT_SPRITE);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

    ClearRenderBuffer();
}


void mono::DrawPolyline(
    const mono::IRenderBuffer* vertices,
    const mono::IRenderBuffer* colors,
    size_t offset,
    size_t count,
    float width,
    IColorShader* shader)
{
    glLineWidth(width);
    
    glEnableVertexAttribArray(0);
    glEnableVertexAttribArray(1);

    vertices->Use();
    glVertexAttribPointer(shader->GetPositionAttributeLocation(), 2, GL_FLOAT, GL_FALSE, 0, 0);

    colors->Use();
    glVertexAttribPointer(shader->GetColorAttributeLocation(), 2, GL_FLOAT, GL_FALSE, 0, 0);

    glDrawArrays(GL_LINE_LOOP, offset, count);

    glDisableVertexAttribArray(0);
    glDisableVertexAttribArray(1);

    ClearRenderBuffer();
}
