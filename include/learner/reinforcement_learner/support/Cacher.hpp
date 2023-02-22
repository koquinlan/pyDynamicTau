#pragma once

#include <any>
#include <string>
#include <tuple>
#include <map>
#include <utility>

class Cacher {
public:
    std::map<std::string, std::any> cache;

    template<typename T>
    T useCache(const std::string functionName, std::function< T() > func) {
        if (notCached(functionName)) {
          storeInCache(functionName, func());
        }
        return getFromCache<T>(functionName);
    }

    bool notCached(const std::string& functionName) {
      return cache.find(functionName) == cache.end();
    }

    void storeInCache(const std::string& functionName, std::any value) {
      cache[functionName] = std::move(value);
    }

    template<typename T>
    T getFromCache(const std::string& functionName) {
        return std::any_cast<T>(cache[functionName]);
    }
};
