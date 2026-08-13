set(VE_RENDER_BACKEND_WINDOW_SOURCES
    "${VE_SOURCE_ROOT}/render/backends/Window.cpp"
    "${VE_SOURCE_ROOT}/render/backends/WindowCallbacks.cpp"
    "${VE_SOURCE_ROOT}/render/backends/WindowCursor.cpp"
    "${VE_SOURCE_ROOT}/render/backends/WindowDisplay.cpp"
    "${VE_SOURCE_ROOT}/render/backends/WindowInitialization.cpp"
    "${VE_SOURCE_ROOT}/render/backends/WindowProperties.cpp"
    "${VE_SOURCE_ROOT}/render/backends/WindowRuntime.cpp"
)

set(VE_RENDER_BACKEND_OPENGL_SOURCES
    "${VE_SOURCE_ROOT}/render/backends/OpenGLCompatibilityBackend.cpp"
    "${VE_SOURCE_ROOT}/render/backends/OpenGLRenderCommandExecutor.cpp"
)

set(VE_RENDER_BACKEND_FACTORY_SOURCES
    "${VE_SOURCE_ROOT}/render/backends/DirectX12Backend.cpp"
    "${VE_SOURCE_ROOT}/render/backends/RenderBackendFactory.cpp"
)

file(GLOB VE_RENDER_BACKEND_VULKAN_SOURCES CONFIGURE_DEPENDS
    "${VE_SOURCE_ROOT}/render/backends/Vulkan*.cpp"
)
list(SORT VE_RENDER_BACKEND_VULKAN_SOURCES)
