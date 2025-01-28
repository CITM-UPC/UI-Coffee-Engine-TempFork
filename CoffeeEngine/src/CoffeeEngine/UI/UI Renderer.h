#pragma once

#include "CoffeeEngine/Core/Base.h"
#include "CoffeeEngine/Renderer/Buffer.h"
#include <Math.h>
#include "CoffeeEngine/Renderer/Shader.h"
#include "CoffeeEngine/Renderer/VertexArray.h"
#include "CoffeeEngine/Scene/Components.h"

namespace Coffee
{

    class UIRenderer
    {
      public:
        static void Init();
        static void Shutdown();
        static void Render();

        static void DrawText(const TextComponent& textComponent);
        static void DrawRectangle(const glm::vec2& position, const glm::vec2& size, const glm::vec4& color);

        static void RenderCanvas(const Entity& entity, UIComponent& uiComponent, const glm::mat4& worldTransform);
        //static void RenderText(const UIComponent& textComponent, const glm::mat4& worldTransform);
        static void RenderText(const TextComponent& textComponent, const glm::mat4& worldTransform);


        


      private:
        static Ref<Shader> m_UIShader;
        static Ref<VertexArray> m_UIVertexArray;
        static Ref<VertexBuffer> m_UIVertexBuffer;

        static constexpr size_t MaxVertices = 100000;
        static DebugVertex m_UIVertices[MaxVertices];
        static int m_UIVertexCount;
    };
}


