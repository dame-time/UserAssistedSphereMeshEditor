#include <Window.hpp>
#include <TriMesh.hpp>
#include <Shader.hpp>
#include <Camera.hpp>
#include <SphereMesh.hpp>
#include <Region.hpp>

#include <YAMLUtils.hpp>

#include <iostream>
#include <filesystem>

Renderer::TriMesh* mesh;
Renderer::SphereMesh* sm;
Renderer::Shader* sphereShader;
Renderer::Shader* mainShader;

bool loadCachedResult() {    
    if (std::filesystem::exists(".cache")) {
        std::filesystem::path filePath = std::filesystem::absolute(".cache");
        
        std::string referenceMeshPath = getYAMLRenderableMeshPath(filePath);
        
        mesh = new Renderer::TriMesh(referenceMeshPath, mainShader);
        sm = new Renderer::SphereMesh(mesh, sphereShader);
        
        sm->loadFromYaml(filePath);
        
        return true;
    }
    
    return false;
}

int main()
{
	Renderer::Region::initialize();
	
    auto* mainCamera = new Renderer::Camera();
    auto* window = new Renderer::Window(1200, 1000, "Custom Renderer", mainCamera);
    
    mainShader = new Renderer::Shader("/Users/davidepaollilo/Workspaces/C++/SphereMeshEditor/Shader/GLSL/vertex.vert",
									  "/Users/davidepaollilo/Workspaces/C++/SphereMeshEditor/Shader/GLSL/fragment.frag");
    sphereShader = new Renderer::Shader("/Users/davidepaollilo/Workspaces/C++/SphereMeshEditor/Shader/GLSL/impostor"
										".vert", "/Users/davidepaollilo/Workspaces/C++/SphereMeshEditor/Shader/GLSL"
												 "/impostor"
												 ".frag");
    
    if (!loadCachedResult()) {
//        mesh = new Renderer::TriMesh("/Users/davidepaollilo/Workspaces/C++/SphereMeshEditor"
//											"/Assets/Models/camel-poses"
//											"/camel-reference-4040.obj", mainShader);
	    mesh = new Renderer::TriMesh("/Users/davidepaollilo/Workspaces/C++/SphereMeshEditor/Assets/Models"
											"/smaug"
											".obj", mainShader);
//        mesh = new Renderer::TriMesh("/Users/davidepaollilo/Workspaces/C++/Thesis/Assets/Models/bunny250NH.obj", mainShader);
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
