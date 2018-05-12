#include "FirstPersonView.h"

#include "Input/Input.h"
#include "Util/Matrix4f.h"
#include "Util/Vector3f.h"
#include "Util/Vector2f.h"
#include "Util/Math.h"

namespace Flux {
    void FirstPersonView::start(Scene& scene) {

    }

    void FirstPersonView::update(Scene& scene) {
        Entity* e = scene.getMainCamera();
        Transform& transform = e->getComponent<Transform>();
        Camera& camera = e->getComponent<Camera>();

        // Mouselook
        Vector2f deltaMouse = (Input::getMousePos() - mousePos) * 0.03f;
        storedDX += deltaMouse.x;
        storedDY += deltaMouse.y;
        storedDX *= 0.8f;
        storedDY *= 0.8f;

        mousePos = Input::getMousePos();

        transform.rotation.y -= storedDX;
        transform.rotation.x -= storedDY;

        if (transform.rotation.x < -90) {
            transform.rotation.x = -90;
        }
        if (transform.rotation.x > 90) {
            transform.rotation.x = 90;
        }

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
        direction = Math::directionFromRotation(transform.rotation, direction, false);

        transform.position += direction * 0.2f;
    }
}
