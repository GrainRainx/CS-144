#include "sender_harness.hh"
#include "tcp_config.hh"
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

            TCPSenderTestHarness test{"FIN sent test", cfg};
            test.execute(ExpectSegment{}.with_no_flags().with_syn(true).with_payload_size(0).with_seqno(isn));
            test.execute(AckReceived{WrappingInt32{isn + 1}});
            test.execute(ExpectState{TCPSenderStateSummary::SYN_ACKED});
            test.execute(Close{});
            test.execute(ExpectState{TCPSenderStateSummary::FIN_SENT});
            test.execute(ExpectBytesInFlight{1});
            test.execute(ExpectSegment{}.with_fin(true).with_seqno(isn + 1));
            test.execute(ExpectNoSegment{});
        }

        printf("2\n");
        {
            TCPConfig cfg;
            WrappingInt32 isn(rd());
            cfg.fixed_isn = isn;

            TCPSenderTestHarness test{"FIN acked test", cfg};
            test.execute(ExpectSegment{}.with_no_flags().with_syn(true).with_payload_size(0).with_seqno(isn));
            test.execute(AckReceived{WrappingInt32{isn + 1}});
            test.execute(ExpectState{TCPSenderStateSummary::SYN_ACKED});
            test.execute(Close{});
            test.execute(ExpectState{TCPSenderStateSummary::FIN_SENT});
            test.execute(ExpectSegment{}.with_fin(true).with_seqno(isn + 1));
            test.execute(AckReceived{WrappingInt32{isn + 2}});
            test.execute(ExpectState{TCPSenderStateSummary::FIN_ACKED});
            test.execute(ExpectBytesInFlight{0});
            test.execute(ExpectNoSegment{});
        }

        printf("3\n");
        {
            TCPConfig cfg;
            WrappingInt32 isn(rd());
            cfg.fixed_isn = isn;

            TCPSenderTestHarness test{"FIN not acked test", cfg};
            test.execute(ExpectSegment{}.with_no_flags().with_syn(true).with_payload_size(0).with_seqno(isn));
            test.execute(AckReceived{WrappingInt32{isn + 1}});
            test.execute(ExpectState{TCPSenderStateSummary::SYN_ACKED});
            test.execute(Close{});
            test.execute(ExpectState{TCPSenderStateSummary::FIN_SENT});
            test.execute(ExpectSegment{}.with_fin(true).with_seqno(isn + 1));
            test.execute(AckReceived{WrappingInt32{isn + 1}});
            test.execute(ExpectState{TCPSenderStateSummary::FIN_SENT});
            test.execute(ExpectBytesInFlight{1});
            test.execute(ExpectNoSegment{});
        }

        printf("4\n");
        {
            TCPConfig cfg;
            WrappingInt32 isn(rd());
            cfg.fixed_isn = isn;

            TCPSenderTestHarness test{"FIN retx test", cfg};
            printf("4.1\n");
            test.execute(ExpectSegment{}.with_no_flags().with_syn(true).with_payload_size(0).with_seqno(isn));
            printf("4.2\n");
            test.execute(AckReceived{WrappingInt32{isn + 1}});
            printf("4.3\n");
            test.execute(ExpectState{TCPSenderStateSummary::SYN_ACKED});
            printf("4.4\n");
            test.execute(Close{});
            printf("4.5\n");
            test.execute(ExpectState{TCPSenderStateSummary::FIN_SENT});
            printf("4.6\n");
            test.execute(ExpectSegment{}.with_fin(true).with_seqno(isn + 1));
            printf("4.7\n");
            test.execute(AckReceived{WrappingInt32{isn + 1}});
            printf("4.8\n");
            test.execute(ExpectState{TCPSenderStateSummary::FIN_SENT});
            printf("4.9\n");
            test.execute(ExpectBytesInFlight{1});
            printf("4.10\n");
            test.execute(ExpectNoSegment{});
            printf("4.11\n");
            test.execute(Tick{TCPConfig::TIMEOUT_DFLT - 1});
            printf("4.12\n");
            test.execute(ExpectState{TCPSenderStateSummary::FIN_SENT});
            printf("4.13\n");
            test.execute(ExpectBytesInFlight{1});
            printf("4.14\n");
            test.execute(ExpectNoSegment{});
            printf("4.15\n");
            test.execute(Tick{1});
            printf("4.16\n");
            test.execute(ExpectState{TCPSenderStateSummary::FIN_SENT});
            printf("4.17\n");
            test.execute(ExpectBytesInFlight{1});
            printf("4.18\n");
            test.execute(ExpectSegment{}.with_fin(true).with_seqno(isn + 1));
            printf("4.19\n");
            test.execute(ExpectNoSegment{});
            printf("4.20\n");
            test.execute(Tick{1});
            printf("4.21\n");
            test.execute(ExpectState{TCPSenderStateSummary::FIN_SENT});
            printf("4.22\n");
            test.execute(ExpectBytesInFlight{1});
            printf("4.23\n");
            test.execute(ExpectNoSegment{});
            printf("4.24\n");
            test.execute(AckReceived{WrappingInt32{isn + 2}});
            printf("4.25\n");
            test.execute(ExpectState{TCPSenderStateSummary::FIN_ACKED});
            printf("4.26\n");
            test.execute(ExpectBytesInFlight{0});
            printf("4.27\n");
            test.execute(ExpectNoSegment{});
            printf("4.28\n");
        }

    } catch (const exception &e) {
        cerr << e.what() << endl;
        return 1;
    }

    return EXIT_SUCCESS;
}
