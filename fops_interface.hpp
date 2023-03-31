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
		kOutRagne, kOffsetOutOfRagne, kPatterSizeOutOfRange
	};
	// STATE
	enum class state_t {
		kFound, kNotFound, kCheckSuccesfull
	};
	// INDEX
	struct Index {
		static constexpr uint8_t kError{};
		static constexpr uint8_t kState{1};
		static constexpr uint8_t kPtr{2};
	};
	// RET
	using ret_t = std::variant<error_t, state_t, uint8_t const*>;
}
#endif
