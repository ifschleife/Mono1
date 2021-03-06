
#include "TextureShader.h"
#include "System/OpenGL.h"
#include "Math/Matrix.h"
#include "Rendering/Color.h"

#include "ShaderFunctions.h"


namespace
{
    constexpr const char* vertexSource =

#ifdef __IPHONEOS__
    "precision mediump float;"
#endif

    "attribute vec2 vertexPosition;"
    "attribute vec2 textureCoord;"

    "uniform mat4 mv_matrix;"
    "uniform mat4 p_matrix;"

    "varying vec2 vTextureCoord;"

    "void main()"
    "{"
    "    gl_Position = p_matrix * mv_matrix * vec4(vertexPosition, 0.0, 1.0);"
    "    vTextureCoord = textureCoord;"
    "}";

    constexpr const char* fragmentSource =

#ifdef __IPHONEOS__
    "precision mediump float;"
#endif

    "varying vec2 vTextureCoord;"

    "uniform vec4 colorShade;"
    "uniform sampler2D sampler;"
    "uniform bool isAlphaTexture;"

    "void main()"
    "{"
    "    if(isAlphaTexture)"
    "        gl_FragColor = texture2D(sampler, vTextureCoord).aaaa * colorShade;"
    "    else"
    "        gl_FragColor = texture2D(sampler, vTextureCoord) * colorShade;"
    "}";
}


using namespace mono;

TextureShader::TextureShader()
{
    const GLuint vertexShader = CompileShader(mono::ShaderType::VERTEX, vertexSource);
    const GLuint fragmentShader = CompileShader(mono::ShaderType::FRAGMENT, fragmentSource);
    
    mProgram = LinkProgram(vertexShader, fragmentShader);

    mMVMatrixLocation = glGetUniformLocation(mProgram, "mv_matrix");
    mPMatrixLocation = glGetUniformLocation(mProgram, "p_matrix");

    mSamplerLocation = glGetUniformLocation(mProgram, "sampler");
    mColorShadeLocation = glGetUniformLocation(mProgram, "colorShade");
    mIsAlphaTextureLocation = glGetUniformLocation(mProgram, "isAlphaTexture");

    mPositionAttributeLocation = (unsigned int)glGetAttribLocation(mProgram, "vertexPosition");
    mTextureAttributeLocation = (unsigned int)glGetAttribLocation(mProgram, "textureCoord");
}

TextureShader::~TextureShader()
{
    glDeleteProgram(mProgram);
}

void TextureShader::Use()
{
    glUseProgram(mProgram);
}

unsigned int TextureShader::GetShaderId() const
{
    return mProgram;
}

void TextureShader::LoadProjectionMatrix(const math::Matrix& projection)
{
    glUniformMatrix4fv(mPMatrixLocation, 1, GL_FALSE, projection.data);
}

void TextureShader::LoadModelViewMatrix(const math::Matrix& modelView)
{
    glUniformMatrix4fv(mMVMatrixLocation, 1, GL_FALSE, modelView.data);
}

unsigned int TextureShader::GetPositionAttributeLocation() const
{
    return mPositionAttributeLocation;
}

unsigned int TextureShader::GetTextureAttributeLocation() const
{
    return mTextureAttributeLocation;
}

void TextureShader::SetShade(const mono::Color::RGBA& shade)
{
    glUniform4f(mColorShadeLocation, shade.red, shade.green, shade.blue, shade.alpha);
}

void TextureShader::SetAlphaTexture(bool isAlpha)
{
    glUniform1i(mIsAlphaTextureLocation, isAlpha);
}
