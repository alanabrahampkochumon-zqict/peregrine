#pragma once
/**
 * @file Component.h
 * @author Alan Abraham
 * @date Created on: August 21, 2026
 *
 * @brief Interface for various components like transform, draw, and so on.
 *
 * @copyright Copyright (c) 2026 Alan Abraham P Kochumon
 */

namespace asteroids::comp
{

    class Component
    {
        /**
         * @brief Create a component for a given actor.
         *
         * @param owner       The actor that owns this component.
         * @param updateOrder The order for update. Lower order indicates earlier updates. Default: 100
         */
        constexpr Component(class Actor* owner, int updateOrder = 100) noexcept;

        virtual ~Component();

        /// Update this component by delta time.
        virtual void update(float deltaTime) noexcept;

        int getUpdateOrder() const { return _updateOrder; }

    private:
        int _updateOrder{};

        class Actor* owner{ nullptr };
    };

} // namespace asteroids::comp
