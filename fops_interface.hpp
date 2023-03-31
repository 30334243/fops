#ifndef FOPS_INTERFACE_HPP
#define FOPS_INTERFACE_HPP

#include <cstdint>
#include <variant>

// NAMESPACE FOPS
namespace Fops {
	constexpr uint8_t kSig{2};
	constexpr uint8_t kLsig{4};
	static char const* kSigExt{".sig"};
	static char const* kLsigExt{".lsig"};
	// ERROR
	enum class error_t {
		kOutRagne, kOffsetOutRagne, kPatterSizeOutRange
	};
	// STATE
	enum class state_t {
		kFound, kNotFound
	};
	// RET
	using ret_t = std::variant<error_t, state_t, uint8_t const*>;
}
#endif
