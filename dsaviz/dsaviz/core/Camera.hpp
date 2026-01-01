#pragma once
#include <dsaviz/core/VizContext.hpp>
#include <dsaviz/util/glm.hpp>
#include <string>

namespace dsaviz {

    enum class CameraMode {
        FPS,
        FreeFly,
        AxisAligned
    };

    enum class ProjectionType {
        Perspective,
        Orthographic
    };

    class Camera {
    public:
        Camera();

        void setMode(VizContext &ctx, CameraMode mode);

        // Set projection
        void setPerspective(float fovY, float aspect, float nearPlane, float farPlane);
        void setOrthographic(float left, float right, float bottom, float top, float nearPlane, float farPlane);

        // Update camera state from input + deltaTime
        void update(const VizContext& ctx);
        void updateAspectRatio(int width, int height);

        // Get view / projection matrices
        const glm::mat4& getViewMatrix() const;
        const glm::mat4& getProjectionMatrix() const;
        glm::mat4 getViewProjectionMatrix() const;

        // Helpers
        glm::vec3 getForward() const;
        glm::vec3 getRight() const;
        glm::vec3 getUp() const;


        // Accessors
        const glm::vec3& getPosition() const;
        const glm::quat& getRotation() const;

        // Optional: set position or rotation directly
        void setPosition(const glm::vec3& pos);
        void setRotation(const glm::quat& rot);
        void setOrientation(const glm::vec3& pos, const glm::quat& rot);

        std::string toString() const;

    private:
        void updateFPS(const VizContext& ctx, float deltaTime);
        void updateFreeFly(const VizContext& ctx, float deltaTime);
        void updateAxisAligned(const VizContext& ctx, float deltaTime);
        // State
        CameraMode mode;
        ProjectionType projectionType;

        glm::vec3 position;
        glm::quat rotation;

        // Projection params
        float fovY, aspect, nearPlane, farPlane;
        float orthoLeft, orthoRight, orthoBottom, orthoTop;

        glm::mat4 viewMatrix;
        glm::mat4 projectionMatrix;

        // Internal: yaw/pitch for mouse rotation
        float yaw;   // around Y axis
        float pitch; // around X axis
        float roll;
        float orthoViewSize;
        // Settings
        float moveSpeed;     // units per second
        float fineSpeed;
        float mouseSpeed;    // radians per pixel
    };
}
