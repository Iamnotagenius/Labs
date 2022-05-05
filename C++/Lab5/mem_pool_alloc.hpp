#include <stdexcept>
#include <cstdint>
#include <cstdlib>

namespace memory {
    template<typename T>
    class pool_allocator {
        private:
            T* heap;
            T** free_list;
            std::size_t size_of_piece;
        public:
            using pointer = T*;
            using value_type = T;
            pool_allocator(std::size_t size, std::size_t count) 
                : heap(std::malloc(size * count * sizeof(T))), 
                  free_list(static_cast<T**>(heap)), 
                  size_of_piece(size) {
                    if (size * sizeof(T) < sizeof(std::size_t)) {
                        throw std::invalid_argument("Piece size must be at least pointer size");
                    }
                auto end_addr = heap + size * count * sizeof(T);
                
                for (std::size_t i = 0; i < count; ++i) {
                    auto current = heap + i * size * sizeof(T);
                    auto next = current + size * sizeof(T);
                    // Store address of next piece in the free memory
                    *reinterpret_cast<T**>(current) = next < end_addr ? next : nullptr;
                }
            }

            ~pool_allocator() {
                std::free(heap);
            }
            
            [[nodiscard]] T* allocate(std::size_t n) {
                if (n > size_of_piece) {
                    std::invalid_argument("Can't allocate more than size of piece");
                }
                if (*free_list == nullptr) {
                    std::bad_alloc();
                }

                auto allocating = reinterpret_cast<T*>(free_list);
                free_list = static_cast<T**>(*free_list);

                return allocating;

            }
            void deallocate(T* p, std::size_t) {
                if (p == nullptr) {
                    return;
                }

                if (free_list == nullptr) {
                    free_list = reinterpret_cast<T**>(p);
                    *free_list = nullptr;
                    return;
                }
                T** temp = free_list;
                free_list = reinterpret_cast<T**>(p);
                *free_list = reinterpret_cast<T*>(temp);
            }

    };
}
