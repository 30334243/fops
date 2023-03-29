#ifndef FOPS_INTERFACE_HPP
#define FOPS_INTERFACE_HPP

#include <cstdint>

// NAMESPACE FOPS
namespace Fops {
	constexpr uint8_t kSig{2};
	constexpr uint8_t kLsig{4};
	static char const* kSigExt{".sig"};
	static char const* kLsigExt{".lsig"};
}
#endif
