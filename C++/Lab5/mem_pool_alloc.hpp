#include <stdexcept>
#include <functional>
#include <iostream>
#include <memory>
#include <string>
#include <cstdint>
#include <cstdlib>

namespace memory {
    template<typename T>
    class pool_allocator {
        private:
            std::shared_ptr<void> _heap;
            std::shared_ptr<void**> _free_list;
            std::size_t _size_of_piece;

        public:
            using pointer = T*;
            using value_type = T;
            template<typename U>
            struct rebind {
                using other = pool_allocator<U>;
            };
            pool_allocator(std::size_t size = 16, std::size_t count = 1024) 
                : _heap(std::shared_ptr<void>(std::malloc(size * count), 
                                               [](void* ptr) { std::free(ptr); })), 
                  _free_list(std::make_shared<void**>(reinterpret_cast<void**>(_heap.get()))), 
                  _size_of_piece(size) {
                if (size < sizeof(std::uintptr_t)) {
                    throw std::invalid_argument("Piece size must be at least pointer size");
                }

                auto end_addr = reinterpret_cast<char*>(_heap.get()) + size * count;
                for (std::size_t i = 0; i < count; ++i) {
                    auto current = reinterpret_cast<char*>(_heap.get()) + i * size;
                    auto next = current + size;
                    // Store address of next piece in the free memory
                    *reinterpret_cast<void**>(current) = next < end_addr ? next : nullptr;
                }
            }

            bool operator==(const pool_allocator& rhs) {
                return _heap == rhs._heap;
            }

            const std::shared_ptr<void>& get_heap() const {
                return _heap;
            }

            const std::shared_ptr<void**>& get_free_list() const {
                return _free_list;
            }

            std::size_t get_block_size() const {
                return _size_of_piece;
            }
            
            template<typename U>
            pool_allocator(const pool_allocator<U>& other) :
                _heap(other.get_heap()),
                _free_list(other.get_free_list()),
                _size_of_piece(other.get_block_size()) {
                }


            [[nodiscard]] T* allocate(std::size_t n) {

                if (n * sizeof(T) > _size_of_piece) {
                    using namespace std::string_literals;
                    throw std::invalid_argument("Can't allocate more than "s + std::to_string(n) + " elements of size " + std::to_string(sizeof(T)));
                }
                if (*_free_list == nullptr) {
                    throw std::bad_alloc();
                }

                auto allocating = reinterpret_cast<T*>(*_free_list);
                *_free_list = reinterpret_cast<void**>(**_free_list);
                
#ifdef DEBUG
                auto list = *_free_list;
                int chunks = 0;
                while (list) {
                    list = reinterpret_cast<void**>(*list);
                    chunks++;
                }
                std::cerr << "\033[32mAllocation: \033[33;1m" << chunks << "\033[0m chunks left\n";
#endif

                return allocating;

            }

            void deallocate(T* p, std::size_t) {
                if (p == nullptr) {
                    return;
                }

                if (*_free_list == nullptr) {
                    *_free_list = reinterpret_cast<void**>(p);
                    **_free_list = nullptr;
                }
                else {
                    void** temp = *_free_list;
                    *_free_list = reinterpret_cast<void**>(p);
                    **_free_list = reinterpret_cast<void*>(temp);
                }
#ifdef DEBUG
                auto list = *_free_list;
                int chunks = 0;
                while (list) {
                    list = reinterpret_cast<void**>(*list);
                    chunks++;
                }
                std::cerr << "\033[31mDeallocation: \033[33;1m" << chunks << "\033[0m chunks left\n";
#endif

            }

    };
}
