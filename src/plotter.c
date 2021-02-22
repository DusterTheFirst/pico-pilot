#include "plotter.h"

void plot(PlotterData all_data[], uint data_length) {
    puts("P: ");

    for (uint i = 0; i < data_length; ++i) {
        PlotterData data = all_data[i];

        switch (data.type) {
            case FloatingPoint:
                printf("%f", data.value);
                break;
            case Integer:
                printf("%i", data.value);
                break;
            case UnsignedInteger:
                printf("%u", data.value);
                break;
        }

        if (i + 1 < data_length) {
            puts(",");
        }
    }
}