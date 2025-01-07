#pragma once
#include <glm/glm.hpp>
#include <vector>
#include "../components/components.h"
#include "camera.h"
#include <taskflow/taskflow.hpp>
#include <taskflow/algorithm/for_each.hpp>
#include <atomic>

class World {
public:
	World(uint32_t computeShader);
	void build(std::vector<GameObject>& objects, std::vector<Sphere>& spheres);
	uint32_t update(float dt, CameraSystem* camera);
	std::vector<BVHNode> nodes;
private:
	uint32_t computeShader, objectCountLocation;
	std::vector<StateObject> gameObjectStates;
	std::vector<UpdateObject> gameObjectVelocities;
	std::vector<StateObject> visibleObjects;
	std::atomic<uint32_t> visibleObjectCount = 0;

	std::vector<uint32_t> objectIDs;
	std::vector<BVHNode> bvhNodes;
	uint32_t nodesUsed = 0;
	Sphere boundingSpheres[objectTypeCount];
	uint32_t maxBoxSize = 1024;

	void build_bvh();
	void update_bounds(uint32_t nodeIndex);
	void subdivide(uint32_t nodeIndex);

	tf::Executor executor;
	tf::Taskflow taskGraph;
	float frametime;
	const uint32_t jobCount = 10;
	BVHNode currentNode;
	void record_objects(uint32_t jobID);
};