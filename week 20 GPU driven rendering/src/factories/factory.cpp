#include "factory.h"

Factory::Factory() {
	for (size_t i = 0; i < objectTypeCount; ++i) {
		objectCounts[i] = 0;
	}
}

GameObject Factory::make_object() {

	GameObject object;

	constexpr float radius = 50.0f;
	object.pos_and_scale[0] = radius * (2.0f * random_float() - 1.0f);
	object.pos_and_scale[1] = radius * (2.0f * random_float() - 1.0f);
	object.pos_and_scale[2] = radius * (2.0f * random_float() - 1.0f);
	object.pos_and_scale[3] = 1.8f * random_float() + 0.2f;

	object.euler_and_zero[0] = 360.0f * random_float();
	object.euler_and_zero[1] = 360.0f * random_float();
	object.euler_and_zero[2] = 360.0f * random_float();
	object.euler_and_zero[3] = 0.0f;

	constexpr float maxSpeed = 10.0f;
	object.angular_velocity[0] = maxSpeed * (2.0f * random_float() - 1.0f);
	object.angular_velocity[1] = maxSpeed * (2.0f * random_float() - 1.0f);
	object.angular_velocity[2] = maxSpeed * (2.0f * random_float() - 1.0f);

	object.objectType = random_int_in_range(objectTypeCount);

	objectCounts[object.objectType] += 1;

	return object;
}