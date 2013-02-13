//
// Lol Engine
//
// Copyright: (c) 2010-2013 Sam Hocevar <sam@hocevar.net>
//            (c) 2009-2013 Cédric Lecacheur <jordx@free.fr>
//            (c) 2009-2013 Benjamin "Touky" Huet <huet.benjamin@gmail.com>
//   This program is free software; you can redistribute it and/or
//   modify it under the terms of the Do What The Fuck You Want To
//   Public License, Version 2, as published by Sam Hocevar. See
//   http://www.wtfpl.net/ for more details.
//

//
// The EasyMesh class
// ------------------
//

#define VU_BONES   2
#define VU_TEX_UV  1
#define VU_VANILLA 0

#define VERTEX_USEAGE VU_VANILLA

#if !defined __EASYMESH_EASYMESH_H__
#define __EASYMESH_EASYMESH_H__

namespace lol
{

/* A safe enum for MeshCSG operations. */
struct CSGUsage
{
    enum Value
    {
        Union,
        Substract,
        SubstractLoss, //will remove B from A, but not add inverted B
        And,
        Xor,
    }
    m_value;

    inline CSGUsage(Value v) : m_value(v) {}
    inline operator Value() { return m_value; }
};

/* A safe enum for VertexDictionnary operations. */
struct VDictType
{
    enum Value
    {
        DoesNotExist=-3,
        Alone=-2,
        Master=-1
    }
    m_value;

    inline VDictType(Value v) : m_value(v) {}
    inline operator Value() { return m_value; }
};

//a class whose goal is to keep a list of the adjacent vertices for mesh operations purposes
class VertexDictionnary
{
public:
    int FindVertexMaster(const int search_idx);
    bool FindMatchingVertices(const int search_idx, Array<int> &matching_ids);
    bool FindConnectedVertices(const int search_idx, const Array<int> &tri_list, Array<int> &connected_vert, Array<int> const *ignored_tri = NULL);
    bool FindConnectedTriangles(const int search_idx, const Array<int> &tri_list, Array<int> &connected_tri, Array<int> const *ignored_tri = NULL);
    bool FindConnectedTriangles(const ivec2 &search_idx, const Array<int> &tri_list, Array<int> &connected_tri, Array<int> const *ignored_tri = NULL);
    bool FindConnectedTriangles(const ivec3 &search_idx, const Array<int> &tri_list, Array<int> &connected_tri, Array<int> const *ignored_tri = NULL);
    void AddVertex(int vert_id, vec3 vert_coord);
    void Clear() { vertex_list.Empty(); }
private:
    //<VertexId, VertexLocation, VertexMasterId>
    Array<int, vec3, int>   vertex_list;
    //List of the master_ vertices
    Array<int>              master_list;
};

struct MeshBuildOperation
{
    enum Value
    {
        Scale_Winding   = 1 << 0,

        All     = 0xffffffff
    }
    m_value;

    inline MeshBuildOperation(Value v) : m_value(v) {}
    inline MeshBuildOperation(uint64_t i) : m_value((Value)i) {}
    inline operator Value() { return m_value; }
};

class EasyMesh
{
    friend class EasyMeshParser;

public:
    EasyMesh();

    bool Compile(char const *command);
    void MeshConvert(Shader* ProvidedShader = NULL);
    void Render(mat4 const &model, float damage = 0.f);

private:
    void UpdateVertexDict(Array< int, int > &vertex_dict);

    //-------------------------------------------------------------------------
    //Mesh CSG operations
    //-------------------------------------------------------------------------
private:
    void MeshCsg(CSGUsage csg_operation);
public:
    /* [cmd:csgu] Performs a Union operation as (mesh0_Outside + mesh1_Outside) */
    void CsgUnion()         { MeshCsg(CSGUsage::Union); }
    /* [cmd:csgs] Performs a Substract operation as (mesh0_Outside + mesh1_Inside-inverted) */
    void CsgSubstract()     { MeshCsg(CSGUsage::Substract); }
    /* [cmd:csgsl] Performs a Substract operation without keeping the mesh1 part */
    void CsgSubstractLoss() { MeshCsg(CSGUsage::SubstractLoss); }
    /* [cmd:csga] Performs an And operation as (mesh0_Inside + mesh1_Inside) */
    void CsgAnd()           { MeshCsg(CSGUsage::And); }
    /* [cmd:csgx] Performs a Xor operation as (m0_Outside/m0_Inside-inverted + m1_Outside/m1_Inside-inverted) */
    void CsgXor()           { MeshCsg(CSGUsage::Xor); }

public:
    /* [cmd:[] from this point onward, any operation will not be performed on previous vertices */
    void OpenBrace();
    /* [cmd:]] Merge current vertices with previous context */
    void CloseBrace();
    /* [cmd:tsw] When activation, on negative-scaling, normal fixing will not occur */
    void ToggleScaleWinding();
    /* [cmd:sc] Set vertices color */
    void SetCurColor(vec4 const &color);
    /* [cmd:scb] Set vertices color 2 */
    void SetCurColor2(vec4 const &color);

private:
    //-------------------------------------------------------------------------
    //Internal : Basic triangle/vertex operations
    //-------------------------------------------------------------------------
    void AddVertex(vec3 const &coord);
    void AddDuplicateVertex(int i);
    void AppendQuad(int i1, int i2, int i3, int i4, int base);
    void AppendQuadDuplicateVerts(int i1, int i2, int i3, int i4, int base);
    void AppendTriangle(int i1, int i2, int i3, int base);
    void AppendTriangleDuplicateVerts(int i1, int i2, int i3, int base);
    void ComputeNormals(int start, int vcount);
public: //DEBUG
    void ComputeTexCoord(float uv_scale, int uv_offset);

    //-------------------------------------------------------------------------
    //Vertices operations
    //-------------------------------------------------------------------------
    void SetVertColor(vec4 const &color);
    void SetTexCoordData(vec2 const &new_offset, vec2 const &new_scale);

    void SetCurVertNormal(vec3 const &normal);
    void SetCurVertColor(vec4 const &color);
    void SetCurVertTexCoord(vec2 const &texcoord);

public:
    //-------------------------------------------------------------------------
    //Mesh transform operations
    //-------------------------------------------------------------------------

    /* [cmd:t/tx/ty/tz] Translate vertices
        - v : Translation quantity.
     */
    void Translate(vec3 const &v);
    /* See Rotate */
    void RotateX(float angle);
    /* See Rotate */
    void RotateY(float angle);
    /* See Rotate */
    void RotateZ(float angle);
    /* [cmd:r/rx/ry/rz] Rotate vertices
        - angle : rotation quantity.
        - axis : rotation axis.
     */
    void Rotate(float angle, vec3 const &axis);
    /* [cmd:rj] Randomly move vertices along Origin-to-vertex as o2v *= (1.0 + rand(r))
        - r : jitter maximum value.
     */
    void RadialJitter(float r);
    //TODO : twist
    //TODO : bend
    //TODO : stretch
    //TODO : shear
    /* [cmd:tax] multiply y&z by (1.0 + (n * x + xoff))
        - y : value of n for y.
        - z : value of n for z.
        - xoff : value of xoff.
     */
    void TaperX(float y, float z, float xoff);
    /* [cmd:tay] multiply x&z by (1.0 + (n * y + yoff))
        - x : value of n for x.
        - z : value of n for z.
        - yoff : value of yoff.
     */
    void TaperY(float x, float z, float yoff);
    /* [cmd:taz] multiply x&y by (1.0 + (n * z + zoff))
        - x : value of n for x.
        - y : value of n for y.
        - zoff : value of zoff.
     */
    void TaperZ(float x, float y, float zoff);
    /* [cmd:s/sx/sy/sz] Scale vertices
        - s : scale quantity.
     */
    void Scale(vec3 const &s);
    /* [cmd:mx] Mirror vertices through X-plane
        Acts as an OpenBrace
     */
    void MirrorX();
    /* [cmd:my] Mirror vertices through Y-plane
        Acts as an OpenBrace
     */
    void MirrorY();
    /* [cmd:mz] Mirror vertices through Z-plane
        Acts as an OpenBrace
     */
    void MirrorZ();
    /* [no-cmd] Duplicates vertices and scale duplicate
        Acts as an OpenBrace
     */
    void DupAndScale(vec3 const &s);
    /* [cmd:ch] Performs a chamfer operation //TODO : Make it work.
        - f : Chamfer quantity.
     */
    void Chamfer(float f);

    //-------------------------------------------------------------------------
    //Mesh shape operations
    //-------------------------------------------------------------------------

    /* [cmd:ac] Cylinder centered on (0,0,0) with BBox [-max(r1, r2), -.5*h, -max(r1, r2)][max(r1, r2), .5*h, max(r1, r2)]
        - nbsides : Number of sides.
        - h : Height of the cylinder.
        - r1 : Lower radius. TODO:convert to diameter to be coherent with other funcs
        - r2 : Upper radius. TODO:convert to diameter to be coherent with other funcs
        - dualside : if (1) will also create inner sides : TOOD:TOREMOVE?? : needed ?
        - smooth : if (1) will smooth normals : TOOD:TOREMOVE : smooth should be handled elsewhere
        - close : if (1) will add discs to close the cylinder
     */
    void AppendCylinder(int nsides, float h, float r1, float r2,
                        int dualside, int smooth, int close);
    /* [cmd:asph] Sphere centered on (0,0,0) with BBox [-size][size]
        - ndivisions : number of subdivisions each Sphere triangle will sustain.
        - size : size of the Sphere.
     */
    void AppendSphere(int ndivisions, float r);
    /* [cmd:acap] Capsule centered on (0,0,0) with BBox [-r, -(r+h) ,-r][r, (r+h) ,r]
        - ndivisions : number of subdivisions each Sphere triangle will sustain.
        - h : Inner height.
        - r : Radius. TODO:convert to diameter to be coherent with other funcs
     */
    void AppendCapsule(int ndivisions, float h, float r);
    /* [cmd:ato] Torus centered on (0,0,0) with BBox [-r2][r2]
        - ndivisions : number of subdivisions of the torus.
        - r1 : Inner radius. TODO:convert to diameter to be coherent with other funcs
        - r2 : Outer radius. TODO:convert to diameter to be coherent with other funcs
     */
    void AppendTorus(int ndivisions, float r1, float r2);
    /* [cmd:ab] Box centered on (0,0,0) with BBox [-.5 * size][.5 * size]
        - size : size of the box.
        - chamf : size of the chamfer.
        - smooth : if (1) will smooth normals : TOOD:TOREMOVE : smooth should be handled elsewhere
     */
    void AppendBox(vec3 const &size, float chamf = 0.f);
    //Same as AppendBox
    void AppendSmoothChamfBox(vec3 const &size, float chamf);
    //Same as AppendBox
    void AppendFlatChamfBox(vec3 const &size, float chamf);
    //Same as AppendBox
    void AppendBox(vec3 const &size, float chamf, bool smooth);
    /* [cmd:as]
       Append a Star centered on (0,0,0) contained within a disc of "max(r1, r2)" radius.
        - nbranches : Number of branches.
        - r1 : Length of the branches.
        - r2 : Length of the "branch" located between r1-branches.
        - fade : if (1) in-between branches use Color2.
        - fade2 : if (1) Star branches use Color2.
     */
    void AppendStar(int nbranches, float r1, float r2,
                    int fade = 0, int fade2 = 0);
    /* [cmd:aes] Star centered on (0,0,0) contained within a disc of "max(max(r1, r2), max(r1 + extrar, r2 + extrar))" radius.
       Expanded star branches use Color2.
        - nbranches : Number of branches.
        - r1 : Length of the branches.
        - r2 : Length of the "branch" located between r1-branches.
        - extrar : Extra length added to expand all branches.
     */
    void AppendExpandedStar(int nbranches, float r1, float r2, float extrar);
    /* [cmd:ad] Disc centered on (0,0,0) with BBox [-size][size]
        - nbsides : Number of sides.
        - r : Radius. TODO:convert to diameter to be coherent with other funcs
        - fade : if (1) Outer vertices will use Color2
     */
    void AppendDisc(int nsides, float r, int fade = 0);
    /* [cmd:at] Triangle centered on (0,0,0) contained within a disc of "size" radius.
        - size : Size of vector : origin-TO-vertex.
        - fade : if (1) 2nd & 3rd Vertices will use Color2
     */
    void AppendSimpleTriangle(float size, int fade = 0);
    /* [cmd:aq] Quad centered on (0,0,0) contained within BBox [-size,0,-size][size,0,size]
        - size : Size of quad.
        - fade : if (1) 3rd & 4th Vertices will use Color2
     */
    void AppendSimpleQuad(float size, int fade = 0);
private:
    //complex version of above one
    void AppendSimpleQuad(vec2 p1, vec2 p2, float z = 0.f, int fade = 0);
public:
    /* [cmd:acg] Gear centered on (0,0,0) contained within BBox [-max(r1,r2), -.5*h, -max(r1, r2)][max(r1, r2), .5*h, max(r1, r2)]
        - h : Height of the Gear.
        - r10 : Upper Inner radius.
        - r20 : Lower Inner radius.
        - r1  : Upper Outer radius.
        - r2  : Lower Outer radius.
        - r12 : Upper Cog radius.
        - r22 : Lower Cog radius.
        - sidemul : multiplier for the size of the cogs.
        - offset : useless
     */
    void AppendCog(int nbsides, float h, float r10, float r20, float r1,
                   float r2, float r12, float r22, float sidemul, int offset);

    //-------------------------------------------------------------------------
    //TODO : Mesh Bone operations
    //-------------------------------------------------------------------------
    //void AddBone(int parent_id) {}

    //Convenience functions
public:
    int GetVertexCount() { return m_vert.Count(); }
    vec3 const &GetVertexLocation(int i) { return m_vert[i].m1; }

private:
    vec4 m_color, m_color2;
    Array<uint16_t> m_indices;
#if VERTEX_USEAGE == VU_BONES
    //TODO : -- BONE SUPPORT --
    //TODO : <COORD, NORM, COLOR, BONE_ID, BONE_WEIGHT>
    Array<vec3, vec3, vec4, ivec2, vec2> m_vert;
    //TODO : More bone blend support than 2 ?
#elif VERTEX_USEAGE == VU_TEX_UV
    //TODO : -- UV SUPPORT --
    //TODO : <COORD, NORM, COLOR, UV>
    Array<vec3, vec3, vec4, vec2> m_vert;
#else
    //-- VANILLA --
    //<COORD, NORM, COLOR>
    Array<vec3, vec3, vec4> m_vert;
#endif

    //<vert count, indices count>
    Array<int, int> m_cursors;
    //When this flag is up, negative scaling will not invert faces.
    bool m_ignore_winding_on_scale;
    //Texture coordinate modifiers.
    vec2 m_texcoord_offset;
    vec2 m_texcoord_scale;

    /* FIXME: put this in a separate class so that we can copy meshes. */
    struct
    {
        /* FIXME: very naughty way of handling debug render modes */
        Array<Shader *>shader;
#if VERTEX_USEAGE == VU_BONES
        //TODO : -- BONE SUPPORT --
        Array<ShaderAttrib> coord, norm, color, bone_id, bone_weight;
#elif VERTEX_USEAGE == VU_TEX_UV
        //-- UV SUPPORT --
        Array<ShaderAttrib> coord, norm, color, tex_coord;
#else
        //-- VANILLA --
        Array<ShaderAttrib> coord, norm, color;
#endif
        Array<ShaderUniform> modelview, invmodelview, view, invview, proj, normalmat, damage, lights;

        VertexDeclaration *vdecl;
        VertexBuffer *vbo;
        IndexBuffer *ibo;
        int vertexcount, indexcount;
    }
    m_gpu;
};

} /* namespace lol */

#endif /* __EASYMESH_EASYMESH_H__ */

