#include "byte_stream.hh"

// Dummy implementation of a flow-controlled in-memory byte stream.

// For Lab 0, please replace with a real implementation that passes the
// automated checks run by `make check_lab0`.

// You will need to add private members to the class declaration in `byte_stream.hh`

template <typename... Targs>
void DUMMY_CODE(Targs &&... /* unused */) {}

using namespace std;

// ByteStream::ByteStream(const size_t capacity) { DUMMY_CODE(capacity); }

// size_t ByteStream::write(const string &data) {
//     DUMMY_CODE(data);
//     return {};
// }

// //! \param[in] len bytes will be copied from the output side of the buffer
// string ByteStream::peek_output(const size_t len) const {
//     DUMMY_CODE(len);
//     return {};
// }

// //! \param[in] len bytes will be removed from the output side of the buffer
// void ByteStream::pop_output(const size_t len) { DUMMY_CODE(len); }

// //! Read (i.e., copy and then pop) the next "len" bytes of the stream
// //! \param[in] len bytes will be popped and returned
// //! \returns a string
// std::string ByteStream::read(const size_t len) {
//     DUMMY_CODE(len);
//     return {};
// }

// void ByteStream::end_input() {}

// bool ByteStream::input_ended() const { return {}; }

// size_t ByteStream::buffer_size() const { return {}; }

// bool ByteStream::buffer_empty() const { return {}; }

// bool ByteStream::eof() const { return false; }

// size_t ByteStream::bytes_written() const { return {}; }

// size_t ByteStream::bytes_read() const { return {}; }

// size_t ByteStream::remaining_capacity() const { return {}; }






// MYTODO
ByteStream::ByteStream(const size_t capacity_):store_byte(), capacity(capacity_), can_read(0), 
can_write(capacity_), total_read(0), total_write(0), is_end_of(false)
{  }



size_t ByteStream::write(const string &data) {
    DUMMY_CODE(data);
    size_t len = data.size();
    len = min(len, can_write);
    if(is_end_of) return 0;
    for(size_t i = 0; i < len; i ++)
        store_byte.push_back(data[i]);
    can_read += len;
    can_write -= len;
    total_write += len;
    return len;
}

//! \param[in] len bytes will be copied from the output side of the buffer
string ByteStream::peek_output(const size_t len) const {
    DUMMY_CODE(len);
    size_t n = min(len, can_read);
    return string(store_byte.begin(), store_byte.begin() + n);
}

//! \param[in] len bytes will be removed from the output side of the buffer
void ByteStream::pop_output(const size_t len) 
{ 
    DUMMY_CODE(len);
    if(len > can_read) 
    {
        set_error();
        return ;
    }
    for(size_t i = 0; i < len; i++) store_byte.pop_front();
    can_read -= len;
    can_write += len;
    total_read += len;
}

//! Read (i.e., copy and then pop) the next "len" bytes of the stream
//! \param[in] len bytes will be popped and returned
//! \returns a string
std::string ByteStream::read(const size_t len) {
    DUMMY_CODE(len);
    string ans = "";
    if(len > can_read)
    {
        set_error();
        return ans;
    }
    ans = peek_output(len);
    pop_output(len); 
    return ans;
}

void ByteStream::end_input() { is_end_of = true;}

bool ByteStream::input_ended() const { return is_end_of; }

size_t ByteStream::buffer_size() const { return capacity - can_write; }

bool ByteStream::buffer_empty() const { return can_write == capacity; }

bool ByteStream::eof() const { return buffer_empty() && is_end_of; }

size_t ByteStream::bytes_written() const { return total_write; }

size_t ByteStream::bytes_read() const { return total_read; }

size_t ByteStream::remaining_capacity() const { return can_write; }