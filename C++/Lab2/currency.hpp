#include <string>
#include <map>
#include <vector>

namespace Currency {
    struct CurrencyStats {
        const std::string currency;
        const float average;
        const float median;
    
        CurrencyStats(std::string c, float a, float m) : 
                currency(c), average(a), median(m) {}
    };
    
    class CurrencyInfo {
        public:
            CurrencyInfo(std::initializer_list<std::string> currencies);
    
            CurrencyInfo(std::vector<std::string> currencies);
    
            const std::vector<float>& operator[](const std::string& currency) const;
    
            void Push(std::string currency, float value);
    
            float GetAverage(const std::string& currency) const;
    
            float GetMedian(const std::string& currency) const;
    
            std::vector<std::string> GetCurrencies() const;
            
            struct Iterator {
                using iterator_category = std::forward_iterator_tag;
    
                Iterator(const std::map<std::string, std::vector<float>>::iterator& i);
                Iterator(const Iterator& i);

                Iterator& operator++();

                bool operator== (const Iterator& other);
    
                CurrencyStats const operator*() const;
                
                private:
                    std::map<std::string, std::vector<float>>::iterator _it;
            };
    
        Iterator begin();
        Iterator end();
    
        private:
            std::map<std::string, std::vector<float>> _info;
    };
}
