#pragma once

#include <glad/glad.h>
#include <GLFW/glfw3.h>

#include <Vector3.hpp>

#include <RenderableMesh.hpp>
#include <Shader.hpp>
#include <Camera.hpp>

#include <SphereMesh.hpp>

#include "imgui.h"
#include "imgui_impl_opengl3.h"
#include "imgui_impl_glfw.h"

#include <iostream>

namespace Renderer {
    class Window {
        public:
            static constexpr Math::Scalar UNKNOWN = -666;
            GLFWwindow* window;
        
            RenderableMesh* pickedMesh;
            std::vector<RenderableMesh*> pickedMeshes;
        
            float rotationSensitivity;
            float scrollSpeed;
        
            Window(unsigned int width, unsigned int height, const std::string& title, Camera* mainCamera);
            ~Window();
            
            void setShader(Shader* shader);
            void setTargetMesh(RenderableMesh* targetMesh);
            void setSphereMesh(SphereMesh* sphereMesh);
        
            Math::Vector3 screenPosToObjPos(const Math::Vector3& mouse);
        
            void render();
            
        private:
            unsigned int SCR_WIDTH;
            unsigned int SCR_HEIGHT;
            Renderer::Shader* mainShader;
            Renderer::RenderableMesh* mesh;
            Renderer::SphereMesh* sm;
            Renderer::Camera* mainCamera;
            bool commandPressed;
            float lastX, lastY;
        
            float deltaTime;
            float lastFrame;
        
            void processInput();
        
            void renderImGUI();

            static void framebuffer_size_callback(GLFWwindow* window, int width, int height);
            static void mouse_callback(GLFWwindow* window, double xpos, double ypos);
            static void key_callback(GLFWwindow* window, int key, int scancode, int action, int mods);
    };
}
