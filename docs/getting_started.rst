Getting Started
===============

This version of the Gaia plant sensor works on the ESP32 C3.
This means you have to download and install the IDF tooling to flash the firmware on the device.
IDF can be found `here <https://docs.espressif.com/projects/esp-idf/en/latest/esp32/get-started/index.html#installation>`_

Then head over to the `git <https://github.com/GaiaByTeamLime/firmware-v2>`_ and clone the project.

Once IDF installed, we have to make sure the proper target is set, IDF should detect this automatically, but setting it manually doesn't hurt.

First move into the project directory, then execute the following to set the ESP32 C3 as the target:

``idf.py set-target esp32c3``

You should only need to set the target once in the entire project.

After having the target set, you can flash the firmware on the device by executing the following:

``idf.py flash``

To view the output, you can execute:

``idf.py monitor``

**Note:** to exit this view, press `Ctrt + ]`

Or combine both in one line:

``idf.py flash monitor``

There are many other options available, please reference the 
`official IDF documentation <https://docs.espressif.com/projects/esp-idf/en/latest/esp32/api-guides/tools/idf-py.html>`_
for those.

