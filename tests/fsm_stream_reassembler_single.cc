#include "byte_stream.hh"
#include "fsm_stream_reassembler_harness.hh"
#include "stream_reassembler.hh"
#include "util.hh"

#include <exception>
#include <iostream>

using namespace std;

int main() {

    // printf("i am in single test\n");
    try {
        // printf("i am in start test\n");
        {
            ReassemblerTestHarness test{65000};

            test.execute(BytesAssembled(0));
            test.execute(BytesAvailable(""));
            test.execute(NotAtEof{});
        }
        printf("1\n");
        {
            ReassemblerTestHarness test{65000};
            test.execute(SubmitSegment{"a", 0});
            test.execute(BytesAssembled(1));
            test.execute(BytesAvailable("a"));
            test.execute(NotAtEof{});
        }
        // printf("2\n");
        {
            ReassemblerTestHarness test{65000};
            // printf("2.1\n");
            test.execute(SubmitSegment{"a", 0}.with_eof(true));
            // printf("2.2\n");
            test.execute(BytesAssembled(1));
            // printf("2.3\n");
            test.execute(BytesAvailable("a"));
            // printf("2.4\n");
            test.execute(AtEof{});
            // printf("2.5\n");
        }
        // printf("3\n");
        {
            ReassemblerTestHarness test{65000};
            // printf("3.1\n");
            test.execute(SubmitSegment{"", 0}.with_eof(true));
            // printf("3.2\n");
            test.execute(BytesAssembled(0));
            // printf("3.3\n");
            test.execute(BytesAvailable(""));
            // printf("3.4\n");
            test.execute(AtEof{});
            // printf("3.5\n");
        }
        // printf("4\n");
        {
            ReassemblerTestHarness test{65000};

            test.execute(SubmitSegment{"b", 0}.with_eof(true));

            test.execute(BytesAssembled(1));
            test.execute(BytesAvailable("b"));
            test.execute(AtEof{});
        }
        // printf("5\n");
        {
            ReassemblerTestHarness test{65000};

            test.execute(SubmitSegment{"", 0});

            test.execute(BytesAssembled(0));
            test.execute(BytesAvailable(""));
            test.execute(NotAtEof{});
        }
        // printf("6\n");
        {
            ReassemblerTestHarness test{8};
            // printf("6.1\n");
            test.execute(SubmitSegment{"abcdefgh", 0});
            // printf("6.2\n");

            test.execute(BytesAssembled(8));
            // printf("6.3\n");
            test.execute(BytesAvailable{"abcdefgh"});
            // printf("6.4\n");
            test.execute(NotAtEof{});
            // printf("6.5\n");
        }
        // printf("7\n");
        {
            ReassemblerTestHarness test{8};
            // printf("7.1\n");
            test.execute(SubmitSegment{"abcdefgh", 0}.with_eof(true));
            // printf("7.2\n");
            test.execute(BytesAssembled(8));
            // printf("7.3\n");
            test.execute(BytesAvailable{"abcdefgh"});
            // printf("7.4\n");
            test.execute(AtEof{});
            // printf("7.5\n");
        }
        // printf("8\n");
        {
            ReassemblerTestHarness test{8};
            // printf("8.1\n");
            test.execute(SubmitSegment{"abc", 0});
            // printf("8.2\n");
            test.execute(BytesAssembled(3));
            // printf("8.3\n");
            test.execute(SubmitSegment{"bcdefgh", 1}.with_eof(true));
            // printf("8.4\n");
            test.execute(BytesAssembled(8));
            // printf("8.5\n");
            test.execute(BytesAvailable{"abcdefgh"});
            // printf("8.6\n");
            test.execute(AtEof{});
            // printf("8.7\n");
        }
        // printf("9\n");
        {
            ReassemblerTestHarness test{8};
            // printf("9.1\n");
            test.execute(SubmitSegment{"abc", 0});
            // printf("9.2\n");
            test.execute(BytesAssembled(3));
            // printf("9.3\n");
            test.execute(NotAtEof{});
            // printf("9.4\n");
            test.execute(SubmitSegment{"ghX", 6}.with_eof(true));
            // printf("9.5\n");
            test.execute(BytesAssembled(3));
            // printf("9.6\n");
            test.execute(NotAtEof{});
            // printf("9.7\n");
            test.execute(SubmitSegment{"cdefg", 2});
            // printf("9.8\n");
            test.execute(BytesAssembled(8));
            // printf("9.9\n");
            test.execute(BytesAvailable{"abcdefgh"});
            // printf("9.10\n");
            test.execute(NotAtEof{});
            // printf("9.11\n");
        }
        // printf("10\n");

        // credit for test: Bill Lin (2020)
        {
            ReassemblerTestHarness test{8};

            test.execute(SubmitSegment{"abc", 0});
            test.execute(BytesAssembled(3));
            test.execute(NotAtEof{});

            // Stream re-assembler should ignore empty segments
            test.execute(SubmitSegment{"", 6});
            test.execute(BytesAssembled(3));
            test.execute(NotAtEof{});

            test.execute(SubmitSegment{"de", 3}.with_eof(true));
            test.execute(BytesAssembled(5));
            test.execute(BytesAvailable("abcde"));
            test.execute(AtEof{});
        }
        printf("success !!!!!!!!!!!!!!!!!!!!\n");
    } catch (const exception &e) {
        cerr << "Exception: " << e.what() << endl;
        return EXIT_FAILURE;
    }

    return EXIT_SUCCESS;
}
