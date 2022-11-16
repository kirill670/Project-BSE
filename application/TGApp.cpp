#define SPR_NO_DEBUG_OUTPUT 1
#include "TGApp.hpp"
#include "module/TGAppGUI.hpp"
#include <IO/IOModule.hpp>
#include <TGEngine.hpp>
#include <Util.hpp>
#include <glm/glm.hpp>
#include <glm/gtx/rotate_vector.hpp>
#include <glslang/Public/ShaderLang.h>
#include <glslang/SPIRV/GlslangToSpv.h>
#include <graphics/GameGraphicsModule.hpp>
#include "module/TGAppIO.hpp"
#include <array>
#include <cmath>
#include <limits>
#include <string>
#include "module/NifLoader.hpp"
#include <glm/glm.hpp>
#include <iostream>

#undef min
#undef max

using namespace tge::main;
using namespace tge::graphics;
using namespace tge;

bool finishedLoading = false;
std::mutex waitMutex;

int initTGEditor(const int count, const char **strings)
{
	waitMutex.lock();
	lateModules.push_back(guiModul);
	lateModules.push_back(ioModul);	
	lateModules.push_back(tge::nif::nifModule);

	const auto initResult = init();
	if (initResult != main::Error::NONE)
	{
		printf("Error in init!");
		return -1;
	}
	waitMutex.unlock();
	auto api = getAPILayer();

	ioModul->ggm = getGameGraphicsModule();
	guiModul->api = api;
	guiModul->ggm = ioModul->ggm;
	const auto extent = api->getRenderExtent();
	ioModul->ggm->updateViewMatrix(glm::perspective(glm::radians(45.0f), extent.x / extent.y, 0.01f, 10000.0f));

	auto &light = guiModul->light;
	light.color = glm::vec3(1, 1, 1);
	light.pos = glm::vec3(0, 10, 0);
	light.intensity = 1.0f;
	guiModul->lightID = api->pushLights(1, &light);

	finishedLoading = true;
	const auto startResult = start();
	if (startResult != main::Error::NONE)
	{
		printf("Error in start!");
		return -1;
	}
	finishedLoading = false;
	return (uint32_t)startResult;
}

bool isFinished() {
	return finishedLoading;
}

void waitFinishedInit() {
	std::lock_guard lg(waitMutex);
}
