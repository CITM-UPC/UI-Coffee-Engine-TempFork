#pragma once

#include <string>
#include <map>
#include <vector>
#include <memory>
#include <glm/glm.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <glad/glad.h>
#include <ft2build.h>
#include <glm/gtc/matrix_transform.hpp>
#include "CoffeeEngine/Scene/Components.h"
#include "CoffeeEngine/Renderer/Shader.h"
#include "CoffeeEngine/Renderer/Texture.h"
#include "CoffeeEngine/Renderer/Renderer.h"


struct Character
{
    unsigned int textureID; // ID de la textura del glifo
    glm::ivec2 size;        // Tamaño del glifo (ancho, alto)
    glm::ivec2 bearing;     // Offset desde la línea base al punto superior izquierdo
    unsigned int advance;   // Desplazamiento horizontal al renderizar el siguiente carácter
};

class TextRenderer {
public:
    TextRenderer();
    ~TextRenderer();

    void init();
    void RenderText(const std::string& text, const glm::vec2& position, float scale, const glm::vec3& color);
    //void RenderText(uiComponent.Text, position, scale);
    void loadFont(const std::string &fontPath);
    void SetFont(const std::string& fontName, int fontSize);
    std::string GetFont() const;

    
private:
    std::map<char, Character> characters;
    unsigned int VAO, VBO;
    //Shader shader; // Cambiado de Shader a std::shared_ptr<Shader>
};
