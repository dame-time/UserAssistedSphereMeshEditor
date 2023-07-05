#include <Window.hpp>

#include "imgui.h"
#include "imgui_impl_opengl3.h"
#include "imgui_impl_glfw.h"

namespace Renderer {
    Window::Window(unsigned int SCR_WIDTH, unsigned int SCR_HEIGHT, const std::string& title, Camera* mainCamera)
        : SCR_WIDTH(SCR_WIDTH), SCR_HEIGHT(SCR_HEIGHT),
          commandPressed(false), lastX(0), lastY(0), mainCamera(mainCamera)
    {
        deltaTime = 0.0f;
        lastFrame = 0.0f;
        
        rotationSensitivity = 0.3f;
        scrollSpeed = 1.0f;
        
        glfwInit();
        glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
        glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
        glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

    #ifdef __APPLE__
        glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
    #endif

        window = glfwCreateWindow(SCR_WIDTH, SCR_HEIGHT, title.c_str(), NULL, NULL);
        if (window == NULL)
        {
            std::cout << "Failed to create GLFW window" << std::endl;
            glfwTerminate();
            throw std::runtime_error("Failed to create GLFW window");
        }
        
        glfwMakeContextCurrent(window);
        glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);
        glfwSetWindowUserPointer(window, this);
        glfwSetCursorPosCallback(window, mouse_callback);
        glfwSetKeyCallback(window, key_callback);
        
        if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress))
        {
            std::cout << "Failed to initialize GLAD" << std::endl;
            glfwTerminate();
            throw std::runtime_error("Failed to initialize GLAD");
        }
        
        glEnable(GL_DEPTH_TEST);
        
        IMGUI_CHECKVERSION();
        ImGui::CreateContext();
        ImGuiIO& io = ImGui::GetIO(); (void)io;

        ImGui_ImplGlfw_InitForOpenGL(window, true);
        ImGui_ImplOpenGL3_Init();

        ImGui::StyleColorsDark();
    }

    Window::~Window() {
        glfwTerminate();
    }

    void Window::setShader(Shader* shader) {
        this->mainShader = shader;
    }

    void Window::setTargetMesh(RenderableMesh *targetMesh) {
        this->mesh = targetMesh;
    }

    void Window::setSphereMesh(SphereMesh* sphereMesh) {
        this->sm = sphereMesh;
    }

    void Window::render() {
        Math::Scalar cameraDistance = 2 * mesh->getBoundingSphereRadius();
        mainCamera->translate(cameraDistance);
        
        mainCamera->setTarget(mesh->getCentroid());
        
        Math::Matrix4 perspective = mainCamera->getPerspectiveMatrix(90.0, 16.0 / 9.0, 0.1, 100000.0);

        while (!glfwWindowShouldClose(window))
        {
            ImGui_ImplOpenGL3_NewFrame();
            ImGui_ImplGlfw_NewFrame();
            ImGui::NewFrame();
            
            renderImGUI();
            
            Math::Scalar currentFrame = glfwGetTime();
            deltaTime = currentFrame - lastFrame;
            lastFrame = currentFrame;
            
            processInput();

            glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
            glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
            
            mainShader->use();
            mainShader->setMat4("view", mainCamera->getViewMatrix());
            mainShader->setMat4("projection", perspective);
            
            mainShader->setVec3("light.position", Math::Vector3(-1, 1, 0));
            mainShader->setVec3("light.ambient", Math::Vector3(.5, .5, .5));
            mainShader->setVec3("light.diffuse", Math::Vector3(0.3, 0.3, 0.3));
            mainShader->setVec3("light.specular", Math::Vector3(0.3, 0.3, 0.3));

            mesh->render();
            
            ImGui::Render();
            ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());

            glfwSwapBuffers(window);
            glfwPollEvents();
        }
    }

    void Window::processInput()
    {
        if(glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
            glfwSetWindowShouldClose(window, true);
        
        Math::Scalar cameraSpeed = scrollSpeed;
        if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS)
            mainCamera->translate(-cameraSpeed * deltaTime);
        if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS)
            mainCamera->translate(cameraSpeed * deltaTime);
        
    }

    void Window::key_callback(GLFWwindow* window, int key, int scancode, int action, int mods)
    {
        Window* windowClassInstance = static_cast<Window*>(glfwGetWindowUserPointer(window));
        
        if (glfwGetKey(window, GLFW_KEY_C) == GLFW_PRESS && windowClassInstance->pickedMeshes.size() > 1) {
            windowClassInstance->sm->collapse(windowClassInstance->pickedMeshes[windowClassInstance->pickedMeshes.size() - 1]->getID(), windowClassInstance->pickedMeshes[windowClassInstance->pickedMeshes.size() - 2]->getID());
            windowClassInstance->sm->clearRenderedMeshes();
            windowClassInstance->sm->renderSpheresOnly();
            windowClassInstance->pickedMeshes.clear();
            windowClassInstance->pickedMesh = nullptr;
        }
        
        if (key == GLFW_KEY_A && action == GLFW_RELEASE && windowClassInstance->pickedMeshes.size() > 1)
        {
            int i = windowClassInstance->pickedMeshes.size();
            while (i - 1 > 0) {
                windowClassInstance->sm->collapse(windowClassInstance->pickedMeshes[i - 1]->getID(), windowClassInstance->pickedMeshes[i - 2]->getID());
                i -= 2;
            }
            
            windowClassInstance->sm->clearRenderedMeshes();
            windowClassInstance->sm->renderSpheresOnly();
            
            windowClassInstance->pickedMeshes.clear();
            windowClassInstance->pickedMesh = nullptr;
        }
        
        if (key == GLFW_KEY_R && action == GLFW_RELEASE && windowClassInstance->pickedMeshes.size() > 0)
        {
            for (auto& m : windowClassInstance->pickedMeshes)
                m->setUniformColor(Math::Vector3(1, 1, 1));
            
            windowClassInstance->pickedMesh = nullptr;
            windowClassInstance->pickedMeshes.clear();
        }
        
        if (key == GLFW_KEY_E && action == GLFW_RELEASE)
        {
            windowClassInstance->pickedMeshes.clear();
            windowClassInstance->sm->clearRenderedMeshes();
            windowClassInstance->sm->reset();
            windowClassInstance->sm->renderSpheresOnly();
        }
        
        static bool status = false;
        
        if (key == GLFW_KEY_V && action == GLFW_RELEASE)
        {
            status = !status;
        }
        
        if (key == GLFW_KEY_V && action == GLFW_PRESS && windowClassInstance->pickedMeshes.size() > 0)
        {
            if (!status) {
                for (int i = 0; i < windowClassInstance->pickedMeshes.size(); i++)
                    windowClassInstance->sm->renderSphereVertices(windowClassInstance->pickedMeshes[i]->getID());
            }
            else {
                windowClassInstance->sm->clearRenderedSphereVertices();
            }
        }
    }

    void Window::framebuffer_size_callback(GLFWwindow* window, int width, int height)
    {
        glViewport(0, 0, width, height);
    }

    Math::Vector3 Window::screenPosToObjPos(const Math::Vector3& mouse) {
        // Get the size of the window (in screen coordinates)
        int window_width, window_height;
        glfwGetWindowSize(window, &window_width, &window_height);
        
        // Get the size of the framebuffer (in pixels)
        int framebuffer_width, framebuffer_height;
        glfwGetFramebufferSize(window, &framebuffer_width, &framebuffer_height);

        // Calculate the DPI scaling factor
        float dpi_scaling_factor_x = (float)framebuffer_width / (float)window_width;
        float dpi_scaling_factor_y = (float)framebuffer_height / (float)window_height;
        
        // Scale the mouse coordinates to account for DPI scaling
        Math::Scalar scaled_mouse_x = mouse.coordinates.x * dpi_scaling_factor_x;
        Math::Scalar scaled_mouse_y = mouse.coordinates.y * dpi_scaling_factor_y;

        // Get the viewport
        GLint viewport[4];
        glGetIntegerv(GL_VIEWPORT, viewport);

        // Unproject this point back into the world
        Math::Matrix4 view = mainCamera->getViewMatrix();
        Math::Matrix4 projection = mainCamera->getPerspectiveMatrix(90.0, 16.0 / 9.0, 0.1, 100000.0);
        Math::Vector3 wincoord = Math::Vector3(scaled_mouse_x, viewport[3] - scaled_mouse_y, mouse.coordinates.z);
        Math::Vector3 objcoord = Math::Vector3::unProject(wincoord, view, projection, Math::Vector4(viewport[0], viewport[1], viewport[2], viewport[3]));

        return Math::Vector3(objcoord.coordinates.x, objcoord.coordinates.y, objcoord.coordinates.z);
    }

    void Window::mouse_callback(GLFWwindow* window, double xpos, double ypos)
    {
        Window* windowClassInstance = static_cast<Window*>(glfwGetWindowUserPointer(window));
        GLfloat depth;
        
        Math::Vector3 pickedPoint = Math::Vector3();
        
        if(glfwGetMouseButton(window, GLFW_MOUSE_BUTTON_LEFT) == GLFW_PRESS) {
            const Math::Scalar threshold = 1;
            
            // Get the size of the window (in screen coordinates)
            int window_width, window_height;
            glfwGetWindowSize(window, &window_width, &window_height);
            
            // Get the size of the framebuffer (in pixels)
            int framebuffer_width, framebuffer_height;
            glfwGetFramebufferSize(window, &framebuffer_width, &framebuffer_height);

            // Calculate the DPI scaling factor
            float dpi_scaling_factor_x = (float)framebuffer_width / (float)window_width;
            float dpi_scaling_factor_y = (float)framebuffer_height / (float)window_height;
            
            // Scale the mouse coordinates to account for DPI scaling
            Math::Scalar scaled_mouse_x = xpos * dpi_scaling_factor_x;
            Math::Scalar scaled_mouse_y = ypos * dpi_scaling_factor_y;
            
            GLint viewport[4];
            glGetIntegerv(GL_VIEWPORT, viewport);
            
            glReadPixels(scaled_mouse_x, viewport[3] - scaled_mouse_y, 1, 1, GL_DEPTH_COMPONENT, GL_FLOAT, &depth);
            pickedPoint = windowClassInstance->screenPosToObjPos(Math::Vector3(xpos, ypos, depth));
            
            Math::Scalar minDistance = DBL_MAX;
            for (int i = 0; i < windowClassInstance->mesh->subSpheres.size(); i++) {
                auto check = (pickedPoint - windowClassInstance->mesh->subSpheres[i].getPosition()).magnitude() - windowClassInstance->mesh->subSpheres[i].getScale().coordinates.x;
                
                if (check < threshold) {
                    if (check < minDistance) {
                        windowClassInstance->pickedMesh = windowClassInstance->mesh->getSubSphere(i);
                        
                        minDistance = check;
                    }
                }
            }
            
            if (windowClassInstance->pickedMesh != nullptr) {
                windowClassInstance->pickedMesh->setUniformColor(Math::Vector3(1, 0, 0));
                
                bool isPresent = false;
                for (int i = 0; i < windowClassInstance->pickedMeshes.size(); i++)
                    if(windowClassInstance->pickedMeshes[i]->getID() == windowClassInstance->pickedMesh->getID()) {
                        isPresent = true;
                        continue;
                    }
                        
                if (!isPresent)
                    windowClassInstance->pickedMeshes.push_back(windowClassInstance->pickedMesh); // TODO: Use this in order to select spheres to collapse
            }
        }
        
        static bool isRightPressed = false;
        static double pickX = 0.0;
        static double pickY = 0.0;
        if(glfwGetMouseButton(window, GLFW_MOUSE_BUTTON_RIGHT) == GLFW_PRESS && windowClassInstance->pickedMesh != nullptr) {
            if (!isRightPressed) {
                isRightPressed = true;
                pickX = xpos;
                pickY = ypos;
            }
            
            Math::Scalar xoffset = pickX - xpos;

            pickX = xpos;

            Math::Scalar sensitivity = 0.01;

            xoffset *= sensitivity;
            
            Math::Vector3 scale = windowClassInstance->pickedMesh->getScale();
            if (scale.coordinates.x >= 0) {
                windowClassInstance->pickedMesh->scale(Math::Vector3(scale.coordinates.x - xoffset, scale.coordinates.y - xoffset, scale.coordinates.z - xoffset));
                if (windowClassInstance->pickedMesh->getScale().coordinates.x < 0.01)
                    windowClassInstance->pickedMesh->scale(Math::Vector3(0.01, 0.01, 0.01));
            }
                
        }
        
        if(glfwGetMouseButton(window, GLFW_MOUSE_BUTTON_RIGHT) == GLFW_RELEASE)
            isRightPressed = false;
        
        static bool isLeftShiftPressed = false;
        static double translateX = 0.0;
        static double translateY = 0.0;
        static Math::Vector3 initialWorldPoint;
        if(glfwGetKey(window, GLFW_KEY_LEFT_SHIFT) == GLFW_PRESS && windowClassInstance->pickedMesh != nullptr) {
            if (!isLeftShiftPressed) {
                isLeftShiftPressed = true;
                translateX = xpos;
                translateY = ypos;
                
                // Unproject to get the initial world point
                initialWorldPoint = windowClassInstance->screenPosToObjPos(Math::Vector3(xpos, ypos, depth));
            }
            
            Math::Scalar xoffset = translateX - xpos;
            Math::Scalar yoffset = translateY - ypos;

            translateX = xpos;
            translateY = ypos;

            Math::Scalar sensitivity = 0.01;

            xoffset *= sensitivity;
            yoffset *= sensitivity;
            
            Math::Vector3 oldPosition = windowClassInstance->pickedMesh->getPosition();
            windowClassInstance->pickedMesh->translate(Math::Vector3(oldPosition.coordinates.x - xoffset, oldPosition.coordinates.y + yoffset, oldPosition.coordinates.z));
        }
        
        if(glfwGetKey(window, GLFW_KEY_LEFT_SHIFT) == GLFW_RELEASE)
            isLeftShiftPressed = false;
        
        static bool isRightShiftPressed = false;
        static double translateZ = 0.0;
        if(glfwGetKey(window, GLFW_KEY_RIGHT_SHIFT) == GLFW_PRESS && windowClassInstance->pickedMesh != nullptr) {
            if (!isRightShiftPressed) {
                isRightShiftPressed = true;
                translateZ = xpos;
            }
            
            Math::Scalar zoffset = translateZ - xpos;

            translateZ = xpos;

            Math::Scalar sensitivity = 0.01;

            zoffset *= sensitivity;
            
            Math::Vector3 oldPosition = windowClassInstance->pickedMesh->getPosition();
            windowClassInstance->pickedMesh->translate(Math::Vector3(oldPosition.coordinates.x, oldPosition.coordinates.y, oldPosition.coordinates.z - zoffset));
        }
        
        if(glfwGetKey(window, GLFW_KEY_RIGHT_SHIFT) == GLFW_RELEASE)
            isRightShiftPressed = false;

        if(glfwGetKey(window, GLFW_KEY_LEFT_SUPER) == GLFW_PRESS || glfwGetKey(window, GLFW_KEY_RIGHT_SUPER) == GLFW_PRESS) {
            if (!windowClassInstance->commandPressed) {
                windowClassInstance->commandPressed = true;
                windowClassInstance->lastX = xpos;
                windowClassInstance->lastY = ypos;
            }

            Math::Scalar xoffset = windowClassInstance->lastX - xpos;
            Math::Scalar yoffset = windowClassInstance->lastY - ypos;

            windowClassInstance->lastX = xpos;
            windowClassInstance->lastY = ypos;

            Math::Scalar sensitivity = windowClassInstance->rotationSensitivity;

            xoffset *= sensitivity;
            yoffset *= sensitivity;

            windowClassInstance->mainCamera->rotateAroundX(yoffset);
            windowClassInstance->mainCamera->rotateAroundY(xoffset);
        } else {
            windowClassInstance->commandPressed = false;
        }
    }

    void Window::renderImGUI() {
        static float color[3] = { 1.0f, 1.0f, 1.0f };

        ImVec4 col = ImVec4(color[0], color[1], color[2], 1.0f);
        if (ImGui::ColorButton("Mesh Color##3c", col, ImGuiColorEditFlags_NoTooltip)) {
            ImGui::OpenPopup("Mesh Color##3c");
        }
        ImGui::SameLine();
        ImGui::Text("Mesh Color");

        if (ImGui::BeginPopup("Mesh Color##3c")) {
            if (ImGui::ColorPicker3("Mesh Color##3c", color)) {
                mesh->setUniformColor(Math::Vector3(color[0], color[1], color[2]));
            }
            ImGui::EndPopup();
        }
        
        static float wireframeColor[3] = { 1.0f, 1.0f, 1.0f };
        ImVec4 c = ImVec4(wireframeColor[0], wireframeColor[1], wireframeColor[2], 1.0f);
        if (ImGui::ColorButton("Wireframe Color##3c", c, ImGuiColorEditFlags_NoTooltip)) {
            ImGui::OpenPopup("Wireframe Color##3c");
        }
        ImGui::SameLine();
        ImGui::Text("Wireframe Color");

        if (ImGui::BeginPopup("Wireframe Color##3c")) {
            if (ImGui::ColorPicker3("Wireframe Color##3c", wireframeColor)) {
                mesh->setWireframeColor(Math::Vector3(wireframeColor[0], wireframeColor[1], wireframeColor[2]));
            }
            ImGui::EndPopup();
        }

        ImGui::Separator();
        
        static bool checkboxValue = false;
        if (ImGui::Checkbox("Wireframe", &checkboxValue))
            mesh->setWireframe(checkboxValue);
        
        static bool checkboxVal = true;
        if (ImGui::Checkbox("Filled", &checkboxVal))
            mesh->setFilled(checkboxVal);
        
        ImGui::Separator();
        
        static float scroll = 1.0f;
        ImGui::SliderFloat("Scroll Speed", &scroll, 0.0f, 10.0f);
        scrollSpeed = scroll;
        
        ImGui::Separator();
        
        static float rotationSpeed = 0.3f;
        ImGui::SliderFloat("Rotation Speed", &rotationSpeed, 0.0f, 1.0f);
        rotationSensitivity = rotationSpeed;
        
        // Create a button
        if (ImGui::Button("Collapse Best BF Sphere Mesh Edge"))
        {
            sm->clearRenderedMeshes();
            sm->collapseSphereMesh();
            sm->renderSpheresOnly();
        }
        
        static int j = 0;
        ImGui::PushItemWidth(120);
        ImGui::InputInt("Number Of Spheres to Reach After Collapse", &j);
        ImGui::PopItemWidth();
        
        ImGui::SameLine();
        
        if (ImGui::Button("Render Sphere BF Mesh Vetices Collapsed"))
        {
            sm->clearRenderedMeshes();
            sm->collapseSphereMesh(j);
            sm->renderSpheresOnly();
        }
        
        static int k = 0;
        ImGui::PushItemWidth(120);
        ImGui::InputInt("Number Of Spheres to Collapse", &k);
        ImGui::PopItemWidth();
        
        ImGui::SameLine();
        
        if (ImGui::Button("Render BF Sphere Mesh Vetices"))
        {
            sm->clearRenderedMeshes();
            for (int i = 0; i < k; i++)
                sm->collapseSphereMesh();
            sm->renderSpheresOnly();
        }
        
        if (ImGui::Button("Select Best BF Sphere Mesh Edge"))
        {
            sm->renderSelectedSpheresOnly();
        }
        
        ImGui::Separator();
        
        if (ImGui::Button("Collapse Best Fast Sphere Mesh Edge"))
        {
            sm->clearRenderedMeshes();
            sm->collapseSphereMeshFast();
            sm->renderSpheresOnly();
        }
        
        static int f = 0;
        ImGui::PushItemWidth(120);
        ImGui::InputInt("Number Of Fast Spheres to Collapse", &f);
        ImGui::PopItemWidth();
        
        ImGui::SameLine();
        
        if (ImGui::Button("Render Fast Sphere Mesh Vetices"))
        {
            sm->clearRenderedMeshes();
            for (int i = 0; i < f; i++)
                sm->collapseSphereMeshFast();
            sm->renderSpheresOnly();
        }
        
        static int d = 0;
        ImGui::PushItemWidth(120);
        ImGui::InputInt("Number Of Fast Spheres to Reach After Collapse", &d);
        ImGui::PopItemWidth();
        
        ImGui::SameLine();
        
        if (ImGui::Button("Render Fast Sphere Mesh Vetices Collapsed"))
        {
            sm->clearRenderedMeshes();
            sm->collapseSphereMeshFast(d);
            sm->renderSpheresOnly();
        }
        
        if (ImGui::Button("Select Fast Best Sphere Mesh Edge"))
        {
            sm->renderFastSelectedSpheresOnly();
        }
        
        ImGui::Separator();
        
        static int n = 1;
        ImGui::PushItemWidth(120);
        ImGui::InputInt("Spheres Per Edge", &n);
        ImGui::PopItemWidth();
        
        Math::Math::clamp(1, 20, n);
        
        ImGui::SameLine();
        
        if (ImGui::Button("Render N Full Sphere Mesh"))
        {
            sm->clearRenderedMeshes();
            sm->renderWithNSpherePerEdge(n);
        }
        
        if (ImGui::Button("Render Full Sphere Mesh"))
        {
            sm->render();
        }
        
        if (ImGui::Button("Render Sphere Only Sphere Mesh"))
        {
            sm->renderSpheresOnly();
        }
        
        if (ImGui::Button("Render Connectivity of Sphere Mesh"))
        {
            sm->renderConnectivity();
        }
        
        if (ImGui::Button("Clear Connectivity of Sphere Mesh"))
        {
            sm->clearRenderedEdges();
        }
        
        if (ImGui::Button("Clear Sphere Mesh"))
        {
            sm->clearRenderedMeshes();
        }
        
        ImGui::Separator();
        
//        if (ImGui::Button("Save Sphere Mesh To YAML"))
//        {
//            sm->saveYAML();
//        }
        
        if (ImGui::Button("Save Sphere Mesh To TXT"))
        {
            sm->saveTXT();
        }
        
        ImGui::Separator();
        
        ImGui::Text("Left click over a sphere in order to select it");
        ImGui::Text("Right click over a sphere in order to scale it");
        ImGui::Text("Hold LEFT SHIFT while dragging your mouse in order to translate a sphere over the XY plane");
        ImGui::Text("Hold RIGHT SHIFT while dragging your mouse in order to translate a sphere over the Z axis");
        ImGui::Text("Press 'C' to collapse the last two spheres selected");
        ImGui::Text("Press 'A' to collapse all the spheres selected");
        ImGui::Text("Press 'V' to visualize the vertices of the selected spheres");
        ImGui::Text("Press 'R' to reset the selection");
        ImGui::Text("Press 'E' to reset the Sphere Mesh to the original Sphere Mesh");
//        ImGui::Text("Press 'F' to toggle the filling of the mesh");
//        ImGui::Text("Press 'W' to toggle the wireframe of the mesh");
        ImGui::Text("Press 'W' to increase zoom percentage");
        ImGui::Text("Press 'S' to increase zoom percentage");
    }
}
