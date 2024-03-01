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
#include <utility>

namespace Renderer {
    struct Console
    {
        char InputBuf[256]{};
        struct LogItem {
            std::string Text;
            ImVec4 Color;
            LogItem(std::string text, const ImVec4& color) : Text(std::move(text)), Color(color) {}
        };
        std::vector<LogItem> Items;

        Console()
        {
            ClearLog();
        }

        void ClearLog()
        {
            Items.clear();
        }

        void AddLog(const std::string& log, ImVec4 color = ImVec4(1.0f, 1.0f, 1.0f, 1.0f))
        {
            Items.emplace_back(log, color);
        }

        void Draw(const char* title, bool* p_open)
        {
            ImGui::Begin(title, p_open);
            if (ImGui::Button("Clear")) ClearLog();
            ImGui::SameLine();
            bool copy = ImGui::Button("Copy");
            ImGui::Separator();
            
            ImGui::BeginChild("scrolling", ImVec2(0,0), false, ImGuiWindowFlags_HorizontalScrollbar);
            if (copy) ImGui::LogToClipboard();

            for (auto & Item : Items) {
                ImGui::PushStyleColor(ImGuiCol_Text, Item.Color);
                ImGui::TextUnformatted(Item.Text.c_str());
                ImGui::PopStyleColor();
            }

            ImGui::SetScrollHereY(1.0f);
            ImGui::EndChild();

            ImGui::End();
        }
    };

    class Window {
        public:
            static constexpr Math::Scalar UNKNOWN = -666;
            GLFWwindow* window;
        
            Sphere* pickedMesh{};
            std::vector<Sphere*> pickedMeshes;
        
            float rotationSensitivity;
        
            Window(unsigned int width, unsigned int height, const std::string& title, Camera* mainCamera);
            ~Window();
            
            void setMeshShader(Shader* shader);
            void setSphereMeshShader(Shader* shader);
            void setTargetMesh(RenderableMesh* targetMesh);
            void setSphereMesh(SphereMesh* sphereMesh);
        
            Math::Vector3 worldPosToClipPos(const Math::Vector3& worldPos);
            Math::Vector3 screenPosToObjPos(const Math::Vector3& mouse);
        
            void render();
            
        private:
            Console console;
        
            static int viewportH;
            static int viewportW;
        
            unsigned int SCR_WIDTH;
            unsigned int SCR_HEIGHT;
            Renderer::Shader* mainShader{};
            Renderer::Shader* sphereShader{};
            Renderer::RenderableMesh* mesh{};
            Renderer::SphereMesh* sm{};
            Renderer::Camera* mainCamera;
            bool commandPressed;
            Math::Scalar lastX, lastY;
        
            bool renderSM;
            bool renderWFmesh;
        
            bool isCameraPerspective;
        
            bool renderVertices{};
            int renderFullSMWithNSpheres;
            bool renderConnectivity;
            float sphereSize{};
        
            std::vector<std::vector<Sphere>> sphereBuffer;
            
            int connectivitySpheresPerEdge;
            Math::Scalar connectivitySpheresSize;
        
            Math::Scalar deltaTime;
            Math::Scalar lastFrame;
        
            void processInput();
        
            void renderImGUI();
            void renderMenu();
            void renderSphereMesh(const Math::Matrix4& perspective);
        
            void addSphereVectorToBuffer(const std::vector<Sphere>& spheres);
            void removeLastSphereVectorFromBuffer();
        
            void displayErrorMessage(const std::string& message);
            void displayWarningMessage(const std::string& message);
            void displayLogMessage(const std::string& message);
        
            [[nodiscard]] Math::Matrix4 getProjectionMatrix() const;

            static void framebuffer_size_callback(GLFWwindow* window, int width, int height);
            static void mouse_callback(GLFWwindow* window, double xpos, double ypos);
            static void key_callback(GLFWwindow* window, int key, int scancode, int action, int mods);
    };
}
