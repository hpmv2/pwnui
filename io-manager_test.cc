#include "io-manager.h"
#include "absl/debugging/failure_signal_handler.h"
#include "absl/debugging/symbolize.h"
#include "gmock/gmock.h"
#include "google/protobuf/text_format.h"
#include "google/protobuf/util/message_differencer.h"
#include "gtest/gtest.h"
#include "process.h"

class FakeProcess : public Process {
 public:
  void read(
      const std::function<void(std::string_view, bool)>& handler) override {
    read_handler_ = handler;
  }
  void write(std::string_view data) override { write_handler_(data); }
  void OnWrite(const std::function<void(std::string_view)>& handler) {
    write_handler_ = handler;
  }
  void DoRead(std::string_view data) { read_handler_(data, false); }

 private:
  std::function<void(std::string_view, bool)> read_handler_;
  std::function<void(std::string_view)> write_handler_;
};

struct TextProto {
  explicit TextProto(std::string text) : text(text) {}
  template <class T>
  operator T() const {
    T t;
    CHECK(google::protobuf::TextFormat::ParseFromString(text, &t));
    return t;
  }

  std::string text;
};

class IOManagerTest : public testing::Test {
 public:
  void SetUp() override {
    process_.OnWrite([this](std::string_view data) { stdin_ += data; });
  }

 protected:
  void AssertReadResult(int chain_id, IOReadResult expected) {
    IOReadResult actual;
    manager_.ReadSync(chain_id, &actual);

    EXPECT_TRUE(
        google::protobuf::util::MessageDifferencer::Equals(actual, expected))
        << "Actual: " << actual.DebugString()
        << "\nExpected: " << expected.DebugString();
  }

  FakeProcess process_;
  IOManager manager_{&process_};
  std::string stdin_;
};

TEST_F(IOManagerTest, Basic) {
  manager_.Read(TextProto("any {} chain_id: 1"));
  manager_.Read(TextProto("literal { literal: 'abc' } chain_id: 1"));
  process_.DoRead("lolabcdef");
  AssertReadResult(1, TextProto(R"(
    data: "lolabc"
    chain {
      inner { data: "lol" any {} }
      inner { data: "abc" literal {} }
    }
  )"));
}

TEST_F(IOManagerTest, SuccessiveChains) {
  manager_.Read(TextProto("any {} chain_id: 1"));
  manager_.Read(TextProto("literal { literal: 'are' } chain_id: 1"));
  manager_.Read(TextProto("any {} chain_id: 2"));
  manager_.Read(TextProto("number {} chain_id: 2"));
  manager_.Read(TextProto("any {} chain_id: 2"));
  manager_.Read(TextProto("literal { literal: '!' } chain_id: 2"));
  manager_.Read(TextProto("any {} chain_id: 3"));
  manager_.Read(TextProto("literal { literal: '!' } chain_id: 3"));
  process_.DoRead("There are 512 apples! Woohoo!");
  AssertReadResult(1, TextProto(R"(
    data: "There are"
    chain {
      inner { data: "There " any {} }
      inner { data: "are" literal {} }
    }
  )"));
  AssertReadResult(2, TextProto(R"(
    data: " 512 apples!"
    chain {
      inner { data: " " any {} }
      inner { data: "512" number {} }
      inner { data: " apples" any {} }
      inner { data: "!" literal {} }
    }
  )"));
  AssertReadResult(3, TextProto(R"(
    data: " Woohoo!"
    chain {
      inner { data: " Woohoo" any {} }
      inner { data: "!" literal {} }
    }
  )"));
}

// TODO(hpmv): Add more tests here.

int main(int argc, char** argv) {
  absl::InitializeSymbolizer(argv[0]);
  absl::FailureSignalHandlerOptions options;
  absl::InstallFailureSignalHandler(options);
  testing::InitGoogleTest(&argc, argv);
  return RUN_ALL_TESTS();
}