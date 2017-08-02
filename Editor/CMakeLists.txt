set(DIR ${CMAKE_CURRENT_SOURCE_DIR})

set(EDITOR_HEADERS
    ${DIR}/Entity.h
    ${DIR}/MaterialDesc.h
    ${DIR}/Mesh.h
    ${DIR}/Model.h
    ${DIR}/ModelImporter.h
    ${DIR}/SceneConverter.h
    ${DIR}/SceneDesc.h
    ${DIR}/SceneImporter.h
    ${DIR}/Sky.h
    ${DIR}/Skybox.h
    ${DIR}/Skysphere.h
    PARENT_SCOPE
)

set(EDITOR_SOURCES
    ${DIR}/Model.cpp
    ${DIR}/ModelImporter.cpp
    ${DIR}/SceneConverter.cpp
    ${DIR}/SceneImporter.cpp
    PARENT_SCOPE
)
