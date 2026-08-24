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

namespace asteroids
{
    /// Forward declaration
    class Actor;

    namespace comp
    {
        class Component
        {
        public:
            /**
             * @brief Create a component for a given actor.
             *
             * @param owner       The actor that owns this component.
             * @param updateOrder The order for update. Lower order indicates earlier updates. Default: 100
             */
            explicit constexpr Component(Actor* owner, const int updateOrder = 100) noexcept
                : _owner{ owner }, _updateOrder{ updateOrder } {}


            /// Update this component by delta time.
            virtual void update(float deltaTime) noexcept = 0;


            [[nodiscard]] int getUpdateOrder() const { return _updateOrder; }


            virtual ~Component() = default;

        private:
            [[maybe_unused]] Actor* _owner{ nullptr };
            int _updateOrder{};
        };
    } // namespace comp

} // namespace asteroids
