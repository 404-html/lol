//
// EasyMesh-Csg: The code belonging to CSG operations
//
// Copyright: (c) 2010-2013 Sam Hocevar <sam@hocevar.net>
//            (c) 2010-2013 Benjamin "Touky" Huet <huet.benjamin@gmail.com>
//   This program is free software; you can redistribute it and/or
//   modify it under the terms of the Do What The Fuck You Want To
//   Public License, Version 2, as published by Sam Hocevar. See
//   http://www.wtfpl.net/ for more details.
//

#pragma once

namespace lol
{

#define LEAF_ABOVE      2
#define LEAF_FRONT      1
#define LEAF_BACK       0
#define LEAF_CURRENT   -1

//Naïve bsp for the poor people
class CsgBspLeaf
{
    friend class CsgBsp;

public:
    CsgBspLeaf(vec3 origin, vec3 normal, int above_idx)
    {
        m_origin = origin;
        m_normal = normal;
        m_leaves[LEAF_ABOVE] = above_idx;

        m_leaves[LEAF_FRONT] = -1;
        m_leaves[LEAF_BACK] = -1;
    }

private:
    vec3            m_origin;
    vec3            m_normal;
    array< int, vec3, vec3, vec3 >    m_tri_list;
    ivec3           m_leaves;
};

//Naïve bsp for the poor people
class CsgBsp
{
public:
    void AddTriangleToTree(int const &tri_idx, vec3 const &tri_p0, vec3 const &tri_p1, vec3 const &tri_p2);

    //return 0 when no split has been done.
    //return 1 when split has been done.
    //return -1 when error.
    int TestTriangleToTree(vec3 const &tri_p0, vec3 const &tri_p1, vec3 const &tri_p2,
                            //In order to easily build the actual vertices list afterward, this list stores each Vertices location and its source vertices & Alpha.
                            //<Point_Loc, Src_V0, Src_V1, Alpha> as { Point_Loc = Src_V0 + (Src_V1 - Src_V0) * Alpha; }
                            array< vec3, int, int, float > &vert_list,
                            //This is the final triangle list : If Side_Status is LEAF_CURRENT, a new test will be done point by point.
                            //<{IN|OUT}side_status, v0, v1, v2>
                            array< int, int, int, int > &tri_list);

private:
    int AddLeaf(int leaf_type, vec3 origin, vec3 normal, int above_idx);
    int TestPoint(int leaf_idx, vec3 point);

    array<CsgBspLeaf> m_tree;
};

} /* namespace lol */

