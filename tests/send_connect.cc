#include "sender_harness.hh"
#include "wrapping_integers.hh"

#include <cstdint>
#include <cstdlib>
#include <exception>
#include <iostream>
#include <optional>
#include <stdexcept>
#include <string>

using namespace std;

int main() {
    try {
        auto rd = get_random_generator();
        printf("1\n");
        {
            TCPConfig cfg;
            WrappingInt32 isn(rd());
            cfg.fixed_isn = isn;

            TCPSenderTestHarness test{"SYN sent test", cfg};
            test.execute(ExpectState{TCPSenderStateSummary::SYN_SENT});
            test.execute(ExpectSegment{}.with_no_flags().with_syn(true).with_payload_size(0).with_seqno(isn));
            test.execute(ExpectBytesInFlight{1});
        }
        printf("2\n");
        {
            TCPConfig cfg;
            WrappingInt32 isn(rd());
            cfg.fixed_isn = isn;

            TCPSenderTestHarness test{"SYN acked test", cfg};
            printf("2.1\n");
            test.execute(ExpectState{TCPSenderStateSummary::SYN_SENT});
            printf("2.2\n");
            test.execute(ExpectSegment{}.with_no_flags().with_syn(true).with_payload_size(0).with_seqno(isn));
            printf("2.3\n");
            test.execute(ExpectBytesInFlight{1});
            printf("2.4\n");
            test.execute(AckReceived{WrappingInt32{isn + 1}});
            printf("2.5\n");
            test.execute(ExpectState{TCPSenderStateSummary::SYN_ACKED});
            printf("2.6\n");
            test.execute(ExpectNoSegment{});
            printf("2.7\n");
            test.execute(ExpectBytesInFlight{0});
            printf("2.8\n");
        }
        printf("3\n");
        {
            TCPConfig cfg;
            WrappingInt32 isn(rd());
            cfg.fixed_isn = isn;

            TCPSenderTestHarness test{"SYN -> wrong ack test", cfg};
            printf("3.1\n");
            test.execute(ExpectState{TCPSenderStateSummary::SYN_SENT});
            printf("3.2\n");
            test.execute(ExpectSegment{}.with_no_flags().with_syn(true).with_payload_size(0).with_seqno(isn));
            printf("3.3\n");
            test.execute(ExpectBytesInFlight{1});
            printf("3.4\n");
            test.execute(AckReceived{WrappingInt32{isn}});
            printf("3.5\n");
            test.execute(ExpectState{TCPSenderStateSummary::SYN_SENT});
            printf("3.6\n");
            test.execute(ExpectNoSegment{});
            printf("3.7\n");
            test.execute(ExpectBytesInFlight{1});
            printf("3.8\n");
        }
        printf("4\n");
        {
            TCPConfig cfg;
            WrappingInt32 isn(rd());
            cfg.fixed_isn = isn;

            TCPSenderTestHarness test{"SYN acked, data", cfg};
            printf("4.1\n");
            test.execute(ExpectState{TCPSenderStateSummary::SYN_SENT});
            printf("4.2\n");
            test.execute(ExpectSegment{}.with_no_flags().with_syn(true).with_payload_size(0).with_seqno(isn));
            printf("4.3\n");
            test.execute(ExpectBytesInFlight{1});
            printf("4.4\n");
            test.execute(AckReceived{WrappingInt32{isn + 1}});
            printf("4.5\n");
            test.execute(ExpectState{TCPSenderStateSummary::SYN_ACKED});
            printf("4.6\n");
            test.execute(ExpectNoSegment{});
            printf("4.7\n");
            test.execute(ExpectBytesInFlight{0});
            printf("4.8\n");
            test.execute(WriteBytes{"abcdefgh"});
            // return EXIT_SUCCESS;
            printf("4.9\n");
            test.execute(Tick{1});
            printf("4.10\n");
            test.execute(ExpectState{TCPSenderStateSummary::SYN_ACKED});
            printf("4.11\n");
            test.execute(ExpectSegment{}.with_seqno(isn + 1).with_data("abcdefgh"));
            printf("4.12\n");
            test.execute(ExpectBytesInFlight{8});
            printf("4.13\n");
            test.execute(AckReceived{WrappingInt32{isn + 9}});
            printf("4.14\n");
            test.execute(ExpectState{TCPSenderStateSummary::SYN_ACKED});
            printf("4.15\n");
            test.execute(ExpectNoSegment{});
            printf("4.16\n");
            test.execute(ExpectBytesInFlight{0});
            printf("4.17\n");
            test.execute(ExpectSeqno{WrappingInt32{isn + 9}});
            printf("4.18\n");
        }

    } catch (const exception &e) {
        cerr << e.what() << endl;
        return 1;
    }

    return EXIT_SUCCESS;
}
