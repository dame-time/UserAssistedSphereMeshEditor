#include <Window.hpp>
#include <RenderableMesh.hpp>
#include <Shader.hpp>
#include <Camera.hpp>
#include <SphereMesh.hpp>

#include <YAMLUtils.hpp>

#include <iostream>
#include <filesystem>

Renderer::RenderableMesh* mesh;
Renderer::SphereMesh* sm;
Renderer::Shader* sphereShader;
Renderer::Shader* mainShader;

bool loadCachedResult() {    
    if (std::filesystem::exists(".cache")) {
        std::filesystem::path filePath = std::filesystem::absolute(".cache");
        
        std::string referenceMeshPath = getYAMLRenderableMeshPath(filePath);
//        ShaderPath shaderPath = getYAMLSphereShaderPath(filePath);
//        sphereShader = new Renderer::Shader(shaderPath.vertexShaderPath.c_str(), shaderPath.fragmentShaderPath.c_str());
        
        mesh = new Renderer::RenderableMesh(referenceMeshPath, mainShader);
        sm = new Renderer::SphereMesh(mesh, sphereShader);
        
        sm->loadFromYaml(filePath);
        
        return true;
    } else {
        std::cout << "No file selected!" << std::endl;
    }
    
    return false;
}

int main()
{
    Renderer::Camera* mainCamera = new Renderer::Camera();
    Renderer::Window* window = new Renderer::Window(1200, 1000, "Custom Renderer", mainCamera);
    
    mainShader = new Renderer::Shader("/Users/davidepaollilo/Workspaces/C++/CustomRenderer/Shader/GLSL/vertex.vert", "/Users/davidepaollilo/Workspaces/C++/CustomRenderer/Shader/GLSL/fragment.frag");
    sphereShader = new Renderer::Shader("/Users/davidepaollilo/Workspaces/C++/CustomRenderer/Shader/GLSL/sphere.vert", "/Users/davidepaollilo/Workspaces/C++/CustomRenderer/Shader/GLSL/sphere.frag");
//    sphereShader = new Renderer::Shader("/Users/davidepaollilo/Workspaces/C++/CustomRenderer/Shader/GLSL/impostor.vert", "/Users/davidepaollilo/Workspaces/C++/CustomRenderer/Shader/GLSL/impostor.frag");
    
    if (!loadCachedResult()) {
        mesh = new Renderer::RenderableMesh("/Users/davidepaollilo/Workspaces/C++/Thesis/Assets/Models/foot-500.obj", mainShader);
        sm = new Renderer::SphereMesh(mesh, sphereShader);
    }
    
    window->setMeshShader(mainShader);
    window->setSphereMeshShader(sphereShader);
    
    window->setTargetMesh(mesh);
    window->setSphereMesh(sm);
    
    window->render();
    
    delete window;
    return 0;
}
