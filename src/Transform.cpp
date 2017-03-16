#include "Transform.h"
#include <string>

Transform::Transform()
{
	dirtyModel = dirtyDirs = true;

	yaw = 0;
	pitch = 0;
	roll = 0;

	pos = vec3(0, 0, 0);
	size = vec3(1, 1, 1);
}

void Transform::LookAt(vec3 target)
{
	forward = normalize(target - pos);
	// why does it have to be inverted and offset by Pi/2?
	pitch = -degrees(asin(forward.y));
	yaw = -degrees(atan2(forward.z, forward.x)) + 90;
	roll = 0;

	UpdateRot();
}

void Transform::UpdateRot()
{
	rotM = rotate(mat4(1), radians(yaw),   vec3(0, 1, 0));
	rotM = rotate(rotM,    radians(pitch), vec3(1, 0, 0));
	rotM = rotate(rotM,    radians(roll),  vec3(0, 0, 1));

	// why does this has to be inverted?
	right   = rotM * vec4(-1, 0, 0, 0);
	up      = rotM * vec4(0, 1, 0, 0);
	forward = rotM * vec4(0, 0, 1, 0);

	dirtyDirs = false;
}

void Transform::UpdateModel(vec3 center)
{
	if (dirtyDirs)
		UpdateRot();

	modelM = translate(mat4(1), pos);
	modelM = modelM * rotM;
	modelM = scale(modelM, size);
	modelM = translate(modelM, -center);

	dirtyModel = false;
}