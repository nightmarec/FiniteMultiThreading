#pragma once
#include <map>

template<typename K, typename V>
class CThreadSafeMap
{
public:
	CThreadSafeMap()
	{
		omp_init_lock(&lock);
	}
	~CThreadSafeMap()
	{
		omp_unset_lock(&lock);
		if (!map_.empty())
		{
			map_.clear();
		}
	}

	void insert(const K& key, const V& value)
	{
		omp_set_lock(&lock);
		map_.insert(std::pair<K, V>(key, value));
		omp_unset_lock(&lock);
	}

	bool find(const K& key, V& value)
	{
		bool ret = false;
		omp_set_lock(&lock);
		if (map_.size() > 0)
		{
			typedef typename std::map<K, V>::iterator iter_thread;
			iter_thread iter = map_.find(key);
			if (iter != map_.end())
			{
				value = iter->second;
				ret = true;
			}
		}
		omp_unset_lock(&lock);

		return ret;
	}

	bool findAndSet(const K& key, const V& new_value, V& old_value)
	{
		bool ret = false;
		omp_set_lock(&lock);
		if (map_.size() > 0)
		{
			typedef typename std::map<K, V>::iterator iter_thread;
			iter_thread iter = map_.find(key);
			if (iter != map_.end())
			{
				old_value = iter->second;
				map_.erase(iter);
				map_.insert(std::pair<K, V>(key, new_value));
				ret = true;
			}
		}
		omp_unset_lock(&lock);

		return ret;
	}

	void erase(const K& key)
	{
		omp_set_lock(&lock);
		map_.erase(key);
		omp_unset_lock(&lock);
	}

	void clear()
	{
		omp_set_lock(&lock);
		if (!map_.empty())
		{
			map_.clear();
		}
		omp_unset_lock(&lock);
		return;
	}

	int size()
	{
		omp_set_lock(&lock);
		int size = map_.size();
		omp_unset_lock(&lock);
		return size;
	}

	bool empty()
	{
		omp_set_lock(&lock);
		bool is_empty = map_.empty();
		omp_unset_lock(&lock);
		return is_empty;
	}

private:
	omp_lock_t lock;
	std::map<K, V> map_;
};