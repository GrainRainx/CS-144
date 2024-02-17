#include "tcp_sender.hh"

#include "tcp_config.hh"

#include <random>
#include <iostream>
// Dummy implementation of a TCP sender

// For Lab 3, please replace with a real implementation that passes the
// automated checks run by `make check_lab3`.

template <typename... Targs>
void DUMMY_CODE(Targs &&... /* unused */) {}

using namespace std;

//! \param[in] capacity the capacity of the outgoing byte stream
//! \param[in] retx_timeout the initial amount of time to wait before retransmitting the oldest outstanding segment
//! \param[in] fixed_isn the Initial Sequence Number to use, if set (otherwise uses a random ISN)
TCPSender::TCPSender(const size_t capacity, const uint16_t retx_timeout, const std::optional<WrappingInt32> fixed_isn)
    : _isn(fixed_isn.value_or(WrappingInt32{random_device()()}))
    , _initial_retransmission_timeout{retx_timeout}
    , retransmission_timeout_(retx_timeout)
    , _stream(capacity) {}

uint64_t TCPSender::bytes_in_flight() const { return _bytes_in_flight; }

void TCPSender::fill_window() 
{
    TCPSegment tcp_segment;
    if(is_fin)
        return ;
    if(!is_syn)
    {
        is_syn = true;
        tcp_segment.header().syn = true;
        send_tcp_segment(tcp_segment);
        return ;
    }

    // printf("???\n");
    uint16_t window_size = (receiver_window_size_ > 0 ? receiver_window_size_ : 1);
    if(_stream.eof() && _rece_seqno + window_size > _next_seqno)
    {
        tcp_segment.header().fin = true;
        send_tcp_segment(tcp_segment);
        is_fin = true;
        return ;
    }
    // printf("test window %d\n", window_size);
    // printf("rece seqno + windowsize - next seqno is %ld\n", _rece_seqno + window_size - _next_seqno);
    while(!_stream.buffer_empty() && _rece_seqno + window_size > _next_seqno)
    {
        size_t read_length = min({TCPConfig::MAX_PAYLOAD_SIZE, _stream.buffer_size(), _rece_seqno + window_size - _next_seqno});
        // printf("every time read length is %ld\n", read_length);
        string _payload = _stream.read(read_length);
        tcp_segment.payload() = Buffer(std::move(_payload));
        if(_stream.eof() && tcp_segment.length_in_sequence_space() < window_size)
        {
            tcp_segment.header().fin = true;
            is_fin = true;
        }
        send_tcp_segment(tcp_segment);
        // cout << "tcp payload is " << _payload << endl;
    }


}


void TCPSender::send_tcp_segment(TCPSegment & tcp_segment)
{
    tcp_segment.header().seqno = wrap(_next_seqno, _isn);
    _segments_out.push(tcp_segment);
    _segments_wait.push(tcp_segment);

    _next_seqno += tcp_segment.length_in_sequence_space();
    _bytes_in_flight += tcp_segment.length_in_sequence_space();

    if(!is_start_retransmission_timeout)
    {
        is_start_retransmission_timeout = true;
        total_retransmission_time = 0;
    }
}


//! \param ackno The remote receiver's ackno (acknowledgment number)
//! \param window_size The remote receiver's advertised window size
void TCPSender::ack_received(const WrappingInt32 ackno, const uint16_t window_size) 
{
    uint64_t abs_ackno = unwrap(ackno, _isn, _next_seqno);

    if(abs_ackno > _next_seqno)
    {
        printf("out of range!\n");
        return ;
    }
    if(abs_ackno >= _rece_seqno)
    {
        receiver_window_size_ = window_size;
        _rece_seqno = abs_ackno;
    }
    bool is_pop = false;
    while(!_segments_wait.empty())
    {
        TCPSegment &seg = _segments_wait.front();
        if(abs_ackno < unwrap(seg.header().seqno, _isn, _next_seqno)  + seg.length_in_sequence_space()) return ;
        _bytes_in_flight -= seg.length_in_sequence_space();
        _segments_wait.pop();
        is_pop = true;
        printf("am i can here???\n");
        consecutive_retransmissions_count = 0;
        total_retransmission_time = 0;
        retransmission_timeout_ = _initial_retransmission_timeout;
    }

    if(is_pop)
        fill_window();
    
    is_start_retransmission_timeout = !_segments_wait.empty();
}

//! \param[in] ms_since_last_tick the number of milliseconds since the last call to this method
void TCPSender::tick(const size_t ms_since_last_tick)
{
    if(!is_start_retransmission_timeout) return ;
    total_retransmission_time += ms_since_last_tick;
    printf("total retrans time %d\n", total_retransmission_time);
    printf("retrans timeout %d\n", retransmission_timeout_);
    if(!_segments_wait.empty() && total_retransmission_time >= retransmission_timeout_ )
    {
        TCPSegment tcp_segment = _segments_wait.front();
        _segments_out.push(tcp_segment);
        total_retransmission_time = 0;
        printf("retrans !!! \n");
        if(receiver_window_size_ > 0 || tcp_segment.header().syn) {
            consecutive_retransmissions_count += 1;
            retransmission_timeout_ *= 2;
        }
    }
}

unsigned int TCPSender::consecutive_retransmissions() const 
{
    return consecutive_retransmissions_count; 
}

void TCPSender::send_empty_segment() 
{
    TCPSegment tcp_segment;
    tcp_segment.header().seqno = next_seqno();
    _segments_out.push(tcp_segment);
}
