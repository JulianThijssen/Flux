#include "FirstPersonView.h"

#include <Engine/Source/Input/Input.h>
#include <Engine/Source/Matrix4f.h>
#include <Engine/Source/Vector3f.h>
#include <Engine/Source/Vector2f.h>

namespace Flux {
    void FirstPersonView::start(Scene& scene) {

    }

    void FirstPersonView::update(Scene& scene) {
        Entity* e = scene.getMainCamera();
        Transform* transform = e->getComponent<Transform>();
        Camera* camera = e->getComponent<Camera>();

        if (transform == nullptr || camera == nullptr) {
            return;
        }

        // Mouselook
        Vector2f deltaMouse = (Input::getMousePos() - mousePos) * 0.5;
        mousePos = Input::getMousePos();

        transform->rotation.y -= deltaMouse.x;
        transform->rotation.x -= deltaMouse.y;

        if (transform->rotation.x < -90) {
            transform->rotation.x = -90;
        }
        if (transform->rotation.x > 90) {
            transform->rotation.x = 90;
        }

        float pitch = transform->rotation.x;
        float yaw = transform->rotation.y;

        Vector3f direction;
        if (Input::isKeyDown(Input::KEY_W)) {
            direction.z = -1;
        }
        if (Input::isKeyDown(Input::KEY_S)) {
            direction.z = 1;
        }
        if (Input::isKeyDown(Input::KEY_A)) {
            direction.x = -1;
        }
        if (Input::isKeyDown(Input::KEY_D)) {
            direction.x = 1;
        }

        // Move
        Matrix4f yawMatrix;
        yawMatrix.rotate(yaw, 0, 1, 0);

        Matrix4f pitchMatrix;
        pitchMatrix.rotate(pitch, 1, 0, 0);

        direction = pitchMatrix.transform(direction);
        direction = yawMatrix.transform(direction);

        transform->position += direction * 0.1f;
    }
}
