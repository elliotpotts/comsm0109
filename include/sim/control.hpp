#include <unordered_map>
#include <vector>
#include <boost/circular_buffer.hpp>

#include <sim/isa.hpp>
#include <sim/insn.hpp>
#include <sim/reservation_station.hpp>
#include <sim/reorder_buffer.hpp>

namespace sim {
    using memcell = std::variant<sim::word, sim::encoded_insn>;
    
    static inline const int pipeline_width = 6;
    static inline int pc = 0;
    static inline std::unordered_map<sim::areg, sim::word> crf;
    static inline std::vector<memcell> main_memory(1024 * 1024, 0);

    static inline boost::circular_buffer<memcell> decode_buffer {pipeline_width};
    static inline boost::circular_buffer<sim::encoded_insn> insn_queue {pipeline_width};
    static inline std::unordered_map<sim::areg, sim::future<sim::word>> rat;
    static inline std::vector<sim::reservation_station> reservation_stations {pipeline_width};
    static inline reorder_buffer rob;

    void fetch();
    void decode();
    void issue();
    void execute();
    void commit();
    void tick();
}