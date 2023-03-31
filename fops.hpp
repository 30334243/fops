#ifndef FOPS_HPP
#define FOPS_HPP

#include <iostream>
#include <fstream>
#include <cstdint>
#include <vector>
#include <map>
#include <string>
#include <algorithm>
#include "fops_interface.hpp"

// NAMESPACE FOPS
namespace Fops {
	// WRITE
	static void Write(std::ofstream& dst,
							uint8_t const* pbeg,
							size_t const sz,
							uint8_t const type) {
		dst.write((char*)&sz, type);
		dst.write((char*)pbeg, sz);
	}
	// WRITE
	static constexpr auto Write(std::ofstream& dst) {
		return [&dst] (uint8_t const* pbeg,
							size_t const sz,
							uint8_t const type = kSig) {
			Write(dst, pbeg, sz, type);
		};
	}
}
// NAMESPACE FOPS
namespace Fops {
	// READ
	static std::vector<uint8_t> Read(std::ifstream& src,
												uint8_t const type = kSig) {
		size_t sz{};
		src.read((char*)&sz, type);
		std::vector<uint8_t> vec(sz);
		src.read((char*)vec.data(), vec.size());
		return vec;
	}
	// READ
	static constexpr auto InitRead(std::ifstream& src,
											 uint8_t const type = kSig) {
		return [&src, type] () {
			return Read(src, type);
		};
	}
}
// NAMESPACE FOPS WRITE
namespace Fops::Multi {
	// WRITE
	template<class T, class Write>
		static constexpr auto Write(std::map<uint64_t, T>& map, Write write) {
			return [&map, write] (std::ofstream& dst) {
				return [&map, write, &dst] (uint8_t const* pbeg,
													 size_t const sz,
													 uint64_t const key,
													 uint8_t const type = kSig) {
					auto it{map.find(key)};
					if (it != map.cend()) {
						write(dst, pbeg, sz, type);
					} else {
						char const* type_ext{type == kSig ? kSigExt : kLsigExt};
						std::ofstream* new_dst{
							new std::ofstream{
								std::to_string(key) + type_ext}};
						write(new_dst, pbeg, sz, type);
						map[key] = std::move(new_dst);
					}
				};
			};
		}
}
// NAMESPACE FOPS CHECK
namespace Fops::Check {
	static ret_t Search(uint8_t const* pbeg,
							  uint8_t const* pend,
							  uint8_t const* pattern_pbeg,
							  uint8_t const* pattern_pend,
							  size_t offset = 0) {
		ret_t ret{};
		if (!(pbeg < pend)) {
			ret = error_t::kOutRagne;
		} else if (!(pbeg + offset < pend)) {
			ret = error_t::kOffsetOutRagne;
		} else if (!((pattern_pend - pattern_pbeg) < pend - pend)) {
			ret = error_t::kPatterSizeOutRange;
		} else {
			ret = state_t::kCheckSuccesfull;
		}
		return ret;
	}
}
// NAMESPACE FOPS
namespace Fops {
	// SEARCH
	static ret_t Search(uint8_t const* pbeg,
							  uint8_t const* pend,
							  uint8_t const* pattern_pbeg,
							  uint8_t const* pattern_pend,
							  size_t offset = 0) {
		ret_t ret{};
		uint8_t const* new_pend{offset ? pbeg + offset : pend};
		uint8_t const* it{std::search(pbeg, new_pend, pattern_pbeg, pattern_pend)}; 
		if (it != new_pend) {
			ret = it;
		} else {
			ret = state_t::kNotFound;
		}
		return ret;
	}
}
#endif
