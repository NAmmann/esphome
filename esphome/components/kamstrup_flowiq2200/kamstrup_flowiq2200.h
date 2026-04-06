#pragma once

#include "esphome/components/sensor/sensor.h"
#include "esphome/components/uart/uart.h"
#include "esphome/core/component.h"
#include <queue>

namespace esphome {
namespace kamstrup_flowiq2200 {

/*
    ===========================================================================
    ===                            KAMSTRUP FlowIQ2200                      ===
    ===========================================================================

    Kamstrup Meter Protocol (KMP) is a protocol used also with Kamstrup
    flow meters, e.g. Kamstrup FlowIQ2200.
    These devices register consumed flow and volume.

    The Kamstrup FlowIQ2200 has an optical interface just above the display.
    This interface is essentially an RS-232 interface using a proprietary
    protocol (Kamstrup Meter Protocol [KMP]).

    The integration uses this optical interface to periodically read the
    configured values (sensors) from the meter. Supported sensors are:
      - Water Flow                [l/h]
      - Water Volume              [m3]

    Note:
    The optical interface is enabled as soon as a magnet is placed on the meter.
    The interface stays active for a few minutes. To keep the interface 'alive'
    the magnet must be placed around the optical sensor and the communication needs
    to be continous.

    Units:
    Units are set using the regular Sensor config in the user yaml. However,
    KMP does also send the correct unit with every value. When DEBUG logging
    is enabled, the received value with the received unit are logged.

    Acknowledgement:
    This interface is a clone of the Kampstrup_KMP interface with some adjustments.
    The information was gathered at the following locations:
      - https://atomstar.tweakblogs.net/blog/19110/reading-out-kamstrup-multical-402-403-with-home-built-optical-head
      - https://wiki.hal9k.dk/projects/kamstrup
*/

// KMP Commands
static const uint16_t CMD_FLOW = 0x004A;
static const uint16_t CMD_VOLUME = 0x0044;

// KMP units
static const char *const UNITS[] = {
    "",      "Wh",   "kWh",  "MWh",   "GWh",     "J",       "kJ",       "MJ",       "GJ",       "Cal",
    "kCal",  "Mcal", "Gcal", "varh",  "kvarh",   "Mvarh",   "Gvarh",    "VAh",      "kVAh",     "MVAh",
    "GVAh",  "kW",   "kW",   "MW",    "GW",      "kvar",    "kvar",     "Mvar",     "Gvar",     "VA",
    "kVA",   "MVA",  "GVA",  "V",     "A",       "kV",      "kA",       "C",        "K",        "l",
    "m3",    "l/h",  "m3/h", "m3xC",  "ton",     "ton/h",   "h",        "hh:mm:ss", "yy:mm:dd", "yyyy:mm:dd",
    "mm:dd", "",     "bar",  "RTC",   "ASCII",   "m3 x 10", "ton x 10", "GJ x 10",  "minutes",  "Bitfield",
    "s",     "ms",   "days", "RTC-Q", "Datetime"};

class KamstrupFlowIQ2200Component : public PollingComponent, public uart::UARTDevice {
 public:
  void set_flow_sensor(sensor::Sensor *sensor) { this->flow_sensor_ = sensor; }
  void set_volume_sensor(sensor::Sensor *sensor) { this->volume_sensor_ = sensor; }
  void dump_config() override;
  void update() override;
  void loop() override;

 protected:
  // Sensors
  sensor::Sensor *flow_sensor_{nullptr};
  sensor::Sensor *volume_sensor_{nullptr};

  // Command queue
  std::queue<uint16_t> command_queue_;

  // Methods

  // Sends a command to the meter and receives its response
  void send_command_(uint16_t command);
  // Sends a message to the meter. A prefix/suffix and CRC are added
  void send_message_(const uint8_t *msg, int msg_len);
  // Clears and data that might be in the UART Rx buffer
  void clear_uart_rx_buffer_();
  // Reads and validates the response to a send command
  void read_command_(uint16_t command);
  // Parses a received message
  void parse_command_message_(uint16_t command, const uint8_t *msg, int msg_len);
  // Sets the received value to the correct sensor
  void set_sensor_value_(uint16_t command, float value, uint8_t unit_idx);
};

// "true" CCITT CRC-16
uint16_t crc16_ccitt(const uint8_t *buffer, int len);

}  // namespace kamstrup_flowiq2200
}  // namespace esphome
