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
    struct config {
        int order;
        int lsq_length;
        int res_stn_count;
        int rob_length;
        int alu_count;
        int lunit_count;
        int sunit_count;
        const char* name;
    };

    extern int branches;
    extern int mispredicts;
    extern int cc;
    extern int ic;
    extern addr_t pc;

    using memcell = std::variant<sim::word, sim::encoded_insn>;
    extern std::vector<memcell> main_memory;
    //   Fetch ⭣
    extern boost::circular_buffer<encoded_insn> decode_buffer;
    //  Decode ⭣
    extern boost::circular_buffer<insn> insn_queue;
    //   Issue ⭣
    extern std::unordered_map<areg, future<word>> rat;
    extern boost::circular_buffer<load_store> lsq;
    extern std::vector<std::optional<reservation>> res_stn;
    // Execute ⭣
    extern std::vector<std::unique_ptr<execution_unit>> execution_units;
    extern boost::circular_buffer<commitment> rob;
    //  Commit ⭣
    extern std::map<areg, word> crf; // ⭢⭡

    void reset(config cfg, const std::vector<memcell>& img, addr_t start);
    void run_until_halt();
    void print_stats();
    void pdebug();

    void flush();
    void fetch();
    void decode();
    void issue();
    void execute();
    void commit();
    void tick();
}