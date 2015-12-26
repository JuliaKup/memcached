#include <deque>
#include <unordered_map>
#include <time.h>
#include <iostream>
#include <string>

template <typename Key, typename Value>
class Cache {
 private:
 	uint64_t limit;
 	std::unordered_map<Key, Value> map;
 	std::unordered_map<Key, time_t> exp_times;
 	std::unordered_map<Key, time_t> update_times;
 	std::unordered_map<Key, uint32_t> flags;
 	std::deque<Key *> order;
 	std::unordered_map<Key, typename std::deque<Key *>::iterator> ordpt;

 public:
 	Cache(size_t size) : limit(size) {};

 	bool get(Key key, Value* value, time_t* exp_time, time_t* update_time, uint32_t *flag) {
 		auto it = map.find(key);

 		if (it == map.end()) return false;

 		*value = map[key];
 		*exp_time = exp_times[key];
 		*update_time = update_times[key];
 		*flag = flags[key];

 		order.push_back(&key);  // setting the item to its new place in order oldest->newest
 		ordpt[key] = order.end() - 1;  //  pointing at the new place of the item in order

 		return true;
 	}

	void set(Key key, uint32_t flag, time_t exp_time, Value value) {
 		auto it = map.find(key);

 		time_t update_time;
 		time(&update_time);

 		uint32_t tm = (uint32_t) exp_time;

	 	if (tm <= 60 * 60 * 24 * 30 && tm != 0) {
	 		exp_time += (uint32_t) update_time;
		}

 		if (it != map.end()) {
	 		remove(key);
 		}

	 	if (order.size() == limit) remove(**order.begin());

	 	map[key] = value;
		exp_times[key] = exp_time;
 		update_times[key] = update_time;
 		flags[key] = flag;

 		order.push_back(&key);
 		ordpt.insert({key, order.end() - 1});
 	}

 	bool remove(Key key) {
 		auto it = map.find(key);
 		if (it == map.end()) return false;

 		order.erase(ordpt[key]);
 		ordpt.erase(key);

 		map.erase(key);
 		exp_times.erase(key);
 		update_times.erase(key);
 		flags.erase(key);

 		return true;
 	}

 	void clean() {
 		time_t current_time;
 		time(&current_time);

 		for (auto ptr = order.begin(); ptr < order.end(); ++ptr) {
 			if (current_time >= exp_times[**ptr] && exp_times[**ptr] != 0) {
 				remove(**ptr);
 			}
 		}
 	}
};