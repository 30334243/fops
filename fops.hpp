#ifndef FOPS_HPP
#define FOPS_HPP

#include <iostream>
#include <fstream>
#include <cstdint>
#include <vector>
#include <map>
#include <string>
#include <random>
#include <algorithm>
#include <memory>
#include <functional>
#include <thread>
#include <chrono>
#include <filesystem>
#include "fops_interface.hpp"

// NAMESPACE FOPS
namespace Fops {
	// NAMESPACE FILESYSTEM
	namespace fs = std::filesystem;
	// WRITE
	static void Write(std::ofstream& dst,
							uint8_t const* pbeg,
							size_t const sz,
							uint8_t const type) {
		dst.write((char*)&sz, type);
		dst.write((char*)pbeg, sz);
	}
	// WRITE
	template<class T>
		static void Write(T dst,
								uint8_t const* pbeg,
								size_t const sz,
								uint8_t const type) {
			dst->write((char*)&sz, type);
			dst->write((char*)pbeg, sz);
		}
	// WRITE
	constexpr auto Write() {
		return [] (auto dst,
					  uint8_t const* pbeg,
					  size_t const sz,
					  uint8_t const type) {
			Write(dst, pbeg, sz, type);
		};
	}
	// WRITE
	constexpr auto Write(uint8_t const* pbeg, size_t const sz, uint8_t const type) {
		return [pbeg, sz, type] (std::shared_ptr<std::ofstream> dst) {
			Write(dst, pbeg, sz, type);
		};
	}
	// WRITE
	static constexpr auto InitWrite(std::ofstream& dst,
											  uint8_t const type = kSig) {
		return [&dst, type] (uint8_t const* pbeg,
									size_t const sz) {
			Write(dst, pbeg, sz, type);
		};
	}
	// WRITE
	static constexpr auto InitWrite(uint8_t const type) {
		return [type] (uint8_t const* pbeg, size_t const sz) {
			return [type, pbeg, sz] (auto dst) {
				Write(dst, pbeg, sz, type);
			};
		};
	}
	// CREATE
	static auto InitCreate(fs::path out, uint8_t const type) {
		return [out, type] (fs::path name) {
			std::string_view const ext{type == Fops::kSig ?
				Fops::kSigExt : Fops::kLsigExt};
			std::string d{out/name};
			std::shared_ptr<std::ofstream> new_dst{
				new std::ofstream{
					out/name.concat(ext.cbegin(), ext.cend()),
						std::ios::binary}};
			return new_dst;
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
// NAMESPACE FOPS CHECK
namespace Fops::Check {
	// OUT OF RANGE
	static ret_t OutOfRagne(uint8_t const* pbeg,
									uint8_t const* pend,
									uint8_t const* pattern_pbeg,
									uint8_t const* pattern_pend,
									size_t offset = 0) {
		ret_t ret{};
		if (!(pbeg < pend)) {
			ret = error_t::kOutRagne;
		} else if (!(pbeg + offset < pend)) {
			ret = error_t::kOffsetOutOfRagne;
		} else if (!((pend - pend) < (pattern_pend - pattern_pbeg))) {
			ret = error_t::kPatterSizeOutOfRange;
		} else {
			ret = state_t::kCheckSuccesfull;
		}
		return ret;
	}
	// OUT OF RANGE
	template<class Action>
		inline constexpr auto OutOfRagne(Action action) {
			return [action] (uint8_t const* pbeg,
								  uint8_t const* pend,
								  std::vector<std::string> const& lst,
								  size_t offset = 0) {
				for (auto const& str : lst) {
					ret_t const ret{
						OutOfRagne(pbeg, pend,
									  (uint8_t const*)str.data(),
									  (uint8_t const*)(str.data() + str.size()),
									  offset)};
					if (ret.index() == Index::kState) {
						action(pbeg, pend,
								 (uint8_t const*)str.data(),
								 (uint8_t const*)(str.data() + str.size()),
								 offset);
					}
				}
			};
		}
}
// NAMESPACE FOPS
namespace Fops {
	// DUMMY
	constexpr auto Dummy() {return[]{};};
	using dummy_t = std::function<void()>;
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
	// SEARCH
	inline constexpr auto Search() {
		return [] (uint8_t const* pbeg,
					  uint8_t const* pend,
					  uint8_t const* pattern_pbeg,
					  uint8_t const* pattern_pend,
					  size_t offset = 0) {
			return Search(pbeg,
							  pend,
							  pattern_pbeg,
							  pattern_pend,
							  offset);
		};
	}
	// SEARCH
	template<class Save = dummy_t>
		inline constexpr auto Search(Save save = dummy_t()) {
			return [save] (uint8_t const* pbeg,
								uint8_t const* pend,
								uint8_t const* pattern_pbeg,
								uint8_t const* pattern_pend,
								size_t offset = 0) {
				ret_t const ret{
					Search(pbeg, pend, pattern_pbeg, pattern_pend, offset)};
				if constexpr (!std::is_same_v<Save, dummy_t>) {
					if (ret.index() == Index::kPtr) {
						save(pbeg, pend - pbeg);
					}
				}
			};
		}
}
#endif
