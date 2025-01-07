#include "factory.h"

Factory::Factory() {
	for (size_t i = 0; i < objectTypeCount; ++i) {
		objectCounts[i] = 0;
	}
}

GameObject Factory::make_object() {

	GameObject object;

	constexpr float radius = 50.0f;
	object.state.pos[0] = radius * (2.0f * random_float() - 1.0f);
	object.state.pos[1] = radius * (2.0f * random_float() - 1.0f);
	object.state.pos[2] = radius * (2.0f * random_float() - 1.0f);
	object.state.id = objectsMade++;

	object.state.eulers[0] = 360.0f * random_float();
	object.state.eulers[1] = 360.0f * random_float();
	object.state.eulers[2] = 360.0f * random_float();
	object.state.objectType = random_int_in_range(objectTypeCount);

	objectCounts[object.state.objectType] += 1;

	object.velocity.dPos[0] = 0.0f;
	object.velocity.dPos[1] = 0.0f;
	object.velocity.dPos[2] = 0.0f;

	constexpr float maxSpeed = 10.0f;
	object.velocity.dEulers[0] = maxSpeed * (2.0f * random_float() - 1.0f);
	object.velocity.dEulers[1] = maxSpeed * (2.0f * random_float() - 1.0f);
	object.velocity.dEulers[2] = maxSpeed * (2.0f * random_float() - 1.0f);

	return object;
}