//#include "span-lite.hpp"
#include <cmath>
#include <vector>

class RangeSplitter {
    int max;
    int items_per_range;

public:
    RangeSplitter(int max, int num_ranges) : max(max) {
        items_per_range = std::ceil(float(max) / float(num_ranges));
    };

    // Get the lower end for the i-th range
    inline int get_min(int index) const {
        return std::min(items_per_range * index, max);
    };
    // Get the higher end for the i-th range
    inline int get_max(int index) const {
        return std::min(items_per_range * (index + 1), max);
    };
    // Get whether the i-th range is empty
    inline bool is_empty(int index) const {
        return get_min(index) == get_max(index);
    };
};

