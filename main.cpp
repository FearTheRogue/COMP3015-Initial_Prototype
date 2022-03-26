#include "helper/scene.h"
#include "helper/scenerunner.h"
#include "scenebasic_uniform.h"
#include "scene_initial_prototype.h"
#include "scene_initial_prototype_hdr.h";


int main(int argc, char* argv[])
{
	SceneRunner runner("Foggy Park");

	std::unique_ptr<Scene> scene;

	scene = std::unique_ptr<Scene>(new Scene_Initial_Prototype_HDR());

	return runner.run(*scene);
}