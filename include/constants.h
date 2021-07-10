#pragma once

// V_out = (V x R2)/(R1 + R2)
// V_out((R1 + R2)/R2) = V
// (R1 + R2)/R2 = V_fac
// V_out * V_fac = V
#define voltage_divider_factor(r1, r2) ((double)(r1 + r2) / (double)r2)

/// The onboard LED of the pico
static const unsigned int LED_PIN = 25;

/// A GPIO tied, with a voltage divider, to the v_bus to detect its presence
static const unsigned int V_BUS_MONITOR_PIN = 24;

/// A GPIO tied to the PS (Power Save) pin on the internal SMPS to switch
/// between PFM (Low Power, 0) and PWM (High Power, 1) modes.
static const unsigned int PSU_PS_PIN = 23;

static const unsigned int TVC_X_AXIS_PWM = 21;
static const unsigned int TVC_Z_AXIS_PWM = 20;

static const unsigned int V_SYS_ADC_PIN = 29;
static const unsigned int V_BAT_ADC_PIN = 28;
static const unsigned int GND_REF_ADC_PIN = 27;

static const double V_BAT_CONV_FACTOR = voltage_divider_factor(220000, 100000);

static const unsigned int TARGET_BAUD = /* 7812500 */ 115200;
