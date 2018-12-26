#include <unordered_map>
#include <map>
#include <vector>
#include <utility>
#include <boost/circular_buffer.hpp>

#include <sim/isa.hpp>
#include <sim/insn.hpp>
#include <sim/reservation_station.hpp>
#include <sim/reorder_buffer.hpp>
#include <sim/future.hpp>
#include <sim/execution_unit.hpp>
#include <sim/lsq.hpp>

namespace sim {
    using memcell = std::variant<sim::word, sim::encoded_insn>;

    inline int t = 0;
    inline int cc = 0;
    const inline int pipeline_width = 6;
    inline future<addr_t> pc = ready(0);

    inline std::vector<memcell> main_memory(1024 * 1024, 0);
    //   Fetch ⭣
    inline boost::circular_buffer<std::pair<addr_t, memcell>> decode_buffer { pipeline_width };
    //  Decode ⭣
    inline boost::circular_buffer<std::unique_ptr<insn>> insn_queue {pipeline_width};
    //   Issue ⭣
    inline std::unordered_map<areg, future<word>> rat;
    inline boost::circular_buffer<load_store> lsq {20};
    inline std::vector<std::optional<std::unique_ptr<reservation>>> res_stn {36};
    // Execute ⭣
    inline std::vector<std::unique_ptr<execution_unit>> execution_units;
    inline boost::circular_buffer<commitment> rob {40};
    //  Commit ⭣
    inline std::map<areg, word> crf; // ⭢⭡

    //void speculate(const static_insn& branch);
    void fetch();
    void decode();
    void issue();
    void execute();
    void commit();
    void tick();
}