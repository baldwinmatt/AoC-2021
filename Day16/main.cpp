#include "aoc21/helpers.h"
#include <vector>
#include <map>
#include <numeric>

namespace {
  using BinaryValue = std::vector<bool>;
  const std::map<char, BinaryValue> V{
    { '0', { 0, 0, 0, 0 } },
    { '1', { 0, 0, 0, 1 } },
    { '2', { 0, 0, 1, 0 } },
    { '3', { 0, 0, 1, 1 } },
    { '4', { 0, 1, 0, 0 } },
    { '5', { 0, 1, 0, 1 } },
    { '6', { 0, 1, 1, 0 } },
    { '7', { 0, 1, 1, 1 } },
    { '8', { 1, 0, 0, 0 } },
    { '9', { 1, 0, 0, 1 } },
    { 'A', { 1, 0, 1, 0 } },
    { 'B', { 1, 0, 1, 1 } },
    { 'C', { 1, 1, 0, 0 } },
    { 'D', { 1, 1, 0, 1 } },
    { 'E', { 1, 1, 1, 0 } },
    { 'F', { 1, 1, 1, 1 } },
  };

  class Packet {
  protected:
    int version; // 3 bits
    int type;    // 3 bits

    size_t length; // in bits

    std::vector<Packet> sub_packets; // sub packets of this packet
    int64_t literal;
  public:
    Packet(int v, int t)
      : version(v)
      , type(t)
      , literal(INT64_MIN)
    { }

    static Packet read_packet(const BinaryValue& stream, size_t& pos, size_t& sum) {
      // First 3 are version
      const int version = Packet::read_integer_value(stream, pos, 3);
      // Next 3 are type
      const int type = Packet::read_integer_value(stream, pos, 3);
      // Read the packet
      sum += version;
      DEBUG_PRINT("Packet: Version: " << version << " Type: " << type << " sum: " << sum);
      Packet p{version, type};
      p.consume(stream, pos, sum);

      return p;
    }

    int64_t compute() const {
      switch (type) {
        case 0:
          return std::accumulate(sub_packets.begin(), sub_packets.end(), 0LL,
            [](int64_t res, const auto& p) -> int64_t {
              const auto r = p.compute();
              DEBUG_PRINT("add " << res << ", " << r);
              return res + r;
            });
        case 1:
          return std::accumulate(sub_packets.begin(), sub_packets.end(), 1LL,
            [](int64_t res, const auto& p) -> int64_t {
              const auto r = p.compute();
              DEBUG_PRINT("mul " << res << ", " << r);
              return res * r;
            });
        case 2:
          return std::accumulate(sub_packets.begin(), sub_packets.end(), INT64_MAX,
            [](int64_t res, const auto& p) -> int64_t {
              const auto r = p.compute();
              DEBUG_PRINT("min " << res << ", " << r);
              return std::min(res, r);
            });
        case 3:
          return std::accumulate(sub_packets.begin(), sub_packets.end(), INT64_MAX,
            [](int64_t res, const auto& p) -> int64_t {
              const auto r = p.compute();
              DEBUG_PRINT("max " << res << ", " << r);
              return std::max(res, r);
            });
        case 5:
          assert(sub_packets.size() == 2);
          {
            const auto r1 = sub_packets[0].compute();
            const auto r2 = sub_packets[1].compute();
            DEBUG_PRINT("gt " << r1 << ", " << r2);
            return r1 > r2 ? 1 : 0;
          }
        case 6:
          assert(sub_packets.size() == 2);
          {
            const auto r1 = sub_packets[0].compute();
            const auto r2 = sub_packets[1].compute();
            DEBUG_PRINT("lt " << r1 << ", " << r2);
            return r1 < r2 ? 1 : 0;
          }
        case 7:
          assert(sub_packets.size() == 2);
          {
            const auto r1 = sub_packets[0].compute();
            const auto r2 = sub_packets[1].compute();
            DEBUG_PRINT("eq " << r1 << ", " << r2);
            return r1 == r2 ? 1 : 0;
          }
        case 4:
          return literal;
        default:
          return 0;
      }
    }

    void consume(const BinaryValue& stream, size_t& pos, size_t& sum) {
      switch (type) {
        case 4:
          // literal
          {
            literal = 0;
            bool last = false;
            do {
              last = read_integer_value(stream, pos, 1) == 0;
              const int d = read_integer_value(stream, pos, 4);
              literal <<= 4;
              literal |= d;
            } while (!last);
            DEBUG_PRINT("Literal: " << literal);
          }
          break;
        default:
          // else not
          {
            const auto length_type_id = read_integer_value(stream, pos, 1);
            switch (length_type_id) {
              case 0:
                // length in bits of all sub-packets
                {
                  int n = read_integer_value(stream, pos, 15);
                  const size_t end_pos = n + pos;
                  DEBUG_PRINT("Sub packet lengths: " << n);
                  while (pos < end_pos) {
                    sub_packets.push_back(read_packet(stream, pos, sum));
                  }
                }
                break;
              case 1:
                // number of sub-packets
                {
                  size_t n = read_integer_value(stream, pos, 11);
                  DEBUG_PRINT("Sub packet count: " << n);
                  while (n > 0) {
                    sub_packets.push_back(read_packet(stream, pos, sum));
                    n--;
                  }
                }
                break;
              default:
                break;
            }
          }
          break;
      }
    }

    static int read_integer_value(const BinaryValue& stream, size_t& pos, size_t sz) {
      //assert (pos + sz < stream.size());
      int v = 0;
      for (size_t i = 0; i < sz; i++) {
        v *= 2;
        v |= stream[pos++];
      }
      return v;
    }

  };
};

int main(int argc, char** argv) {
  aoc::AutoTimer t;

  auto f = aoc::open_argv_1(argc, argv);

  std::string line;
  std::vector<BinaryValue> streams;
  while (aoc::getline(f, line)) {
    size_t stream_pos = 0;
    const size_t idx = streams.size();
    streams.resize(idx + 1);
    for (const auto& c : line) {
      // Fill with zeroes
      auto r = V.find(c);
      assert(r != V.end());
      streams[idx].resize(stream_pos + 4, false);
      for (size_t p = 0; p < r->second.size(); p++) {
        streams[idx][stream_pos] = r->second[p];
        stream_pos++;
      }
    }
  }

  f.close();

  aoc::AutoTimer t2("execute");
  for (const auto& stream : streams) {
    size_t stream_pos = 0;
    size_t sum = 0;
    const auto p = Packet::read_packet(stream, stream_pos, sum);

    aoc::print_result(1, sum);
    const auto result = p.compute();
    DEBUG_PRINT(" = " << result);
    aoc::print_result(2, result);
  }

  return 0;
}

