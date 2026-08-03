ve_add_backend_component(ve_render_backend_window Window
    SOURCES ${VE_RENDER_BACKEND_WINDOW_SOURCES}
    HEADERS ${VE_RENDER_BACKENDS_HEADERS}
    PUBLIC_LINKS ve_render
    PRIVATE_LINKS glfw
)

ve_add_backend_component(ve_render_backend_opengl OpenGL
    SOURCES ${VE_RENDER_BACKEND_OPENGL_SOURCES}
    HEADERS ${VE_RENDER_BACKENDS_HEADERS}
    PUBLIC_LINKS ve_render
    PRIVATE_LINKS GLEW::GLEW
)

ve_add_backend_component(ve_render_backend_vulkan Vulkan
    SOURCES ${VE_RENDER_BACKEND_VULKAN_SOURCES}
    HEADERS ${VE_RENDER_BACKENDS_HEADERS}
    PUBLIC_LINKS ve_core ve_render ve_world volk::volk_headers Vulkan::Headers
    PRIVATE_LINKS Boost::container glfw imgui::imgui volk::volk GPUOpen::VulkanMemoryAllocator
)

ve_add_backend_component(ve_render_backend_factory Factory
    SOURCES ${VE_RENDER_BACKEND_FACTORY_SOURCES}
    HEADERS ${VE_RENDER_BACKENDS_HEADERS}
    PUBLIC_LINKS ve_render
    PRIVATE_LINKS ve_render_backend_opengl ve_render_backend_vulkan
)

add_library(ve_render_backends INTERFACE)
target_include_directories(ve_render_backends INTERFACE "${VE_SOURCE_ROOT}/render/backends")
target_link_libraries(ve_render_backends INTERFACE
    ve_render_backend_factory
    ve_render_backend_opengl
    ve_render_backend_vulkan
    ve_render_backend_window
)
set_target_properties(ve_render_backends PROPERTIES FOLDER "Engine/Render")
