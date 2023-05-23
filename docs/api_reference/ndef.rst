NDEF & TLV
==================

Information about the NDEF & TLV format used on the MiFare type 2 tags.


Below is an overfew of a few important data formats.
A lot of this information was found `here <https://stackoverflow.com/a/37224161>`_

The TLV header format:
 - 1-byte The type
 - 1-byte The size
 - x-bytes The payload

The NDEF header
 - 1-byte Record header
 - 1-byte Type length
 - 1-byte Payload length
 - x-bytes Type field (size of this gets determined by type-length)

The Record header
 - bit 7    = MB  = First record of NDEF message
 - bit 6    = ME  = Last record of NDEF message
 - bit 5    = CF  = Last or only record of chain
 - bit 4    = SR  = Short record length field
 - bit 3    = IL  = ID length fields
 - bit 2..0 = TNF = Type name field

The Record header explained in greater detail:
 - TNF: Type name field
    A 3 bit field indicating which type this record is
 - IL: ID length field
    This is set if an ID length field is present. If not, then there is no ID length field.
 - SR: Short record bit
    This is set if the Payload length is one byte.
 - CF: Chunk flag
    If this is set, this is a chunk of a greater NDEF Record.
 - ME: Message end
    If this is set, this is the last NDEF record in the NDEF message.
 - MB: Message begin
    If this is set, this is the first NDEF record in the NDEF message.

.. doxygenfile:: ndef.c
    :sections: func
.. doxygenfile:: ndef.h
   :sections: define
