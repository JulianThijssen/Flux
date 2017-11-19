#include "FirstPersonView.h"

#include "Input/Input.h"
#include "Matrix4f.h"
#include "Vector3f.h"
#include "Vector2f.h"

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
        Vector2f deltaMouse = (Input::getMousePos() - mousePos) * 0.03f;
        storedDX += deltaMouse.x;
        storedDY += deltaMouse.y;
        storedDX *= 0.8f;
        storedDY *= 0.8f;

        mousePos = Input::getMousePos();

        transform->rotation.y -= storedDX;
        transform->rotation.x -= storedDY;

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

        direction = pitchMatrix.transform(direction, 0);
        direction = yawMatrix.transform(direction, 0);

        transform->position += direction * 0.2f;
    }
}
