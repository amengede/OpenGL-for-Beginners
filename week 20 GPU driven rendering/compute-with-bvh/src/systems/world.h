#pragma once
#include <glm/glm.hpp>
#include <vector>
#include "../components/components.h"
#include "camera.h"
#include <taskflow/taskflow.hpp>

class World {
public:
	World(uint32_t computeShader);
	void build(std::vector<GameObject>& objects, std::vector<AABB>& boxes);
	uint32_t update(float dt, CameraSystem* camera);
	std::vector<BVHNode> nodes;
private:
	uint32_t objectBuffer, computeShader, objectCountLocation;
	std::vector<StateObject> gameObjectStates;
	std::vector<UpdateObject> gameObjectVelocities;
	std::vector<StateObject> visibleObjects;
	uint32_t visibleObjectCount = 0;

	std::vector<uint32_t> objectIDs;
	std::vector<BVHNode> bvhNodes;
	uint32_t nodesUsed = 0;
	Sphere boundingSpheres[objectTypeCount];
	uint32_t maxBoxSize = 256;

	void build_bvh();
	void update_bounds(uint32_t nodeIndex);
	void subdivide(uint32_t nodeIndex);
	void update_region(uint32_t i);

	tf::Executor executor;
	tf::Taskflow work;
	tf::Task parallelJob;
	float frametime;

	//void* writeLocation;
};