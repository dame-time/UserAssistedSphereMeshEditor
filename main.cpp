#include <Window.hpp>
#include <RenderableMesh.hpp>
#include <Shader.hpp>
#include <Camera.hpp>
#include <SphereMesh.hpp>

#include <iostream>

int main()
{
    Renderer::Camera* mainCamera = new Renderer::Camera();
    Renderer::Window* window = new Renderer::Window(1200, 1000, "Custom Renderer", mainCamera);
    
    Renderer::Shader* mainShader = new Renderer::Shader("/Users/davidepaollilo/Workspaces/C++/CustomRenderer/Shader/GLSL/vertex.vert", "/Users/davidepaollilo/Workspaces/C++/CustomRenderer/Shader/GLSL/fragment.frag");
    Renderer::RenderableMesh* mesh = new Renderer::RenderableMesh("/Users/davidepaollilo/Workspaces/C++/Thesis/Assets/Models/foot-500.obj", mainShader);
//    Renderer::RenderableMesh* mesh = new Renderer::RenderableMesh("/Users/davidepaollilo/Workspaces/C++/Thesis/Assets/Models/Capsule.obj", mainShader);
    Renderer::SphereMesh* sm = new Renderer::SphereMesh(mesh);

    sm->renderSpheresOnly();
//    auto& s = mesh->addSubSphere();
//    s.scaleUniform(0.5f);
//    s.translateUniform(-0.5f);
//    mesh->addSubSphere();
//
//    for (int i = 0; i < 15000; i++)
//        mesh->addSubSphere();
    
    window->setShader(mainShader);
    window->setTargetMesh(mesh);
    window->setSphereMesh(sm);
    
    window->render();
    
    delete window;
    return 0;
}
