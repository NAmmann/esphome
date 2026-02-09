import esphome.codegen as cg
from esphome.components import sensor, uart
import esphome.config_validation as cv
from esphome.const import (
    CONF_FLOW,
    CONF_ID,
    CONF_VOLUME,
    DEVICE_CLASS_EMPTY,
    DEVICE_CLASS_VOLUME,
    STATE_CLASS_MEASUREMENT,
    STATE_CLASS_TOTAL_INCREASING,
    UNIT_CUBIC_METER,
    UNIT_EMPTY,
)

CODEOWNERS = ["@NAmmann"]
DEPENDENCIES = ["uart"]

kamstrup_flowiq2200_ns = cg.esphome_ns.namespace("kamstrup_flowiq2200")
KamstrupFlowIQ2200Component = kamstrup_flowiq2200_ns.class_(
    "KamstrupFlowIQ2200Component", cg.PollingComponent, uart.UARTDevice
)
UNIT_LITRE_PER_HOUR = "l/h"

# Note: The sensor units are set automatically based un the received data from the meter
CONFIG_SCHEMA = (
    cv.Schema(
        {
            cv.GenerateID(): cv.declare_id(KamstrupFlowIQ2200Component),
            cv.Optional(CONF_FLOW): sensor.sensor_schema(
                accuracy_decimals=1,
                device_class=DEVICE_CLASS_VOLUME,
                state_class=STATE_CLASS_MEASUREMENT,
                unit_of_measurement=UNIT_LITRE_PER_HOUR,
            ),
            cv.Optional(CONF_VOLUME): sensor.sensor_schema(
                accuracy_decimals=1,
                device_class=DEVICE_CLASS_VOLUME,
                state_class=STATE_CLASS_TOTAL_INCREASING,
                unit_of_measurement=UNIT_CUBIC_METER,
            ),
        }
    )
    .extend(cv.polling_component_schema("60s"))
    .extend(uart.UART_DEVICE_SCHEMA)
)

FINAL_VALIDATE_SCHEMA = uart.final_validate_device_schema(
    "kamstrup_flowiq2200", baud_rate=1200, require_rx=True, require_tx=True
)


async def to_code(config):
    var = cg.new_Pvariable(config[CONF_ID])
    await cg.register_component(var, config)
    await uart.register_uart_device(var, config)

    # Standard sensors
    for key in [
        CONF_FLOW,
        CONF_VOLUME,
    ]:
        if key not in config:
            continue
        conf = config[key]
        sens = await sensor.new_sensor(conf)
        cg.add(getattr(var, f"set_{key}_sensor")(sens))
