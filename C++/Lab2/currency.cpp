#include <map>
#include <algorithm>
#include <string>
#include <vector>
#include <numeric>
#include "currency.hpp"

namespace Currency {

    CurrencyInfo::CurrencyInfo(std::initializer_list<std::string> currencies) :
        _info(std::map<std::string, std::vector<float>>()) {
            for (auto currency : currencies) {
                _info[currency] = std::vector<float>();
            }
    }

    CurrencyInfo::CurrencyInfo(std::vector<std::string> currencies) :
        _info(std::map<std::string, std::vector<float>>()) {
            for (auto currency : currencies) {
                _info[currency] = std::vector<float>();
            }
    }

    const std::vector<float>& CurrencyInfo::operator[](const std::string& currency) const {
        return _info.at(currency);
    }

    void CurrencyInfo::Push(std::string currency, float value) {
        auto& v = _info[currency];
        v.push_back(value);
        // perform an nth element execution in non-const method
        std::nth_element(v.begin(), v.begin() + v.size() / 2, v.end());
    }

    float CurrencyInfo::GetAverage(const std::string& currency) const {
        const auto& values = _info.at(currency);
        return std::accumulate(values.cbegin(), values.cend(), 0.0f) / (float)values.size();
    }

    float CurrencyInfo::GetMedian(const std::string& currency) const {
        const auto& values = _info.at(currency);
        return values[values.size() / 2];
    }

    std::vector<std::string> CurrencyInfo::GetCurrencies() const {
        auto result = std::vector<std::string>();
        result.reserve(_info.size());
        std::transform(_info.begin(), _info.end(),
            std::back_inserter(result),
            [](auto pair){ return pair.first; });
        return result;
    }

    CurrencyInfo::Iterator::Iterator(const std::map<std::string, std::vector<float>>::iterator& i) : _it(i) {}

    CurrencyInfo::Iterator::Iterator(const Iterator& i) : _it(i._it) {}

    CurrencyInfo::Iterator& CurrencyInfo::Iterator::operator++() {
        ++_it;
        return *this;
    }

    bool CurrencyInfo::Iterator::operator== (const CurrencyInfo::Iterator& other) {
        return _it == other._it;
    }

    CurrencyStats const CurrencyInfo::Iterator::operator*() const {
        auto currency = _it->first;
        auto values = _it->second;

        auto average = (float)(std::accumulate(values.begin(), values.end(), 0.0f) / (float)values.size());
        return CurrencyStats(currency, average, values[values.size() / 2]);
    }

    CurrencyInfo::Iterator CurrencyInfo::begin() {
        return Iterator(_info.begin());
    }

    CurrencyInfo::Iterator CurrencyInfo::end() {
        return Iterator(_info.end());
    }

}
