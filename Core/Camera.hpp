#ifndef CAMERA_HPP
#define CAMERA_HPP

#include <Vector3.hpp>
#include <Matrix4.hpp>

namespace Renderer {
    class Camera {
    private:
        Math::Scalar phi;
        Math::Scalar theta;
        Math::Scalar ro;
        
        Math::Matrix4 yRotation;
        Math::Matrix4 xRotation;
        Math::Matrix4 originTranslation;
        
        Math::Matrix4 targetTranslation;
    
    public:
        Camera();
        
        void setTarget(const Math::Vector3& target);
        
        void rotateAroundY(Math::Scalar delta);
        void rotateAroundX(Math::Scalar delta);
        void resetRotation();
        
        void translate(Math::Scalar delta);
        void resetTranslation();
        
        Math::Matrix4 getViewMatrix();
        Math::Matrix4 getPerspectiveMatrix(Math::Scalar fov, Math::Scalar aspect, Math::Scalar near, Math::Scalar far);
    };
}

#endif
