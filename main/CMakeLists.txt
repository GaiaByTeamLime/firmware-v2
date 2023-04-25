idf_component_register(SRCS "main.c"
                    INCLUDE_DIRS ".")

# In release, comment out the below line
component_compile_definitions("DEBUG")
