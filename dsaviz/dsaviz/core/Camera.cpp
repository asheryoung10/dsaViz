#include "VizContext.hpp"
#include <dsaviz/core/Camera.hpp>
#include <fmt/format.h>
#include <dsaviz/util/FrameTimeTracker.hpp>


namespace dsaviz {

    Camera::Camera()
        : mode(CameraMode::FPS),
          projectionType(ProjectionType::Perspective),
          position(0.0f),
          rotation(1,0,0,0),
          yaw(0.0f),
          pitch(0.0f),
          roll(0.0f),
          moveSpeed(5.0f),
          fineSpeed(0.1f),
          mouseSpeed(0.002f),
          orthoViewSize(1.0f)

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

    void Camera::setMode(VizContext &ctx, CameraMode m) {
        mode = m;
        switch(mode) {
            case(CameraMode::FPS):
            case(CameraMode::FreeFly):
                setPerspective(fovY, aspect, nearPlane, farPlane);
            break;
            case(CameraMode::AxisAligned):
                yaw = 0;
                pitch = 0;
                roll = 0;
                orthoViewSize = 1.0f;
                setOrthographic(orthoLeft, orthoRight, orthoBottom, orthoTop, nearPlane, farPlane);
            break;
        }
        updateAspectRatio(ctx.framebufferWidth, ctx.framebufferHeight);
    }

    void Camera::setPerspective(float fovY_, float aspect_, float near_, float far_) {
        projectionType = ProjectionType::Perspective;
        fovY = fovY_;
        aspect = aspect_;
        nearPlane = near_;
        farPlane = far_;
        projectionMatrix = glm::perspective(fovY, aspect, nearPlane, farPlane);
    }

    void Camera::updateAspectRatio(int width, int height) {
        float aspect = static_cast<float>(width) / static_cast<float>(height);

        // Maintain aspect ratio by expanding X or Y depending on window shape
        float viewLeft, viewRight, viewBottom, viewTop;

        if (aspect >= 1.0f) {
            // Wide window → expand X
            viewLeft  = -aspect;
            viewRight =  aspect;
            viewBottom = -1.0f;
            viewTop    =  1.0f;
        } else {
            // Tall window → expand Y
            viewLeft  = -1.0f;
            viewRight =  1.0f;
            viewBottom = -1.0f / aspect;
            viewTop    =  1.0f / aspect;
            aspect = 1 / aspect;
        }

        switch(mode) {
            case(CameraMode::FPS):
            case(CameraMode::FreeFly):
                setPerspective(fovY, aspect, nearPlane, farPlane);
            break;
            case(CameraMode::AxisAligned):
                yaw = 0;
                pitch = 0;
                roll = 0;
                setOrthographic(viewLeft, viewRight, viewBottom, viewTop, nearPlane, farPlane);
            break;
        }

    }

    void Camera::setOrthographic(float left, float right, float bottom, float top, float near_, float far_) {
        projectionType = ProjectionType::Orthographic;
        orthoLeft = left; orthoRight = right; orthoBottom = bottom; orthoTop = top;
        nearPlane = near_; farPlane = far_;
        projectionMatrix = glm::ortho(left*orthoViewSize, right*orthoViewSize, bottom*orthoViewSize, top*orthoViewSize, nearPlane, farPlane);
    }

    void Camera::update(const VizContext& ctx) {
        float dt = static_cast<float>(ctx.frameTimeTracker->getCurrentFrameTime());

        switch (mode) {
            case CameraMode::FPS: updateFPS(ctx, dt); break;
            case CameraMode::FreeFly: updateFreeFly(ctx, dt); break;
            case CameraMode::AxisAligned: updateAxisAligned(ctx, dt); break;
        }

        // Update view matrix
        glm::mat4 rotMat = glm::toMat4(rotation);
        viewMatrix = glm::inverse(glm::translate(glm::mat4(1.0f), position) * rotMat);
    }


    std::string Camera::toString() const {
        return fmt::format(
            "Camera {{\n"
            "  mode: {},\n"
            "  projection: {},\n"
            "  position: ({:.3f}, {:.3f}, {:.3f}),\n"
            "  rotation (quat): ({:.3f}, {:.3f}, {:.3f}, {:.3f}),\n"
            "  yaw: {:.3f}, pitch: {:.3f}, roll: {:.3f},\n"
            "  fovY: {:.3f}, aspect: {:.3f},\n"
            "  near: {:.4f}, far: {:.1f}\n"
            "}}",
            static_cast<int>(mode),
            static_cast<int>(projectionType),
            position.x, position.y, position.z,
            rotation.w, rotation.x, rotation.y, rotation.z,
            yaw, pitch, roll,
            fovY, aspect,
            nearPlane, farPlane
        );
    }

    const glm::mat4& Camera::getViewMatrix() const { return viewMatrix; }
    const glm::mat4& Camera::getProjectionMatrix() const { return projectionMatrix; }
    glm::mat4 Camera::getViewProjectionMatrix() const { return projectionMatrix * viewMatrix; }

    const glm::vec3& Camera::getPosition() const { return position; }
    const glm::quat& Camera::getRotation() const { return rotation; }
    void Camera::setPosition(const glm::vec3& pos) { position = pos; }
    void Camera::setRotation(const glm::quat& rot) { 
        rotation = rot;
        glm::vec3 euler = glm::eulerAngles(rotation);
        // GLM returns XYZ = pitch, yaw, roll
        this->yaw   = euler.y;
        this->pitch = euler.x;
        this->roll  = euler.z; 
    }
    void Camera::setOrientation(const glm::vec3& pos, const glm::quat& rot) {setPosition(pos); setRotation(rot); }

    glm::vec3 Camera::getForward() const { return rotation * glm::vec3(0,0,-1); }
    glm::vec3 Camera::getRight() const { return rotation * glm::vec3(1,0,0); }
    glm::vec3 Camera::getUp() const { return rotation * glm::vec3(0,1,0); }

    // ------------------------------------------------------------
    // FPS: WASD + mouse look
    void Camera::updateFPS(const VizContext& ctx, float dt) {
        glm::vec3 dir(0.0f);

        if (ctx.inputState.keys[GLFW_KEY_W]) dir += getForward();
        if (ctx.inputState.keys[GLFW_KEY_S]) dir -= getForward();
        if (ctx.inputState.keys[GLFW_KEY_A]) dir -= getRight();
        if (ctx.inputState.keys[GLFW_KEY_D]) dir += getRight();
        if (ctx.inputState.keys[GLFW_KEY_LEFT_SHIFT]) dir -= getUp();
        if (ctx.inputState.keys[GLFW_KEY_SPACE]) dir += getUp();

        float speed = moveSpeed;
        if(ctx.inputState.keys[GLFW_KEY_LEFT_CONTROL]) speed = fineSpeed;

        if (glm::length(dir) > 0.001f)
            position += glm::normalize(dir) * speed * dt;

        // Mouse look
        if(ctx.mouseCaptured) {
            yaw   += static_cast<float>(ctx.inputState.mouseX - ctx.inputState.prevMouseX) * mouseSpeed;
            pitch += static_cast<float>(ctx.inputState.mouseY - ctx.inputState.prevMouseY) * mouseSpeed;
        }
        roll = 0.0;
        // Limit pitch to avoid gimbal
        if(pitch > glm::half_pi<float>() - 0.01f) pitch = glm::half_pi<float>() - 0.01f;
        if(pitch < -glm::half_pi<float>() + 0.01f) pitch = -glm::half_pi<float>() + 0.01f;

        rotation = glm::quat(glm::vec3(-pitch, -yaw, 0.0f));
    }

    // ------------------------------------------------------------
    // Free Fly: yaw/pitch rotation but unrestricted movement
    void Camera::updateFreeFly(const VizContext& ctx, float dt) {
        glm::vec3 dir(0.0f);

        if (ctx.inputState.keys[GLFW_KEY_W]) dir += getForward();
        if (ctx.inputState.keys[GLFW_KEY_S]) dir -= getForward();
        if (ctx.inputState.keys[GLFW_KEY_A]) dir -= getRight();
        if (ctx.inputState.keys[GLFW_KEY_D]) dir += getRight();
        if (ctx.inputState.keys[GLFW_KEY_LEFT_SHIFT]) dir -= getUp();
        if (ctx.inputState.keys[GLFW_KEY_SPACE]) dir += getUp();
        float speed = moveSpeed;
        if(ctx.inputState.keys[GLFW_KEY_LEFT_CONTROL]) speed = fineSpeed;

        if (glm::length(dir) > 0.001f)
            position += glm::normalize(dir) * speed * dt;

        // Mouse rotation
        float yaw   = static_cast<float>(ctx.inputState.mouseX - ctx.inputState.prevMouseX) * mouseSpeed;
        float pitch = static_cast<float>(ctx.inputState.mouseY - ctx.inputState.prevMouseY) * mouseSpeed;
        float roll = 0;
        if(ctx.inputState.keys[GLFW_KEY_Q]) roll += mouseSpeed * dt * 1000;
        if(ctx.inputState.keys[GLFW_KEY_E]) roll -= mouseSpeed * dt * 1000;
        if(ctx.inputState.keys[GLFW_KEY_LEFT_CONTROL]) roll /= 2;
        if(!ctx.mouseCaptured) {
            pitch = 0;
            yaw = 0;
        }
        rotation = glm::normalize(rotation * glm::quat(glm::vec3(-pitch, -yaw, roll)));
        glm::vec3 euler = glm::eulerAngles(rotation);
        // GLM returns XYZ = pitch, yaw, roll
        this->yaw   = euler.y;
        this->pitch = euler.x;
        this->roll  = euler.z;
} 
       

    // ------------------------------------------------------------
    // Axis-aligned movement: no rotation
    void Camera::updateAxisAligned(const VizContext& ctx, float dt) {
        glm::vec3 dir(0.0f);

        if (ctx.inputState.keys[GLFW_KEY_W]) dir += getForward();
        if (ctx.inputState.keys[GLFW_KEY_S]) dir -= getForward();
        if (ctx.inputState.keys[GLFW_KEY_A]) dir -= getRight();
        if (ctx.inputState.keys[GLFW_KEY_D]) dir += getRight();
        if (ctx.inputState.keys[GLFW_KEY_LEFT_SHIFT]) dir -= getUp();
        if (ctx.inputState.keys[GLFW_KEY_SPACE]) dir += getUp();
        float speed = moveSpeed;
        if(ctx.inputState.keys[GLFW_KEY_LEFT_CONTROL]) speed = fineSpeed;
        
        if (glm::length(dir) > 0.001f)
            position += glm::normalize(dir) * speed * dt;

        float mouseSpeed = this->mouseSpeed / (ctx.inputState.keys[GLFW_KEY_LEFT_CONTROL] ? 2 : 1);
        if(ctx.inputState.keys[GLFW_KEY_Q] && ctx.inputState.keys[GLFW_KEY_E]) roll = 0;
        else if(ctx.inputState.keys[GLFW_KEY_Q]) roll += mouseSpeed * dt * 1000;
        else if(ctx.inputState.keys[GLFW_KEY_E]) roll -= mouseSpeed * dt * 1000;
        bool changedViewSize = false;
        float viewScaleSpeed = ctx.inputState.keys[GLFW_KEY_LEFT_SHIFT] ? 2.0 : 1.0;
        if(ctx.inputState.keys[GLFW_KEY_W]) {
            orthoViewSize -= dt * viewScaleSpeed;
            changedViewSize = true;
            if(orthoViewSize < 0.001) {
                orthoViewSize = 0.01;
            }
        }
        if(ctx.inputState.keys[GLFW_KEY_S]) {
            orthoViewSize += dt * viewScaleSpeed;
            changedViewSize = !changedViewSize;
        }
        if(changedViewSize) setOrthographic(orthoLeft,  orthoRight, orthoBottom, orthoTop, nearPlane, farPlane);

        rotation = glm::angleAxis(roll, glm::vec3(0,0,1));
    }
}