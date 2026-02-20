#include <GL/glew.h>
#include "Camera.h"

Camera::Camera(double x, double y, double z, double yaw, double pitch, double speed, double sensitivity)
{
    _position = glm::vec3(x, y, z);
    _yaw = yaw;
    _pitch = pitch;
    _speed = speed;
    _sensitivity = sensitivity;
};
Camera::Camera(glm::vec3 position, double  yaw, double pitch, double speed, double sensitivity)
{
    _position = position;
    _yaw = yaw;
    _pitch = pitch;
    _speed = speed;
    _sensitivity = sensitivity;
};

void Camera::apply_to_scene()
{
    glRotatef(-_pitch, 1.0f, 0.0f, 0.0f);
    glRotatef(-_yaw, 0.0f, 1.0f, 0.0f);
    glTranslatef(-_position.x, -_position.y, -_position.z);
}
void Camera::move_forward()
{
    _position.x += sin(glm::radians(_yaw)) * _speed;
    _position.z -= cos(glm::radians(_yaw)) * _speed;
}
void Camera::move_backward()
{
    _position.x -= sin(glm::radians(_yaw)) * _speed;
    _position.z += cos(glm::radians(_yaw)) * _speed;
}
void Camera::move_left()
{
    _position.x -= cos(glm::radians(_yaw)) * _speed;
    _position.z -= sin(glm::radians(_yaw)) * _speed;
}
void Camera::move_right()
{
    _position.x += cos(glm::radians(_yaw)) * _speed;
    _position.z += sin(glm::radians(_yaw)) * _speed;
}
void Camera::move_up()
{
    _position.y += _speed;
}
void Camera::move_down()
{
    _position.y -= _speed;
}

void Camera::rotate(double xoffset, double yoffset)
{
    xoffset *= _sensitivity;
    yoffset *= _sensitivity;

    _yaw += xoffset;
    _pitch += yoffset;

    if (_pitch > 89.0f)
        _pitch = 89.0f;
    if (_pitch < -89.0f)
        _pitch = -89.0f;
}