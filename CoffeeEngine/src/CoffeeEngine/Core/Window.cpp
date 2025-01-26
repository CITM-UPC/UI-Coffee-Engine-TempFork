#include "CoffeeEngine/Core/Window.h"
#include "CoffeeEngine/Core/Assert.h"
#include "CoffeeEngine/Core/Base.h"
#include "CoffeeEngine/Core/Log.h"
#include "SDL3/SDL_init.h"
#include "SDL3/SDL_pixels.h"
#include "SDL3/SDL_surface.h"
#include "SDL3/SDL_video.h"

#include <SDL3/SDL.h>

#include <tracy/Tracy.hpp>

#include <stb_image.h>

namespace Coffee {

	static uint8_t s_SDLWindowCount = 0;

	Window::Window(const WindowProps& props)
	{
        ZoneScoped;

		Init(props);
	}

	Window::~Window()
	{
        ZoneScoped;

		Shutdown();
	}

	void Window::Init(const WindowProps& props)
	{
        ZoneScoped;

		m_Data.Title = props.Title;
		m_Data.Width = props.Width;
		m_Data.Height = props.Height;

		COFFEE_CORE_INFO("Creating window {0} ({1}, {2})", props.Title, props.Width, props.Height);

        if (s_SDLWindowCount == 0)
		{
            ZoneScopedN("SDL3 Init");
			int success = SDL_Init(SDL_INIT_VIDEO);
			COFFEE_CORE_ASSERT(success, "Could not initialize SDL!");
		}

            SDL_GL_SetAttribute(SDL_GL_CONTEXT_PROFILE_MASK, SDL_GL_CONTEXT_PROFILE_CORE);
            SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, 4);
            SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, 5);

            SDL_GL_SetAttribute(SDL_GL_DOUBLEBUFFER, 1);
            SDL_GL_SetAttribute(SDL_GL_DEPTH_SIZE, 24);
            SDL_GL_SetAttribute(SDL_GL_STENCIL_SIZE, 8);

        {
            ZoneScopedN("SDLCreateWindow");

            #if defined(COFFEE_DEBUG)
			SDL_GL_SetAttribute(SDL_GL_CONTEXT_FLAGS, SDL_GL_CONTEXT_DEBUG_FLAG);
		    #endif

            Uint32 windowFlags = SDL_WINDOW_OPENGL | SDL_WINDOW_RESIZABLE | SDL_WINDOW_HIDDEN | SDL_WINDOW_HIGH_PIXEL_DENSITY;

			m_Window = SDL_CreateWindow(m_Data.Title.c_str(), (int)props.Width, (int)props.Height, windowFlags);

            COFFEE_CORE_ASSERT(m_Window, "Failed to create the window! Error: " + std::string(SDL_GetError()));

            SDL_SetWindowPosition(m_Window, SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED);

			++s_SDLWindowCount;
        }
		//Create OpenGL Context
		m_Context = GraphicsContext::Create(m_Window);
		m_Context->Init();

		SetVSync(true);

        SDL_ShowWindow(m_Window);
    }

	void Window::Shutdown()
	{
        ZoneScoped;
        
        m_Context->Shutdown();
		SDL_DestroyWindow(m_Window);
		--s_SDLWindowCount;

		if (s_SDLWindowCount == 0)
		{
			SDL_Quit();
		}
	}

	void Window::OnUpdate()
	{
        ZoneScoped;

        m_Context->SwapBuffers();
	}

	void Window::SetVSync(bool enabled)
	{
        ZoneScoped;

		if (enabled)
			m_Context->SwapInterval(1);
		else
			m_Context->SwapInterval(0);

		m_Data.VSync = enabled;
	}

	bool Window::IsVSync() const
	{
		return m_Data.VSync;
	}

    void Window::SetTitle(const std::string& title)
    {
        m_Data.Title = title;
        SDL_SetWindowTitle(m_Window, title.c_str());
    }

    void Window::SetIcon(const std::string& path)
    {
        int width, height, channels;
        unsigned char* pixels = stbi_load(path.c_str(), &width, &height, &channels, 4);
        if (!pixels)
        {
            COFFEE_CORE_ERROR("Failed to load icon image: {0}", path);
            return;
        }
        
        SDL_Surface* icon = SDL_CreateSurfaceFrom(
            width, height, SDL_PIXELFORMAT_RGBA8888, pixels, width * 4);
        
        if(!icon)
        {
            COFFEE_CORE_ERROR("Failed to create icon surface: {0}", path);
            return;
        }

        SDL_SetWindowIcon(m_Window, icon);

        stbi_image_free(pixels);
    }

}

void Coffee::Window::SetSize(unsigned int width, unsigned int height)
{
    ZoneScoped;

    // Validaci�n de entrada
    if (width == 0 || height == 0)
    {
        COFFEE_CORE_WARN("Invalid window size: {0}x{1}. Size must be greater than zero.", width, height);
        return;
    }

    // Ajustar el tama�o de la ventana utilizando SDL
    SDL_SetWindowSize(m_Window, width, height);

    // Actualizar los datos internos de la ventana
    m_Data.Width = width;
    m_Data.Height = height;

    // Mensaje de depuraci�n
    
}