#include "TextRenderer.h"

#include "TextRenderer.h"
#include <iostream>

#include <ft2build.h>
#include FT_FREETYPE_H

TextRenderer::TextRenderer() {
    // Constructor implementation
    // Configura VAO y VBO
    glGenVertexArrays(1, &VAO);
    glGenBuffers(1, &VBO);

    glBindVertexArray(VAO);
    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(float) * 6 * 4, nullptr, GL_DYNAMIC_DRAW);

    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 4, GL_FLOAT, GL_FALSE, 4 * sizeof(float), 0);

    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glBindVertexArray(0);

    // Configura el shader
    shader = Shader("text.vs", "text.fs"); // Reemplaza con tus shaders
    }
}

//void TextRenderer::RenderText(const std::string& text, float x, float y, float scale, const glm::vec3& color)
//{
//	 Render the text
//	 Set the shader color
//	shader.SetUniform3f("textColor", color);
//	 Activate corresponding render state
//	shader.Bind();
//	glActiveTexture(GL_TEXTURE0);
//	glBindVertexArray(VAO);
//
//	 Iterate through all characters
//	std::string::const_iterator c;
//	for (c = text.begin(); c != text.end(); c++) {
//		Character ch = characters[*c];
//
//		float xpos = x + ch.bearing.x * scale;
//		float ypos = y - (ch.size.y - ch.bearing.y) * scale;
//
//		float w = ch.size.x * scale;
//		float h = ch.size.y * scale;
//		 Update VBO for each character
//		float vertices[6][4] = {
//			{ xpos,     ypos + h,   0.0, 0.0 },
//			{ xpos,     ypos,       0.0, 1.0 },
//			{ xpos + w, ypos,       1.0, 1.0 },
//
//			{ xpos,     ypos + h,   0.0, 0.0 },
//			{ xpos + w, ypos,       1.0, 1.0 },
//			{ xpos + w, ypos + h,   1.0, 0.0 }
//		};
//		 Render glyph texture over quad
//		glBindTexture(GL_TEXTURE_2D, ch.textureID);
//		 Update content of VBO memory
//		glBindBuffer(GL_ARRAY_BUFFER, VBO);
//		glBufferSubData(GL_ARRAY_BUFFER, 0, sizeof(vertices), vertices);
//		glBindBuffer(GL_ARRAY_BUFFER, 0);
//		 Render quad
//		glDrawArrays(GL_TRIANGLES, 0, 6);
//		 Now advance cursors for next glyph
//		x += (ch.advance >> 6) * scale; // Bitshift by 6 to get value in pixels (2^6 = 64)
//	}
//	glBindVertexArray(0);
//	glBindTexture(GL_TEXTURE_2D, 0);
//}

void TextRenderer::RenderText(const std::string& text, const glm::vec2& position, float scale, const glm::vec3& color)
{
    // Configura el shader y establece el color del texto
    shader.Bind();                      // Activa el shader
    shader.setVec3("textColor", color); // Establece el color del texto
    glActiveTexture(GL_TEXTURE0);
    glBindVertexArray(VAO);

    // Variables para posicionar el texto
    float x = position.x;
    float y = position.y;

    // Itera sobre cada carácter del texto
    for (const char& c : text)
    {
        // Busca el carácter en el mapa de caracteres
        if (characters.find(c) == characters.end())
        {
            continue; // Si el carácter no está en el mapa, lo omite
        }
        Character ch = characters[c];

        // Calcula la posición y las dimensiones del carácter
        float xpos = x + ch.bearing.x * scale;
        float ypos = y - (ch.size.y - ch.bearing.y) * scale;

        float w = ch.size.x * scale;
        float h = ch.size.y * scale;

        // Actualiza el VBO con los vértices del carácter
        float vertices[6][4] = {
            {xpos, ypos + h, 0.0f, 0.0f}, {xpos, ypos, 0.0f, 1.0f},     {xpos + w, ypos, 1.0f, 1.0f},

            {xpos, ypos + h, 0.0f, 0.0f}, {xpos + w, ypos, 1.0f, 1.0f}, {xpos + w, ypos + h, 1.0f, 0.0f}};

        // Renderiza la textura del carácter
        glBindTexture(GL_TEXTURE_2D, ch.textureID);

        // Actualiza el contenido del VBO
        glBindBuffer(GL_ARRAY_BUFFER, VBO);
        glBufferSubData(GL_ARRAY_BUFFER, 0, sizeof(vertices), vertices);

        // Renderiza el carácter
        glDrawArrays(GL_TRIANGLES, 0, 6);

        // Avanza la posición para el siguiente carácter
        x += (ch.advance >> 6) * scale; // Desplaza el cursor (divide por 64)
    }

    // Limpia los estados de OpenGL
    glBindVertexArray(0);
    glBindTexture(GL_TEXTURE_2D, 0);
    shader.Unbind();
}


void TextRenderer::SetFont(const std::string& fontName, int fontSize) {
    // Set the font for rendering
    this->fontName = fontName;
    this->fontSize = fontSize;
}

std::string TextRenderer::GetFont() const {
    // Return the current font settings
    return fontName + " " + std::to_string(fontSize);
}


void TextRenderer::loadFont(const std::string& fontPath)
{
    // Inicializa FreeType
    FT_Library ft;
    if (FT_Init_FreeType(&ft))
    {
        std::cerr << "ERROR::FREETYPE: Could not init FreeType Library" << std::endl;
        return;
    }

    // Carga la fuente
    FT_Face face;
    if (FT_New_Face(ft, fontPath.c_str(), 0, &face))
    {
        std::cerr << "ERROR::FREETYPE: Failed to load font" << std::endl;
        return;
    }

    // Configura el tamaño de la fuente
    FT_Set_Pixel_Sizes(face, 0, 48);

    // Carga caracteres
    glPixelStorei(GL_UNPACK_ALIGNMENT, 1); // Desactiva el alineamiento byte-aligned

    for (unsigned char c = 0; c < 128; c++)
    {
        // Carga el glifo
        if (FT_Load_Char(face, c, FT_LOAD_RENDER))
        {
            std::cerr << "ERROR::FREETYPE: Failed to load Glyph" << std::endl;
            continue;
        }

        // Genera una textura para el glifo
        unsigned int texture;
        glGenTextures(1, &texture);
        glBindTexture(GL_TEXTURE_2D, texture);
        glTexImage2D(GL_TEXTURE_2D, 0, GL_RED, face->glyph->bitmap.width, face->glyph->bitmap.rows, 0, GL_RED,
                     GL_UNSIGNED_BYTE, face->glyph->bitmap.buffer);

        // Configura parámetros de la textura
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

        // Almacena el carácter en el mapa
        Character character = {texture, glm::ivec2(face->glyph->bitmap.width, face->glyph->bitmap.rows),
                               glm::ivec2(face->glyph->bitmap_left, face->glyph->bitmap_top),
                               static_cast<unsigned int>(face->glyph->advance.x)};
        characters.insert(std::pair<char, Character>(c, character));
    }

    glBindTexture(GL_TEXTURE_2D, 0);
    FT_Done_Face(face);
    FT_Done_FreeType(ft);
}
