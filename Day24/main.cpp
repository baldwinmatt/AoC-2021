#include "aoc21/helpers.h"
#include <map>
#include <array>
#include <queue>
#include <set>
#include <vector>

namespace {
  enum class Register {
    None = -1,
    W,
    X,
    Y,
    Z,
  };

  enum class Opcode {
    Noop,
    Inp,
    Add,
    Mul,
    Div,
    Mod,
    Eql,
  };

  std::ostream& operator<<(std::ostream& os, const Opcode& o) {
    switch (o) {
      case Opcode::Inp: os << "inp"; break;
      case Opcode::Add: os << "add"; break;
      case Opcode::Mul: os << "mul"; break;
      case Opcode::Div: os << "div"; break;
      case Opcode::Mod: os << "mod"; break;
      case Opcode::Eql: os << "eql"; break;
      case Opcode::Noop: os << "noop"; break;
    }
    return os;
  }

  STRING_CONSTANT(INP, "inp");
  STRING_CONSTANT(ADD, "add");
  STRING_CONSTANT(MUL, "mul");
  STRING_CONSTANT(DIV, "div");
  STRING_CONSTANT(MOD, "mod");
  STRING_CONSTANT(EQL, "eql");
  std::map<std::string_view, Opcode, std::less<>> OpcodeMap{ { INP, Opcode::Inp }, { ADD, Opcode::Add }, { MUL, Opcode::Mul },
  { DIV, Opcode::Div }, { MOD, Opcode::Mod }, { EQL, Opcode::Eql } };

  using Parameter = std::pair<Register, int>;
  using Instruction = std::tuple<Opcode, Register, Parameter>;
  using Program = std::vector<Instruction>;

  const auto parse_opcode = [](const auto sv) {
    const auto r = OpcodeMap.find(sv);
    if (r == OpcodeMap.end()) {
      throw std::runtime_error("Invalid input");
    }

    return r->second;
  };

  const auto parse_register = [](const auto sv) {
    if (sv.size() != 1) {
      return Register::None;
    }
    switch(sv[0]) {
      case 'w':
        return Register::W;
      case 'x':
        return Register::X;
      case 'y':
        return Register::Y;
      case 'z':
        return Register::Z;
      default:
        return Register::None;
    }
  };

  const auto parse_parameter = [](const auto sv, Parameter& p) {
    p.first = parse_register(sv);
    if (p.first == Register::None) {
      std::string s(sv.data(), sv.size());
      p.second = std::stoi(s);
    }
  };

  enum class HaltCode {
    Error = -1,
    Halt,
    Input,
  };

  using StateTuple = std::tuple<int, int, int, int, size_t>;

  class ALU {
    std::array<int, 4> registers;
    size_t ip;
    Register store;

  public:
    friend std::ostream& operator<<(std::ostream& os, const ALU& o) {
      os << "w: " << o.registers[0] <<
            " x: " << o.registers[1] <<
            " y: " << o.registers[2] <<
            " z: " << o.registers[3] <<
            " ip: " << o.ip;
      return os;
    }

    ALU()
      : registers()
      , ip(0)
      , store(Register::None)
    {
      reset();
    }

    ALU(const StateTuple& s)
      : ALU()
    {
      std::tie(registers[0], registers[1], registers[2], registers[3], ip) = s;
    }

    static Opcode parse_instruction(Program& code, const std::string_view line) {
      std::string_view s = line;
      std::vector<std::string_view> parts;
      // split into parts
      while (true) {
        auto p = s.find(' ');
        std::string_view part(s.substr(0, p));
        parts.push_back(part);
        if (p == std::string_view::npos) {
          break;
        }
        p++;
        s = s.substr(p);
      }

      assert(parts.size() >= 2 && parts.size() <= 3);
      // instruction
      const auto op = parse_opcode(parts[0]);
      // register
      const auto r1 = parse_register(parts[1]);
      assert(r1 != Register::None);

      // parameter
      Parameter param = std::make_pair(Register::None, 0);
      assert(op == Opcode::Inp || parts.size() == 3);
      if (op != Opcode::Inp) {
        parse_parameter(parts[2], param);
      }

      code.emplace_back(op, r1, param);

      return op;
    }

    StateTuple get_state() const {
      return StateTuple(registers[0], registers[1], registers[2], registers[3], ip);
    }

    void reset() {
      ip = 0;
      for (auto& r : registers) {
        r = 0;
      }
      store = Register::None;
    }

    void set_input(int v) {
      assert(store != Register::None);
      set_register(store, v);
    }

    int get_output() const {
      return get_register(Register::Z);
    }

    HaltCode run(const Program& code) {
      while (ip < code.size()) {
        Opcode op;
        Register a;
        Parameter b;
        std::tie(op, a, b) = code[ip];
        ip++;

        switch (op) {
          case Opcode::Inp:
            store = a;
            return HaltCode::Input;
          case Opcode::Add:
            set_register(a, get_register(a) + get_parameter(b));
            break;
          case Opcode::Mul:
            set_register(a, get_register(a) * get_parameter(b));
            break;
          case Opcode::Div:
            set_register(a, get_register(a) / get_parameter(b));
            break;
          case Opcode::Mod:
            set_register(a, get_register(a) % get_parameter(b));
            break;
          case Opcode::Eql:
            set_register(a, get_register(a) == get_parameter(b));
            break;
          default:
            return HaltCode::Error;
        }
      }

      return HaltCode::Halt;
    }

  protected:
    int get_register(const Register r) const {
      switch (r) {
        case Register::None:
          throw std::runtime_error("Invalid register");
        case Register::W:
        case Register::X:
        case Register::Y:
        case Register::Z:
          {
            const auto i = static_cast<int>(r);
            return registers[i];
          }
      }
      throw std::runtime_error("Invalid register");
    }

    void set_register(const Register r, int v) {
      switch (r) {
        case Register::None:
          throw std::runtime_error("Invalid register");
        case Register::W:
        case Register::X:
        case Register::Y:
        case Register::Z:
          {
            const auto i = static_cast<int>(r);
            registers[i] = v;
            return;
          }
      }
      throw std::runtime_error("Invalid register");
    }

    int get_parameter(const Parameter& p) const {
      switch (p.first) {
        case Register::None:
          return p.second;
        default:
          return get_register(p.first);
      }
    }

  };

  std::string result;
  std::set<StateTuple> seen;

  bool solve(const Program& code, const StateTuple& regs, const std::vector<int>& range) {
    ALU alu(regs);

    {
      const auto st = alu.get_state();
      const auto r = seen.emplace(st);
      if (!r.second) {
        return false;
      }
    }

    if (seen.size() % 10000000 == 0 || std::get<4>(regs) == 1) {
      DEBUG_PRINT("Seen " << seen.size() << ": " << alu << std::endl);
    }

    HaltCode hc;
    do {
      hc = alu.run(code);
      assert(hc != HaltCode::Error);
      if (hc == HaltCode::Input) {
        for (const auto&r : range) {
          alu.set_input(r);
          const StateTuple st = alu.get_state();
          const auto res = solve(code, st, range);
          if (res) {
            const char c = '0' + r;
            result = c + result;

            DEBUG_PRINT(c);
            return true;
          }
        }
        return false;
      }
    } while (hc != HaltCode::Halt);

    return alu.get_output() == 0;
  }
};

int main(int argc, char** argv) {
  aoc::AutoTimer t;

  Program code;
  ALU alu;
  auto f = aoc::open_argv_1(argc, argv);
  std::string s;
  while (aoc::getline(f ,s)) {
    ALU::parse_instruction(code, s);
  }
  f.close();

  std::vector<int> inc_range;
  for (int i = 1; i < 10; i++) {
    inc_range.push_back(i);
  }
  std::vector<int> dec_range;
  for (int i = 9; i > 0; i--) {
    dec_range.push_back(i);
  }

  solve(code, alu.get_state(), dec_range);
  aoc::print_result(1, result);
  result.clear();
  seen.clear();
  solve(code, alu.get_state(), inc_range);
  aoc::print_result(2, result);

  return 0;
}