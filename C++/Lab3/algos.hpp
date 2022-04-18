#include <algorithm>

namespace generic_algorithms {
    template<typename InputIt, typename T>
    InputIt find(InputIt first, InputIt last, const T& value) {
        for (; first != last; ++first) {
            if (value == *first) {
                return first;
            }
        }
        return last;
    }
    template<typename InputIt, typename Predicate>
    InputIt find_if(InputIt first, InputIt last, Predicate p) {
        for (; first != last; ++first) {
            if (p(*first)) {
                return first;
            }
        }
        return last;
    }
    template<typename InputIt, typename T>
    InputIt find_not(InputIt first, InputIt last, const T& value) {
        for (; first != last; ++first) {
            if (value != *first) {
                return first;
            }
        }
        return last;
    }
    template<typename InputIt, typename Predicate>
    InputIt find_if_not(InputIt first, InputIt last, Predicate p) {
        for (; first != last; ++first) {
            if (!p(*first)) {
                return first;
            }
        }
        return last;
    }

    template<typename InputIt, typename Predicate>
    InputIt find_backward(InputIt first, InputIt last, Predicate p) {
        --last;
        --first;

        for (; first != last; --last) {
            if (!p(*last)) {
                return last;
            }
        }
        ++first;
        return first;
    }
    
    template<typename InputIt, typename Predicate>
    bool all_of(InputIt first, InputIt last, Predicate p) {
        return find_if_not(first, last, p) == last;
    }

    template<typename InputIt, typename Predicate>
    bool any_of(InputIt first, InputIt last, Predicate p) {
        return find_if(first, last, p) != last;
    }

    template<typename InputIt, typename Predicate>
    bool none_of(InputIt first, InputIt last, Predicate p) {
        return find_if(first, last, p) == last;
    }
    
    template<typename InputIt, typename Predicate>
    bool is_sorted(InputIt first, InputIt last, Predicate p) {
        InputIt next(first);
        ++next;
        for (; next != last; ++first, ++next) {
            if (!p(*first, *next)) {
                return false;
            }
        }
        return true;
    }
    template<typename InputIt, typename Predicate>
    bool is_partitioned(InputIt first, InputIt last, Predicate p) {
        bool initial = p(*first);
        ++first;

        for (; first != last; ++first) {
            if (p(*first) != initial) {
                break;
            }
        }

        for (; first != last; ++first) {
            if (p(*first) == initial) {
                return false;
            }
        }
        
        return true;
    }

    template<typename InputIt, typename Predicate>
    bool is_palindrome(InputIt first, InputIt last, Predicate p) {
        --last;

        for (; first != last; ++first) {
            if (!p(*first, *last)) {
                return false;
            }
            if (first == --last) {
                break;
            }
        }
        
        return true;
    }
}
