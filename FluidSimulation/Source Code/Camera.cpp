#include "Camera.h"
#include <cmath>

vec3 Camera::up = vec3(0);
vec3 Camera::pos = vec3(0);
double Camera::minDist = 0;
double Camera::maxDist = 0;

Camera::Camera()
{
        minDist = 0;
        maxDist = 100;
        pos = vec3(0,0,30);
        up = vec3(0,1,0);
}

Camera::Camera(double minCamDist, double range)
{
        minDist = minCamDist + 2;
        maxDist = minDist + range;
        pos = vec3(0,0,1);
        pos *= minDist + 1;
        up = vec3(0,1,0);
}

void Camera::rotateLeft(double degrees)
{
        pos = rotation3D(-up,degrees) * pos;
}

void Camera::spinLeft(double degrees)
{
        up = rotation3D(-pos,degrees) * up;
}

void Camera::rotateArbitrary(vec2 direction, double degrees)
{
        direction.normalize();
        double theta = acos(direction * vec2(0.0,1.0));//this will give us the angle that the direction forms with the up vector
        theta = 360 * theta / M2_PI;//convert to degrees
        if(direction[VX] < 0)
                theta = -theta;//quadrant fix
        vec3 dir = rotation3D(pos, theta) * up;//get the 3d version of direction
        vec3 rotAxis = dir ^ pos;
        rotAxis.normalize();
        mat4 M = rotation3D(rotAxis, degrees);
        pos = M * pos;
        up = M * up;
}

void Camera::rotateUp(double degrees)
{
        vec3 rotAxis = pos ^ up;
        rotAxis.normalize();
        mat4 M = rotation3D(rotAxis, degrees);//generate our rotation matrix
        pos = M * pos;
        up = M * up;//rotate both pos and up around it
}

void Camera::zoom(double amount)
{
        if(amount + pos.length() > minDist && amount + pos.length() < maxDist)//limits on how far you can zoom
                pos += amount * (pos / pos.length());//move outward by amount
}

vec3 Camera::getPos()
{
        return pos;
}

vec3 Camera::getUp()
{
        return up;
}

Camera::~Camera()
{
}