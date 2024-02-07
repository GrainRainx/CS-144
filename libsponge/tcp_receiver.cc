#include "tcp_receiver.hh"

// Dummy implementation of a TCP receiver

// For Lab 2, please replace with a real implementation that passes the
// automated checks run by `make check_lab2`.

template <typename... Targs>
void DUMMY_CODE(Targs &&... /* unused */) {}

using namespace std;

void TCPReceiver::segment_received(const TCPSegment &seg) {
    TCPHeader header = seg.header();
    if(!header.syn && !is_syn)
        return ;
    WrappingInt32 _seqno = header.seqno;
    if(header.syn)
    {
        _isn = header.seqno;
        is_syn = true;
        _seqno = _seqno + 1;
    }
    uint64_t _checkpoint = _reassembler.stream_out().bytes_written();
    uint64_t absolute_seqno = unwrap(_seqno, _isn, _checkpoint);
    bool eof = header.fin && is_syn;
    _reassembler.push_substring(seg.payload().copy(), absolute_seqno - 1, eof);
}

std::optional<WrappingInt32> TCPReceiver::ackno() const 
{
    if(!is_syn) return {};
    
    uint64_t absolute_ackno = _reassembler.stream_out().bytes_written() + 1;
    if(_reassembler.stream_out().input_ended()) absolute_ackno += 1;
    WrappingInt32 ans = wrap(absolute_ackno, _isn);
    return ans; 
}

size_t TCPReceiver::window_size() const { return _reassembler.stream_out().remaining_capacity(); }
