#ifndef SIERPINSKI_TRIANGLE_APPLICATION_H_
#define SIERPINSKI_TRIANGLE_APPLICATION_H_

#include "ObsidianEngine/Application.h"
#include "ObsidianEngine/Vertex.h"
#include "ObsidianEngine/MeshComponent.h"
#include "ObsidianEngine/Entity.h"

#include <vector>
#include <unordered_map>

using namespace ObsidianEngine;

struct VertexKey 
{
    Vector2 pos;
    Vector3 color;

    bool operator==(const VertexKey& other) const 
    {
        return pos.x == other.pos.x && pos.y == other.pos.y && color.x == other.color.x && color.y == other.color.y && color.z == other.color.z;
    }
};

struct VertexKeyHash 
{
    size_t operator()(const VertexKey& v) const 
    {
        size_t h1 = std::hash<float>()(v.pos.x);
        size_t h2 = std::hash<float>()(v.pos.y);
        size_t h3 = std::hash<float>()(v.color.x);
        return h1 ^ (h2 << 1) ^ (h3 << 2);
    }
};

struct Triangle 
{
    Vertex a, b, c;
    bool isHole = false;

    Triangle(Vertex _a, Vertex _b, Vertex _c, bool _isHole = false) : a(_a), b(_b), c(_c), isHole(_isHole) {}

    void subdivide(std::vector<Triangle>& outSolid, std::vector<Triangle>& outHoles) const 
    {
        //       a
        //      / \
        //    v1---v3
        //    / \ / \
        //   b---v2---c
        Vertex v1{ Vector2::lerp(a.pos, b.pos, 0.5f), a.color };
        Vertex v2{ Vector2::lerp(b.pos, c.pos, 0.5f), a.color };
        Vertex v3{ Vector2::lerp(c.pos, a.pos, 0.5f), a.color };

        outSolid.push_back({ a, v3, v1, false });
        outSolid.push_back({ v1, v2, b, false });
        outSolid.push_back({ v3, c, v2, false });

        outHoles.push_back({ v1, v3, v2, true });
    }

    static std::vector<Triangle> generateSierpinski(Triangle base, uint32_t depth, float holeFade) 
    {
        std::vector<Triangle> solids = { base };
        std::vector<Triangle> allTriangles;

        for (uint32_t i = 0; i < depth; i++) 
        {
            std::vector<Triangle> nextSolids;
            std::vector<Triangle> currentHoles;

            for (const auto& tri : solids) 
            {
                tri.subdivide(nextSolids, currentHoles);
            }

            bool isNewestLevel = (i == depth - 1);

            Vector3 holeColor = isNewestLevel ? (Vector3{ 1.0f, 1.0f, 1.0f } *(1.0f - holeFade)) : Vector3{ 0.0f, 0.0f, 0.0f };

            for (auto& h : currentHoles) 
            {
                h.a.color = h.b.color = h.c.color = holeColor;
                allTriangles.push_back(h);
            }
            solids = std::move(nextSolids);
        }

        for (auto& s : solids)
        {
            s.a.color = s.b.color = s.c.color = Vector3{ 1, 1, 1 };
            allTriangles.push_back(s);
        }

        for (Triangle& t : allTriangles) 
        {
            float area = (t.b.pos.x - t.a.pos.x) * (t.c.pos.y - t.a.pos.y) - (t.b.pos.y - t.a.pos.y) * (t.c.pos.x - t.a.pos.x);
            if (area < 0.0f) { std::swap(t.b, t.c); }
        }

        return allTriangles;
    }

    static std::pair<std::vector<Vertex>, std::vector<uint16_t>> buildMesh(const std::vector<Triangle>& triangles)
    {
        std::vector<Vertex> vertices;
        std::vector<uint16_t> indices;

        std::unordered_map<VertexKey, uint16_t, VertexKeyHash> lookup;

        for (const Triangle& t : triangles)
        {
            Vertex corners[3] = { t.a, t.b, t.c };

            for (int i = 0; i < 3; i++)
            {
                const Vertex& v = corners[i];
                VertexKey key{ v.pos, v.color };

                auto it = lookup.find(key);
                if (it != lookup.end())
                {
                    indices.push_back(it->second);
                }
                else
                {
                    uint16_t newIndex = static_cast<uint16_t>(vertices.size());
                    vertices.push_back(v);
                    lookup[key] = newIndex;
                    indices.push_back(newIndex);
                }
            }
        }

        return { std::move(vertices), std::move(indices) };
    }
};

class SierpinskiSystem : public ISystem {
public:
    SierpinskiSystem(Scene* scene, EntityID target, Triangle base, uint32_t maxDepth, bool enableFading = true) 
        : m_scene(scene), m_target(target), m_base(base), m_maxDepth(maxDepth), m_enableFading(enableFading) {}

    void update(Registry& registry, float dt) override 
    {
        constexpr float switchTime = 3.0f;
        m_timer += dt;

        if (m_timer >= switchTime)
        {
            m_timer = 0.0f;
            m_curDepth += m_dir;

            if (m_curDepth >= (int32_t)m_maxDepth || m_curDepth <= 0) 
            {
                m_dir *= -1;
            }

            if (!m_enableFading) 
            {
                rebuild();
            }
        }

        if (m_enableFading) 
        {
            rebuild();
        }
    }

private:
    void rebuild()
    {
        uint32_t activeDepth;
        float fade;

        if (m_enableFading) 
        {
            activeDepth = (m_dir > 0) ? (m_curDepth + 1) : m_curDepth;
            fade = (m_dir > 0) ? m_timer : (1.0f - m_timer);
        }
        else 
        {
            activeDepth = static_cast<uint32_t>(m_curDepth);
            fade = 1.0f;
        }

        if (activeDepth == 0) 
        {
            auto& mesh = m_scene->getRegistry().getComponent<MeshComponent>(m_target);
            mesh.vertices = { m_base.a, m_base.b, m_base.c };
            mesh.indices = { 0, 1, 2 };
            mesh.isDirty = true;
            return;
        }

        std::vector<Triangle> triangles = Triangle::generateSierpinski(m_base, activeDepth, fade);
        if (triangles.empty()) return;

        auto [verts, inds] = Triangle::buildMesh(triangles);
        auto& mesh = m_scene->getRegistry().getComponent<MeshComponent>(m_target);
        mesh.vertices = std::move(verts);
        mesh.indices = std::move(inds);
        mesh.isDirty = true;
    }

    Scene* m_scene;
    EntityID m_target;
    Triangle m_base;
    uint32_t m_maxDepth;
    int32_t m_curDepth = 0;
    int m_dir = 1;
    float m_timer = 0.0f;
    bool m_enableFading;
};

class SierpinskiTriangleApplication : public ObsidianEngine::Application 
{
protected:
    void onStartup() override {
        constexpr float size = 0.9f;
        constexpr Vector3 color{ 1, 1, 1 };
        
        Triangle tri = {
            { { 0.0f, -size }, color },
            { { size, size }, color },
            { { -size, size }, color }
        };

        EntityID entity = m_activeScene->createEntity();
        m_activeScene->getRegistry().addComponent<MeshComponent>(entity, std::vector<Vertex>{tri.a, tri.b ,tri.c}, std::vector<uint16_t>{0, 1, 2}, -1);

        m_activeScene->addSystem<SierpinskiSystem>(m_activeScene.get(), entity, tri, 6, true);
    }
};

#endif