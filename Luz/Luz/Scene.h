#pragma once
#ifndef SCENE_H
#define SCENE_H

#ifndef TYPEDEFS_H
#include "TypeDefs.h"
#endif

class Renderable;

__interface IScene
{
    virtual bool Initialize() = 0;
    virtual UpdateResult Update(double delta) = 0;
    virtual bool Shutdown() = 0;
};


class Scene : public IScene
{
public:
    Scene();
    ~Scene();

    virtual bool Initialize() override;
    virtual UpdateResult Update(double delta) override;
    virtual bool Shutdown();

    bool AddRenderable(std::shared_ptr<Renderable> renderable);
    bool RemoveRenderable(std::shared_ptr<Renderable> renderable);

private:
    std::vector<std::shared_ptr<Renderable>> m_renderables;
};

#endif // !ISCENE_H
