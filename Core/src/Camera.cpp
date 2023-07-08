#include <Camera.hpp>

#include <Vector4.hpp>

namespace Renderer {
    Camera::Camera() {
        yRotation = Math::Matrix4();
        xRotation = Math::Matrix4();
        originTranslation = Math::Matrix4();
        
        targetTranslation = Math::Matrix4();
        
        ro = 0;
        phi = 0;
        theta = 0;
    }

    void Camera::setTarget(const Math::Vector3& target) {
        targetTranslation.setColumnVector(3, Math::Vector4(-target, 1));
    }

    void Camera::rotateAroundY(Math::Scalar delta) {
        phi += delta;
    }

    void Camera::rotateAroundX(Math::Scalar delta) {
        theta += delta;
        
        theta = Math::Math::clamp(-90.0, 90.0, theta);
    }

    void Camera::resetRotation() {
        phi = 0;
        theta = 0;
    }

    void Camera::translate(Math::Scalar delta) {
        ro += delta;
    }

    void Camera::resetTranslation() {
        ro = 0;
    }

    Math::Matrix4 Camera::getViewMatrix() {
        Math::Scalar phiRadians = Math::Math::degreeToRandiansAngle(phi);
        Math::Scalar thetaRadians = Math::Math::degreeToRandiansAngle(theta);
        
        originTranslation = Math::Matrix4();
        originTranslation.setColumnVector(3, Math::Vector4(0, 0, -ro, 1));
        
        xRotation = Math::Matrix4();
        xRotation.setColumnVector(0, Math::Vector4(1, 0, 0, 0));
        xRotation.setColumnVector(1, Math::Vector4(0, std::cos(-thetaRadians), std::sin(-thetaRadians), 0));
        xRotation.setColumnVector(2, Math::Vector4(0, -std::sin(-thetaRadians), std::cos(-thetaRadians), 0));
        
        yRotation = Math::Matrix4();
        yRotation.setColumnVector(0, Math::Vector4(std::cos(-phiRadians), 0, -std::sin(-phiRadians), 0));
        yRotation.setColumnVector(1, Math::Vector4(0, 1, 0, 0));
        yRotation.setColumnVector(2, Math::Vector4(std::sin(-phiRadians), 0, std::cos(-phiRadians), 0));
        
        return originTranslation * xRotation * yRotation * targetTranslation;
    }

    Math::Matrix4 Camera::getPerspectiveMatrix(Math::Scalar fov, Math::Scalar aspect, Math::Scalar near, Math::Scalar far) {
        Math::Matrix4 perspectiveMatrix = Math::Matrix4();
        
        Math::Scalar tanHalfFOV = std::tan(Math::Math::degreeToRandiansAngle(fov / 2.0));

        perspectiveMatrix.data[0] = 1.0 / (aspect * tanHalfFOV);
        perspectiveMatrix.data[5] = 1.0 / tanHalfFOV;
        perspectiveMatrix.data[10] = -(far + near) / (far - near);
        perspectiveMatrix.data[14] = -(2.0 * far * near) / (far - near);
        perspectiveMatrix.data[14] = -1.0;
        
        return perspectiveMatrix;
    }
}
