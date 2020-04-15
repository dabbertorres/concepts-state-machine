#pragma once

#include <type_traits>

namespace freeze
{

template<typename T, typename U>
concept Same = std::is_same_v<T, U>;

template<typename T, typename... Args>
concept Callable = requires(T& t, Args&&... args)
{
    t(args...);
};

}
