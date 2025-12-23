#include <dsaViz/camera.hpp>
#define GLM_ENABLE_EXPERIMENTAL
#include <glm/glm.hpp>
#include <glm/gtc/quaternion.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtx/quaternion.hpp>


namespace dsaViz {

    Camera::Camera()
        : mode(CameraMode::FPS),
          projectionType(ProjectionType::Perspective),
          position(0.0f),
          rotation(1,0,0,0),
          yaw(0.0f),
          pitch(0.0f),
          moveSpeed(5.0f),
          mouseSpeed(0.002f)
    {
        fovY = glm::radians(60.0f);
        aspect = 4.0f/3.0f;
        nearPlane = 0.001f;
        farPlane = 100.0f;
        orthoLeft = -10.0f; orthoRight = 10.0f;
        orthoBottom = -10.0f; orthoTop = 10.0f;

        viewMatrix = glm::mat4(1.0f);
        setPerspective(fovY, aspect, nearPlane, farPlane);
    }

    void Camera::setMode(CameraMode m) {
        mode = m;
    }

    void Camera::setPerspective(float fovY_, float aspect_, float near_, float far_) {
        projectionType = ProjectionType::Perspective;
        fovY = fovY_;
        aspect = aspect_;
        nearPlane = near_;
        farPlane = far_;
        projectionMatrix = glm::perspective(fovY, aspect, nearPlane, farPlane);
    }

    void Camera::setOrthographic(float left, float right, float bottom, float top, float near_, float far_) {
        projectionType = ProjectionType::Orthographic;
        orthoLeft = left; orthoRight = right; orthoBottom = bottom; orthoTop = top;
        nearPlane = near_; farPlane = far_;
        projectionMatrix = glm::ortho(left, right, bottom, top, nearPlane, farPlane);
    }

    void Camera::update(const DSAContext& ctx) {
        float dt = static_cast<float>(ctx.time.delta);

        switch (mode) {
            case CameraMode::FPS: updateFPS(ctx, dt); break;
            case CameraMode::FreeFly: updateFreeFly(ctx, dt); break;
            case CameraMode::AxisAligned: updateAxisAligned(ctx, dt); break;
        }

        // Update view matrix
        glm::mat4 rotMat = glm::toMat4(rotation);
        viewMatrix = glm::inverse(glm::translate(glm::mat4(1.0f), position) * rotMat);
    }

    const glm::mat4& Camera::getViewMatrix() const { return viewMatrix; }
    const glm::mat4& Camera::getProjectionMatrix() const { return projectionMatrix; }
    glm::mat4 Camera::getViewProjectionMatrix() const { return projectionMatrix * viewMatrix; }

    const glm::vec3& Camera::getPosition() const { return position; }
    const glm::quat& Camera::getRotation() const { return rotation; }
    void Camera::setPosition(const glm::vec3& pos) { position = pos; }
    void Camera::setRotation(const glm::quat& rot) { rotation = rot; }

    glm::vec3 Camera::getForward() const { return rotation * glm::vec3(0,0,-1); }
    glm::vec3 Camera::getRight() const { return rotation * glm::vec3(1,0,0); }
    glm::vec3 Camera::getUp() const { return rotation * glm::vec3(0,1,0); }

    // ------------------------------------------------------------
    // FPS: WASD + mouse look
    void Camera::updateFPS(const DSAContext& ctx, float dt) {
        glm::vec3 dir(0.0f);

        if (ctx.input.keys[GLFW_KEY_W]) dir += getForward();
        if (ctx.input.keys[GLFW_KEY_S]) dir -= getForward();
        if (ctx.input.keys[GLFW_KEY_A]) dir -= getRight();
        if (ctx.input.keys[GLFW_KEY_D]) dir += getRight();
        if (ctx.input.keys[GLFW_KEY_LEFT_SHIFT]) dir -= getUp();
        if (ctx.input.keys[GLFW_KEY_SPACE]) dir += getUp();

        float speed = moveSpeed;
        if(ctx.input.keys[GLFW_KEY_LEFT_CONTROL]) speed /= 100.0f;

        if (glm::length(dir) > 0.001f)
            position += glm::normalize(dir) * speed * dt;

        // Mouse look
        yaw   += static_cast<float>(ctx.input.mouseX - ctx.input.prevMouseX) * mouseSpeed;
        pitch += static_cast<float>(ctx.input.mouseY - ctx.input.prevMouseY) * mouseSpeed;

        // Limit pitch to avoid gimbal
        if(pitch > glm::half_pi<float>() - 0.01f) pitch = glm::half_pi<float>() - 0.01f;
        if(pitch < -glm::half_pi<float>() + 0.01f) pitch = -glm::half_pi<float>() + 0.01f;

        rotation = glm::quat(glm::vec3(-pitch, -yaw, 0.0f));
    }

    // ------------------------------------------------------------
    // Free Fly: yaw/pitch rotation but unrestricted movement
    void Camera::updateFreeFly(const DSAContext& ctx, float dt) {
        glm::vec3 dir(0.0f);

        if (ctx.input.keys[GLFW_KEY_W]) dir += getForward();
        if (ctx.input.keys[GLFW_KEY_S]) dir -= getForward();
        if (ctx.input.keys[GLFW_KEY_A]) dir -= getRight();
        if (ctx.input.keys[GLFW_KEY_D]) dir += getRight();
        if (ctx.input.keys[GLFW_KEY_Q]) dir -= getUp();
        if (ctx.input.keys[GLFW_KEY_E]) dir += getUp();

        if (glm::length(dir) > 0.001f)
            position += glm::normalize(dir) * moveSpeed * dt;

        // Mouse rotation
        yaw   += static_cast<float>(ctx.input.mouseX - ctx.input.prevMouseX) * mouseSpeed;
        pitch += static_cast<float>(ctx.input.mouseY - ctx.input.prevMouseY) * mouseSpeed;

        rotation = glm::quat(glm::vec3(-pitch, -yaw, 0.0f));
    }

    // ------------------------------------------------------------
    // Axis-aligned movement: no rotation
    void Camera::updateAxisAligned(const DSAContext& ctx, float dt) {
        glm::vec3 dir(0.0f);

        if (ctx.input.keys[GLFW_KEY_W]) dir += glm::vec3(0,0,-1);
        if (ctx.input.keys[GLFW_KEY_S]) dir += glm::vec3(0,0,1);
        if (ctx.input.keys[GLFW_KEY_A]) dir += glm::vec3(-1,0,0);
        if (ctx.input.keys[GLFW_KEY_D]) dir += glm::vec3(1,0,0);
        if (ctx.input.keys[GLFW_KEY_Q]) dir += glm::vec3(0,-1,0);
        if (ctx.input.keys[GLFW_KEY_E]) dir += glm::vec3(0,1,0);

        if (glm::length(dir) > 0.001f)
            position += glm::normalize(dir) * moveSpeed * dt;

        rotation = glm::quat(1,0,0,0); // no rotation
    }

}
