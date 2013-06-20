//
// Lol Engine
//
// Copyright: (c) 2010-2013 Sam Hocevar <sam@hocevar.net>
//   This program is free software; you can redistribute it and/or
//   modify it under the terms of the Do What The Fuck You Want To
//   Public License, Version 2, as published by Sam Hocevar. See
//   http://www.wtfpl.net/ for more details.
//

#if defined HAVE_CONFIG_H
#   include "config.h"
#endif

#include <cstdlib>

#include "core.h"

namespace lol
{

template<typename T> class TrackedState
{
public:
    inline TrackedState()
      : m_changed(false)
    {}

    inline void TrackValue(T const &value)
    {
        m_value = value;
        m_changed = true;
    }

    inline bool HasChanged()
    {
        return m_changed;
    }

    inline T GetValue()
    {
        return m_value;
    }

private:
    T m_value;
    bool m_changed;
};

class RenderContextData
{
    friend class RenderContext;

private:
    Scene *m_scene;

    TrackedState<vec4> m_clear_color;
    TrackedState<float> m_clear_depth;
    TrackedState<bool> m_alpha_blend;
    TrackedState<bool> m_depth_test;
    TrackedState<bool> m_face_culling;
};

/*
 * Public RenderContext class
 */

RenderContext::RenderContext()
  : m_data(new RenderContextData())
{
    m_data->m_scene = Scene::GetDefault();
}

RenderContext::~RenderContext()
{
    if (m_data->m_alpha_blend.HasChanged())
        g_renderer->SetAlphaBlend(m_data->m_alpha_blend.GetValue());

    if (m_data->m_depth_test.HasChanged())
        g_renderer->SetDepthTest(m_data->m_depth_test.GetValue());

    if (m_data->m_face_culling.HasChanged())
        g_renderer->SetFaceCulling(m_data->m_face_culling.GetValue());

    delete m_data;
}

void RenderContext::SetClearColor(vec4 color)
{
    if (!m_data->m_clear_color.HasChanged())
        m_data->m_clear_color.TrackValue(g_renderer->GetClearColor());

    g_renderer->SetClearColor(color);
}

void RenderContext::SetClearDepth(float depth)
{
    if (!m_data->m_clear_depth.HasChanged())
        m_data->m_clear_depth.TrackValue(g_renderer->GetClearDepth());

    g_renderer->SetClearDepth(depth);
}

void RenderContext::SetAlphaBlend(bool set)
{
    if (!m_data->m_alpha_blend.HasChanged())
        m_data->m_alpha_blend.TrackValue(g_renderer->GetAlphaBlend());

    g_renderer->SetAlphaBlend(set);
}

void RenderContext::SetDepthTest(bool set)
{
    if (!m_data->m_depth_test.HasChanged())
        m_data->m_depth_test.TrackValue(g_renderer->GetDepthTest());

    g_renderer->SetDepthTest(set);
}

void RenderContext::SetFaceCulling(bool set)
{
    if (!m_data->m_face_culling.HasChanged())
        m_data->m_face_culling.TrackValue(g_renderer->GetFaceCulling());

    g_renderer->SetFaceCulling(set);
}

} /* namespace lol */

