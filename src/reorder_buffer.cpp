#include <sim/reorder_buffer.hpp>
bool sim::reorder_buffer::full() {
    return commits.full();
}
void sim::reorder_buffer::plan(sim::commitment cmt) {
    commits.push_back(cmt);
}