#include <player_camera.h>
#include "SDL.h"

#include <glm/gtx/transform.hpp>
void PlayerCamera::process_input_event(SDL_Event* ev)
{
	if (ev->type == SDL_KEYDOWN)
	{
		switch (ev->key.keysym.sym)
		{
		case SDLK_UP:
		case SDLK_w:
			inputAxis.x += 1.f;
			break;
		case SDLK_DOWN:
		case SDLK_s:
			inputAxis.x -= 1.f;
			break;
		case SDLK_LEFT:
		case SDLK_a:
			inputAxis.y -= 1.f;
			break;
		case SDLK_RIGHT:
		case SDLK_d:
			inputAxis.y += 1.f;
			break;
		}
	}
	else if (ev->type == SDL_KEYUP)
	{
		switch (ev->key.keysym.sym)
		{
		case SDLK_UP:
		case SDLK_w:
			inputAxis.x -= 1.f;
			break;
		case SDLK_DOWN:
		case SDLK_s:
			inputAxis.x += 1.f;
			break;
		case SDLK_LEFT:
		case SDLK_a:
			inputAxis.y += 1.f;
			break;
		case SDLK_RIGHT:
		case SDLK_d:
			inputAxis.y -= 1.f;
			break;
		}
	}
	else if (ev->type == SDL_MOUSEMOTION) {
		if (!bLocked)
		{
			pitch -= ev->motion.yrel * 0.003f;
			yaw -= ev->motion.xrel * 0.003f;
		}
	}

	inputAxis = glm::clamp(inputAxis, { -1.0,-1.0,-1.0 }, { 1.0,1.0,1.0 });
}

void PlayerCamera::update_camera(float deltaSeconds)
{
	const float cam_vel = 0.001f;
	glm::vec3 forward = { 0,0,cam_vel };
	glm::vec3 right = { cam_vel,0,0 };

	glm::mat4 cam_rot = get_rotation_matrix();

	forward = cam_rot * glm::vec4(forward, 0.f);
	right = cam_rot * glm::vec4(right, 0.f);

	velocity = inputAxis.x * forward + inputAxis.y * right;

	velocity *= 10 * deltaSeconds;

	position += velocity;
}


glm::mat4 PlayerCamera::get_view_matrix()
{
	glm::vec3 camPos = position;

	glm::mat4 cam_rot = (get_rotation_matrix());

	glm::mat4 view = glm::translate(glm::mat4{ 1 }, camPos) * cam_rot;

	//we need to invert the camera matrix
	view = glm::inverse(view);

	return view;
}

glm::mat4 PlayerCamera::get_projection_matrix(bool bReverse /*= true*/)
{
	if (bReverse)
	{
		glm::mat4 pro = glm::perspective(glm::radians(70.f), 1700.f / 900.f, 5000.0f, 0.1f);
		pro[1][1] *= -1;
		return pro;
	}
	else {
		glm::mat4 pro = glm::perspective(glm::radians(70.f), 1700.f / 900.f, 0.1f, 5000.0f);
		pro[1][1] *= -1;
		return pro;
	}
}

glm::mat4 PlayerCamera::get_rotation_matrix()
{
	glm::mat4 yaw_rot = glm::rotate(glm::mat4{ 1 }, yaw, { 0,-1,0 });
	glm::mat4 pitch_rot = glm::rotate(glm::mat4{ yaw_rot }, pitch, { -1,0,0 });

	return pitch_rot;
}
