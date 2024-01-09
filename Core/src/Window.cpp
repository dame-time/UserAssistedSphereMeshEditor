#include <Window.hpp>

#define IMGUI_ENABLE_DOCKING

#include "imgui.h"
#include "imgui_impl_opengl3.h"
#include "imgui_impl_glfw.h"

#include <tinyfiledialogs.h>

#include <YAMLUtils.hpp>

#include <chrono>

#define DEBUG_CHRONO 1

#define ICON_FA_SAVE "\xEF\x83\x87"
#define STORE_ICON "\xEF\x88\xB3"
#define STORE_TEXT_ICON "\xEF\x80\x8B"
#define UPLOAD_ICON "\xEF\x82\x93"
#define FILE_UPLOAD_ICON "\xEF\x87\x89"
#define SETTINGS_ICON "\xEF\x80\x93"

namespace Renderer {

    int Window::viewportH = 0;
    int Window::viewportW = 0;

    Window::Window(unsigned int SCR_WIDTH, unsigned int SCR_HEIGHT, const std::string& title, Camera* mainCamera)
        : SCR_WIDTH(SCR_WIDTH), SCR_HEIGHT(SCR_HEIGHT),
          commandPressed(false), lastX(0), lastY(0), mainCamera(mainCamera)
    {
        deltaTime = 0.0f;
        lastFrame = 0.0f;
        
        rotationSensitivity = 0.3f;
        scrollSpeed = 1.0f;
        
        renderFullSMWithNSpheres = 0;
        renderConnectivity = false;
        
        connectivitySpheresPerEdge = 0;
        connectivitySpheresSize = 0;
        
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

        ImGui_ImplGlfw_InitForOpenGL(window, true);
        ImGui_ImplOpenGL3_Init();

        ImGui::StyleColorsDark();
        
        isCameraPerspective = true;
        
        ImGuiIO& io = ImGui::GetIO(); (void)io;
        
        io.Fonts->AddFontDefault();

        // Load the FontAwesome icon font
        static const ImWchar icons_ranges[] = { 0xf000, 0xf3ff, 0 }; // Will cover your icon characters
        ImFontConfig icons_config;
        icons_config.MergeMode = true;
        icons_config.PixelSnapH = true;

        std::filesystem::path currentPath = std::filesystem::current_path();
        auto parentPath = currentPath.parent_path().parent_path();

        std::string fontPath = (parentPath.string() + "/Assets/Fonts/fa-Solid-900.ttf");

        if (!std::filesystem::exists(fontPath)) {
            std::cerr << "Font file does not exist!" << std::endl;
        } else {
            io.Fonts->AddFontFromFileTTF(fontPath.c_str(), 16.0f, &icons_config, icons_ranges);
        }

        io.Fonts->Build();
        renderSM = true;
        renderWFmesh = false;
    }

    Window::~Window() {
        glfwTerminate();
    }

    void Window::setMeshShader(Shader* shader) {
        this->mainShader = shader;
    }

    void Window::setSphereMeshShader(Shader* shader) {
        this->sphereShader = shader;
    }

    void Window::setTargetMesh(RenderableMesh *targetMesh) {
        this->mesh = targetMesh;
    }

    void Window::setSphereMesh(SphereMesh* sphereMesh) {
        this->sm = sphereMesh;
        sphereBuffer.clear();
    }

    void Window::render() {
        Math::Scalar cameraDistance = 2 * mesh->getBoundingSphereRadius();
        mainCamera->translate(cameraDistance);
        
        mainCamera->setTarget(mesh->getCentroid());

        while (!glfwWindowShouldClose(window))
        {
            Math::Matrix4 perspective = getProjectionMatrix();
            
            ImGui::GetIO().ConfigFlags |= ImGuiConfigFlags_DockingEnable;
            
            ImGui_ImplOpenGL3_NewFrame();
            ImGui_ImplGlfw_NewFrame();
            ImGui::NewFrame();
            
            ImGuiID dockspace_id = ImGui::DockSpaceOverViewport(ImGui::GetMainViewport(), ImGuiDockNodeFlags_PassthruCentralNode);
            
            ImGui::Begin("Inspector");
                renderImGUI();
            ImGui::End();
            
            ImGui::Begin("Application Stats");
                ImGui::Text("Frames per second: %f", ImGui::GetIO().Framerate);
                ImGui::Text("Mesh Vertices: %lu", mesh->vertices.size());
                ImGui::Text("Sphere Mesh Spheres: %lu", sm->sphere.size());
                ImGui::Text("Sphere Mesh Edges: %d", sm->getEdgeSize());
                ImGui::Text("Sphere Mesh Triangles: %d", sm->getTriangleSize());
                ImGui::Text("Rendered vertices per sphere: %d", sm->getPerSphereVertexCount());
                ImGui::Text("Total vertices rendered: %lu", (sm->getRenderCalls() * sm->getPerSphereVertexCount()) +
                            (mesh->isFilled || mesh->isBlended || mesh->isWireframe ? mesh->vertices.size() : 0));
            ImGui::End();
            sm->resetRenderCalls();
            
            static bool isOpen = true;
            console.Draw("Console", &isOpen);
            
            ImGuiStyle& style = ImGui::GetStyle();
            float originalPaddingY = style.FramePadding.y;

            style.FramePadding.y += 5.0f;

            if (ImGui::BeginMainMenuBar()) {
                renderMenu();
                ImGui::EndMainMenuBar();
            }

            style.FramePadding.y = originalPaddingY;
                
            
            Math::Scalar currentFrame = glfwGetTime();
            deltaTime = currentFrame - lastFrame;
            lastFrame = currentFrame;
            
            processInput();

            glClearColor(1.0f, 1.0f, 1.0f, 1.0f);
            glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
            
            mainShader->use();
            mainShader->setMat4("view", mainCamera->getViewMatrix());
            mainShader->setMat4("projection", perspective);
            
            mainShader->setVec3("light.position", Math::Vector3(-1, 1, 0));
            mainShader->setVec3("light.ambient", Math::Vector3(.5, .5, .5));
            mainShader->setVec3("light.diffuse", Math::Vector3(0.3, 0.3, 0.3));
            mainShader->setVec3("light.specular", Math::Vector3(0.3, 0.3, 0.3));
            
            if (renderSM)
                renderSphereMesh(perspective);
            
            if (renderWFmesh)
            {
                mesh->setFilled(true);
                mesh->render();
                mesh->setWireframe(true);
                mesh->setWireframeColor(Math::Vector3(0, 0, 0));
                mesh->render();
            }
            else
                mesh->render();
            
            
            ImGui::Render();
            ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());

            glfwSwapBuffers(window);
            glfwPollEvents();
        }
    }

    void Window::renderSphereMesh(const Math::Matrix4& perspective)
    {
        sphereShader->use();
        sphereShader->setMat4("view", mainCamera->getViewMatrix());
        sphereShader->setMat4("projection", perspective);
        
        sphereShader->setVec3("light.position", Math::Vector3(-1, 1, 0));
        sphereShader->setVec3("light.ambient", Math::Vector3(.5, .5, .5));
        sphereShader->setVec3("light.diffuse", Math::Vector3(0.3, 0.3, 0.3));
        sphereShader->setVec3("light.specular", Math::Vector3(0.3, 0.3, 0.3));
        
        sm->renderSpheresOnly();
        if (renderVertices)
            for (int i = 0; i < pickedMeshes.size(); i++)
                sm->renderSphereVertices(pickedMeshes[i]->getID());
        
        if (renderFullSMWithNSpheres > 0)
            sm->renderWithNSpherePerEdge(renderFullSMWithNSpheres, sphereSize);
        
        if (renderConnectivity && connectivitySpheresPerEdge == 0)
            sm->renderConnectivity();
        else if (renderConnectivity && connectivitySpheresPerEdge > 0)
            sm->renderConnectivity(connectivitySpheresPerEdge, connectivitySpheresSize);
    }

    void Window::processInput()
    {
        if(glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
            glfwSetWindowShouldClose(window, true);
        
        Math::Scalar cameraSpeed = scrollSpeed;
        if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS)
            if (isCameraPerspective)
                mainCamera->translate(-cameraSpeed * deltaTime);
            else
                mainCamera->scale(-cameraSpeed/10 * deltaTime);
        
        if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS)
            if (isCameraPerspective)
                mainCamera->translate(cameraSpeed * deltaTime);
            else
                mainCamera->scale(cameraSpeed/10 * deltaTime);
        
        if ((glfwGetKey(window, GLFW_KEY_LEFT_CONTROL) == GLFW_PRESS || glfwGetKey(window, GLFW_KEY_RIGHT_CONTROL) == GLFW_PRESS)
            && (glfwGetKey(window, GLFW_KEY_LEFT_SHIFT) == GLFW_PRESS || glfwGetKey(window, GLFW_KEY_RIGHT_SHIFT) == GLFW_PRESS)
            && glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS)
            sm->saveYAML(".", ".cache");
        
        if ((glfwGetKey(window, GLFW_KEY_LEFT_CONTROL) == GLFW_PRESS || glfwGetKey(window, GLFW_KEY_RIGHT_CONTROL) == GLFW_PRESS)
            && (glfwGetKey(window, GLFW_KEY_LEFT_SHIFT) == GLFW_PRESS || glfwGetKey(window, GLFW_KEY_RIGHT_SHIFT) == GLFW_PRESS)
            && glfwGetKey(window, GLFW_KEY_Y) == GLFW_PRESS)
        {
            const char *defaultDescription = "Yaml files";
            const char *defaultExtension = ".yaml";
            const char *filterPatterns[1] = { "*.yaml" };

            const char *selectedSavePath = tinyfd_saveFileDialog("Save sphere mesh as *.yaml file", "", 1, filterPatterns, defaultDescription);

            if (selectedSavePath) {
                std::string savePath(selectedSavePath);

                // Extracting file name from path
                std::filesystem::path pathObj(savePath);
                std::string fileNameStr = pathObj.filename().string();
                if (fileNameStr.size() <= 5 || fileNameStr.substr(fileNameStr.size() - 5) != ".yaml")
                    fileNameStr += ".yaml";
                
                std::cout << "Selected save path: " << pathObj.parent_path().string() << std::endl;

                sm->saveYAML(pathObj.parent_path().string() + "/", fileNameStr);
            } else {
                displayErrorMessage("No path selected for saving!");
            }
        }
            
        if ((glfwGetKey(window, GLFW_KEY_LEFT_CONTROL) == GLFW_PRESS || glfwGetKey(window, GLFW_KEY_RIGHT_CONTROL) == GLFW_PRESS)
            && (glfwGetKey(window, GLFW_KEY_LEFT_SHIFT) == GLFW_PRESS || glfwGetKey(window, GLFW_KEY_RIGHT_SHIFT) == GLFW_PRESS)
            && glfwGetKey(window, GLFW_KEY_T) == GLFW_PRESS)
        {
            const char *defaultDescription = "Txt files";
            const char *defaultExtension = ".txt";
            const char *filterPatterns[1] = { "*.txt" };

            const char *selectedSavePath = tinyfd_saveFileDialog("Save sphere mesh as *.txt file", "", 1, filterPatterns, defaultDescription);

            if (selectedSavePath) {
                std::string savePath(selectedSavePath);

                // Extracting file name from path
                std::filesystem::path pathObj(savePath);
                std::string fileNameStr = pathObj.filename().string();
                if (fileNameStr.size() <= 5 || fileNameStr.substr(fileNameStr.size() - 5) != ".txt")
                    fileNameStr += ".txt";
                
                std::cout << "Selected save path: " << pathObj.parent_path().string() << std::endl;

                sm->saveTXT(pathObj.parent_path().string() + "/", fileNameStr);
            } else {
                displayErrorMessage("No path selected for saving!");
            }
        }
            
        if ((glfwGetKey(window, GLFW_KEY_LEFT_CONTROL) == GLFW_PRESS || glfwGetKey(window, GLFW_KEY_RIGHT_CONTROL) == GLFW_PRESS)
            && (glfwGetKey(window, GLFW_KEY_LEFT_SHIFT) == GLFW_PRESS || glfwGetKey(window, GLFW_KEY_RIGHT_SHIFT) == GLFW_PRESS)
            && glfwGetKey(window, GLFW_KEY_L) == GLFW_PRESS)
        {
            const char *filters[0] = { };
            const char *selectedFilePath = tinyfd_openFileDialog("Select a sphere mesh *.yaml file", "", 0, filters, "Yaml files", 0);

            if (selectedFilePath) {
                std::string filePath(selectedFilePath);
                std::cout << "Selected file path: " << filePath << std::endl;
                
                delete mesh;
                delete sm;
                
                std::string referenceMeshPath = getYAMLRenderableMeshPath(filePath);
                
                mesh = new RenderableMesh(referenceMeshPath, mainShader);
                sm = new Renderer::SphereMesh(mesh, sphereShader);
                
                sm->loadFromYaml(filePath);
            } else {
                displayWarningMessage("No file selected!");
            }
            
            if (sm->getRenderType() == RenderType::SPHERES)
                *sphereShader = Shader("/Users/davidepaollilo/Workspaces/C++/CustomRenderer/Shader/GLSL/sphere.vert", "/Users/davidepaollilo/Workspaces/C++/CustomRenderer/Shader/GLSL/sphere.frag");
            else
                *sphereShader = Shader("/Users/davidepaollilo/Workspaces/C++/CustomRenderer/Shader/GLSL/impostor.vert", "/Users/davidepaollilo/Workspaces/C++/CustomRenderer/Shader/GLSL/impostor.frag");
        }
            
        if ((glfwGetKey(window, GLFW_KEY_LEFT_CONTROL) == GLFW_PRESS || glfwGetKey(window, GLFW_KEY_RIGHT_CONTROL) == GLFW_PRESS)
            && glfwGetKey(window, GLFW_KEY_M) == GLFW_PRESS)
        {
            const char *filters[2] = { "*.obj", "*.OBJ" };
            const char *selectedFilePath = tinyfd_openFileDialog("Select a mesh *.obj file", "", 2, filters, "Object files", 0);

            if (selectedFilePath) {
                std::string filePath(selectedFilePath);
                std::cout << "Selected file path: " << filePath << std::endl;
                
                delete mesh;
                delete sm;
                
                mesh = new RenderableMesh(filePath, mainShader);
                sm = new Renderer::SphereMesh(mesh, sphereShader);
                
                mainCamera->resetRotation();
                mainCamera->resetTranslation();
                
                mainCamera->setTarget(mesh->getCentroid());
            } else {
                displayWarningMessage("No file selected!");
            }
        }
    }

    Math::Vector3 Window::worldPosToClipPos(const Math::Vector3& worldPos) {
        auto translation = pickedMesh->center;
        
        Math::Matrix4 model = Math::Matrix4();
        model.setColumnVector(3, Math::Vector4(translation, 1));
        
        Math::Matrix4 view = mainCamera->getViewMatrix();
        Math::Matrix4 projection = getProjectionMatrix();

        Math::Vector4 clipPos = projection * view * model * Math::Vector4(worldPos, 1.0f);

        clipPos /= clipPos.coordinates.w;

        return Math::Vector3(clipPos.coordinates.x, clipPos.coordinates.y, clipPos.coordinates.z);
    }

    void Window::addSphereVectorToBuffer(const std::vector<Sphere>& spheres) {
        if (sphereBuffer.size() > 30)
            sphereBuffer.erase(sphereBuffer.begin());
        
        sphereBuffer.push_back(spheres);
    }

    void Window::removeLastSphereVectorFromBuffer() {
        sphereBuffer.pop_back();
    }

    void Window::key_callback(GLFWwindow* window, int key, int scancode, int action, int mods)
    {
        Window* windowClassInstance = static_cast<Window*>(glfwGetWindowUserPointer(window));
        
        static bool isFilled = false;
        static bool isBlended = false;
        static bool isWireframe = false;
        
        if (key == GLFW_KEY_TAB && action == GLFW_PRESS) {
            isFilled = windowClassInstance->mesh->isFilled;
            isBlended = windowClassInstance->mesh->isBlended;
            isWireframe = windowClassInstance->mesh->isWireframe;
            
            windowClassInstance->mesh->setFilled(false);
            windowClassInstance->mesh->setBlended(false);
            windowClassInstance->mesh->setWireframe(false);
        }
        
        if (key == GLFW_KEY_TAB && action == GLFW_RELEASE) {
            windowClassInstance->mesh->setFilled(isFilled);
            windowClassInstance->mesh->setBlended(isBlended);
            windowClassInstance->mesh->setWireframe(isWireframe);
        }
        
        if (glfwGetKey(window, GLFW_KEY_C) == GLFW_PRESS && windowClassInstance->pickedMeshes.size() > 1) {
            windowClassInstance->addSphereVectorToBuffer(windowClassInstance->sm->sphere);
            for (auto& m : windowClassInstance->pickedMeshes)
                m->color = Math::Vector3(1, 0, 0);
            
            auto result = windowClassInstance->sm->collapse(windowClassInstance->pickedMeshes[windowClassInstance->pickedMeshes.size() - 1]->getID(), windowClassInstance->pickedMeshes[windowClassInstance->pickedMeshes.size() - 2]->getID());
            windowClassInstance->pickedMeshes.clear();
            windowClassInstance->pickedMesh = nullptr;
            
            if (!result)
                windowClassInstance->displayErrorMessage("Failed to collapse the two spheres");
            
            windowClassInstance->displayLogMessage("Current spheres: " + std::to_string(windowClassInstance->sm->sphere.size()));
        }
        
        if (key == GLFW_KEY_A && action == GLFW_RELEASE && windowClassInstance->pickedMeshes.size() > 1)
        {
            windowClassInstance->addSphereVectorToBuffer(windowClassInstance->sm->sphere);
            for (auto& m : windowClassInstance->pickedMeshes)
                m->color = Math::Vector3(1, 0, 0);
            
            int i = windowClassInstance->pickedMeshes.size();
            while (i - 1 > 0) {
                auto result = windowClassInstance->sm->collapse(windowClassInstance->pickedMeshes[i - 1]->getID(),
                                                                windowClassInstance->pickedMeshes[i - 2]->getID());
                
                if (!result)
                    windowClassInstance->displayErrorMessage("Failed to collapse the two spheres: (" + std::to_string(i) + ", " + std::to_string(i + 1) + ")");
                i -= 2;
            }
            
            windowClassInstance->pickedMeshes.clear();
            windowClassInstance->pickedMesh = nullptr;
            
            windowClassInstance->displayLogMessage("Current spheres: " + std::to_string(windowClassInstance->sm->sphere.size()));
        }
        
        if (key == GLFW_KEY_R && action == GLFW_RELEASE && windowClassInstance->pickedMeshes.size() > 0)
        {
            for (auto& m : windowClassInstance->pickedMeshes)
                m->color = Math::Vector3(1, 0, 0);
            
            windowClassInstance->pickedMesh = nullptr;
            windowClassInstance->pickedMeshes.clear();
        }
        
        if (key == GLFW_KEY_E && action == GLFW_RELEASE)
        {
            windowClassInstance->addSphereVectorToBuffer(windowClassInstance->sm->sphere);
            windowClassInstance->pickedMeshes.clear();
            windowClassInstance->sm->reset();
        }
        
        if (key == GLFW_KEY_V && action == GLFW_PRESS)
        {
            windowClassInstance->renderVertices = !windowClassInstance->renderVertices;
        }
        
        if (key == GLFW_KEY_B && action == GLFW_PRESS)
        {
            if (windowClassInstance->sm->getRenderType() == RenderType::SPHERES) {
                windowClassInstance->sm->setRenderType(RenderType::BILLBOARDS);
                *windowClassInstance->sphereShader = Shader("/Users/davidepaollilo/Workspaces/C++/CustomRenderer/Shader/GLSL/impostor.vert", "/Users/davidepaollilo/Workspaces/C++/CustomRenderer/Shader/GLSL/impostor.frag");
            } else {
                windowClassInstance->sm->setRenderType(RenderType::SPHERES);
                *windowClassInstance->sphereShader = Shader("/Users/davidepaollilo/Workspaces/C++/CustomRenderer/Shader/GLSL/sphere.vert", "/Users/davidepaollilo/Workspaces/C++/CustomRenderer/Shader/GLSL/sphere.frag");
            }
        }
        
        if (key == GLFW_KEY_Z && action == GLFW_PRESS)
        {
            if (windowClassInstance->sphereBuffer.size() > 0) {
                windowClassInstance->sm->sphere = windowClassInstance->sphereBuffer[windowClassInstance->sphereBuffer.size() - 1];
                windowClassInstance->removeLastSphereVectorFromBuffer();
                
                if (windowClassInstance->pickedMesh != nullptr)
                    windowClassInstance->pickedMesh->color = Math::Vector3(1, 0, 0);
                
                windowClassInstance->pickedMesh = nullptr;
            }
        }
        
        if (key == GLFW_KEY_N && action == GLFW_PRESS) {
            if (windowClassInstance->pickedMesh != nullptr){
                windowClassInstance->sm->addEdge(windowClassInstance->pickedMesh->getID());
                windowClassInstance->pickedMesh->color = Math::Vector3(1, 0, 0);
                windowClassInstance->pickedMesh = nullptr;
            }
        }
        
        if (key == GLFW_KEY_T && action == GLFW_PRESS) {
            if (windowClassInstance->pickedMesh != nullptr && windowClassInstance->pickedMeshes.size() > 1){
                windowClassInstance->sm->addTriangle(windowClassInstance->pickedMesh->getID(), windowClassInstance->pickedMeshes[windowClassInstance->pickedMeshes.size() - 2]->getID());
                windowClassInstance->pickedMesh->color = Math::Vector3(1, 0, 0);
                windowClassInstance->pickedMesh = nullptr;
                windowClassInstance->pickedMeshes.pop_back();
                windowClassInstance->pickedMeshes[windowClassInstance->pickedMeshes.size() - 1]->color = Math::Vector3(1, 0, 0);
                windowClassInstance->pickedMeshes.pop_back();
            }
        }
        
        if (key == GLFW_KEY_P && action == GLFW_PRESS) {
            windowClassInstance->isCameraPerspective = !windowClassInstance->isCameraPerspective;
        }
        
        if (key == GLFW_KEY_D && action == GLFW_PRESS) {
            if (windowClassInstance->pickedMesh != nullptr) {
                windowClassInstance->sm->removeSphere(windowClassInstance->pickedMesh->getID());
                windowClassInstance->pickedMesh = nullptr;
                windowClassInstance->pickedMeshes.clear();
            }
        }
    }

    void Window::renderMenu() {
        if (ImGui::BeginMenu("File")) {
            if (ImGui::MenuItem((std::string(ICON_FA_SAVE) + " Cache Sphere Mesh").c_str(), "Ctrl+Shift+S")) {
                sm->saveYAML(".", ".cache");
            }
            
            ImGui::Separator();
            
            if (ImGui::MenuItem((std::string(STORE_ICON) + " Save YAML Sphere Mesh To...").c_str(), "Ctrl+Shift+Y")) {
                const char *defaultDescription = "Yaml files";
                const char *defaultExtension = ".yaml";
                const char *filterPatterns[1] = { "*.yaml" };

                const char *selectedSavePath = tinyfd_saveFileDialog("Save sphere mesh as *.yaml file", "", 1, filterPatterns, defaultDescription);

                if (selectedSavePath) {
                    std::string savePath(selectedSavePath);

                    // Extracting file name from path
                    std::filesystem::path pathObj(savePath);
                    std::string fileNameStr = pathObj.filename().string();
                    if (fileNameStr.size() <= 5 || fileNameStr.substr(fileNameStr.size() - 5) != ".yaml")
                        fileNameStr += ".yaml";
                    
                    std::cout << "Selected save path: " << pathObj.parent_path().string() << std::endl;

                    sm->saveYAML(pathObj.parent_path().string() + "/", fileNameStr);
                } else {
                    displayErrorMessage("No path selected for saving!");
                }
            }
            
            if (ImGui::MenuItem((std::string(STORE_TEXT_ICON) + " Save TXT Sphere Mesh To...").c_str(), "Ctrl+Shift+T")) {
                const char *defaultDescription = "Txt files";
                const char *defaultExtension = ".txt";
                const char *filterPatterns[1] = { "*.txt" };

                const char *selectedSavePath = tinyfd_saveFileDialog("Save sphere mesh as *.txt file", "", 1, filterPatterns, defaultDescription);

                if (selectedSavePath) {
                    std::string savePath(selectedSavePath);

                    // Extracting file name from path
                    std::filesystem::path pathObj(savePath);
                    std::string fileNameStr = pathObj.filename().string();
                    if (fileNameStr.size() <= 5 || fileNameStr.substr(fileNameStr.size() - 5) != ".txt")
                        fileNameStr += ".txt";
                    
                    std::cout << "Selected save path: " << pathObj.parent_path().string() << std::endl;

                    sm->saveTXT(pathObj.parent_path().string() + "/", fileNameStr);
                } else {
                    displayErrorMessage("No path selected for saving!");
                }
            }
            
            ImGui::Separator();
            
            if (ImGui::MenuItem((std::string(UPLOAD_ICON) + " Load YAML Sphere Mesh...").c_str(), "Ctrl+Shift+L")) {
                const char *filters[0] = { };
                const char *selectedFilePath = tinyfd_openFileDialog("Select a sphere mesh *.yaml file", "", 0, filters, "Yaml files", 0);

                if (selectedFilePath) {
                    std::string filePath(selectedFilePath);
                    std::cout << "Selected file path: " << filePath << std::endl;
                    
                    delete mesh;
                    delete sm;
                    
                    std::string referenceMeshPath = getYAMLRenderableMeshPath(filePath);
                    
                    mesh = new RenderableMesh(referenceMeshPath, mainShader);
                    sm = new Renderer::SphereMesh(mesh, sphereShader);
                    
                    sm->loadFromYaml(filePath);
                } else {
                    displayWarningMessage("No file selected!");
                }
                
                if (sm->getRenderType() == RenderType::SPHERES)
                    *sphereShader = Shader("/Users/davidepaollilo/Workspaces/C++/CustomRenderer/Shader/GLSL/sphere.vert", "/Users/davidepaollilo/Workspaces/C++/CustomRenderer/Shader/GLSL/sphere.frag");
                else
                    *sphereShader = Shader("/Users/davidepaollilo/Workspaces/C++/CustomRenderer/Shader/GLSL/impostor.vert", "/Users/davidepaollilo/Workspaces/C++/CustomRenderer/Shader/GLSL/impostor.frag");
            }
            
            ImGui::Separator();
            
            if (ImGui::MenuItem((std::string(FILE_UPLOAD_ICON) + " Load new mesh...").c_str(), "Ctrl+M")) {
                const char *filters[2] = { "*.obj", "*.OBJ" };
                const char *selectedFilePath = tinyfd_openFileDialog("Select a mesh *.obj file", "", 2, filters, "Object files", 0);

                if (selectedFilePath) {
                    std::string filePath(selectedFilePath);
                    std::cout << "Selected file path: " << filePath << std::endl;
                    
                    delete mesh;
                    delete sm;
                    
                    mesh = new RenderableMesh(filePath, mainShader);
                    sm = new Renderer::SphereMesh(mesh, sphereShader);
                    
                    mainCamera->resetRotation();
                    mainCamera->resetTranslation();
                    mainCamera->resetScale();
                    
                    mainCamera->setTarget(mesh->getCentroid());
                } else {
                    displayWarningMessage("No file selected!");
                }
            }
            
            ImGui::EndMenu();
        }
        
        if (ImGui::BeginMenu("Actions")) {
            if (ImGui::MenuItem("Collapse Two Sphere", "C")) {
                if (pickedMesh != nullptr && pickedMeshes.size() > 1) {
                    addSphereVectorToBuffer(sm->sphere);
                    for (auto& m : pickedMeshes)
                        m->color = Math::Vector3(1, 0, 0);
                    
                    auto result = sm->collapse(pickedMeshes[pickedMeshes.size() - 1]->getID(), pickedMeshes[pickedMeshes.size() - 2]->getID());
                    pickedMeshes.clear();
                    pickedMesh = nullptr;
                    
                    if (!result)
                        displayErrorMessage("Failed to collapse the two spheres");
                } else {
                    displayErrorMessage("Not enough spheres selected");
                }
            }
            
            if (ImGui::MenuItem("Collapse All Spheres", "A")) {
                if (pickedMeshes.size() > 1) {
                    addSphereVectorToBuffer(sm->sphere);
                    for (auto& m : pickedMeshes)
                        m->color = Math::Vector3(1, 0, 0);
                    
                    int i = pickedMeshes.size();
                    while (i - 1 > 0) {
                        auto result = sm->collapse(pickedMeshes[i - 1]->getID(),
                                                                        pickedMeshes[i - 2]->getID());
                        
                        if (!result)
                            displayErrorMessage("Failed to collapse the two spheres: (" + std::to_string(i) + ", " + std::to_string(i + 1) + ")");
                        i -= 2;
                    }
                    
                    pickedMeshes.clear();
                    pickedMesh = nullptr;
                }
            }
            
            if (ImGui::MenuItem("Reset Selection", "R")) {
                for (auto& m : pickedMeshes)
                    m->color = Math::Vector3(1, 0, 0);
                
                pickedMesh = nullptr;
                pickedMeshes.clear();
            }
            
            if (ImGui::MenuItem("Toggle selected Spheres Vertices", "V")) {
                renderVertices = !renderVertices;
            }
            
            if (ImGui::MenuItem("Add edge Sphere", "N")) {
                if (pickedMesh != nullptr){
                    sm->addEdge(pickedMesh->getID());
                    pickedMesh->color = Math::Vector3(1, 0, 0);
                    pickedMesh = nullptr;
                }
            }
            
            if (ImGui::MenuItem("Add triangle Sphere", "T")) {
                if (pickedMesh != nullptr && pickedMeshes.size() > 1){
                    sm->addTriangle(pickedMesh->getID(), pickedMeshes[pickedMeshes.size() - 2]->getID());
                    pickedMesh->color = Math::Vector3(1, 0, 0);
                    pickedMesh = nullptr;
                    pickedMeshes.pop_back();
                    pickedMeshes[pickedMeshes.size() - 1]->color = Math::Vector3(1, 0, 0);
                    pickedMeshes.pop_back();
                }
            }
            
            if (ImGui::MenuItem("Delete Sphere", "D")) {
                if (pickedMesh != nullptr) {
                    sm->removeSphere(pickedMesh->getID());
                    pickedMesh = nullptr;
                    pickedMeshes.clear();
                }
            }
            
            if (ImGui::MenuItem("Reset Sphere Mesh", "E")) {
               addSphereVectorToBuffer(sm->sphere);
               pickedMeshes.clear();
               sm->reset();
            }
            
            if (ImGui::MenuItem("UNDO", "Z")) {
                if (sphereBuffer.size() > 0) {
                    sm->sphere = sphereBuffer[sphereBuffer.size() - 1];
                    removeLastSphereVectorFromBuffer();
                    
                    if (pickedMesh != nullptr)
                        pickedMesh->color = Math::Vector3(1, 0, 0);
                    
                    pickedMesh = nullptr;
                }
            }
            
            ImGui::EndMenu();
        }
        
        if (ImGui::BeginMenu("Rendering")) {
            if (ImGui::MenuItem("Toggle camera mode", "P"))
                isCameraPerspective = !isCameraPerspective;
            
            if (ImGui::MenuItem("Toggle rendering mode", "B")) {
                if (sm->getRenderType() == RenderType::SPHERES) {
                    sm->setRenderType(RenderType::BILLBOARDS);
                    *sphereShader = Shader("/Users/davidepaollilo/Workspaces/C++/CustomRenderer/Shader/GLSL/impostor.vert", "/Users/davidepaollilo/Workspaces/C++/CustomRenderer/Shader/GLSL/impostor.frag");
                } else {
                    sm->setRenderType(RenderType::SPHERES);
                    *sphereShader = Shader("/Users/davidepaollilo/Workspaces/C++/CustomRenderer/Shader/GLSL/sphere.vert", "/Users/davidepaollilo/Workspaces/C++/CustomRenderer/Shader/GLSL/sphere.frag");
                }
            }
            
            ImGui::EndMenu();
        }
        
        if (ImGui::BeginMenu("?")) {
            ImGui::Text("Left click over a sphere in order to select it");
            ImGui::Text("Right click over a sphere in order to scale it");
            ImGui::Text("Hold LEFT SHIFT while dragging your mouse in order to translate a sphere over the XY plane");
            ImGui::Text("Hold RIGHT SHIFT while dragging your mouse in order to translate a sphere over the Z axis");
            ImGui::Text("Hold TAB to toggle on and off the core mesh");
            ImGui::Text("Press 'P' to switch camera from projection to perspective");
            ImGui::Text("Press 'C' to collapse the last two spheres selected");
            ImGui::Text("Press 'A' to collapse all the spheres selected");
            ImGui::Text("Press 'V' to visualize the vertices of the selected spheres");
            ImGui::Text("Press 'R' to reset the selection");
            ImGui::Text("Press 'E' to reset the Sphere Mesh to the original Sphere Mesh");
            ImGui::Text("Press 'W' to increase zoom percentage");
            ImGui::Text("Press 'S' to increase zoom percentage");
            ImGui::Text("Press 'Z' to undo up to 50 actions!");
            ImGui::Text("Press 'B' to toggle between billboards and concrete spheres");
            ImGui::Text("Press 'N' after selecting a sphere to create an edge with a new sphere");
            ImGui::Text("Press 'T' after selecting two spheres to create a triangle with a new mid point sphere");
            ImGui::Text("Press 'D' to delete a sphere (NOT UNDOABLE)");
            ImGui::EndMenu();
        }
        
        if (ImGui::BeginMenu(std::string(SETTINGS_ICON).c_str())) {
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
            
            static float scroll = 1.0f;
            ImGui::SliderFloat("Scroll Speed", &scroll, 0.0f, 10.0f);
            scrollSpeed = scroll;
            
            
            static float rotationSpeed = 0.3f;
            ImGui::SliderFloat("Rotation Speed", &rotationSpeed, 0.0f, 1.0f);
            rotationSensitivity = rotationSpeed;
            
            ImGui::EndMenu();
        }
    }

    void Window::displayErrorMessage(const std::string& message) {
        ImVec4 redColor = ImVec4(1.0f, 0.0f, 0.0f, 1.0f);
        console.AddLog(message, redColor);
    }

    void Window::displayWarningMessage(const std::string& message) {
        ImVec4 yellowColor = ImVec4(1.0f, 1.0f, 0.0f, 1.0f);
        console.AddLog(message, yellowColor);
    }

    void Window::displayLogMessage(const std::string& message) {
        ImVec4 whiteColor = ImVec4(1.0f, 1.0f, 1.0f, 1.0f);
        console.AddLog(message, whiteColor);
    }

    void Window::framebuffer_size_callback(GLFWwindow* window, int width, int height)
    {
        viewportH = height;
        viewportW = width;
        
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
        Math::Matrix4 projection = getProjectionMatrix();
        Math::Vector3 wincoord = Math::Vector3(scaled_mouse_x, viewport[3] - scaled_mouse_y, mouse.coordinates.z);
        Math::Vector3 objcoord = Math::Vector3::unProject(wincoord, view, projection, Math::Vector4(viewport[0], viewport[1], viewport[2], viewport[3]));

        return Math::Vector3(objcoord.coordinates.x, objcoord.coordinates.y, objcoord.coordinates.z);
    }

    Math::Matrix4 Window::getProjectionMatrix() const
    {
//        if (isCameraPerspective)
//            std::cout << "Perspective" << std::endl;
//        else
//            std::cout << "Orthogonal" << std::endl;
        
        if (viewportW == 0 || viewportH == 0)
        {
            viewportW = this->SCR_WIDTH;
            viewportH = this->SCR_HEIGHT;
        }
        
//        std::cout << "Viewport: " << viewportW << "x" << viewportH << std::endl;
        return isCameraPerspective ? mainCamera->getPerspectiveMatrix(90.0, Math::Scalar(viewportW) / viewportH, 0.1, 1000.0) :
        mainCamera->getOrthographicMatrix(viewportW, viewportH, 0.1, 1000.0);
    }

    void Window::mouse_callback(GLFWwindow* window, double xpos, double ypos)
    {
        Window* windowClassInstance = static_cast<Window*>(glfwGetWindowUserPointer(window));
        static GLfloat depth;
        
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
            
            if (windowClassInstance->pickedMesh != nullptr)
                windowClassInstance->pickedMesh->color = Math::Vector3(0.5, 0.5, 0);
            
            Math::Scalar minDistance = DBL_MAX;
            for (int i = 0; i < windowClassInstance->sm->sphere.size(); i++) {
                auto check = (pickedPoint - windowClassInstance->sm->sphere[i].center).magnitude() - windowClassInstance->sm->sphere[i].radius;

                if (check < threshold) {
                    if (check < minDistance) {
                        windowClassInstance->pickedMesh = &windowClassInstance->sm->sphere[i];
                        minDistance = check;
                    }
                }
            }
            
            if (windowClassInstance->pickedMesh != nullptr) {
                windowClassInstance->pickedMesh->color = Math::Vector3(1, 1, 0);
                
                bool isPresent = false;
                for (int i = 0; i < windowClassInstance->pickedMeshes.size(); i++)
                    if((windowClassInstance->pickedMeshes[i]->center - windowClassInstance->pickedMesh->center).magnitude() < Math::EPSILON &&
                       std::abs(windowClassInstance->pickedMeshes[i]->radius - windowClassInstance->pickedMesh->radius) < Math::EPSILON) {
                        isPresent = true;
                        continue;
                    }
                        
                if (!isPresent)
                    windowClassInstance->pickedMeshes.push_back(windowClassInstance->pickedMesh);
            }
        }
        
        static bool isRightPressed = false;
        static double pickX = 0.0;
        static double pickY = 0.0;
        if(glfwGetMouseButton(window, GLFW_MOUSE_BUTTON_RIGHT) == GLFW_PRESS && windowClassInstance->pickedMesh != nullptr) {
            if (!isRightPressed) {
                windowClassInstance->addSphereVectorToBuffer(windowClassInstance->sm->sphere);
                isRightPressed = true;
                pickX = xpos;
                pickY = ypos;
            }
            
            Math::Scalar xoffset = pickX - xpos;

            pickX = xpos;

            Math::Scalar sensitivity = 0.01;

            xoffset *= sensitivity;
            
            Math::Scalar radius = windowClassInstance->pickedMesh->radius;
            if (radius >= 0) {
                windowClassInstance->pickedMesh->radius = Math::Math::clamp(0.01, DBL_MAX, radius - xoffset);
            }
        }
        
        if(glfwGetMouseButton(window, GLFW_MOUSE_BUTTON_RIGHT) == GLFW_RELEASE)
            isRightPressed = false;
        
        static bool isLeftShiftPressed = false;
        static double translateX = 0.0;
        static double translateY = 0.0;
        static Math::Vector3 initialWorldPoint;
        if((glfwGetKey(window, GLFW_KEY_LEFT_SHIFT) == GLFW_PRESS || glfwGetKey(window, GLFW_KEY_RIGHT_SHIFT) == GLFW_PRESS)
           && windowClassInstance->pickedMesh != nullptr) {
            if (!isLeftShiftPressed) {
                windowClassInstance->addSphereVectorToBuffer(windowClassInstance->sm->sphere);
                isLeftShiftPressed = true;
                translateX = xpos;
                translateY = ypos;
                
                initialWorldPoint = windowClassInstance->screenPosToObjPos(Math::Vector3(xpos, ypos, depth));
            }
            
            Math::Vector3 endWorldPoint = windowClassInstance->screenPosToObjPos(Math::Vector3(xpos, ypos, depth));
            Math::Vector3 delta = endWorldPoint - initialWorldPoint;
            
            windowClassInstance->pickedMesh->center += delta;
            initialWorldPoint = endWorldPoint;
        }
        else
            isLeftShiftPressed = false;

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
        static bool blended = true;
        static bool filled = false;
        static bool wireframe = false;
        
        if (sm->getRenderType() != RenderType::BILLBOARDS)
            if (ImGui::Checkbox("Wireframe", &wireframe)) {
                mesh->setWireframe(wireframe);
                
                if (wireframe) {
                    filled = false;
                    blended = false;
                }
            }
        
        if (ImGui::Checkbox("Filled", &filled)) {
            mesh->setFilled(filled);
            
            if (filled) {
                wireframe = false;
                blended = false;
            }
        }
        
        if (ImGui::Checkbox("Blended", &blended)) {
            mesh->setBlended(blended);
            
            if (blended) {
                wireframe = false;
                filled = false;
            }
        }
        
        ImGui::Separator();
        
        static float epsilon = 0.04f;
        ImGui::PushItemWidth(80);
        ImGui::SliderFloat("Slider", &epsilon, 0.0001f, 1.0f, "%.4f");
        ImGui::PopItemWidth();
        ImGui::SameLine();
        ImGui::PushItemWidth(120);
        ImGui::InputFloat("Input", &epsilon, 0.0001f, 1.0f, "%.4f");
        ImGui::PopItemWidth();
        ImGui::SameLine();
        
        epsilon = Math::Math::clamp01(epsilon);

        if (ImGui::Button("Set EPSILON"))
            sm->setEpsilon(static_cast<Math::Scalar>(epsilon));

        
        ImGui::Separator();
        
        if (ImGui::Button("Collapse Edge"))
        {
            auto result = sm->collapseSphereMesh();
            
            if (!result)
                displayErrorMessage("Could not find any good sphere to collapse");
            
            sm->renderSpheresOnly();
        }
        
        static int j = 0;
        ImGui::PushItemWidth(120);
        ImGui::InputInt("n Spheres to Reach", &j);
        ImGui::PopItemWidth();
        
        ImGui::SameLine();
        
        if (ImGui::Button("Collapse"))
        {
            int initialSpheres = (int)sm->sphere.size();
#if DEBUG_CHRONO == 1
            auto start = std::chrono::high_resolution_clock::now();
#endif
            
            auto result = sm->collapseSphereMesh(j);
            
#if DEBUG_CHRONO == 1
            auto stop = std::chrono::high_resolution_clock::now();
            auto duration = std::chrono::duration_cast<std::chrono::microseconds>(stop - start);
            
            displayLogMessage("Duration: " + std::to_string(duration.count() / 1e6));
#endif
            
            if (!result)
                displayErrorMessage("Could not find any good sphere to collapse,\nspheres collapsed: " + std::to_string(initialSpheres - sm->sphere.size()));
            
            sm->renderSpheresOnly();
        }
        
        static int k = 0;
        ImGui::PushItemWidth(120);
        ImGui::InputInt("n Spheres to Collapse", &k);
        ImGui::PopItemWidth();
        
        ImGui::SameLine();
        
        if (ImGui::Button(("Collapse " + std::to_string(k)).c_str()))
        {
            for (int i = 0; i < k; i++)
                if (!sm->collapseSphereMesh()) {
                    std::cerr << "Could not find any good sphere to collapse,\nspheres collapsed: " << i << std::endl;
                    break;
                }
            sm->renderSpheresOnly();
        }
        
        if (ImGui::Button("Select Best BF Sphere Mesh Edge"))
        {
            sm->renderSelectedSpheresOnly();
        }
        
        ImGui::Separator();
        
        if (ImGui::Button("Collapse Edge FAST"))
        {
            auto result = sm->collapseSphereMeshFast();

            if (!result)
                displayErrorMessage("Could not find any good sphere to collapse");
            
            sm->renderSpheresOnly();
        }
        
        static int f = 0;
        ImGui::PushItemWidth(120);
        ImGui::InputInt("n Spheres to Collapse FAST", &f);
        ImGui::PopItemWidth();
        
        ImGui::SameLine();
        
        if (ImGui::Button("Collapse FAST"))
        {
            for (int i = 0; i < f; i++)
                if (!sm->collapseSphereMeshFast())
                {
                    std::cerr << "Could not find any good sphere to collapse,\nspheres collapsed: " << i << std::endl;
                    break;
                }
            sm->renderSpheresOnly();
        }
        
        static int d = 0;
        ImGui::PushItemWidth(120);
        ImGui::InputInt("n Spheres to Reach FAST", &d);
        ImGui::PopItemWidth();
        
        ImGui::SameLine();
        
        if (ImGui::Button(("Collapse " + std::to_string(d) + " FAST").c_str()))
        {
            int initialSpheres = sm->sphere.size();
            auto result = sm->collapseSphereMeshFast(d);
            
            if (!result)
                displayErrorMessage("Could not find any good sphere to collapse,\nspheres collapsed: " + std::to_string(initialSpheres - sm->sphere.size()));
            sm->renderSpheresOnly();
        }
        
        if (ImGui::Button("Select Fast Best Sphere Mesh Edge"))
        {
            sm->renderFastSelectedSpheresOnly();
        }
        
        ImGui::Separator();
        
        static int n = 1;
        ImGui::PushItemWidth(120);
        ImGui::InputInt("Spheres x Edge", &n);
        ImGui::PopItemWidth();
        
        Math::Math::clamp(1, 50, n);
        
        ImGui::SameLine();
        static float sphereSizes = 1.0f;
        ImGui::PushItemWidth(80);
        ImGui::SliderFloat("Size", &sphereSizes, 0.f, 1.0f, "%.4f");
        ImGui::PopItemWidth();
        ImGui::SameLine();
        
        if (sphereSize != sphereSizes)
            sphereSize = sphereSizes;
        
        if (ImGui::Button("Render"))
        {
            sm->renderWithNSpherePerEdge(n, sphereSizes, 0.05);
            renderFullSMWithNSpheres = n;
        }
        
        if (ImGui::Button("Reset Full Sphere Mesh Size"))
            sphereSizes = 1;
        
        if (ImGui::Button("Render Sphere Only Sphere Mesh"))
        {
            renderFullSMWithNSpheres = 0;
            sm->renderSpheresOnly();
        }
        
        if (ImGui::Button("Clear Sphere Mesh"))
        {
            renderFullSMWithNSpheres = 0;
        }
        
        if (ImGui::Button("Toggle Sphere Mesh"))
        {
            renderSM = !renderSM;
        }
        
        if (ImGui::Button("Render Wireframe Filled Mesh"))
        {
            renderWFmesh = !renderWFmesh;
            if (!renderWFmesh)
            {
                mesh->setWireframe(false);
                mesh->setBlended(true);
            }
        }
    }
}
