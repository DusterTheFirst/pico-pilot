#pragma once

// V_out = (V x R2)/(R1 + R2)
// V_out((R1 + R2)/R2) = V
// (R1 + R2)/R2 = V_fac
// V_out * V_fac = V
#define voltage_divider_factor(r1, r2) ((double)(r1 + r2) / (double)r2)

/// The conversion factor between the battery voltage and the voltage reported
/// by the ADC
static const double V_BAT_CONV_FACTOR = voltage_divider_factor(220000, 100000);
