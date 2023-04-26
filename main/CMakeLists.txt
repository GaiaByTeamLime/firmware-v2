idf_component_register(
    SRCS
        "main.c"
        "wifi/wifi.c"
    INCLUDE_DIRS
        "."
        "wifi"
)

# In release, comment out the below line
component_compile_definitions("DEBUG")
