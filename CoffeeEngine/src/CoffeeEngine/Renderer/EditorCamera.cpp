#include "CoffeeEngine/Renderer/EditorCamera.h"
#include "CoffeeEngine/Core/Base.h"
#include "CoffeeEngine/Core/Input.h"
#include "CoffeeEngine/Core/KeyCodes.h"
#include "CoffeeEngine/Core/Log.h"
#include "CoffeeEngine/Core/MouseCodes.h"
#include "CoffeeEngine/Events/Event.h"
#include "CoffeeEngine/Events/MouseEvent.h"
#include <glm/ext/matrix_transform.hpp>
#include <glm/fwd.hpp>
#include <glm/gtc/quaternion.hpp>

namespace Coffee {

    EditorCamera::EditorCamera(float fov, ProjectionType projection, float aspectRatio, float nearClip, float farClip)
    {
        m_FOV = fov;
        m_ProjectionType = projection;
        m_AspectRatio = aspectRatio;
        m_NearClip = nearClip;
        m_FarClip = farClip;

        UpdateView();
        UpdateProjection();
    }

    void EditorCamera::OnUpdate(float dt)
    {
        glm::vec2 mousePos = Input::GetMousePosition();
        glm::vec2 delta = (mousePos - m_InitialMousePosition) * 0.01f;
        m_InitialMousePosition = mousePos;

        if (Input::IsMouseButtonPressed(Mouse::ButtonMiddle))
        {
            if (Input::IsKeyPressed(Key::LShift))
            {
                MousePan(delta);
            }
            else
            {
                MouseRotate(delta);
            }
        }
        else if (Input::IsMouseButtonPressed(Mouse::ButtonRight))
        {
            m_CurrentState = CameraState::FLY;
            Fly(delta);
        }
        else
        {
            m_CurrentState = CameraState::ORBIT;
        }

        UpdateView();
    }

    void EditorCamera::OnEvent(Event& event)
    {
        EventDispatcher dispatcher(event);
        dispatcher.Dispatch<MouseScrolledEvent>(COFFEE_BIND_EVENT_FN(OnMouseScroll));
    }

    void EditorCamera::MouseRotate(const glm::vec2& delta)
    {
        m_Yaw += delta.x;
        m_Pitch += delta.y;

        // Clamp the pitch to prevent flipping
        float pitchLimit = glm::radians(89.0f);
        m_Pitch = glm::clamp(m_Pitch, -pitchLimit, pitchLimit);
    }

    void EditorCamera::MousePan(const glm::vec2& delta)
    {
        m_FocalPoint += -GetRightDirection() * delta.x * m_Distance * 0.1f;
        m_FocalPoint += GetUpDirection() * delta.y * m_Distance * 0.1f;
    }

    void EditorCamera::MouseZoom(float delta)
    {
        if (m_CurrentState == CameraState::FLY)
        {
            m_Position += GetForwardDirection() * delta;
        }
        else if (m_CurrentState == CameraState::ORBIT)
        {
            m_Distance -= delta;
            if (m_Distance < 0.1f)
            {
                m_Distance = 0.1f;
            }
            if (m_Distance > 100.0f) // Maximum zoom out distance
            {
                m_Distance = 100.0f;
            }
        }
    }

    void EditorCamera::Fly(const glm::vec2& mouseDelta)
    {
        MouseRotate(mouseDelta);

        glm::vec3 forward = GetForwardDirection();
        glm::vec3 right = GetRightDirection();
        glm::vec3 up = GetUpDirection();

        if (Input::IsKeyPressed(Key::LShift))
        {
            m_CurrentSpeed = m_BaseSpeed * 2.0f;
        }
        else
        {
            m_CurrentSpeed = m_BaseSpeed;
        }

        if (Input::IsKeyPressed(Key::W))
        {
            m_Position += forward * m_CurrentSpeed;
        }
        if (Input::IsKeyPressed(Key::S))
        {
            m_Position -= forward * m_CurrentSpeed;
        }
        if (Input::IsKeyPressed(Key::A))
        {
            m_Position -= right * m_CurrentSpeed;
        }
        if (Input::IsKeyPressed(Key::D))
        {
            m_Position += right * m_CurrentSpeed;
        }
        if (Input::IsKeyPressed(Key::Q))
        {
            m_Position -= up * m_CurrentSpeed;
        }
        if (Input::IsKeyPressed(Key::E))
        {
            m_Position += up * m_CurrentSpeed;
        }

        m_FocalPoint = m_Position + GetForwardDirection() * m_Distance;
        UpdateView();
    }

    bool EditorCamera::OnMouseScroll(MouseScrolledEvent& event)
    {
        float delta = event.GetYOffset();

        if (m_CurrentState == CameraState::FLY)
        {
            float speedIncrement = glm::exp(0.1 * m_BaseSpeed) - 1.0f;
            m_BaseSpeed += delta * speedIncrement;
            m_BaseSpeed = glm::clamp(m_BaseSpeed, 0.01f, 1.0f);
        }
        else if (m_CurrentState == CameraState::ORBIT)
        {
            // Values that work well to multiply to the m_Distance are 0.02, 0.03(the best), 0.04
            float zoomIncrement = glm::exp(0.03 * m_Distance) - 1.0f;
            MouseZoom(delta * zoomIncrement);
        }

        UpdateView();
        return false;
    }

    void EditorCamera::UpdateView()
    {
        if (m_CurrentState == CameraState::FLY)
        {
            m_ViewMatrix = glm::lookAt(m_Position, m_Position + GetForwardDirection(), GetUpDirection());
        }
        else if (m_CurrentState == CameraState::ORBIT)
        {
            m_Position = CalculatePosition();
            m_ViewMatrix = glm::lookAt(m_Position, m_FocalPoint, GetUpDirection());
        }
    }

    glm::vec3 EditorCamera::GetUpDirection() const
    {
        return GetOrientation() * glm::vec3(0.0f, 1.0f, 0.0f);
    }

    glm::vec3 EditorCamera::GetRightDirection() const
    {
        return GetOrientation() * glm::vec3(1.0f, 0.0f, 0.0f);
    }

    glm::vec3 EditorCamera::GetForwardDirection() const
    {
        return GetOrientation() * glm::vec3(0.0f, 0.0f, -1.0f);
    }

    glm::quat EditorCamera::GetOrientation() const
    {
        return glm::quat(glm::vec3(-m_Pitch, -m_Yaw, 0.0f));
    }

    glm::vec3 EditorCamera::CalculatePosition() const
    {
        return m_FocalPoint - GetForwardDirection() * m_Distance;
    }

}