# quirc_example
Quirc QR code decoding library example.   
This project uses [this](https://components.espressif.com/components/espressif/quirc) component.   
Unfortunately, there is no sample project available.   
I created this project to verify the operation of this library.   

__Note__   
__This project is incomplete.__  

# Software requirements   
ESP-IDF V5.0 or later.   
ESP-IDF V4.4 release branch reached EOL in July 2024.   
ESP-IDF V5.1 is required when using ESP32C6 and ESP32H2.   

# Hardware requirements   
An ESP32 equipped with PSRAM.   
__PSRAM is required.__   

# Installation
```
git clone https://github.com/nopnop2002/quirc_example
idf.py flash monitor 
```

# Issues
The QR code is successfully parsed, but an assertion failure occurs.   
The cause is unknown.   
I am making this project public to identify the cause.   
```
I (1973) main_task: Calling app_main()
jpg_len=72811
jpg2rgb565 success=1
QR num_codes=1 heap_caps_get_free_size=8450932
quirc_decode err=0
qr_data.payload_len=6
qr_data.payload=[T7GHBY]

assert failed: heap_caps_free heap_caps_base.c:80 (heap != NULL && "free() target pointer is outside heap areas")


Backtrace: 0x40379ae9:0x3fce7ce0 0x40379ab1:0x3fce7d00 0x40380401:0x3fce7d20 0x40375cb9:0x3fce7e40 0x4038041d:0x3fce7e60 0x4200c9ad:0x3fce7e80 0x42009f4d:0x3fce7ea0 0x4037a53d:0x3fceb110
--- 0x40379ae9: panic_abort at /home/nop/esp-idf/components/esp_system/panic.c:477
--- 0x40379ab1: esp_system_abort at /home/nop/esp-idf/components/esp_system/port/esp_system_chip.c:87
--- 0x40380401: __assert_func at /home/nop/esp-idf/components/newlib/src/assert.c:80
--- 0x40375cb9: heap_caps_free at /home/nop/esp-idf/components/heap/heap_caps_base.c:80
--- 0x4038041d: free at /home/nop/esp-idf/components/newlib/src/heap.c:34
--- 0x4200c9ad: quirc_destroy at /home/nop/rtos/quirc_example/managed_components/espressif__quirc/quirc/lib/quirc.c:39
--- 0x42009f4d: qr_task at /home/nop/rtos/quirc_example/main/main.c:127
--- 0x4037a53d: vPortTaskWrapper at /home/nop/esp-idf/components/freertos/FreeRTOS-Kernel/portable/xtensa/port.c:139

ELF file SHA256: fe3083b6b
```
