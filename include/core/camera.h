#pragma once
#include <iostream>
#include <ostream>
#include <glm/glm.hpp>
#include <glm/detail/type_quat.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/quaternion.hpp>

#include "fmt/format.h"

class Camera
{
private:
    float fovy;
    float aspect;
    float focusDistance;
    glm::vec3 Position;
    glm::quat Rotation;
    glm::vec3 EularRotation;
    float zNear;
    float zFar;
    glm::vec3 up;
    glm::vec3 right;
    glm::vec3 front;

public:

    Camera(const int width, const int height): fovy(45.0f),
        aspect(static_cast<float>(width)/ static_cast<float>(height)),
        focusDistance(1.0f),
        Position(glm::vec3(0.0f, 0.0f, 4.0f)),
        Rotation(glm::quat(1.0f, 0.0f, 0.0f, 0.0f)),
        zNear(0.1f),
        zFar(100.f),
        up(0.f),
        right(0.f),
        front(0.f)
    {
        EularRotation = glm::eulerAngles(Rotation);
    }

    Camera(): Camera(1920, 1080){}

    inline glm::mat4 GetViewMatrix() const
    {
        return glm::lookAt(Position, GetCenter(), GetUpVec());
    }

    inline glm::mat4 GetProjectionMatrix() const
    {
        return glm::perspective(glm::radians(fovy), aspect, zNear, zFar);
    }
    inline float GetFovy() const
    {
        return fovy;
    }

    inline glm::vec2 GetClippingPlanes() {
        return glm::vec2(zNear, zFar);
    }

    inline float GetAspect() const
    {
        return aspect;
    }

    inline float GetFocusDist() const
    {
        return focusDistance;
    }

    inline glm::vec3 GetPosition() const
    {
        return Position;
    }

    inline glm::vec3 GetEularRotation() const
    {
        return EularRotation;
    }

    inline glm::quat GetRotation() const {
        return Rotation;
    }

    inline glm::vec3 GetCenter() const
    {
        return Position + front;
    }

    inline glm::vec3 GetUpVec() const {
        return up;
    }

    inline glm::vec3 GetRight() const {
        return right;
    }

    inline glm::vec3 GetForward() const {
        return front;
    }

    inline void SetUpVec()
    {
        glm::vec3 t = glm::cross(2.f *glm::vec3(Rotation.x,Rotation.y,Rotation.z), glm::vec3(0,1,0));
        up = glm::vec3(0,1,0) + Rotation.w * t + cross(glm::vec3(Rotation.x,Rotation.y,Rotation.z), t);
    }

    inline void SetRightVec() {
        glm::vec3 t = glm::cross(2.f *glm::vec3(Rotation.x,Rotation.y,Rotation.z), glm::vec3(-1,0,0));
        right = glm::vec3(-1,0,0) + Rotation.w * t + cross(glm::vec3(Rotation.x,Rotation.y,Rotation.z), t);
    }

    inline void SetForwardVec() {
        glm::vec3 t = glm::cross(2.f *glm::vec3(Rotation.x,Rotation.y,Rotation.z), glm::vec3(0,0,1));
        front = glm::vec3(0,0,1) + Rotation.w * t + cross(glm::vec3(Rotation.x,Rotation.y,Rotation.z), t);
    }

    inline void SetAspect(const int width, const int height)
    {
        aspect = float(width) / float(height);
    }

    inline void SetClippingPlanes(glm::vec2 clipPlanes) {
        zNear = clipPlanes.x;
        zFar = clipPlanes.y;
    }

    void SetAspect(const float _aspect) {
        aspect = _aspect;
    }

    inline void SetFovy(const float _fovy)
    {
        fovy = glm::radians(_fovy);
    }

    inline void SetFocusDist(const float _focusDistance)
    {
        focusDistance = _focusDistance;
    }

    inline void SetPosition(const glm::vec3 _pos)
    {
        Position = _pos;
    }

    inline void SetRotation(const glm::vec3& eulerAngles)
    {
        EularRotation = eulerAngles;
        glm::vec3 radianAngles = glm::radians(eulerAngles); // Convert degrees to radians

        // Convert Euler angles to quaternion (YXZ Order)
        Rotation = glm::quat(glm::vec3(radianAngles.x, radianAngles.y, radianAngles.z));

        // Normalize to avoid floating point drift
        Rotation = glm::normalize(Rotation);

        SetUpVec();
        SetRightVec();
        SetForwardVec();
    }

};
