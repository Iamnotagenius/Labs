#include <memory>
#include <type_traits>
#include <iterator>

#define MIN_BUF_CAPACITY 16

namespace custom_data_structures {
    template<typename T, class Allocator = std::allocator<T>>
    class circular_buffer {
        public:
            using allocator_type = Allocator;
            using alloc_traits = std::allocator_traits<Allocator>;

            circular_buffer(size_t size, const Allocator& allocator = Allocator()) : 
                _alloc(allocator), _capacity(size), _size(0),
                _start(0), _end(0) {
                    _data = alloc_traits::allocate(_alloc, size);
                }

            circular_buffer(std::initializer_list<T> init, const Allocator& allocator = Allocator()) : 
                _alloc(allocator), _capacity(init.size()), _size(init.size()),
                _start(0), _end(0) {
                    _data = alloc_traits::allocate(_alloc, init.size());
                    for (auto& item : init) {
                        push_back(item);
                    }
                }
            
            circular_buffer(const Allocator& allocator = Allocator()) : 
                circular_buffer(MIN_BUF_CAPACITY, allocator) {}

            circular_buffer(const circular_buffer& other) : 
                _alloc(alloc_traits::select_on_container_copy_construction(other._alloc)),
                 _capacity(other._capacity), _size(other._size), 
                _start(other._start), _end(other._end) {
                _data = alloc_traits::allocate(_alloc, other._capacity);
                for (size_t i = 0; i < _size; ++i) {
                    alloc_traits::construct(_alloc, &(*this)[i], other[i]);
                }
            }

            circular_buffer(const circular_buffer& other, const Allocator& allocator) :
                _alloc(allocator),
                 _capacity(other._capacity), _size(other._size), 
                _start(other._start), _end(other._end) {
                _data = alloc_traits::allocate(_alloc, other._capacity);
                for (size_t i = 0; i < _size; ++i) {
                    alloc_traits::construct(_alloc, &(*this)[i], other[i]);
                }
            }


            circular_buffer(circular_buffer&& other) = default;
            circular_buffer(circular_buffer&& other, const Allocator& allocator) : 
                _alloc(allocator),
                _capacity(std::move(other._capacity)),
                _size(std::move(other._size)),  
                _start(std::move(other._start)),
                _end(std::move(other._end)),
                _data(std::move(other._data)) {}

            circular_buffer& operator=(circular_buffer rhs) {
                rhs.swap(*this);
                return *this;
            }
            circular_buffer& operator=(circular_buffer&&) = default;

            ~circular_buffer() {
                for (size_t i = 0; i < _size; ++i) {
                    alloc_traits::destroy(_alloc, _data + i);
                }

                alloc_traits::deallocate(_alloc, _data, _capacity);
            }

            const Allocator& get_allocator() {
                return _alloc;
            }

            void swap(circular_buffer& other) {
                std::swap(_data, other._data);
                std::swap(_capacity, other._capacity);
                std::swap(_size, other._size);
                std::swap(_start, other._start);
                std::swap(_end, other._end);
            }

            template<typename... Args>
            void emplace_back(Args&&... args) {
                prepare_to_push_back();
                alloc_traits::construct(_alloc, _data + _end, std::forward<Args>(args)...);
                ++_end;
                _end %= _capacity;
            }

            void push_back(const T& element) {
                prepare_to_push_back();
                alloc_traits::construct(_alloc, _data + _end, element);
                ++_end;
                _end %= _capacity;
            }
            
            void push_back(T&& element) {
                emplace_back(std::move(element));
            }

            T pop_back() {
                T temp(std::move(back()));

                alloc_traits::destroy(_alloc, &back());
                --_size;
                _end = _end != 0 ? _end - 1 : _capacity - 1;
                return temp;
            }

            template<typename... Args>
            void emplace_front(Args&&... args) {
                prepare_to_push_front();
                alloc_traits::construct(_alloc, _data + _start, std::forward<Args>(args)...);
            }

            void push_front(const T& element) {
                prepare_to_push_front();
                alloc_traits::construct(_alloc, _data + _start, element);
            }

            void push_front(T&& element) {
                emplace_front(std::move(element));
            }

            T pop_front() {
                T temp(std::move(front()));

                alloc_traits::destroy(_alloc, &front());
                --_size;
                ++_start;
                _start %= _capacity;

                return temp;
            }

            void resize(size_t new_size) {
                if (new_size < _size) {
                    throw std::invalid_argument("New size must not be less than number of elements");
                }
                auto new_data = alloc_traits::allocate(_alloc, new_size);
                for (size_t i = 0; i < _size; ++i) {
                    alloc_traits::construct(_alloc, new_data + i, std::move((*this)[i]));
                }
                for (size_t i = 0; i < _size; ++i) {
                    alloc_traits::destroy(_alloc, _data + i);
                }
                alloc_traits::deallocate(_alloc, _data, _capacity);
                _data = new_data;
                _start = 0;
                _end = _size;
                _capacity = new_size;
            }

            T& front() {
                return _data[_start];
            }

            const T& front() const {
                return _data[_start];
            }

            T& back() {
                return _data[(_start + _size - 1) % _capacity];
            }

            const T& back() const {
                return _data[(_start + _size - 1) % _capacity];
            }

            size_t size() const {
                return _size;
            }

            size_t capacity() const {
                return _capacity;
            }

            bool empty() const {
                return _size == 0;
            }

            T* data() {
                return _data;
            }

            const T* data() const {
                return _data;
            }


            T& operator[](size_t index) {
                return _data[(_start + index % _size) % _capacity];
            }

            const T& operator[](size_t index) const {
                return _data[(_start + index % _size) % _capacity];
            }
            
            template<bool Is_const>
            class iterator {
                using iterator_category = std::random_access_iterator_tag;
                using value_type = T;
                using difference_type = ptrdiff_t;
                using reference = std::conditional_t<Is_const, const T&, T&>;
                using pointer = std::conditional_t<Is_const, const T*, T*>;

                private:
                    size_t actual_index() const { 
                        return (_start + (_index % _size + _size) % _size) % _capacity; 
                    }
                    pointer _data;
                    difference_type _index;
                    size_t _capacity, _start, _size;

                public:
                iterator() : _data(nullptr), _index(0), _capacity(0), _start(0), _size(0) {}

                iterator(pointer data, size_t capacity, size_t start_index, size_t index, size_t size) : 
                    _data(data), _index(index), 
                    _capacity(capacity), _start(start_index), _size(size) {}
                
                iterator(const iterator& it) = default;
                iterator& operator=(const iterator& it) = default;
                
                reference operator*() {
                    return *(_data + actual_index());
                }

                iterator& operator++() {
                    ++_index;
                    return *this;
                }

                const iterator& operator++(int) {
                    iterator temp = *this;
                    ++(*this);
                    return temp;
                }

                iterator& operator--() {
                    --_index;
                    return *this;
                }

                const iterator& operator--(int) {
                    iterator temp = *this;
                    --(*this);
                    return temp;
                }

                friend bool operator==(const iterator& lhs, const iterator& rhs) {
                    return lhs._index == rhs._index && 
                           lhs._data == rhs._data &&
                           lhs._size == rhs._size;
                }

                friend bool operator!=(const iterator& lhs, const iterator& rhs) { 
                    return !(lhs == rhs);
                }

                friend bool operator<(const iterator& lhs, const iterator& rhs) {
                    return lhs._index < rhs._index;
                }

                friend bool operator>(const iterator& lhs, const iterator& rhs) {
                    return rhs < lhs;
                }

                friend bool operator>=(const iterator& lhs, const iterator& rhs) {
                    return !(lhs < rhs);
                }

                friend bool operator<=(const iterator& lhs, const iterator& rhs) {
                    return !(lhs > rhs);
                }

                pointer operator->() {
                    return _data + actual_index();
                }

                iterator& operator+=(difference_type n) {
                    _index += n;
                    return *this;
                }

                iterator& operator-=(difference_type n) {
                    _index -= n;
                    return *this;
                }


                friend iterator operator+(const iterator& lhs, difference_type rhs) {
                    return iterator(lhs._data, lhs._capacity, lhs._start, lhs._index + rhs, lhs._size);
                }

                friend iterator operator+(difference_type lhs, const iterator& rhs) {
                    return rhs + lhs;
                }

                friend iterator operator-(const iterator& lhs, difference_type rhs) {
                    return iterator(lhs._data, lhs._capacity, lhs._start, lhs._index - rhs, lhs._size);
                }

                friend iterator operator-(difference_type lhs, const iterator& rhs) {
                    return rhs - lhs;
                }

                friend difference_type operator-(const iterator& lhs, const iterator& rhs) {
                    return lhs._index - rhs._index;
                }

                T& operator[](size_t index) {
                    return *(_data + actual_index());
                }

                const T& operator[](size_t index) const {
                    return *(_data + actual_index());
                }
                    

            };

            iterator<false> begin() { 
                return iterator<false>(_data, _capacity, _start, 0, _size); 
            }

            iterator<false> end() {
                return iterator<false>(_data, _capacity, _start, _size, _size);
            }

            iterator<true> cbegin() const { 
                return iterator<true>(_data, _capacity, _start, 0, _size); 
            }

            iterator<true> cend() const {
                return iterator<true>(_data, _capacity, _start, _size, _size);
            }

            std::reverse_iterator<iterator<false>> rbegin() { 
                return std::make_reverse_iterator(end()); 
            }

            std::reverse_iterator<iterator<false>> rend() {
                return std::make_reverse_iterator(begin());
            }

            std::reverse_iterator<iterator<true>> crbegin() const { 
                return std::make_reverse_iterator(cend());
            }

            std::reverse_iterator<iterator<true>> crend() const {
                return std::make_reverse_iterator(cbegin());
            }



        private:
            void prepare_to_push_back() {
                if (_size == _capacity) {
                    alloc_traits::destroy(_alloc, _data + _end);
                    ++_start;
                }
                if (_size < _capacity) {
                    ++_size;
                }
            }
            void prepare_to_push_front() {
                _start = _start != 0 ? _start - 1 : _capacity - 1;
                if (_size == _capacity) {
                    alloc_traits::destroy(_alloc, _data + _start);
                }
                if (_size < _capacity) {
                    ++_size;
                }
            }
            Allocator _alloc;
            size_t _capacity, _size, _start, _end;
            T* _data;
    };
}
