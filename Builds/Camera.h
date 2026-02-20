#pragma once
#pragma once
#include <glm/glm.hpp>

class Camera
{
private:
	glm::vec3 _position;
	double _yaw;
	double _pitch;
	double _speed;
	double _sensitivity;
public:
	Camera(double x, double y, double z, double yaw, double pitch, double speed, double sensitivity);
	Camera(glm::vec3 position, double yaw, double pitch, double speed, double sensitivity);
	void apply_to_scene();
	void move_forward();
	void move_backward();
	void move_left();
	void move_right();
	void move_up();
	void move_down();
	void rotate(double xoffset, double yoffset);
};
