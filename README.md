# Embedded Sensor Hub

A production-grade embedded sensor data acquisition system written in **C11**, demonstrating real-world firmware architecture patterns. Reads temperature/humidity/pressure (BME280 over I2C) and 3-axis acceleration (ADXL345 over SPI), processes data through moving-average filters, and triggers GPIO-based alerts with hysteresis.

## Architecture

```
┌─────────────────────────────────────────────────────┐
│                    Application Layer                  │
│  ┌──────────────┐ ┌───────────────┐ ┌─────────────┐ │
│  │ Sensor       │ │ Data          │ │ Alert       │ │
│  │ Manager      │ │ Processor     │ │ System      │ │
│  └──────┬───────┘ └───────────────┘ └──────┬──────┘ │
│         │                                   │        │
├─────────┼───────────────────────────────────┼────────┤
│         │        Driver Layer               │        │
│  ┌──────┴───────┐            ┌──────────────┴──┐    │
│  │ BME280       │            │ ADXL345         │    │
│  │ (I2C)        │            │ (SPI)           │    │
│  └──────┬───────┘            └──────┬──────────┘    │
│         │                           │                │
├─────────┼───────────────────────────┼────────────────┤
│         │  Hardware Abstraction Layer (HAL)           │
│  ┌──────┴──┐ ┌─────┐ ┌──────┐ ┌──────┐ ┌───────┐   │
│  │  I2C    │ │ SPI │ │ UART │ │ GPIO │ │ Timer │   │
│  └─────────┘ └─────┘ └──────┘ └──────┘ └───────┘   │
│                                                      │
│  [Simulation HAL for desktop / Real HAL for MCU]     │
└─────────────────────────────────────────────────────┘
```

## Key Features

- **Layered architecture** — HAL, drivers, application are fully separated. Swap the HAL to port to any MCU (STM32, ESP32, nRF52).
- **Real sensor protocols** — BME280 (I2C) with datasheet compensation formulas, ADXL345 (SPI) with proper register-level access.
- **Data processing** — Moving average filter with configurable window, min/max tracking.
- **Alert system** — Threshold-based alerting with hysteresis to prevent oscillation, GPIO output for LED/buzzer control.
- **UART logging** — Severity-level logging (DEBUG/INFO/WARN/ERROR) over UART.
- **Unit tests** — Custom lightweight test framework, tests for every module.
- **Simulation HAL** — Runs on desktop (Windows/Linux/macOS) without hardware for development and CI.

## Technology Stack

| Component | Technology |
|---|---|
| Language | C11 |
| Build System | CMake 3.14+ |
| Protocols | I2C, SPI, UART, GPIO |
| Sensors | Bosch BME280, Analog Devices ADXL345 |
| Testing | Custom unit test framework (zero dependencies) |
| CI Ready | CMake + CTest |

## Building

### Prerequisites
- GCC (MinGW on Windows, or any C11 compiler)
- CMake 3.14+

### Build & Run
```bash
mkdir build && cd build
cmake ..
cmake --build .

# Run the application
./sensor_hub

# Run tests
ctest --output-on-failure
# Or directly:
./test_all
```

### Expected Output
```
[INF][INIT] Sensor Manager starting...
[INF][INIT] BME280 initialized
[INF][INIT] ADXL345 initialized
[INF][INIT] Sensor Manager ready
[DBG][DATA] T=23.45C H=42.3% P=101325Pa X=0.062g Y=-0.062g Z=0.998g
...
[INF][REPORT] === Sensor Report (samples: 20, errors: 0) ===
[INF][REPORT] Temperature: avg=23.45C min=23.45C max=23.45C
[INF][REPORT] Humidity:    avg=42.3% min=42.3% max=42.3%
[INF][REPORT] Pressure:    avg=101325Pa
```

### Test Output
```
===== Embedded Sensor Hub — Unit Tests =====

--- test_ring_buffer ---
--- test_data_processor ---
--- test_bme280 ---
--- test_adxl345 ---
--- test_alert_system ---

========================================
Tests run: 35 | Passed: 35 | Failed: 0
========================================
```

## Project Structure

```
embedded-sensor-hub/
├── CMakeLists.txt          # Build system
├── README.md
├── LICENSE
├── include/
│   ├── hal/                # HAL interfaces (platform-independent)
│   │   ├── hal_types.h     # Status codes, enums, common types
│   │   ├── hal_i2c.h       # I2C bus operations
│   │   ├── hal_spi.h       # SPI bus operations
│   │   ├── hal_uart.h      # UART transmit/receive
│   │   ├── hal_gpio.h      # GPIO pin control
│   │   └── hal_timer.h     # System tick and delay
│   ├── drivers/            # Sensor driver interfaces
│   │   ├── bme280.h        # Temperature/humidity/pressure (I2C)
│   │   └── adxl345.h       # 3-axis accelerometer (SPI)
│   ├── app/                # Application logic
│   │   ├── sensor_manager.h
│   │   ├── data_processor.h
│   │   └── alert_system.h
│   └── utils/
│       ├── ring_buffer.h
│       └── logger.h
├── src/
│   ├── hal/                # Simulated HAL (swap for real MCU HAL)
│   ├── drivers/            # Sensor driver implementations
│   ├── app/                # Application logic
│   ├── utils/              # Utilities
│   └── main.c              # Entry point
└── tests/                  # Unit tests
    ├── test_framework.h    # Lightweight assertion macros
    ├── test_main.c         # Test runner
    └── test_*.c            # Per-module tests
```

## Design Decisions

1. **HAL abstraction** — All hardware access goes through `hal_*.h` interfaces. The `src/hal/` implementation is a simulation that stores register maps in arrays. To port to STM32, replace these files with `HAL_I2C_Mem_Read()` etc. — no driver or app code changes needed.

2. **Integer-only sensor compensation** — The BME280 driver uses the Bosch datasheet integer compensation formulas (section 4.2.3) to avoid floating-point in the driver core. Float conversion happens only at the API boundary.

3. **Ring buffer with overwrite** — When full, new samples overwrite the oldest. This matches real embedded behavior where you always want the most recent data.

4. **Alert hysteresis** — Prevents rapid on/off toggling near threshold. A 35°C alert with 2°C hysteresis triggers at 35°C but doesn't clear until 33°C.

## Porting to Real Hardware

To run on STM32 (or similar):

1. Replace `src/hal/*.c` files with STM32 HAL calls:
   - `hal_i2c_read_reg()` → `HAL_I2C_Mem_Read()`
   - `hal_spi_transfer()` → `HAL_SPI_TransmitReceive()`
   - `hal_uart_transmit()` → `HAL_UART_Transmit()`
   - `hal_gpio_write()` → `HAL_GPIO_WritePin()`
   - `hal_timer_get_tick_ms()` → `HAL_GetTick()`
2. Remove `-DSIMULATION_BUILD` from CMakeLists.txt
3. Add STM32 startup files and linker script
4. Everything in `drivers/`, `app/`, and `utils/` stays unchanged
