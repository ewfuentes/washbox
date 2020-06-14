
#include "common/logging/log_writer.hh"

#include <cstdlib>

#include "gtest/gtest.h"

namespace washbox::logging {
namespace {
class TestRecord {
  public:
    TestRecord(int record_size) : record_size_(record_size) {}
    static std::string GetTypeName() { return "TestRecord"; }
    std::ostream *SerializeToOstream(std::ostream *out) const {
        for (int i = 0; i < record_size_; i++) {
            const char to_write = i % 128;
            out->write(&to_write, 1);
        }
        return out;
    }

  private:
    int record_size_;
};
} // namespace

TEST(LogWriterTest, HappyCase) {
    // SETUP
    const char *temp_dir = std::getenv("TEST_TMPDIR");
    ASSERT_NE(temp_dir, nullptr);
    constexpr int MAX_BLOCK_SIZE = 128;
    constexpr int RECORD_1_SIZE = 32;
    constexpr int RECORD_2_SIZE = 48;
    constexpr int NUM_RECORDS_TO_WRITE = 16;
    const std::string topic_1_name = "Topic1";
    const std::string topic_2_name = "Topic2";
    TestRecord record_1(RECORD_1_SIZE);
    TestRecord record_2(RECORD_2_SIZE);

    // ACTION
    LogWriter writer(std::string(temp_dir), MAX_BLOCK_SIZE);
    for (int i = 0; i < NUM_RECORDS_TO_WRITE; i++) {
        writer.write_topic(topic_1_name, record_1);
        writer.write_topic(topic_2_name, record_2);
    }

    // VERIFICATION
    {
        const auto &index = writer.index_for_topic(topic_1_name);
        int expected_file_idx = 0;
        int expected_location = 0;
        EXPECT_EQ(index.message_name(), TestRecord::GetTypeName());
        for (const auto &entry : index.entries()) {
            EXPECT_EQ(entry.data_file_idx(), expected_file_idx);
            EXPECT_EQ(entry.data_idx(), expected_location);
            expected_location += RECORD_1_SIZE;
            if (expected_location > MAX_BLOCK_SIZE) {
                expected_location = 0;
                expected_file_idx++;
            }
        }
    }
    {
        const auto &index = writer.index_for_topic(topic_2_name);
        int expected_file_idx = 0;
        int expected_location = 0;
        EXPECT_EQ(index.message_name(), TestRecord::GetTypeName());
        for (const auto &entry : index.entries()) {
            EXPECT_EQ(entry.data_file_idx(), expected_file_idx);
            EXPECT_EQ(entry.data_idx(), expected_location);
            expected_location += RECORD_2_SIZE;
            if (expected_location > MAX_BLOCK_SIZE) {
                expected_location = 0;
                expected_file_idx++;
            }
        }
    }
}
} // namespace washbox::logging
