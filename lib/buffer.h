#include <iostream>
#include <initializer_list>
#include <memory>
#include <algorithm>

template<typename value_type>

class Iterator : public std::iterator<std::random_access_iterator_tag, value_type> {
public:
    using pointer = value_type*;
    using reference = value_type&;

    Iterator() = default;

    explicit Iterator(pointer target) : target_(target) {}

    explicit Iterator(pointer begin, size_t size) : begin_(begin), end_(begin + size + 1), target_(begin),
                                                    capacity_(size + 1) {}

    explicit Iterator(pointer begin, size_t size, pointer target) : begin_(begin), end_(begin_ + size + 1),
                                                                    target_(target) {}

    Iterator(const Iterator<value_type>& x) {
        capacity_ = x.capacity_;
        begin_ = x.begin_;
        end_ = x.end_;
        target_ = x.target_;
    }

    Iterator& operator=(const Iterator<value_type>& x) {
        if (this == &x) {
            return *this;
        }

        capacity_ = x.capacity_;
        begin_ = x.begin_;
        end_ = x.end_;
        target_ = x.target_;
        return *this;
    }

    Iterator& operator++() {
        if (target_ == end_) {
            target_ = begin_;
        } else {
            target_++;
        }
        return *this;
    }

    Iterator& operator--() {
        if (target_ == begin_) {
            target_ = end_;
        } else {
            target_--;
        }
        return *this;
    }

    Iterator operator+(const size_t n) {
        Iterator<value_type> temp = *this;
        if (n > temp.end_ - temp.target_) {
            temp.target_ = temp.begin_ + (n - (temp.end_ - temp.target_ + 1));
        } else {
            temp.target_ += n;
        }
        return temp;
    }

    Iterator operator-(const size_t n) {
        Iterator<value_type> temp = *this;
        if (n > target_ - begin_) {
            temp.target_ = temp.end_ - (n % capacity_);
        } else {
            temp.target_ -= n;
        }
        return temp;
    }

    Iterator& operator+=(const int64_t n) {
        if (n >= 0) {
            *this = *this + n;
            return *this;
        } else {
            *this = *this - (-1 * n);
            return *this;
        }
    }

    size_t operator-(const Iterator<value_type>& x) const {
        int dist = target_ - x.target_;
        if (dist >= 0) {
            return dist;
        }
        return capacity_ + dist;
    }

    reference operator*() {
        return *target_;
    }

    Iterator& operator[](const size_t n) {
        return *this += n;
    }

    bool operator==(const Iterator& x) {
        return (target_ == x.target_);
    }

    bool operator!=(const Iterator<value_type>& x) {
        return !(*this == x);
    }

    bool operator<(const Iterator<value_type>& x) {
        return (target_ < x.target_);
    }

    bool operator>(const Iterator<value_type>& x) {
        return (x.target_ < target_);
    }

    bool operator<=(const Iterator<value_type>& x) {
        return (target_ <= x.target_);
    }

    bool operator>=(const Iterator<value_type>& x) {
        return (target_ >= x.target_);
    }

    Iterator<const value_type> toConstIterator() const {
        return Iterator<const value_type>(target_, capacity_, begin_);
    }

private:
    pointer begin_ = nullptr;
    pointer end_ = nullptr;
    pointer target_ = nullptr;
    size_t capacity_ = 0;
};

template<typename value_type, typename alloc = std::allocator<value_type>>

class Buffer {
public:
    using iterator = Iterator<value_type>;
    using const_iterator = const Iterator<value_type>;
    using pointer = value_type*;
    using reference = value_type&;
    using const_reference = const value_type&;

    Buffer() : capacity_(0), buffer_(nullptr), begin_(nullptr), end_(nullptr), size_(0) {}

    Buffer(const Buffer& x) {
        capacity_ = x.capacity_;

        buffer_ = memory_.allocate(x.capacity_ + 1);

        this->size_ = x.size_;

        this->begin_ = Iterator(buffer_, capacity_);
        Buffer::iterator it = begin_;

        for (Buffer::iterator it_x = x.begin_; it_x != x.end_; ++it, ++it_x) {
            *it = *it_x;
        }
        end_ = it;
    }

    template<typename U>
    Buffer(U i, U j): buffer_(memory_.allocate(j - i + 1)), size_(j - i), capacity_(size_) {
        begin_ = Iterator(buffer_, capacity_);

        auto it_x = begin_;
        for (U it = i; it != j; ++it, ++it_x) {
            *it_x = *it;
        }

        end_ = it_x;
    }

    Buffer(const size_t size) {
        capacity_ = size;

        buffer_ = memory_.allocate(size + 1);

        size_ = 0;

        begin_ = iterator(buffer_, size);
        end_ = begin_;
    }

    Buffer(const std::initializer_list<value_type>& list) {
        capacity_ = list.size();
        size_ = list.size();

        buffer_ = memory_.allocate(list.size() + 1);
        begin_ = iterator(buffer_, list.size());

        size_t index = 0;
        for (auto it = list.begin(); it < list.end(); ++it, ++index) {
            buffer_[index] = *it;
        }

        end_ = Iterator(buffer_, capacity_, buffer_ + capacity_);
    }

    Buffer& operator=(const Buffer& x) {
        if (*this == x) {
            return *this;
        }

        size_ = x.size_;
        capacity_ = x.capacity_;

        buffer_ = memory_.allocate(x.capacity_ + 1);
        begin_ = Iterator(buffer_, capacity_);

        Buffer::iterator it = begin_;
        for (Buffer::iterator it_x = x.begin_; it_x != x.end_; ++it_x, ++it) {
            *it = *it_x;
        }
        end_ = it;
        return *this;
    }

    Buffer& operator=(const std::initializer_list<value_type>& list) {
        capacity_ = list.size();

        buffer_ = memory_.allocate(list.size() + 1);
        begin_ = Iterator(buffer_, capacity_);

        Buffer::iterator it = begin_;
        for (auto it_list = list.begin(); it_list != list.end_; ++it_list, ++it) {
            *it = *it_list;
        }
        end_ = it;
        return *this;
    }

    bool operator==(const Buffer<value_type>& x) {
        if (size_ != x.size_) {
            return false;
        } else {
            for (Buffer::iterator it = begin_, it_x = x.begin_; it != end(); ++it, ++it_x) {
                if (*it != *it_x) {
                    return false;
                }
            }
        }
        return true;
    }

    bool operator!=(const Buffer<value_type>& x) {
        return !(*this == x);
    }

    reference operator[](const size_t n) {
        if (n < size_) {
            return *begin()[n];
        } else {
            throw std::invalid_argument("Going beyond the boundaries of the container");
        }
    }

    iterator begin() {
        return begin_;
    }

    iterator end() {
        return end_;
    }

    const_iterator cbegin() const {
        return begin_.toConstIterator();
    }

    const_iterator cend() const {
        return end_.toConstIterator();
    }

    virtual void push(const_reference element) {}

    void pop() {
        if (size_ == 0) {
            throw ::std::invalid_argument("Buffer is empty");
        }

        ++begin_;

        size_--;

        if (size_ == 0) {
            end_ = begin_;
        }

    }

    void swap(Buffer<value_type>& x) {
        std::swap(capacity_, x.capacity_);
        std::swap(begin_, x.begin_);
        std::swap(end_, x.end_);
        std::swap(buffer_, x.buffer_);
        std::swap(size_, x.size_);
    }

    size_t size() {
        return size_;
    }

    size_t max_size() {
        return capacity_;
    }

    bool empty() {
        return begin_ == end_;
    }

    ~Buffer() {
        memory_.deallocate(buffer_, capacity_ + 1);
    }

protected:
    alloc memory_;
    pointer buffer_;
    size_t capacity_;
    iterator begin_;
    iterator end_;
    size_t size_;
};

template<typename T, typename alloc = std::allocator<T>>
class BufferStatic : public Buffer<T, alloc> {
public:
    using iterator = Iterator<T>;
    using pointer = T*;
    using reverence = T&;
    using const_reverence = const T&;

    explicit BufferStatic() : Buffer<T, alloc>() {}

    BufferStatic(const BufferStatic& x) : Buffer<T, alloc>(x) {}

    BufferStatic(const std::initializer_list<T>& list) : Buffer<T, alloc>(list) {}

    template<typename U>
    BufferStatic(U i, U j) : Buffer<T, alloc>(i, j) {}

    explicit BufferStatic(const size_t size) : Buffer<T, alloc>(size) {}

    void push(const_reverence element) override {
        if (this->buffer_ == nullptr) {
            throw ::std::invalid_argument("Buffer size not specified");
        }

        *(this->end_) = element;
        ++(this->end_);

        if (this->size_ == this->capacity_) {
            ++(this->begin_);
        } else {
            (this->size_)++;
        }
    }
};

template<typename T, typename alloc = std::allocator<T>>
class BufferDynamic : public Buffer<T, alloc> {
public:
    using iterator = Iterator<T>;
    using pointer = T*;
    using reverence = T&;
    using const_reverence = const T&;

    explicit BufferDynamic() : Buffer<T, alloc>() {}

    BufferDynamic(const BufferDynamic& x) : Buffer<T, alloc>(x) {}

    BufferDynamic(const std::initializer_list<T>& list) : Buffer<T, alloc>(list) {}

    explicit BufferDynamic(const size_t size) : Buffer<T, alloc>(size) {}

    BufferDynamic(const size_t n, const_reverence element) {
        this->capacity_ = n;
        this->size_ = n;
        this->buffer_ = this->memory_.allocate(n + 1);
        this->begin_ = iterator(this->buffer_, n);
        size_t index = 0;
        for (index; index < n; ++index) {
            this->buffer_[index] = element;
        }
        this->end_ = Iterator(this->buffer_, this->capacity_, this->buffer_ + index);
    }

    BufferDynamic(BufferDynamic::iterator new_begin, BufferDynamic::iterator new_end) {
        size_t new_size = new_end - new_begin;
        this->capacity_ = new_size;
        this->size_ = new_size;

        this->buffer_ = this->memory_.allocate(new_size + 1);
        this->begin_ = iterator(this->buffer_, new_size);

        size_t index = 0;
        for (new_begin; new_begin < new_end; ++new_begin, ++index) {
            this->buffer_[index] = *new_begin;
        }

        this->end_ = Iterator(this->buffer_, this->capacity_, this->buffer_ + index);
    }

    iterator insert(const size_t pos, const_reverence element) {
        if (pos >= this->capacity_) {
            throw ::std::invalid_argument("Going beyond the boundaries of the container");
        }

        pointer new_buffer;
        if (this->size_ == this->capacity_) {
            new_buffer = (this->memory_).allocate(this->capacity_ * 2 + 1);
        } else {
            new_buffer = (this->memory_).allocate(this->capacity_ + 1);
        }

        size_t index = 0;
        BufferDynamic::iterator tmp_it = this->begin_;
        BufferDynamic::iterator pos_it = this->begin()[pos + 1];

        for (tmp_it; tmp_it != pos_it; ++tmp_it, ++index) {
            new_buffer[index] = *tmp_it;
        }

        new_buffer[index++] = element;

        for (tmp_it; tmp_it != this->end_; ++tmp_it, ++index) {
            new_buffer[index] = *tmp_it;
        }

        this->memory_.deallocate(this->buffer_, this->capacity_ + 1);

        this->capacity_ = this->capacity_ * 2;

        this->size_ += 1;

        this->buffer_ = new_buffer;

        this->begin_ = Iterator(this->buffer_, this->capacity_);

        this->end_ = Iterator(this->buffer_, this->capacity_, this->buffer_ + this->size_);

        return this->begin()[pos + 1];
    }

    iterator insert(const size_t pos, const size_t count, const_reverence element) {
        if (pos >= this->capacity_) {
            throw ::std::invalid_argument("Going beyond the boundaries of the container");
        }

        pointer new_buffer;
        if (count + this->size_ >= this->capacity_) {
            size_t new_size = count + this->size_;
            new_buffer = (this->memory_).allocate(new_size + 1);
        } else {
            new_buffer = (this->memory_).allocate(this->capacity_ + 1);
        }

        size_t index = 0;
        BufferDynamic::iterator tmp_it = this->begin_;
        BufferDynamic::iterator pos_it = this->begin()[pos + 1];

        for (tmp_it; tmp_it != pos_it; ++tmp_it, ++index) {
            new_buffer[index] = *tmp_it;
        }

        for (size_t i = 0; i < count; ++i, ++index) {
            new_buffer[index] = element;
        }

        for (tmp_it; tmp_it != this->end_; ++tmp_it, ++index) {
            new_buffer[index] = *tmp_it;
        }

        this->memory_.deallocate(this->buffer_, this->capacity_ + 1);

        this->size_ += count;
        this->capacity_ = this->size_;

        this->buffer_ = new_buffer;


        this->begin_ = Iterator(this->buffer_, this->capacity_);

        this->end_ = Iterator(this->buffer_, this->capacity_, this->buffer_ + this->capacity_);

        return this->begin()[pos + 1];
    }

    iterator insert(const size_t pos, const std::initializer_list<T>& list) {
        if (pos >= this->capacity_) {
            throw ::std::invalid_argument("Going beyond the boundaries of the container");
        }

        pointer new_buffer;
        if (list.size() + this->size_ >= this->capacity_) {
            size_t new_size = list.size() + this->size_;
            new_buffer = (this->memory_).allocate(new_size + 1);
        } else {
            new_buffer = (this->memory_).allocate(this->capacity_ + 1);
        }

        size_t index = 0;
        BufferDynamic::iterator tmp_it = this->begin_;
        BufferDynamic::iterator pos_it = this->begin()[pos + 1];

        for (tmp_it; tmp_it != pos_it; ++tmp_it, ++index) {
            new_buffer[index] = *tmp_it;
        }

        auto it = list.begin();
        for (size_t i = 0; i < list.size(); ++i, ++index, ++it) {
            new_buffer[index] = *it;
        }

        for (tmp_it; tmp_it != this->end_; ++tmp_it, ++index) {
            new_buffer[index] = *tmp_it;
        }

        this->memory_.deallocate(this->buffer_, this->capacity_ + 1);

        this->size_ += list.size();
        this->capacity_ = this->size_;

        this->buffer_ = new_buffer;


        this->begin_ = Iterator(this->buffer_, this->capacity_);

        this->end_ = Iterator(this->buffer_, this->capacity_, this->buffer_ + this->capacity_);

        return this->begin()[pos + 1];
    }

    void assign(BufferDynamic::iterator left_border, BufferDynamic::iterator right_border) {
        if (this->capacity_ != 0) {
            this->memory_.deallocate(this->buffer_, this->capacity_ + 1);
        }
        this->capacity_ = right_border - left_border;

        this->buffer_ = (this->memory_).allocate(this->capacity_ + 1);

        this->begin_ = Iterator(this->buffer_, this->capacity_);
        this->end_ = this->begin_;

        this->size_ = 0;
        for (left_border; left_border != right_border; ++left_border) {
            this->push(*left_border);
        }
    }

    void assign(const std::initializer_list<T>& list) {
        if (this->capacity_ != 0) {
            this->memory_.deallocate(this->buffer_, this->capacity_ + 1);
        }
        this->capacity_ = list.size();

        this->buffer_ = (this->memory_).allocate(this->capacity_ + 1);

        this->begin_ = Iterator(this->buffer_, this->capacity_);
        this->end_ = this->begin_;

        this->size_ = 0;
        for (auto it = list.begin(); it != list.end(); ++it) {
            this->push(*it);
        }
    }

    void assign(const size_t count, const_reverence element) {
        if (this->capacity_ != 0) {
            this->memory_.deallocate(this->buffer_, this->capacity_ + 1);
        }
        this->capacity_ = count;
        this->buffer_ = (this->memory_).allocate(this->capacity_ + 1);

        this->begin_ = Iterator(this->buffer_, this->capacity_);
        this->end_ = this->begin_;

        this->size_ = 0;
        for (size_t i = 0; i < count; ++i) {
            this->push(element);
        }
    }

    void push(const_reverence element) override {
        if (this->buffer_ == nullptr) {
            this->capacity_ = 1;
            this->buffer_ = (this->memory_).allocate(this->capacity_ + 1);
            this->begin_ = Iterator(this->buffer_, this->capacity_);
            this->end_ = this->begin_;
        }

        if (this->size_ == this->capacity_) {
            pointer new_buffer = (this->memory_).allocate(this->capacity_ * 2 + 1);

            size_t index = 0;
            for (BufferDynamic::iterator it = this->begin_; it != this->end_; ++it, ++index) {
                new_buffer[index] = *it;
            }
            this->memory_.deallocate(this->buffer_, this->capacity_ + 1);

            this->capacity_ = this->capacity_ * 2;

            this->buffer_ = new_buffer;

            this->begin_ = Iterator(this->buffer_, this->capacity_);

            this->end_ = Iterator(this->buffer_, this->capacity_, this->buffer_ + index);
        }

        *(this->end_) = element;
        ++(this->end_);
        this->size_++;
    }

    void clear() {
        size_t size = this->size_;
        for (size_t i = 0; i < size; ++i) {
            this->pop();
        }
    }
};