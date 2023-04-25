# Gaia Plant Sensor Firmware V2
This repro maintains the 2nd firmware version for the Gaia plant sensor.

## File structure
Below you could see an example file structure this project holds.

Files with several words in the name are written with `snake_case`. For example: `persistent_storage.h`

```
main
 +- persistent_storage
 |    +- persistent_storage.c
 |    +- persistent_storage.h
 +- wifi
 |    +- wifi.c
 |    +- wifi.h
 +- blufi
 |    +- blufi.c
 |    +- blufi.h
 +- prelude.h
 +- main.c
```

Make sure to update the CMakeLists.txt when you create local module

