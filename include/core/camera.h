#pragma once
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

class Camera
{
private:
    float fovy;
    float aspect;
    glm::vec3 center;
    glm::vec3 eye;
    glm::vec3 up;

public:
    Camera(const int width, const int height): fovy(45.0f),
        aspect(float(width)/ float(height)),
        center(glm::vec3(0.0f, 0.0f, 0.0f)),
        eye(glm::vec3(0.0f, 0.0f, 4.0f)),
        up(glm::vec3(0.0f, 1.0f, 0.0f)) {}

    Camera(): Camera(1920, 1080){}

    inline glm::mat4 GetViewMatrix() const
    {
        //glm::vec3 viewDir = center - eye;
        //up = glm::normalize(glm::cross(up, viewDir));
        return glm::lookAt(eye, center, up);
    }

    inline glm::mat4 GetProjectionMatrix() const
    {
        return glm::perspective(glm::radians(fovy), aspect, 0.1f, 100.0f);
    }

    inline float GetAspect() {
        return aspect;
    }
    
    inline float GetFovy() {
        return fovy;
    }

    inline glm::vec3 GetEye() const
    {
        return eye;
    }

    inline glm::vec3 GetCenter(){
        return center;
    }

    inline glm::vec3 GetUpVec(){
        return up;
    }

    inline void SetAspect(const int width, const int height)
    {
        aspect = float(width) / float(height);
    }

    void SetAspect(const float _aspect) {
        aspect = _aspect;
    }

    inline void SetFovy(const float _fovy)
    {
        fovy = glm::radians(_fovy);
    }

    inline void SetEye(const glm::vec3 _eye)
    {
        eye = _eye;
    }

    inline void SetCenter(const glm::vec3 _center){
        center = _center;
    }

    inline void SetUpVec(const glm::vec3 _upVect){
        up = _upVect;
    }
};