//
//  Lol Engine
//
//  Copyright © 2009—2013 Benjamin “Touky” Huet <huet.benjamin@gmail.com>
//            © 2010—2017 Sam Hocevar <sam@hocevar.net>
//            © 2009—2013 Cédric Lecacheur <jordx@free.fr>
//
//  Lol Engine is free software. It comes without any warranty, to
//  the extent permitted by applicable law. You can redistribute it
//  and/or modify it under the terms of the Do What the Fuck You Want
//  to Public License, Version 2, as published by the WTFPL Task Force.
//  See http://www.wtfpl.net/ for more details.

#include <lol/engine-internal.h>

namespace lol
{

//-----------------------------------------------------------------------------
//helpers func to retrieve a vertex.
int VertexDictionnary::FindVertexMaster(const int search_idx)
{
    //Resolve current vertex idx in the dictionnary (if exist)
    for (int j = 0; j < vertex_list.count(); j++)
        if (vertex_list[j].m1 == search_idx)
            return vertex_list[j].m3;
    return VDictType::DoesNotExist;
}

//-----------------------------------------------------------------------------
//retrieve a list of matching vertices, doesn't include search_idx.
bool VertexDictionnary::FindMatchingVertices(const int search_idx, array<int> &matching_ids)
{
    int cur_mast = FindVertexMaster(search_idx);

    if (cur_mast == VDictType::DoesNotExist || cur_mast == VDictType::Alone)
        return false;

    if (cur_mast == VDictType::Master)
        cur_mast = search_idx;
    else
        matching_ids << vertex_list[cur_mast].m1;

    for (int j = 0; j < vertex_list.count(); j++)
        if (vertex_list[j].m3 == cur_mast && vertex_list[j].m1 != search_idx)
            matching_ids << vertex_list[j].m1;

    return (matching_ids.count() > 0);
}

//-----------------------------------------------------------------------------
//Will return connected vertices (through triangles), if returned vertex has matching ones, it only returns the master.
bool VertexDictionnary::FindConnectedVertices(const int search_idx, const array<uint16_t> &tri_list, const int tri0, array<int> &connected_vert, array<int> const *ignored_tri)
{
    array<int> connected_tri;
    FindConnectedTriangles(search_idx, tri_list, tri0, connected_tri, ignored_tri);

    for (int i = 0; i < connected_tri.count(); i++)
    {
        for (int j = 0; j < 3; j++)
        {
            int v_indice = tri_list[connected_tri[i] + j];
            if (v_indice != search_idx)
            {
                int found_master = FindVertexMaster(tri_list[connected_tri[i] + j]);
                if (found_master == VDictType::Alone || found_master == VDictType::Master)
                    found_master = v_indice;
                if (found_master != search_idx)
                {
                    bool already_exist = false;
                    for (int k = 0; !already_exist && k < connected_vert.count(); k++)
                        if (connected_vert[k] == found_master)
                            already_exist = true;
                    if (!already_exist)
                        connected_vert << found_master;
                }
            }
        }
    }
    return (connected_vert.count() > 0);
}
//-----------------------------------------------------------------------------
bool VertexDictionnary::FindConnectedTriangles(const int search_idx, const array<uint16_t> &tri_list, const int tri0, array<int> &connected_tri, array<int> const *ignored_tri)
{
    return FindConnectedTriangles(ivec3(search_idx, search_idx, search_idx), tri_list, tri0, connected_tri, ignored_tri);
}
//-----------------------------------------------------------------------------
bool VertexDictionnary::FindConnectedTriangles(const ivec2 &search_idx, const array<uint16_t> &tri_list, const int tri0, array<int> &connected_tri, array<int> const *ignored_tri)
{
    return FindConnectedTriangles(ivec3(search_idx, search_idx.x), tri_list, tri0, connected_tri, ignored_tri);
}
//-----------------------------------------------------------------------------
bool VertexDictionnary::FindConnectedTriangles(const ivec3 &search_idx, const array<uint16_t> &tri_list, const int tri0, array<int> &connected_tri, array<int> const *ignored_tri)
{
    int needed_validation = 0;
    array<int> vert_list[3];
    for (int i = 0; i < 3; i++)
    {
        //Small optim since above func will use this one
        if ((i == 1 && search_idx[0] == search_idx[1]) ||
            (i == 2 && (search_idx[0] == search_idx[2] || search_idx[1] == search_idx[2])))
            continue;
        else
        {
            //increment the validation info, hence empty list aren't taken into account.
            needed_validation++;
            vert_list[i] << search_idx[i];
            FindMatchingVertices(search_idx[i], vert_list[i]);
        }
    }

    for (int i = tri0; i < tri_list.count(); i += 3)
    {
        if (ignored_tri)
        {
            bool should_pass = false;
            for (int j = 0; !should_pass && j < ignored_tri->count(); j++)
                if ((*ignored_tri)[j] == i)
                    should_pass = true;
            if (should_pass)
                continue;
        }
        int found_validation = 0;
        for (int j = 0; j < 3; j++)
        {
            bool validated = false;
            for (int k = 0; !validated && k < vert_list[j].count(); k++)
                for (int l = 0; !validated && l < 3; l++)
                    if (vert_list[j][k] == tri_list[i + l])
                        validated = true;
            found_validation += (validated)?(1):(0);
        }
        //triangle is validated store it
        if (found_validation == needed_validation)
            connected_tri << i;
    }

    return (connected_tri.count() > 0);
}

//-----------------------------------------------------------------------------
//Will update the given list with all the vertices on the same spot.
void VertexDictionnary::RegisterVertex(const int vert_id, const vec3 vert_coord)
{
    for (int j = 0; j < vertex_list.count(); j++)
        if (vertex_list[j].m1 == vert_id)
            return;

    //First, build the vertex Dictionnary
    int i = 0;
    for (; i < master_list.count(); i++)
    {
        int cur_mast  = master_list[i];
        int cur_id    = vertex_list[cur_mast].m1;
        vec3 cur_loc  = vertex_list[cur_mast].m2;
        int &cur_type = vertex_list[cur_mast].m3;

        if (cur_id == vert_id)
            return;

        if (sqlength(cur_loc - vert_coord) < TestEpsilon::Get())
        {
            if (cur_type == VDictType::Alone)
                cur_type = VDictType::Master;
            vertex_list.push(vert_id, vert_coord, cur_mast);
            return;
        }
    }

    //We're here because we couldn't find any matching vertex
    master_list.push(vertex_list.count());
    vertex_list.push(vert_id, vert_coord, VDictType::Alone);
}

//-----------------------------------------------------------------------------
//Will update the given list with all the vertices on the same spot.
void VertexDictionnary::RemoveVertex(const int vert_id)
{
    int j = 0;
    for (; j < vertex_list.count(); j++)
        if (vertex_list[j].m1 == vert_id)
            break;

    if (vertex_list[j].m3 == VDictType::Master)
    {
        int jf = -1;
        //change all the master ref in the list
        for (int i = 0; i < vertex_list.count(); i++)
        {
            if (vertex_list[i].m3 == j)
            {
                if (jf < 0)
                {
                    jf = i;
                    vertex_list[i].m3 = VDictType::Master;
                }
                else
                    vertex_list[i].m3 = jf;
            }
        }
    }
    vertex_list.remove(j);
    for (int i = 0; i < master_list.count(); i++)
        if (master_list[j] == j)
            break;
}

} /* namespace lol */
