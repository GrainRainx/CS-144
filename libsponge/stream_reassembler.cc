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
store_byte(), cur_ca_index(0), bytestream_tail(0), store_index()
{}


//! \details This function accepts a substring (aka a segment) of bytes,
//! possibly out-of-order, from the logical stream, and assembles any newly
//! contiguous substrings and writes them into the output stream in order.
void StreamReassembler::push_substring(const string &data, const size_t index, const bool eof) {
    // cout << "i am in reassembler" << endl;
    size_t len = data.length();
    cur_ca_index = _output.bytes_read();
    cout << "pre ==========" << endl;
    cout << "data = " << data << endl;
    cout << "after  ============" << endl;
    // if(index + len > cur_ca_index + _capacity) return;
    if(index + len > bytestream_tail + _capacity) return;

    size_t st = max(index, bytestream_tail);

    for(size_t i = st; i < index + len; i++)
    {
        if(store_index.count(i)) continue;
        store_byte.push({i, data[i - index]});
        store_index.insert(i);
    }
    
    string ans = "";
    cout << endl;
    cout << "=================" << endl;
    cout << "data = " << data << ' ' << "index = " << index << endl;
    cout << "top = " << store_byte.top().first << ' ' << store_byte.top().second << endl;
    cout << "store index ===" << endl;
    for(auto x : store_index)
        cout << x << ' ';
    cout << endl;
    cout << "=================" << endl;
    cout << endl;

    while(store_byte.size() && bytestream_tail == store_byte.top().first)
    {
        // auto de_index = store_index.lower_bound(store_byte.top().first);
        // cout << *de_index << endl;
        // store_index.erase(de_index);
        // cout << "byte tail = " << bytestream_tail << endl;
        ans += store_byte.top().second;
        store_byte.pop();
        bytestream_tail++;
    }
    // if(!ans.size()) return ;
    cout <<  "ans = " << ans << endl;
    _output.write(ans);
    // cout << "total write = " << _output.bytes_written() << endl;
    if (eof) {
        // cout << "is eof" << endl;
        _output.end_input();
    }
}

size_t StreamReassembler::unassembled_bytes() const { return store_byte.size(); }

bool StreamReassembler::empty() const 
{
    if(!unassembled_bytes()) return true;
    return false;
}


