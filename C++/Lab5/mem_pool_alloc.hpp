#include <stdexcept>
#include <memory>
#include <cstdint>
#include <cstdlib>

namespace memory {
    template<typename T>
    class pool_allocator {
        private:
            std::shared_ptr<void*> _heap;
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
                : _heap(std::make_shared<void*>(std::malloc(size * count))), 
                  _free_list(std::make_shared<void**>(reinterpret_cast<void**>(*_heap))), 
                  _size_of_piece(size) {
                    if (size < sizeof(std::size_t)) {
                        throw std::invalid_argument("Piece size must be at least pointer size");
                    }
                auto end_addr = reinterpret_cast<char*>(*_heap) + size * count;
                
                for (std::size_t i = 0; i < count; ++i) {
                    auto current = reinterpret_cast<char*>(*_heap) + i * size;
                    auto next = current + size;
                    // Store address of next piece in the free memory
                    *reinterpret_cast<void**>(current) = next < end_addr ? next : nullptr;
                }
            }

            bool operator==(const pool_allocator& rhs) {
                return _heap == rhs._heap;
            }

            const std::shared_ptr<void*>& get_heap() const {
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
                _size_of_piece(other.get_block_size()) {}


            [[nodiscard]] T* allocate(std::size_t n) {
                if (n > _size_of_piece) {
                    std::invalid_argument("Can't allocate more than size of piece");
                }
                if (**_free_list == nullptr) {
                    std::bad_alloc();
                }

                auto allocating = reinterpret_cast<void*>(*_free_list);
                *_free_list = reinterpret_cast<void**>(**_free_list);

                return reinterpret_cast<T*>(allocating);

            }

            void deallocate(T* p, std::size_t) {
                if (p == nullptr) {
                    return;
                }

                if (*_free_list == nullptr) {
                    *_free_list = reinterpret_cast<void**>(p);
                    **_free_list = nullptr;
                    return;
                }
                void** temp = *_free_list;
                *_free_list = reinterpret_cast<void**>(p);
                **_free_list = reinterpret_cast<void*>(temp);
            }

    };
}
