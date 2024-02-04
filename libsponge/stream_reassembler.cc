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






StreamReassembler::StreamReassembler(const size_t capacity) : _output(capacity), _capacity(capacity)
, index_is_arrive(capacity, false), store_string(capacity, ""), cur_input_bytestream(0), 
is_eof(false), unreass_byte(0)
{}


//! \details This function accepts a substring (aka a segment) of bytes,
//! possibly out-of-order, from the logical stream, and assembles any newly
//! contiguous substrings and writes them into the output stream in order.
void StreamReassembler::push_substring(const string &data, const size_t index, const bool eof) {
    DUMMY_CODE(data, index, eof);

    index_is_arrive[index] = true;
    store_string[index] = data;
    size_t len = data.length();
    unreass_byte += len;
    if(eof)
    {
        is_eof = true;
        _output.end_input();
    }
    while(index_is_arrive[cur_input_bytestream])
    {
        size_t delen = store_string[cur_input_bytestream].length();
        unreass_byte -= delen;
        _output.write(store_string[cur_input_bytestream]);
        cur_input_bytestream++;
    }

}

size_t StreamReassembler::unassembled_bytes() const { return unreass_byte; }

bool StreamReassembler::empty() const 
{
    if(!unassembled_bytes()) return true;
    return false;
}


