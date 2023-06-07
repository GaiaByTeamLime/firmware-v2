idf_component_register(
    SRCS
        "main.c"
        "wifi/wifi.c"
        "adc/adc.c"
        "persistent_storage/persistent_storage.c"
        "spi/spi.c"
        "rfid/rfid.c"
        "ndef/ndef.c"
        "picc/picc.c"
        "sensors/sensors.c"
    INCLUDE_DIRS
        "."
        "wifi"
        "adc"
        "persistent_storage"
        "spi"
        "rfid"
        "ndef"
        "picc"
        "sensors"
)

# In release, comment out the below line
component_compile_definitions("DEBUG")
