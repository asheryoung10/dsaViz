#pragma once
#include <glm/glm.hpp>
#include <glm/gtc/quaternion.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <dsaViz/dsaContext.hpp>
#include <string>

namespace dsaViz {

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

        void setMode(DSAContext &ctx, CameraMode mode);

        // Set projection
        void setPerspective(float fovY, float aspect, float nearPlane, float farPlane);
        void setOrthographic(float left, float right, float bottom, float top, float nearPlane, float farPlane);

        // Update camera state from input + deltaTime
        void update(const DSAContext& ctx);

        // Get view / projection matrices
        const glm::mat4& getViewMatrix() const;
        const glm::mat4& getProjectionMatrix() const;
        glm::mat4 getViewProjectionMatrix() const;

        // Accessors
        const glm::vec3& getPosition() const;
        const glm::quat& getRotation() const;

        // Optional: set position or rotation directly
        void setPosition(const glm::vec3& pos);
        void setRotation(const glm::quat& rot);
        void setOrientation(const glm::vec3& pos, const glm::quat& rot);

        std::string toString() const;

    private:
        void updateFPS(const DSAContext& ctx, float deltaTime);
        void updateFreeFly(const DSAContext& ctx, float deltaTime);
        void updateAxisAligned(const DSAContext& ctx, float deltaTime);

        // Helpers
        glm::vec3 getForward() const;
        glm::vec3 getRight() const;
        glm::vec3 getUp() const;

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

        // Settings
        float moveSpeed;     // units per second
        float fineSpeed;
        float mouseSpeed;    // radians per pixel
    };
}
