//
//  Lol Engine
//
//  Copyright © 2010—2019 Sam Hocevar <sam@hocevar.net>
//            © 2009—2013 Benjamin “Touky” Huet <huet.benjamin@gmail.com>
//
//  Lol Engine is free software. It comes without any warranty, to
//  the extent permitted by applicable law. You can redistribute it
//  and/or modify it under the terms of the Do What the Fuck You Want
//  to Public License, Version 2, as published by the WTFPL Task Force.
//  See http://www.wtfpl.net/ for more details.
//

#include <lol/engine-internal.h>

#include <cstring>
#include <cstdlib>

#if defined _WIN32
#   define strcasecmp _stricmp
#   undef near
#   undef far
#endif

namespace lol
{

Camera::Camera()
{
    m_gamegroup = tickable::group::game::camera;
    m_drawgroup = tickable::group::draw::camera;

    //Arbitrary values when scene renderer is not ready.
    ivec2 screen_size = (Scene::GetCount()) ? (Video::GetSize()) : (ivec2(800, 600));
    m_fov = radians(45.f);
    m_near = -1000.f;
    m_far = 1000.f;
    m_screen_size = (float)screen_size.x;
    m_screen_ratio = (float)screen_size.y / (float)screen_size.x;
    m_is_shifted = false;
    m_screen_scale = vec2(1.f);

    m_target_distance = .0f;

    /* Create a default perspective */
    SetProjection(m_fov, m_near, m_far, m_screen_size, m_screen_ratio);
    SetView(mat4::lookat(vec3(0.f, 50.f, 50.f), vec3::zero, vec3::axis_y));
}

Camera::~Camera()
{
}

//-----------------------------------------------------------------------------
//View functions
//--

//WARNING : Don't forget the position
void Camera::SetView(mat4 const &view)
{
    m_view_matrix = view;
    m_position = inverse(view)[3].xyz;
}

void Camera::SetView(vec3 eye, vec3 target, vec3 up)
{
    m_view_matrix = mat4::lookat(eye, target, up);
    m_position = eye;
    m_target_distance = length(target - m_position);
}

void Camera::SetView(vec3 pos, vec3 rot)
{
    SetView(pos, quat::fromeuler_zyx(rot));
}

void Camera::SetView(vec3 pos, quat rot)
{
    m_view_matrix = inverse(mat4(rot)) * mat4::translate(-pos);
    m_position = pos;
}

mat4 Camera::GetView() const
{
    return m_view_matrix;
}

//-----------------------------------------------------------------------------
//Projections functions
//--
void Camera::SetProjection(mat4 const &proj)
{
    m_proj_matrix = proj;
}

void Camera::SetProjection(float fov, float near, float far)
{
    SetProjection(fov, near, far, m_screen_size, m_screen_ratio);
}

void Camera::SetProjection(float fov, float near, float far, float screen_size, float screen_ratio)
{
    m_fov = fov;
    m_near = near;
    m_far = far;
    m_screen_size = screen_size;
    m_screen_ratio = screen_ratio;
    mat4 screen_scale = mat4::scale(vec3(m_screen_scale.xy, 1.f));
    if (m_fov > .00001f)
    {
        if (m_is_shifted)
            SetProjection(screen_scale * mat4::shifted_perspective(m_fov, screen_size, screen_ratio, m_near, m_far));
        else
            SetProjection(screen_scale * mat4::perspective(m_fov, screen_size, screen_size * screen_ratio, m_near, m_far));
    }
    else
        SetProjection(screen_scale * mat4::ortho(screen_size, screen_size * screen_ratio, m_near, m_far));
}

mat4 Camera::GetProjection() const
{
    return m_proj_matrix;
}

//-----------------------------------------------------------------------------
//Projections manipulation functions
//--
void Camera::SetFov(float fov)
{
    SetProjection(fov, m_near, m_far, m_screen_size, m_screen_ratio);
}

void Camera::SetScreenInfos(float screen_size)
{
    SetScreenInfos(screen_size, m_screen_ratio);
}

void Camera::SetScreenInfos(float screen_size, float screen_ratio)
{
    SetProjection(m_fov, m_near, m_far, screen_size, screen_ratio);
}

void Camera::SetDrawInfos(float far)
{
    SetDrawInfos(m_near, far);
}

void Camera::SetDrawInfos(float near, float far)
{
    SetProjection(m_fov, near, far, m_screen_size, m_screen_ratio);
}

void Camera::SetScreenScale(vec2 screen_scale)
{
    m_screen_scale = screen_scale;
    SetProjection(m_fov, m_near, m_far, m_screen_size, m_screen_ratio);
}

void Camera::UseShift(bool should_shift)
{
    m_is_shifted = should_shift;
    SetProjection(m_fov, m_near, m_far, m_screen_size, m_screen_ratio);
}

void Camera::UseTarget(bool use_target)
{
    m_target_distance = ((use_target)?(max(m_target_distance, 1.f)):(.0f));
}

//-----------------------------------------------------------------------------
//camera manipulation Functions
//--
void Camera::SetPosition(vec3 pos, bool keep_target)
{
    if (keep_target)
        SetView(pos, GetTarget(), GetUp());
    else
        SetView(GetView() * mat4::translate(m_position - pos));
    m_position = pos;
}

void Camera::SetTarget(vec3 target, vec3 up)
{
    SetView(m_position, target, up);
}

void Camera::SetRotation(vec3 rot)
{
    SetView(m_position, rot);
}

void Camera::SetRotation(quat rot)
{
    SetView(m_position, rot);
}

//--
vec3 Camera::GetPosition() const
{
    return m_position;
}

vec3 Camera::GetTarget() const
{
    return m_position + (inverse(m_view_matrix) * vec4(0.f, 0.f, -max(m_target_distance, 1.f), 0.f)).xyz;
}

vec3 Camera::GetUp() const
{
    return (inverse(m_view_matrix) * vec4(0.f, 1.f, 0.f, 0.f)).xyz;
}

vec3 Camera::GetRotationEuler() const
{
    return vec3::toeuler_zyx(GetRotation());
}

quat Camera::GetRotation() const
{
    return quat(inverse(mat3(m_view_matrix)));
}

// Calculate the frustum height at a given distance from the camera.
float Camera::GetFrustumHeightAtDistance(float distance, float fov) const
{
    return 2.f * distance * lol::tan(fov * .5f);
}

// Calculate the FOV needed to get a given frustum height at a given distance.
float Camera::GetFOVForHeightAndDistance(float distance, float height) const
{
    return 2.f * lol::atan(height * .5f / distance);
}

void Camera::tick_game(float seconds)
{
    WorldEntity::tick_game(seconds);
}

void Camera::tick_draw(float seconds, Scene &scene)
{
    WorldEntity::tick_draw(seconds, scene);
}

} /* namespace lol */

