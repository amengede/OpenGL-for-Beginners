#include "factory.h"

Factory::Factory(
    ComponentSet<PhysicsComponent>& physicsComponents,
    ComponentSet<RenderComponent>& renderComponents,
    ComponentSet<TransformComponent>& transformComponents,
    ComponentSet<AnimationComponent>& animationComponents,
	ComponentSet<CameraComponent>& cameraComponents):
physicsComponents(physicsComponents),
renderComponents(renderComponents),
transformComponents(transformComponents),
animationComponents(animationComponents),
cameraComponents(cameraComponents) {
}

unsigned int Factory::allocate_id() {
        
	if (garbage_bin.size() > 0) {
		uint32_t id = garbage_bin[garbage_bin.size() - 1];
		garbage_bin.pop_back();
		return id;
	}
	else {
		return entities_made++;
	}
}

void Factory::make_camera(glm::vec3 position, glm::vec3 eulers) {

	unsigned int id = allocate_id();

    TransformComponent transform;
    transform.position = position;
    transform.eulers = eulers;

    transformComponents.insert(id, transform);

	CameraComponent camera;
    cameraComponents.insert(id, camera);
}

void Factory::destroy_camera(unsigned int id) {

    transformComponents.remove(id);

    cameraComponents.remove(id);

	garbage_bin.push_back(id);
}

void Factory::make_cube(glm::vec3 position, glm::vec3 eulers, 
    glm::vec3 eulerVelocity) {

	unsigned int id = allocate_id();

	TransformComponent transform;
	transform.position = position;
	transform.eulers = eulers;
	transformComponents.insert(id, transform);

	PhysicsComponent physics;
	physics.velocity = {0.0f, 0.0f, 0.0f};
	physics.eulerVelocity = eulerVelocity;
	physicsComponents.insert(id, physics);
	
	RenderComponent render;
	render.objectType = ObjectType::eBox;
	render.animationType = AnimationType::eNone;
	renderComponents.insert(id, render);
}

void Factory::make_revy(glm::vec3 position, glm::vec3 eulers) {

	unsigned int id = allocate_id();

	TransformComponent transform;
	transform.position = position;
	transform.eulers = eulers;
	transformComponents.insert(id, transform);

	AnimationComponent animation;
	animation.frame = 0;
	animation.speed = 0.4f;
	animation.frameCount = 19;
	animationComponents.insert(id, animation);
	
	RenderComponent render;
	render.objectType = ObjectType::eRevy;
	render.animationType = AnimationType::eRun;
	renderComponents.insert(id, render);
}