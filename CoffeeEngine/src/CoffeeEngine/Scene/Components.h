/**
 * @defgroup scene Scene
 * @{
 */

#pragma once

#include "CoffeeEngine/Core/Base.h"
#include "CoffeeEngine/IO/ResourceRegistry.h"
#include "CoffeeEngine/Renderer/Material.h"
#include "CoffeeEngine/Renderer/Mesh.h"
#include "CoffeeEngine/Renderer/Model.h"
#include "CoffeeEngine/Scene/SceneCamera.h"
#include <cereal/cereal.hpp>
#include <cereal/access.hpp>
#include <cereal/types/string.hpp>
#include <glm/ext/matrix_transform.hpp>
#include <glm/fwd.hpp>
#include <glm/glm.hpp>
#include <glm/gtc/quaternion.hpp>
#include "src/CoffeeEngine/IO/Serialization/GLMSerialization.h"
#include "CoffeeEngine/IO/ResourceLoader.h"
#include "string"


#define GLM_ENABLE_EXPERIMENTAL
#include <glm/gtx/quaternion.hpp>
#include <glm/gtx/matrix_decompose.hpp>


namespace Coffee
{
    /**
     * @brief Component representing a tag.
     * @ingroup scene
     */
    struct TagComponent
    {
        std::string Tag; ///< The tag string.

        TagComponent() = default;
        TagComponent(const TagComponent&) = default;
        TagComponent(const std::string& tag) : Tag(tag) {}

        /**
         * @brief Serializes the TagComponent.
         * @tparam Archive The type of the archive.
         * @param archive The archive to serialize to.
         */
        template <class Archive> void serialize(Archive& archive) { archive(cereal::make_nvp("Tag", Tag)); }
    };

    /**
     * @brief Component representing a transform.
     * @ingroup scene
     */
    struct TransformComponent
    {
      private:
        glm::mat4 worldMatrix = glm::mat4(1.0f); ///< The world transformation matrix.
      public:
        glm::vec3 Position = {0.0f, 0.0f, 0.0f}; ///< The position vector.
        glm::vec3 Rotation = {0.0f, 0.0f, 0.0f}; ///< The rotation vector.
        glm::vec3 Scale = {1.0f, 1.0f, 1.0f};    ///< The scale vector.

        TransformComponent() = default;
        TransformComponent(const TransformComponent&) = default;
        TransformComponent(const glm::vec3& position) : Position(position) {}

        /**
         * @brief Gets the local transformation matrix.
         * @return The local transformation matrix.
         */
        glm::mat4 GetLocalTransform() const
        {
            glm::mat4 rotation = glm::toMat4(glm::quat(glm::radians(Rotation)));

            return glm::translate(glm::mat4(1.0f), Position) * rotation * glm::scale(glm::mat4(1.0f), Scale);
        }

        /**
         * @brief Sets the local transformation matrix.
         * @param transform The transformation matrix to set.
         */
        void SetLocalTransform(
            const glm::mat4& transform) // TODO: Improve this function, this way is ugly and glm::decompose is from gtx
                                        // (is supposed to not be very stable)
        {
            glm::vec3 skew;
            glm::vec4 perspective;
            glm::quat orientation;

            glm::decompose(transform, Scale, orientation, Position, skew, perspective);
            Rotation = glm::degrees(glm::eulerAngles(orientation));
        }

        /**
         * @brief Gets the world transformation matrix.
         * @return The world transformation matrix.
         */
        const glm::mat4& GetWorldTransform() const { return worldMatrix; }

        /**
         * @brief Sets the world transformation matrix.
         * @param transform The transformation matrix to set.
         */
        void SetWorldTransform(const glm::mat4& transform) { worldMatrix = transform * GetLocalTransform(); }

        /**
         * @brief Serializes the TransformComponent.
         * @tparam Archive The type of the archive.
         * @param archive The archive to serialize to.
         */
        template <class Archive> void serialize(Archive& archive)
        {
            archive(cereal::make_nvp("Position", Position), cereal::make_nvp("Rotation", Rotation),
                    cereal::make_nvp("Scale", Scale));
        }
    };

    /**
     * @brief Component representing a camera.
     * @ingroup scene
     */
    struct CameraComponent
    {
        SceneCamera Camera; ///< The scene camera.

        CameraComponent() = default;
        CameraComponent(const CameraComponent&) = default;

        /**
         * @brief Serializes the CameraComponent.
         * @tparam Archive The type of the archive.
         * @param archive The archive to serialize to.
         */
        template <class Archive> void serialize(Archive& archive) { archive(cereal::make_nvp("Camera", Camera)); }
    };

    /**
     * @brief Component representing a mesh.
     * @ingroup scene
     */
    struct MeshComponent
    {
        Ref<Mesh> mesh;        ///< The mesh reference.
        bool drawAABB = false; ///< Flag to draw the axis-aligned bounding box (AABB).

        MeshComponent()
        {
            // TEMPORAL! In the future use for example MeshComponent() : mesh(MeshFactory(PrimitiveType::MeshText))
            Ref<Model> m = Model::Load("assets/models/MissingMesh.glb");
            mesh = m->GetMeshes()[0];
        }
        MeshComponent(const MeshComponent&) = default;
        MeshComponent(Ref<Mesh> mesh) : mesh(mesh) {}

        /**
         * @brief Gets the mesh reference.
         * @return The mesh reference.
         */
        const Ref<Mesh>& GetMesh() const { return mesh; }

      private:
        friend class cereal::access;
        /**
         * @brief Serializes the MeshComponent.
         * @tparam Archive The type of the archive.
         * @param archive The archive to serialize to.
         */
        template <class Archive> void save(Archive& archive) const
        {
            archive(cereal::make_nvp("Mesh", mesh->GetUUID()));
        }

        template <class Archive> void load(Archive& archive)
        {
            UUID meshUUID;
            archive(cereal::make_nvp("Mesh", meshUUID));

            Ref<Mesh> mesh = ResourceRegistry::Get<Mesh>(meshUUID);
            this->mesh = mesh;
        }
    };

    /**
     * @brief Component representing a material.
     * @ingroup scene
     */
    struct MaterialComponent
    {
        Ref<Material> material; ///< The material reference.

        MaterialComponent()
        {
            // FIXME: The first time the Default Material is created, the UUID is not saved in the cache and each time
            // the engine is started the Default Material is created again.
            Ref<Material> m = Material::Create("Default Material");
            material = m;
        }
        MaterialComponent(const MaterialComponent&) = default;
        MaterialComponent(Ref<Material> material) : material(material) {}

      private:
        friend class cereal::access;
        /**
         * @brief Serializes the MeshComponent.
         * @tparam Archive The type of the archive.
         * @param archive The archive to serialize to.
         */
        template <class Archive> void save(Archive& archive) const
        {
            archive(cereal::make_nvp("Material", material->GetUUID()));
        }

        template <class Archive> void load(Archive& archive)
        {
            UUID materialUUID;
            archive(cereal::make_nvp("Material", materialUUID));

            Ref<Material> material = ResourceRegistry::Get<Material>(materialUUID);
            this->material = material;
        }
    };

    /**
     * @brief Component representing a light.
     * @ingroup scene
     */
    struct LightComponent
    {
        /**
         * @brief Enum representing the type of light.
         */
        enum Type
        {
            DirectionalLight = 0, ///< Directional light.
            PointLight = 1,       ///< Point light.
            SpotLight = 2         ///< Spot light.
        };

        // Align to 16 bytes(glm::vec4) instead of 12 bytes(glm::vec3) to match the std140 layout in the shader (a vec3
        // is 16 bytes in std140)
        alignas(16) glm::vec3 Color = {1.0f, 1.0f, 1.0f};      ///< The color of the light.
        alignas(16) glm::vec3 Direction = {0.0f, -1.0f, 0.0f}; ///< The direction of the light.
        alignas(16) glm::vec3 Position = {0.0f, 0.0f, 0.0f};   ///< The position of the light.

        float Range = 5.0f;       ///< The range of the light.
        float Attenuation = 1.0f; ///< The attenuation of the light.
        float Intensity = 1.0f;   ///< The intensity of the light.

        float Angle = 45.0f; ///< The angle of the light.

        int type = static_cast<int>(Type::DirectionalLight); ///< The type of the light.

        LightComponent() = default;
        LightComponent(const LightComponent&) = default;

        /**
         * @brief Serializes the LightComponent.
         * @tparam Archive The type of the archive.
         * @param archive The archive to serialize to.
         */
        template <class Archive> void serialize(Archive& archive)
        {
            archive(cereal::make_nvp("Color", Color), cereal::make_nvp("Direction", Direction),
                    cereal::make_nvp("Position", Position), cereal::make_nvp("Range", Range),
                    cereal::make_nvp("Attenuation", Attenuation), cereal::make_nvp("Intensity", Intensity),
                    cereal::make_nvp("Angle", Angle), cereal::make_nvp("Type", type));
        }
    };

    /**
     * @brief Component representing a UI element.
     * @ingroup scene
     */
    /**
     * @brief Component representing a UI element.
     * @ingroup scene
     */
    struct UIComponent
    {
        glm::vec2 Position = {0.0f, 0.0f};          ///< Position of the UI element.
        glm::vec2 Size = {100.0f, 50.0f};           ///< Size of the UI element.
        glm::vec4 Color = {1.0f, 1.0f, 1.0f, 1.0f}; ///< Color of the UI element (RGBA).
        std::string Text = "UI Element";            ///< Text content of the UI element.
        std::string TexturePath = "";               ///< Path to the texture (if any).

        // New properties
        bool IsInteractive = false; ///< Whether the UI element can be interacted with.
        bool IsVisible = true;      ///< Whether the UI element is visible.
        float Rotation = 0.0f;      ///< Rotation of the UI element in degrees.
        float Alpha = 1.0f;         ///< Transparency (1.0 = opaque, 0.0 = fully transparent).

        // Nested UI components
        std::vector<UIComponent> Children; ///< Children UI components (for nesting).

        // Type of UI component (e.g., Button, Panel, etc.)
        enum UIComponentType
        {
            None = 0,
            Canvas,
            Button,
            Panel,
            TextUI,
            Image
        } ComponentType = None;

        UIComponent() = default;
        UIComponent(const UIComponent&) = default;
        UIComponent(const glm::vec2& position, const glm::vec2& size, const glm::vec4& color,
                    const std::string& texturePath = "")
            : Position(position), Size(size), Color(color), TexturePath(texturePath)
        {
        }

        // Adds a child UI component
        void AddChild(const UIComponent& child) { Children.push_back(child); }

        // Removes a child UI component (by index)
        void RemoveChild(size_t index)
        {
            if (index < Children.size())
                Children.erase(Children.begin() + index);
        }

        // Render the UI element
        virtual void Draw()
        {
            if (!IsVisible)
                return;

            // Pseudo-code for drawing:
            // - Apply transformations (position, size, rotation)
            // - Set the color and alpha
            // - Draw the texture if available, otherwise draw a rectangle
            // - Render text if applicable

            // Example:
            // Renderer::DrawQuad(Position, Size, Color, TexturePath);

            // Draw children recursively
            for (auto& child : Children)
            {
                child.Draw();
            }
        }

        // Handle interaction (e.g., clicks)
        virtual void HandleInteraction()
        {
            if (!IsInteractive || !IsVisible)
                return;

            // Pseudo-code for interaction:
            // - Check if mouse is over the element
            // - Trigger specific behavior based on `ComponentType`
            // - Example: if (ComponentType == Button) { OnClick(); }
        }

        // Serialization
        template <class Archive> void serialize(Archive& archive)
        {
            archive(cereal::make_nvp("Position", Position), cereal::make_nvp("Size", Size),
                    cereal::make_nvp("Color", Color), cereal::make_nvp("Text", TextUI),
                    cereal::make_nvp("TexturePath", TexturePath), cereal::make_nvp("ComponentType", ComponentType),
                    cereal::make_nvp("IsInteractive", IsInteractive), cereal::make_nvp("IsVisible", IsVisible),
                    cereal::make_nvp("Rotation", Rotation), cereal::make_nvp("Alpha", Alpha),
                    cereal::make_nvp("Children", Children));
        }
    };

    struct CanvasComponent : UIComponent
    {

        /**
         * @brief Enum representing the render mode of the canvas.
         */
        enum RenderMode
        {
            ScreenSpaceOverlay = 0, ///< Rendered in screen space and overlaid on top of everything.
            ScreenSpaceCamera = 1,  ///< Rendered in screen space but tied to a specific camera.
            WorldSpace = 2          ///< Rendered in world space as a physical object.
        };

        RenderMode Mode = ScreenSpaceOverlay;                 ///< The render mode of the canvas.
        float ReferenceWidth = 1920.0f;                       ///< Reference width for scaling.
        float ReferenceHeight = 1080.0f;                      ///< Reference height for scaling.
        glm::vec2 ScaleFactor = {1.0f, 1.0f};                 ///< Scale factor for adapting to different screen sizes.
        glm::vec4 BackgroundColor = {1.0f, 1.0f, 1.0f, 1.0f}; // Default to white

        bool PixelPerfect = false; ///< Whether the canvas should be rendered pixel-perfect.

        Ref<SceneCamera> Camera; ///< The camera to use if the render mode is ScreenSpaceCamera.

        CanvasComponent() : UIComponent({0.0f, 0.0f}, {0.0f, 0.0f}, {1.0f, 1.0f, 1.0f, 1.0f})
        {
            ComponentType = Canvas;
        }

        CanvasComponent(const CanvasComponent&) = default;

        /**
         * @brief Sets the render mode of the canvas.
         * @param mode The render mode to set.
         */
        void SetRenderMode(RenderMode mode) { Mode = mode; }

        /**
         * @brief Calculates the scale factor based on the current resolution and reference resolution.
         * @param currentWidth The current screen width.
         * @param currentHeight The current screen height.
         */
        void UpdateScaleFactor(float currentWidth, float currentHeight)
        {
            ScaleFactor.x = currentWidth / ReferenceWidth;
            ScaleFactor.y = currentHeight / ReferenceHeight;
        }

        /**
         * @brief Renders the canvas and its child UI components.
         */
        void Draw() override
        {
            if (!IsVisible)
                return;

            // Apply canvas-specific rendering logic.
            if (Mode == ScreenSpaceOverlay)
            {
                // Render as an overlay in screen space.
                // Example: Renderer::SetScreenSpace();
            }
            else if (Mode == ScreenSpaceCamera)
            {
                // Render in screen space but using a camera.
                if (Camera)
                {
                    // Example: Renderer::SetCamera(Camera);
                }
            }
            else if (Mode == WorldSpace)
            {
                // Render in world space.
                // Example: Renderer::SetWorldTransform();
            }

            // Draw children.
            for (auto& child : Children)
            {
                child.Draw();
            }
        }

        /**
         * @brief Handles interaction specific to the canvas.
         */
        void HandleInteraction() override
        {
            if (!IsInteractive || !IsVisible)
                return;

            // Pseudo-code for canvas interaction logic (if needed).
            // Iterate through children and delegate interaction.
            for (auto& child : Children)
            {
                child.HandleInteraction();
            }
        }

        /**
         * @brief Serializes the CanvasComponent.
         * @tparam Archive The type of the archive.
         * @param archive The archive to serialize to.
         */
        template <class Archive> void serialize(Archive& archive)
        {
            archive(cereal::make_nvp("Mode", Mode), cereal::make_nvp("ReferenceWidth", ReferenceWidth),
                    cereal::make_nvp("ReferenceHeight", ReferenceHeight), cereal::make_nvp("ScaleFactor", ScaleFactor),
                    cereal::make_nvp("PixelPerfect", PixelPerfect), cereal::make_nvp("Camera", Camera),
                    cereal::make_nvp("Children", Children));
        }
    };

    struct TextComponent : UIComponent
    {
        std::string TextUI = "Text";                ///< The text content of the UI element.
        float FontSize = 16.0f;                     ///< Font size for the text.
        glm::vec4 Color = {1.0f, 1.0f, 1.0f, 1.0f}; ///< Font color (RGBA).
        std::string FontPath = "assets/fonts/OpenSans-SemiBold.ttf";       ///< Path to the font file.

        enum class Alignment
        {
            Left = 0,
            Center,
            Right
        } Alignment = Alignment::Left; ///< Text alignment.

        bool WrapText = false;  ///< Enable or disable text wrapping.
        float WrapWidth = 0.0f; ///< Width for text wrapping (only relevant if WrapText is true).

        TextComponent() : UIComponent({0.0f, 0.0f}, {0.0f, 0.0f}, {1.0f, 1.0f, 1.0f, 1.0f})
        {
            ComponentType = UIComponentType::TextUI;
        }

        TextComponent(const TextComponent&) = default;

        /**
         * @brief Serializes the TextComponent.
         * @tparam Archive The type of the archive.
         * @param archive The archive to serialize to.
         */
        template <class Archive> void serialize(Archive& archive)
        {
            archive(cereal::make_nvp("Text", TextUI), cereal::make_nvp("FontSize", FontSize),
                    cereal::make_nvp("Color", Color), cereal::make_nvp("FontPath", FontPath),
                    cereal::make_nvp("Alignment", Alignment), cereal::make_nvp("WrapText", WrapText),
                    cereal::make_nvp("WrapWidth", WrapWidth), cereal::make_nvp("Position", Position),
                    cereal::make_nvp("Size", Size), cereal::make_nvp("Rotation", Rotation),
                    cereal::make_nvp("Alpha", Alpha), cereal::make_nvp("IsVisible", IsVisible),
                    cereal::make_nvp("IsInteractive", IsInteractive), cereal::make_nvp("Children", Children));
        }
    };


} // namespace Coffee
  /** @} */