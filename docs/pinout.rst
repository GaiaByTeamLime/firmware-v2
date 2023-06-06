Pinout
======

This page displays the pinout of the current Gaia device.

In text form, the following pins are used:

=== ========== ===========
Pin Function   Description
=== ========== ===========
1   DIGITAL IN |pin01|
--- ---------- -----------
2   MISO       |pin02|
--- ---------- -----------
3   ADC IN     |pin03|
--- ---------- -----------
4   ADC IN     |pin04|
--- ---------- -----------
6   CLK        |pin06|
--- ---------- -----------
7   MOSI       |pin07|
--- ---------- -----------
10  SDA        |pin10|
=== ========== ===========

.. |pin01| replace:: Connected to the soil capacity sensor, we measure the time between highs to determine a value, therefore it's a digital pin
.. |pin02| replace:: The SPI Master-In-Slave-Out pin, used to communicate to the RFID reader
.. |pin03| replace:: Connect to an LDR, reads an analogue value
.. |pin04| replace:: Connected to the battery, this pin is used to measure the battery capacity of the device
.. |pin06| replace:: The SPI clock pin, used to communicate to the RFID reader
.. |pin07| replace:: The SPI Master-Out-Slave-In pin, used to communicate to the RFID reader
.. |pin10| replace:: The SDA pin, used to communicate to the RFID reader

**TODO:** pinout image

