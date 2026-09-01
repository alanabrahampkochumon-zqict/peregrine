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


#include <cstdint>


namespace asteroids
{
    namespace actors
    {
        /// Forward declaration
        class Actor;
    } // namespace actors

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
            explicit constexpr Component(actors::Actor* owner, const int updateOrder = 100) noexcept
                : _owner{ owner }, _updateOrder{ updateOrder }
            {}


            /// Update this component by delta time.
            virtual void update([[maybe_unused]] float deltaTime) noexcept {};


            [[nodiscard]] int getUpdateOrder() const { return _updateOrder; }

            /// Process the input.
            /// @param keyState The keycode to process. 1-to-1 mapping to SDL3 keycodes.
            virtual void processInput([[maybe_unused]] const bool* keyState) noexcept {};

            virtual ~Component() = default;

        protected:
            [[maybe_unused]] actors::Actor* _owner;
            int _updateOrder;
        };
    } // namespace comp

} // namespace asteroids
