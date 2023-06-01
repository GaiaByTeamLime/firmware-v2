PICC
==================

This module contains a few basic functions to extract specific data from a PICC device.
It mostly replaces the old NDEF module, as many of it's functionality is not needed.

The following data can be extracted:
 - Network SSID
 - Network Password
 - Temporal URL
 - Authentication Token

It extracts this data by simply looking for it at a specific memory address. These addresses are hardcoded.
The exact location data gets found can be modified by changing the defines in `picc.h`.

.. doxygenfile:: picc.h
   :sections: define func
