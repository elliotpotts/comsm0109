#include <unordered_map>
#include <vector>
#include <utility>
#include <boost/circular_buffer.hpp>

#include <sim/isa.hpp>
#include <sim/insn.hpp>
#include <sim/reservation_station.hpp>
#include <sim/reorder_buffer.hpp>
#include <sim/future.hpp>

namespace sim {
    using memcell = std::variant<sim::word, sim::encoded_insn>;

    inline int t = 0;
    const inline int pipeline_width = 6;
    inline future<addr_t> pc = ready(0);

    inline std::vector<memcell> main_memory(1024 * 1024, 42);
    //   Fetch ⭣
    inline boost::circular_buffer<std::pair<addr_t, memcell>> decode_buffer {pipeline_width};
    //  Decode ⭣
    inline boost::circular_buffer<static_insn> insn_queue {pipeline_width};
    //   Issue ⭣
    inline std::vector<reservation_station_slot> rs_slots {pipeline_width};
    // Execute ⭣
    //inline reorder_buffer rob {40};
    inline boost::circular_buffer<reorder> rob {40};
    //  Commit ⭣
    inline std::unordered_map<areg, word> crf; // ⭢⭡

    // Auxilliary
    inline std::unordered_map<areg, future<word>> rat;
    future<word> resolve_op(encoded_operand);

    void speculate(const static_insn& branch);
    void fetch();
    void decode();
    void issue();
    void execute();
    void commit();
    void tick();
}