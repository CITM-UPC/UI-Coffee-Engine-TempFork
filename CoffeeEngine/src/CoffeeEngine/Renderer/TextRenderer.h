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

    // Declarar Init como estático
    static void Init(const std::string& fontPath = "assets/fonts/OpenSans-SemiBold.ttf");

    static void RenderText(const std::string& text, const glm::vec2& position, float scale, const glm::vec4& color);

  private:
    static void PreloadCharacters();

    // Convertir miembros no estáticos en estáticos
    static Coffee::Ref<Coffee::Shader> m_Shader;
    static std::map<char, Character> m_Characters;
    static unsigned int m_VAO;
    static unsigned int m_VBO;
    static FT_Library m_FTLibrary;
    static FT_Face m_Face;
};