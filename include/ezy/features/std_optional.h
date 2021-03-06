#ifndef EZY_FEATURES_STD_OPTIONAL_H_INCLUDED
#define EZY_FEATURES_STD_OPTIONAL_H_INCLUDED

#include "result_interface.h"
#include "common.h"

#include <optional>

namespace ezy::features
{
  namespace detail
  {
    template <typename T>
    struct inherit_std_optional_helper : feature<T, inherit_std_optional_helper>
    {
      using base = feature<T, inherit_std_optional_helper>;

      /* operator bool */
      constexpr explicit operator bool() const noexcept
      { return static_cast<bool>((*this).underlying()); }

      /* has_value() */
      constexpr bool has_value() const noexcept
      { return (*this).underlying().has_value(); }

      /* value() */
      constexpr decltype(auto) value() &
      { return (*this).underlying().value(); }

      constexpr decltype(auto) value() const &
      { return (*this).underlying().value(); }

      constexpr decltype(auto) value() &&
      { return std::move(*this).underlying().value(); }

      constexpr decltype(auto) value() const &&
      { return std::move(*this).underlying().value(); }

      /* value_or() */
      template <typename U>
      constexpr decltype(auto) value_or(U&& default_value) const &
      { return (*this).underlying().value_or(std::forward<U>(default_value)); }

      template <typename U>
      constexpr decltype(auto) value_or(U&& default_value) &&
      { return std::move(*this).underlying().value_or(std::forward<U>(default_value)); }

      // TODO consider to add
      // operator=
      // swap (alongside std::swap specialization)
      // reset
      // emplace
    };
  }

  template <typename T>
  struct inherit_std_optional :
    detail::inherit_std_optional_helper<T>,
    operator_arrow<T>,
    operator_star<T>
  {};

  /**
   * adapter for features::result_inferface
   * */
  template <typename Optional>
  struct optional_adapter;

  template <template <typename...> class Optional, typename Value>
  struct optional_adapter<Optional<Value>>
  {
    using type = Optional<Value>;
    using success_type = Value;
    using error_type = std::nullopt_t;
    inline static constexpr auto error_value = std::nullopt;

    template <typename T>
    static bool is_success(T&& t) noexcept
    {
      return t.has_value();
    }

    template <typename T>
    static decltype(auto) get_success(T&& t) noexcept
    {
      return std::forward<T>(t).value();
    }

    template <typename T>
    static decltype(auto) get_error(T&&) noexcept
    {
      return error_value;
    }

    template <typename... Ts>
    static decltype(auto) make_underlying_success(Ts&&... ts)
    {
      return type{std::in_place_t{}, std::forward<Ts>(ts)...};
    }

    template <typename... Ts>
    static decltype(auto) make_underlying_error(Ts&&... ts)
    {
      return type{error_value};
    }

    template <typename NewValue>
    struct rebind_success
    {
      using type = Optional<NewValue>;
    };
    template <typename NewValue>
    using rebind_success_t = typename rebind_success<NewValue>::type;

    template <typename NewValue>
    struct rebind_error
    {
      using type = Optional<Value>;
    };
    template <typename NewValue>
    using rebind_error_t = typename rebind_error<NewValue>::type;
  };


}

#endif
