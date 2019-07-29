set(DIR ${CMAKE_CURRENT_SOURCE_DIR})

set(EDITOR_HEADERS
    ${DIR}/Entity.h
    ${DIR}/MaterialDesc.h
    ${DIR}/Mesh.h
    ${DIR}/Model.h
    ${DIR}/ModelImporter.h
    ${DIR}/SceneExporter.h
    ${DIR}/SceneDesc.h
    ${DIR}/SceneImporter.h
    ${DIR}/Sky.h
    ${DIR}/Skybox.h
    ${DIR}/Skysphere.h
    ${DIR}/Camera.h
    ${DIR}/Texture.h
    ${DIR}/Vector2f.h
    ${DIR}/Math.h
    ${DIR}/Path.h
    ${DIR}/Log.h
    PARENT_SCOPE
)

set(EDITOR_SOURCES
    ${DIR}/Application.cpp
    ${DIR}/glad.c
    ${DIR}/Model.cpp
    ${DIR}/ModelImporter.cpp
    ${DIR}/SceneExporter.cpp
    ${DIR}/SceneImporter.cpp
    ${DIR}/Camera.cpp
    ${DIR}/Texture.cpp
    ${DIR}/Vector2f.cpp
    ${DIR}/Math.cpp
    ${DIR}/Path.cpp
    ${DIR}/Log.cpp
    PARENT_SCOPE
)
