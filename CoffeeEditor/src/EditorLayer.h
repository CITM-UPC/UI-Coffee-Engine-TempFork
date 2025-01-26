#pragma once

#include "CoffeeEngine/Core/Base.h"
#include "CoffeeEngine/Core/Layer.h"
#include "CoffeeEngine/Events/ApplicationEvent.h"
#include "CoffeeEngine/Events/KeyEvent.h"
#include "CoffeeEngine/Renderer/EditorCamera.h"
#include "CoffeeEngine/Scene/Scene.h"
#include "Panels/ContentBrowserPanel.h"
#include "Panels/MonitorPanel.h"
#include "Panels/SceneTreePanel.h"

#include <Panels/ImportPanel.h>
#include <Panels/OutputPanel.h>

namespace Coffee {

    class EditorLayer : public Coffee::Layer
    {
    public:
        EditorLayer();
        virtual ~EditorLayer() = default;

        void OnAttach() override;

        void OnUpdate(float dt) override;

        void OnEvent(Event& event) override;

        bool OnKeyPressed(KeyPressedEvent& event);
        bool OnMouseButtonPressed(MouseButtonPressedEvent& event);
        bool OnFileDrop(FileDropEvent& event);

        void OnDetach() override;

        void OnImGuiRender() override;
    private:
        void OnOverlayRender();
        void ResizeViewport(float width, float height);

        // Editor State
        void OnScenePlay();
        void OnScenePause();
        void OnSceneStop();

        //Project Management
        void NewProject();
        void OpenProject();
        void SaveProject();

        //Scene Management
        void NewScene();
        void OpenScene();
        void SaveScene();
        void SaveSceneAs();
    private:
        Ref<Scene> m_EditorScene;
        Ref<Scene> m_ActiveScene;

        EditorCamera m_EditorCamera;

        enum class SceneState
        {
            Edit = 0,
            Play = 1
        }m_SceneState = SceneState::Edit;

        bool m_ViewportFocused = false, m_ViewportHovered = false;
        //ImVec2 viewportPanelSize = {500.0f, 500.0f};
		glm::vec2 m_ViewportSize = { 0.0f, 0.0f };
        glm::vec2 m_ViewportBounds[2];
        glm::vec2 windowsize = {0.0f, 0.0f};

        int m_GizmoType = -1;

        //Panels
        SceneTreePanel m_SceneTreePanel;
        ContentBrowserPanel m_ContentBrowserPanel;
        OutputPanel m_OutputPanel;
        MonitorPanel m_MonitorPanel;
        ImportPanel m_ImportPanel;
    };

}
