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
    chars_consumed: 6
    chain {
      inner { data: "lol" chars_consumed: 3 any {} }
      inner { data: "abc" chars_consumed: 3 literal {} }
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
    chars_consumed: 9
    chain {
      inner { data: "There " chars_consumed: 6 any {} }
      inner { data: "are" chars_consumed: 3 literal {} }
    }
  )"));
  AssertReadResult(2, TextProto(R"(
    data: " 512 apples!"
    chars_consumed: 12
    chain {
      inner { data: " " chars_consumed: 1 any {} }
      inner { data: "512" chars_consumed: 3 number {} }
      inner { data: " apples" chars_consumed: 7 any {} }
      inner { data: "!" chars_consumed: 1 literal {} }
    }
  )"));
  AssertReadResult(3, TextProto(R"(
    data: " Woohoo!"
    chars_consumed: 8
    chain {
      inner { data: " Woohoo" chars_consumed: 7 any {} }
      inner { data: "!" chars_consumed: 1 literal {} }
    }
  )"));
}

TEST_F(IOManagerTest, Line) {
  manager_.Read(TextProto(R"(
    line {
      predicate {
        chain {
          consumers { any {} }
          consumers { literal { literal: "-" } }
          consumers { any {} }
        }
      }
    }
    chain_id: 1
  )"));
  process_.DoRead("hello-world\n");
  AssertReadResult(1, TextProto(R"(
    data: "hello-world\n"
    chain {
      inner {
        data: "hello-world\n"
        line {
          inner {
            data: "hello-world"
            chain {
              inner {
                data: "hello"
                any {}
                chars_consumed: 5
              }
              inner {
                data: "-"
                literal {}
                chars_consumed: 1
              }
              inner {
                data: "world"
                any {}
                chars_consumed: 5
              }
            }
            chars_consumed: 11
          }
        }
        chars_consumed: 12
      }
    }
    chars_consumed: 12
  )"));
}

TEST_F(IOManagerTest, Peek) {
  manager_.Read(TextProto("any {} chain_id: 1"));
  manager_.Read(
      TextProto("peek { inner { literal { literal: 'abc' } } } chain_id: 1"));
  manager_.Read(TextProto("any {} chain_id: 1"));
  manager_.Read(TextProto("literal { literal: 'def' } chain_id: 1"));
  process_.DoRead("lolabc123defghi");
  AssertReadResult(1, TextProto(R"(
    data: "lolabc123def"
    chain {
      inner {
        data: "lol"
        any {}
        chars_consumed: 3
      }
      inner {
        data: "abc"
        peek {
          inner {
            data: "abc"
            literal {}
            chars_consumed: 3
          }
        }
      }
      inner {
        data: "abc123"
        any {}
        chars_consumed: 6
      }
      inner {
        data: "def"
        literal {}
        chars_consumed: 3
      }
    }
    chars_consumed: 12
  )"));
}

TEST_F(IOManagerTest, IncompleteChain) {
  manager_.Read(TextProto("any {} chain_id: 1"));
  manager_.Read(TextProto("literal { literal: 'fgh!' } chain_id: 1"));
  process_.DoRead("abcdefg");
  std::vector<UIIODataUpdate> updates;
  manager_.OnUIUpdate([&](const UIIODataUpdate& update) {
    updates.push_back(update);
    return false;
  });
  process_.DoRead("h!");
  for (const auto& update : updates) {
    // Just print it out for inspection; the protos are too long.
    LOG(INFO) << update.DebugString();
  }
}

// TODO(hpmv): Add more tests here.

int main(int argc, char** argv) {
  absl::InitializeSymbolizer(argv[0]);
  absl::FailureSignalHandlerOptions options;
  absl::InstallFailureSignalHandler(options);
  testing::InitGoogleTest(&argc, argv);
  return RUN_ALL_TESTS();
}
