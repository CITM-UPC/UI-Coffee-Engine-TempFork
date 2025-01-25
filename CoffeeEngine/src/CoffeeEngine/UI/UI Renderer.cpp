#include <Math.h>
#include "CoffeeEngine/Renderer/RendererAPI.h"
#include "CoffeeEngine/Renderer/Shader.h"
#include "CoffeeEngine/Renderer/VertexArray.h"
#include "CoffeeEngine/UI/UI Renderer.h"

namespace Coffee
{

    Ref<Shader> UIRenderer::m_UIShader;
    Ref<VertexArray> UIRenderer::m_UIVertexArray;
    Ref<VertexBuffer> UIRenderer::m_UIVertexBuffer;

    DebugVertex UIRenderer::m_UIVertices[MaxVertices];
    int UIRenderer::m_UIVertexCount = 0;

    void UIRenderer::Init()
    {
        m_UIShader = CreateRef<Shader>("UIShader", "assets/shaders/UIShader.glsl");

        BufferLayout vertexLayout = {{ShaderDataType::Vec2, "a_Position"}, {ShaderDataType::Vec4, "a_Color"}};

        m_UIVertexArray = VertexArray::Create();
        m_UIVertexBuffer = VertexBuffer::Create(MaxVertices * sizeof(DebugVertex));
        m_UIVertexBuffer->SetLayout(vertexLayout);
        m_UIVertexArray->AddVertexBuffer(m_UIVertexBuffer);
    }

    void UIRenderer::Shutdown()
    {
        // Cleanup if necessary
    }

   void UIRenderer::Render()
    {
        if (m_UIVertexCount > 0)
        {
            m_UIVertexBuffer->SetData(m_UIVertices, m_UIVertexCount * sizeof(DebugVertex));
            m_UIShader->Bind();
            RendererAPI::DrawQuad(m_UIVertexArray, m_UIVertexCount); // Renderizamos los vértices
            m_UIVertexCount = 0;
        }
    }

    void UIRenderer::DrawText(const TextComponent& textComponent)
    {
        // Aquí puedes renderizar el texto 2D usando la información del componente.
        // Esto puede incluir la carga de un bitmap de la fuente, la configuración de color y tamaño, etc.
        // En este ejemplo, solo se visualiza un texto básico.

        glm::vec2 position = textComponent.Position;
        glm::vec2 size = glm::vec2(textComponent.Size.x, textComponent.FontSize); // Ajusta el tamaño según el FontSize
        glm::vec4 color = textComponent.Color;

        // Solo un ejemplo simple de un "quad" que representaría el texto
        DrawRectangle(position, size, color);
    }

   void UIRenderer::DrawRectangle(const glm::vec2& position, const glm::vec2& size, const glm::vec4& color)
    {
        if (m_UIVertexCount + 4 > MaxVertices)
        {
            Render();
        }

        glm::vec3 topLeft = glm::vec3(position, 0.0f);
        glm::vec3 topRight = glm::vec3(position + glm::vec2(size.x, 0.0f), 0.0f);
        glm::vec3 bottomLeft = glm::vec3(position + glm::vec2(0.0f, size.y), 0.0f);
        glm::vec3 bottomRight = glm::vec3(position + size, 0.0f);

        m_UIVertices[m_UIVertexCount++] = {topLeft, color};
        m_UIVertices[m_UIVertexCount++] = {topRight, color};
        m_UIVertices[m_UIVertexCount++] = {bottomLeft, color};
        m_UIVertices[m_UIVertexCount++] = {bottomRight, color};
    }
} 
