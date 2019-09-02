#include "stdafx.h"
#include "Scene.h"

//Scene::Scene()
//{
//
//}
//
//Scene::~Scene()
//{
//
//}
//
//bool Scene::Initialize()
//{
//    return true;
//}
//
//bool Scene::Shutdown()
//{
//    return true;
//}
//
//void Scene::Update(double delta)
//{
//
//}
//
//bool Scene::AddRenderable(std::shared_ptr<Renderable> renderable)
//{
//    m_renderables.emplace_back(std::move(renderable));
//
//    return true;
//}
//
//bool Scene::RemoveRenderable(std::shared_ptr<Renderable> renderable)
//{
//    m_renderables.erase(std::remove_if(m_renderables.begin(), m_renderables.end(), [&renderable](const std::shared_ptr<Renderable> r) { return r == renderable; }), m_renderables.end());
//
//    return true;
//}