#pragma once

#include "glm/ext/matrix_transform.hpp"
#include "glm/fwd.hpp"
#include "glm/trigonometric.hpp"
#include "vulkan/model.h"
#include <memory>

namespace ana
{
struct TransformComponent
{
    glm::vec3 translation{};
    glm::vec3 scale{ 1.f, 1.f, 1.f };
    glm::vec3 rotation{};

    glm ::mat4 mat4()
    {

        auto transform = glm::translate(glm::mat4{ 1.f }, translation);
        transform      = glm::rotate(transform, rotation.y, { 0.f, 1.f, 0.f });
        transform      = glm::rotate(transform, rotation.x, { 1.f, 0.f, 0.f });
        transform      = glm::rotate(transform, rotation.z, { 0.f, 0.f, 1.f });
        transform      = glm::scale(transform, scale);
        return transform;
    }
};

class GameObject
{
public:
    using id_t = unsigned int;

    static GameObject createGameObject()
    {
        static id_t currentId = 0;
        return GameObject{ currentId++ };
    }

    GameObject(const GameObject&)            = delete;
    GameObject& operator=(const GameObject&) = delete;
    GameObject(GameObject&&)                 = default;
    GameObject& operator=(GameObject&&)      = default;

    id_t getId() const
    {
        return Id;
    }

    std::shared_ptr<ana::Model> model{};
    glm::vec3 color{};
    TransformComponent transform{};

private:
    GameObject(id_t objId)
        : Id{ objId }
    {
    }

    id_t Id;
};

} // namespace ana