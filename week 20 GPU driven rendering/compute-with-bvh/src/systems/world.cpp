#include "world.h"
#include <cstdlib>
#include <glad/glad.h>
#include <iostream>
#include <chrono>
#include <intrin.h>
#include <taskflow/algorithm/for_each.hpp>

World::World(uint32_t computeShader) {

	this->computeShader = computeShader;
	glUseProgram(computeShader);
	objectCountLocation = glGetUniformLocation(computeShader, "maxObjectCount");
}

void World::build(std::vector<GameObject>& objects, std::vector<Sphere>& spheres) {

	// Reserve Objects
	gameObjectStates.resize(objects.size());
	gameObjectVelocities.resize(objects.size());
	visibleObjects.resize(objects.size());
	objectIDs.resize(objects.size());
	for (size_t i = 0; i < objects.size(); ++i) {
		gameObjectStates[i] = objects[i].state;
		visibleObjects[i] = objects[i].state;
		gameObjectVelocities[i] = objects[i].velocity;
		objectIDs[i] = objects[i].state.id;
	}
	for (size_t i = 0; i < objectTypeCount; ++i) {
		boundingSpheres[i] = spheres[i];
	}

	bvhNodes.resize(2 * gameObjectStates.size() - 1);
	build_bvh();

	uint32_t flags = GL_DYNAMIC_STORAGE_BIT | GL_MAP_WRITE_BIT | GL_MAP_PERSISTENT_BIT;
	size_t size = objects.size() * sizeof(StateObject);
	glGenBuffers(1, &objectBuffer);
	glBindBuffer(GL_SHADER_STORAGE_BUFFER, objectBuffer);
	glBindBuffer(GL_COPY_WRITE_BUFFER, objectBuffer);
	glBufferStorage(GL_SHADER_STORAGE_BUFFER, size, NULL, flags);
	glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 0, objectBuffer);

	taskGraph.clear();
	auto record = taskGraph.for_each_index(0, 32, 1, [this](int i) {record_objects(i); });
}

void World::record_objects(uint32_t jobID) {

	uint32_t batchSize = std::ceil(currentNode.sphereCount / 32.0);
	uint32_t offset = jobID * batchSize;

	for (size_t i = offset; (i < offset + batchSize) && (i < currentNode.sphereCount); ++i) {
		visibleObjects[visibleObjectCount.fetch_add(1)] = gameObjectStates[objectIDs[i + currentNode.leftChild]];
	}
}

void World::build_bvh() {

	//auto start = std::chrono::high_resolution_clock::now();
	BVHNode root;
	nodesUsed = 0;
	root.leftChild = 0;
	root.sphereCount = static_cast<uint32_t>(gameObjectStates.size());
	bvhNodes[nodesUsed++] = root;
	update_bounds(0);
	subdivide(0);
	//auto stop = std::chrono::high_resolution_clock::now();
	//auto duration = std::chrono::duration_cast<std::chrono::microseconds>(stop - start);
	//std::cout << "BVH build took " << duration.count() << " us." << std::endl;
}

void World::update_bounds(uint32_t nodeIndex) {

	BVHNode& node = bvhNodes[nodeIndex];
	uint32_t first = node.leftChild;

	for (uint32_t i = 0; i < node.sphereCount; ++i) {

		uint32_t j = objectIDs[first + i];
		StateObject object = gameObjectStates[j];
		Sphere sphere = boundingSpheres[gameObjectStates[j].objectType];

		glm::vec3 center = sphere.center + object.pos;
		glm::vec3 rod = glm::vec3(1.0f)* sphere.radius;
		glm::vec3 minCorner = center - rod;
		glm::vec3 maxCorner = center + rod;

		node.min = glm::min(node.min, minCorner);
		node.max = glm::max(node.max, maxCorner);
	}
}

void World::subdivide(uint32_t nodeIndex) {
	
	BVHNode& node = bvhNodes[nodeIndex];
	
	if (node.sphereCount <= maxBoxSize) {
		return;
	}

	glm::vec3 extent = node.max - node.min;
	int axis = 0;
	if (extent[1] > extent[axis]) {
		axis = 1;
		if (extent[2] > extent[axis]) {
			axis = 2;
		}
	}
	
	float splitPosition = node.min[axis] + 0.5 * extent[axis];
	// split group into halves
	uint32_t i = node.leftChild;
	uint32_t j = i + node.sphereCount - 1;
	
	while (i <= j) {
		if (gameObjectStates[objectIDs[i]].pos[axis] < splitPosition) {
			i += 1;
		}
		else {
			uint32_t temp = objectIDs[i];
			objectIDs[i] = objectIDs[j];
			objectIDs[j--] = temp;
		}
	}

	uint32_t leftCount = i - node.leftChild;
	if (leftCount == 0 or leftCount == node.sphereCount) {
		return;
	}

	uint32_t leftChildIndex = nodesUsed++;
	BVHNode& leftNode = bvhNodes[leftChildIndex];
	leftNode.leftChild = node.leftChild;
	leftNode.sphereCount = leftCount;
	node.leftChild = leftChildIndex;

	uint32_t rightchildIndex = nodesUsed++;
	BVHNode& rightNode = bvhNodes[rightchildIndex];
	rightNode.leftChild = i;
	rightNode.sphereCount = node.sphereCount - leftCount;

	update_bounds(leftChildIndex);
	subdivide(leftChildIndex);
	update_bounds(rightchildIndex);
	subdivide(rightchildIndex);

	node.sphereCount = 0;
}

uint32_t World::update(float dt, CameraSystem* camera) {

	//auto start = std::chrono::high_resolution_clock::now();

	frametime = dt;

	// Reset state
	visibleObjectCount.store(0);

	// Update all objects
	//executor.run(work).wait();
	
	__m256* stateChunks = (__m256*)gameObjectStates.data();
	__m256* velocityChunks = (__m256*)gameObjectVelocities.data();
	for (size_t i = 0; i < gameObjectStates.size(); ++i) {
		stateChunks[i] = _mm256_add_ps(stateChunks[i], _mm256_mul_ps(_mm256_set1_ps(frametime * 0.001f), velocityChunks[i]));
	}

	build_bvh();

	// Traverse BVH to record (possibly) visible objects
	BVHNode node = bvhNodes[0];
	BVHNode stack[20];
	uint32_t stackPos = 0;

	bool running = true;
	while (running) {

		uint32_t first = node.leftChild;
		uint32_t count = node.sphereCount;

		if (count > 0) {
			// External Node, record all children
			currentNode = node;
			executor.run(taskGraph).wait();
			if (stackPos == 0) {
				break;
			}
			else {
				node = stack[--stackPos];
				continue;
			}
		}
		else {
			// Internal Node, check both children for visibility
			unsigned char visibility = 0;
			BVHNode leftChild = bvhNodes[first];
			BVHNode rightChild = bvhNodes[first + 1];
			visibility |= camera->visible(leftChild);
			visibility |= camera->visible(rightChild) << 1;

			switch (visibility) {
			case 3:
				// Both children are visible
				node = leftChild;
				stack[stackPos++] = rightChild;
				break;
			case 2:
				// Only right child visible
				node = rightChild;
				break;
			case 1:
				// Only left child visible
				node = leftChild;
				break;
			default:
				// Neither children visble
				if (stackPos == 0) {
					running = false;
					break;
				}
				node = stack[--stackPos];
				break;
			}
		}
	}

	// Upload to GPU
	size_t size = visibleObjectCount * sizeof(StateObject);
	glUseProgram(computeShader);
	glUniform1i(objectCountLocation, visibleObjectCount);
	
	void* writeLocation = glMapBufferRange(GL_COPY_WRITE_BUFFER, 0, size, GL_MAP_WRITE_BIT | GL_MAP_UNSYNCHRONIZED_BIT);
	memcpy(writeLocation, visibleObjects.data(), size);
	glUnmapBuffer(GL_COPY_WRITE_BUFFER);

	//auto stop = std::chrono::high_resolution_clock::now();
	//auto duration = std::chrono::duration_cast<std::chrono::microseconds>(stop - start);
	//std::cout << "update took " << duration.count() << " us." << std::endl;

	return visibleObjectCount;
}