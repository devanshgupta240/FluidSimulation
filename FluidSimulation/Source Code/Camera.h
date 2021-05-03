#pragma once
#include "Algebra3.h"

class Camera
{
private:
        static double minDist;
        static double maxDist;
        static vec3 pos;
        static vec3 up;
public:
        Camera();
        Camera(double minDist, double range);
        static vec3 getPos();
        static vec3 getUp();
        static void rotateLeft(double degrees);
        static void rotateUp(double degrees);
        static void rotateArbitrary(vec2 direction, double degrees);
        static void spinLeft(double degrees);
        static void zoom(double amount);
        ~Camera();
};