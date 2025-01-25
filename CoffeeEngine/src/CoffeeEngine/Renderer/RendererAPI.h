#pragma once

#include "CoffeeEngine/Core/Base.h"
#include "CoffeeEngine/Renderer/VertexArray.h"

#include <glm/glm.hpp>

namespace Coffee {

    /**
     * @defgroup renderer Renderer
     * @brief Renderer components of the CoffeeEngine.
     * @{
     */

    /**
     * @brief Class representing the Renderer API.
     */
    class RendererAPI {
    public:
        /**
         * @brief Initializes the Renderer API.
         */
        static void Init();

        /**
         * @brief Sets the clear color for the renderer.
         * @param color The clear color as a glm::vec4.
         */
        static void SetClearColor(const glm::vec4& color);

        
        static void DrawQuad(const Ref<VertexArray>& vertexArray, int vertexCount);

        /**
         * @brief Clears the current buffer.
         */
        static void Clear();

        /**
         * @brief Enables or disables the depth mask.
         * @param enabled True to enable the depth mask, false to disable it.
         */
        static void SetDepthMask(bool enabled);

        /**
         * @brief Draws the indexed vertices from the specified vertex array.
         * @param vertexArray The vertex array containing the vertices to draw.
         */
        static void DrawIndexed(const Ref<VertexArray>& vertexArray);

        /**
         * @brief Draws lines from the specified vertex array.
         * @param vertexArray The vertex array containing the vertices to draw.
         * @param vertexCount The number of vertices to draw.
         * @param lineWidth The width of the lines.
         */
        static void DrawLines(const Ref<VertexArray>& vertexArray, uint32_t vertexCount, float lineWidth = 1.0f);

        /**
         * @brief Creates a new Renderer API instance.
         * @return A scope pointer to the created Renderer API instance.
         */
        static Scope<RendererAPI> Create();

    private:
        static Scope<RendererAPI> s_RendererAPI; ///< The Renderer API instance.
    };

    /** @} */
}