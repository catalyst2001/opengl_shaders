#pragma once
#include "array_fixed.h"
#include "murmurhash.h"

#define HASH_SEED 15

/* FAST-OPERATE CONTAINER */
template<typename _type, size_t _sort_cache_size>
class fast_operate_container {
	size_t capacity;
	size_t size;
	_type *p_data;

	/* store sorted data location */
	class sort_cache_set {
		uint32_t hash;
		size_t begin;
		size_t end;

	public:
		sort_cache_set() : hash(0), begin(0), end(0) {}
		sort_cache_set(int _hash, size_t _beg, size_t _end) : hash(_hash), begin(_beg), end(_end) {}
		~sort_cache_set() {}

		size_t      get_start() { return begin; }
		size_t      get_end() { return end; }
		bool        idx_in_range(size_t idx) { return begin <= idx && end > idx; }
		uint32_t    get_hash() { return hash; }
		void        set_hash(uint32_t hsh) { hash = hsh; }
	};

	array_fixed<fast_operate_container::sort_cache_set, _sort_cache_size> sort_cache;
protected:

	/*
	 returns cache index if exists
	 otherwise add cache set by this data in fixed array cachesets
	*/
	bool get_cache_get_idx(size_t &dst_idx, uint32_t elem_hash) {
		if (sort_cache.get_size()) {
			for (size_t i = 0; i < sort_cache.get_size(); i++) {
				if (sort_cache[i].get_hash() == elem_hash) {
					dst_idx = i;
					return true;
				}
			}
		}
		return false;
	}

public:
	fast_operate_container(size_t _cap) : capacity(_cap), size(0), p_data(NULL) {}
	~fast_operate_container() {}

	bool add_fast(size_t *p_dst_loc_idx, size_t *p_dst_cache_loc, _type elem) {
		// cache is empty ?
		uint32_t elem_hash = murmurhash(&elem, sizeof(_type), HASH_SEED);



	}

	bool remove_fast(size_t idx_to_remove) {

	}



};

