#include "stream_reassembler.hh"

// Dummy implementation of a stream reassembler.

// For Lab 1, please replace with a real implementation that passes the
// automated checks run by `make check_lab1`.

// You will need to add private members to the class declaration in `stream_reassembler.hh`

template <typename... Targs>
void DUMMY_CODE(Targs &&... /* unused */) {}

using namespace std;

// StreamReassembler::StreamReassembler(const size_t capacity) : _output(capacity), _capacity(capacity) {}

// //! \details This function accepts a substring (aka a segment) of bytes,
// //! possibly out-of-order, from the logical stream, and assembles any newly
// //! contiguous substrings and writes them into the output stream in order.
// void StreamReassembler::push_substring(const string &data, const size_t index, const bool eof) {
//     DUMMY_CODE(data, index, eof);
// }

// size_t StreamReassembler::unassembled_bytes() const { return {}; }

// bool StreamReassembler::empty() const { return {}; }






StreamReassembler::StreamReassembler(const size_t capacity) : _output(capacity), _capacity(capacity),
store_byte(), cur_ca_index(0), bytestream_tail(0), store_index(), end_index(0x3f3f3f3f3f)
{}


//! \details This function accepts a substring (aka a segment) of bytes,
//! possibly out-of-order, from the logical stream, and assembles any newly
//! contiguous substrings and writes them into the output stream in order.
void StreamReassembler::push_substring(const string &data, const size_t index, const bool eof) {
    size_t len = data.length();
    cur_ca_index = _output.bytes_read();

    if(index + len < bytestream_tail || index >= cur_ca_index + _capacity) return ;

    size_t st = max(index, bytestream_tail);

    for(size_t i = st; i < index + len && i < cur_ca_index + _capacity; i++)
    {
        if(store_index.count(i)) continue;
        store_byte.push({i, data[i - index]});
        store_index.insert(i);
    }
    
    string ans = "";
    size_t store_to_byte_cnt = _output.remaining_capacity();
    while(store_byte.size() && bytestream_tail == store_byte.top().first && store_to_byte_cnt)
    {
        ans += store_byte.top().second;
        store_byte.pop();
        bytestream_tail++;
        store_to_byte_cnt--;
    }
    _output.write(ans);
    if (eof) {
        end_index = min(end_index, index + data.length());
    }
    if(bytestream_tail == end_index)
    {
        _output.end_input();
    }
}

size_t StreamReassembler::unassembled_bytes() const { return store_byte.size(); }

bool StreamReassembler::empty() const 
{
    if(!unassembled_bytes()) return true;
    return false;
}


