#pragma once
#include <ft2build.h>
#include <glm/glm.hpp>
#include <map>
#include <string>
#include FT_FREETYPE_H
#include "CoffeeEngine/Core/Base.h"
#include "CoffeeEngine/Renderer/Buffer.h"
#include <Math.h>
#include "CoffeeEngine/Renderer/Shader.h"
#include "CoffeeEngine/Renderer/VertexArray.h"
#include "CoffeeEngine/Scene/Components.h"

struct Character
{
    unsigned int textureID;
    glm::ivec2 size;
    glm::ivec2 bearing;
    unsigned int advance;
};

class TextRenderer
{
  public:
    ~TextRenderer();
    void Init(const std::string& fontPath = "assets/fonts/OpenSans-Regular.ttf");
    void RenderText(const std::string& text, const glm::vec2& position, float scale, const glm::vec4& color);

  private:
    void PreloadCharacters();

    Coffee::Ref<Coffee::Shader> m_Shader;
    std::map<char, Character> m_Characters;
    unsigned int m_VAO = 0, m_VBO = 0;
    FT_Library m_FTLibrary = nullptr;
    FT_Face m_Face = nullptr;
    // Añadir método para obtener el shader
};