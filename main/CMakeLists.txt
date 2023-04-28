idf_component_register(SRCS "main.c" "adc/adc.c"
                    INCLUDE_DIRS "." "adc")

# In release, comment out the below line
component_compile_definitions("DEBUG")
