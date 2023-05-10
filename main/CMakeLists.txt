idf_component_register(
    SRCS
        "main.c"
        "adc/adc.c"
        "persistent_storage/persistent_storage.c"
    INCLUDE_DIRS
        "."
        "adc"
        "persistent_storage"
)

# In release, comment out the below line
component_compile_definitions("DEBUG")
