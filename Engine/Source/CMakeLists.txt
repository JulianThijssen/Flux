set(DIR ${CMAKE_CURRENT_SOURCE_DIR})

set(BASE
    ${DIR}/Application.h
    ${DIR}/Application.cpp
    ${DIR}/AssetManager.h
    ${DIR}/AssetManager.cpp
    ${DIR}/Cubemap.h
    ${DIR}/DeferredRenderer.h
    ${DIR}/DeferredRenderer.cpp
    ${DIR}/Entity.h
    ${DIR}/FirstPersonView.h
    ${DIR}/ForwardRenderer.h
    ${DIR}/ForwardRenderer.cpp
    ${DIR}/Framebuffer.h
    ${DIR}/Material.h
    ${DIR}/Material.cpp
    ${DIR}/Renderer.h
    ${DIR}/Renderer.cpp
    ${DIR}/RenderPhase.h
    ${DIR}/Scene.h
    ${DIR}/Script.h
    ${DIR}/Shader.h
    ${DIR}/Shader.cpp
    ${DIR}/Skybox.h
    ${DIR}/Skybox.cpp
    ${DIR}/Texture.h
    ${DIR}/TextureUnit.h
    ${DIR}/Window.h
    ${DIR}/Window.cpp
    ${DIR}/glad.c
    PARENT_SCOPE
)

set(SCRIPTS
    ${DIR}/FirstPersonView.cpp
    PARENT_SCOPE
)

set(COMPONENTS
    ${DIR}/Component.h
    ${DIR}/AttachedTo.h
    ${DIR}/Camera.h
    ${DIR}/Camera.cpp
    ${DIR}/DirectionalLight.h
    ${DIR}/Mesh.h
    ${DIR}/MeshRenderer.h
    ${DIR}/PointLight.h
    ${DIR}/Transform.h
    PARENT_SCOPE
)

set(UTIL
    ${DIR}/File.h
    ${DIR}/Log.h
    ${DIR}/Log.cpp
    ${DIR}/Math.h
    ${DIR}/Math.cpp
    ${DIR}/Matrix4f.h
    ${DIR}/Matrix4f.cpp
    ${DIR}/Path.h
    ${DIR}/Path.cpp
    ${DIR}/Size.h
    ${DIR}/String.h
    ${DIR}/Vector2f.h
    ${DIR}/Vector3f.h
    ${DIR}/Vector2f.cpp
    ${DIR}/Vector3f.cpp
    PARENT_SCOPE
)

set(EXCEPTIONS
    ${DIR}/Exceptions/ComponentNotFoundException.h
    ${DIR}/Exceptions/ShaderCompilationException.h
    ${DIR}/Exceptions/ShaderLinkException.h
    PARENT_SCOPE
)

set(INPUT
    ${DIR}/Input/Input.h
    ${DIR}/Input/Input.cpp
    PARENT_SCOPE
)

set(RENDERING
    ${DIR}/Renderer/AveragePass.h
    ${DIR}/Renderer/AveragePass.cpp
    ${DIR}/Renderer/ImageBasedRendering.h
    ${DIR}/Renderer/SSAORendering.h
    ${DIR}/Renderer/ImageBasedRendering.cpp
    ${DIR}/Renderer/SSAORendering.cpp
    PARENT_SCOPE
)

set (LOADERS
    ${DIR}/MaterialLoader.h
    ${DIR}/MaterialLoader.cpp
    ${DIR}/SceneLoader.h
    ${DIR}/SceneLoader.cpp
    ${DIR}/ShaderLoader.h
    ${DIR}/ShaderLoader.cpp
    ${DIR}/TextureLoader.h
    ${DIR}/TextureLoader.cpp
    PARENT_SCOPE
)

list(APPEND ENGINE_SOURCES
    ${BASE}
    ${SCRIPTS}
    ${COMPONENTS}
    ${UTIL}
    ${EXCEPTIONS}
    ${INPUT}
    ${RENDERING}
    ${LOADERS}
)
