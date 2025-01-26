#include "CoffeeEngine/Renderer/TextRenderer.h"
#include <glad/glad.h>
#include <iostream>

TextRenderer::~TextRenderer()
{
    // Liberar recursos de FreeType
    if (m_Face)
    {
        FT_Done_Face(m_Face);
        m_Face = nullptr;
    }

    if (m_FTLibrary)
    {
        FT_Done_FreeType(m_FTLibrary);
        m_FTLibrary = nullptr;
    }

    // Liberar recursos de OpenGL
    if (m_VBO)
    {
        glDeleteBuffers(1, &m_VBO);
        m_VBO = 0;
    }

    if (m_VAO)
    {
        glDeleteVertexArrays(1, &m_VAO);
        m_VAO = 0;
    }

    // Liberar texturas de caracteres
    for (auto& pair : m_Characters)
    {
        glDeleteTextures(1, &pair.second.textureID);
    }
}

void TextRenderer::Init(const std::string& fontPath)
{
    // Inicializar FreeType
    if (FT_Init_FreeType(&m_FTLibrary))
    {
        std::cerr << "ERROR: Could not init FreeType Library" << std::endl;
        return;
    }

    // Cargar la fuente
    if (FT_New_Face(m_FTLibrary, fontPath.c_str(), 0, &m_Face))
    {
        std::cerr << "ERROR: Failed to load font at " << fontPath << std::endl;
        FT_Done_FreeType(m_FTLibrary); // Limpia la librería si falló
        m_FTLibrary = nullptr;
        return;
    }

    // Configurar tamaño de los píxeles
    if (FT_Set_Pixel_Sizes(m_Face, 0, 48))
    {
        std::cerr << "ERROR: Failed to set pixel size for font" << std::endl;
        FT_Done_Face(m_Face);
        FT_Done_FreeType(m_FTLibrary);
        m_FTLibrary = nullptr;
        m_Face = nullptr;
        return;
    }

    // Inicializar VAO y VBO para renderizado de texto
    glGenVertexArrays(1, &m_VAO);
    glGenBuffers(1, &m_VBO);
    glBindVertexArray(m_VAO);
    glBindBuffer(GL_ARRAY_BUFFER, m_VBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(float) * 6 * 4, nullptr, GL_DYNAMIC_DRAW);
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 4, GL_FLOAT, GL_FALSE, 4 * sizeof(float), (void*)0);
    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glBindVertexArray(0);

    // Crear el shader para texto
    m_Shader = Coffee::CreateRef<Coffee::Shader>("TextShader", "assets/shaders/text.glsl");

    // Pre-cargar los caracteres
    PreloadCharacters();
}

void TextRenderer::RenderText(const std::string& text, const glm::vec2& position, float scale, const glm::vec4& color)
{
    if (!m_Shader)
    {
        std::cerr << "ERROR: Shader not initialized" << std::endl;
        return;
    }

    m_Shader->Bind();
    m_Shader->setVec4("textColor", color);
    glActiveTexture(GL_TEXTURE0);
    glBindVertexArray(m_VAO);

    float x = position.x;
    float y = position.y;

    for (char c : text)
    {
        if (m_Characters.find(c) == m_Characters.end())
            continue;

        Character ch = m_Characters[c];
        float xpos = x + ch.bearing.x * scale;
        float ypos = y - (ch.size.y - ch.bearing.y) * scale;

        float w = ch.size.x * scale;
        float h = ch.size.y * scale;

        float vertices[6][4] = {{xpos, ypos + h, 0.0f, 0.0f}, {xpos, ypos, 0.0f, 1.0f},
                                {xpos + w, ypos, 1.0f, 1.0f}, {xpos, ypos + h, 0.0f, 0.0f},
                                {xpos + w, ypos, 1.0f, 1.0f}, {xpos + w, ypos + h, 1.0f, 0.0f}};

        glBindTexture(GL_TEXTURE_2D, ch.textureID);
        glBindBuffer(GL_ARRAY_BUFFER, m_VBO);
        glBufferSubData(GL_ARRAY_BUFFER, 0, sizeof(vertices), vertices);
        glDrawArrays(GL_TRIANGLES, 0, 6);

        x += (ch.advance >> 6) * scale; // Avance en 1/64 de píxel
    }

    glBindVertexArray(0);
    glBindTexture(GL_TEXTURE_2D, 0);
}

void TextRenderer::PreloadCharacters()
{
    glPixelStorei(GL_UNPACK_ALIGNMENT, 1); // Asegurar alineación de 1 byte

    for (unsigned char c = 0; c < 128; c++)
    {
        // Cargar el carácter usando FreeType
        if (FT_Load_Char(m_Face, c, FT_LOAD_RENDER))
        {
            std::cerr << "ERROR: Failed to load Glyph " << c << std::endl;
            continue;
        }

        unsigned int texture;
        glGenTextures(1, &texture);
        glBindTexture(GL_TEXTURE_2D, texture);
        glTexImage2D(GL_TEXTURE_2D, 0, GL_RED, m_Face->glyph->bitmap.width, m_Face->glyph->bitmap.rows, 0, GL_RED,
                     GL_UNSIGNED_BYTE, m_Face->glyph->bitmap.buffer);

        // Configurar parámetros de la textura
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

        // Almacenar los datos del carácter
        Character character = {texture, glm::ivec2(m_Face->glyph->bitmap.width, m_Face->glyph->bitmap.rows),
                               glm::ivec2(m_Face->glyph->bitmap_left, m_Face->glyph->bitmap_top),
                               static_cast<unsigned int>(m_Face->glyph->advance.x)};
        m_Characters[c] = character;
    }

    glBindTexture(GL_TEXTURE_2D, 0);
}
