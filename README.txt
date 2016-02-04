SPECS
=====
---------------------------------------
|      Ⓜ                              |
|          o       o  o o  o          |
|  o       o       o       o       o  |
|  1  <6>  2  <7>  3  <8>  4  <9>  5  |
---------------------------------------



EVENTS
------
Press (P): When HIGH, wait 50 ms for no other adjacent switch, trigger, trigger P
Double Press (DP): if two adjacents pressed within 50ms, trigger DP
Press+Hold (PH): Wait 1sec, then cancel last "press", trigger PH
For a press, keep last state for 1sec, if it's a PH, jump back to that state and perform the PH


*LIVE (red)
-----------
1-5 patches
PH9 -> SAVE


*MIDI (blue)
------------
1 & 2 control Zoom
3 & 4 control Nova Drive
5 controls bypass loop #1
PH9 -> SAVE


*LOOPER (green)
---------------
1 2 & 3 control the looper bank up/down/undo


*PRESET (yellow)
----------------
1 bank up
2 bank down
Bank search mode flashing bank leds - select 1-5 to confirm
PH1/PH2 exits "bank search mode" (returns to previous bank)
PH 3 4 5 -> COPYSWAP

COPYSWAPSAVE (purple)
---------------------
1 bank down
2 bank up
PH 1 or 2 PREVSTATE
3 4 or 5 to copy/save transition to COPYSWAPSAVEWAIT (flash 3, 4, 5 LED)

COPYSWAPSAVEWAIT (purple flashing)
----------------------------------
PH 3 4 or 5 to swap/save (will undo previous copy/overwrite) then go back to PRESET with newPreset
if 1.5sec pass and no PH, go to PRESET with newPreset

STARTUP
-------
reset all relays
restore state
  last MIDI program for each device, last known true bypass loops, activate last state, remember last bank/patch

MEMORY
------
Patches (3 bytes each):
15 banks with 3 patches
1 byte for loops (5 bits used)
2 bytes for midi commands (device 1 and 2) - program change + channel is implicit

45 presets (15 banks * 3 patches)
135 bytes for patches (45 * 3 bytes) (addresses 4-138)

State (4 bytes) (addresses 0-3)
1st byte: first 5 bits for active loops. last 2 bits for mode (LIVE, MIDI, LOOPER, PRESET)
2nd byte: which preset active on device 1
3rd byte: which preset active on device 2
4th byte: last active patch in PRESET mode

ex: 00001001 -> True bypass mode, true bypass loop 5 on, rest off
    00000001 -> device 1 gets program 1
    00000010 -> device 2 gets program 2
    00000111 -> save lastActivePatch as 8th patch (binary 7, but 0-index)

When PRESET mode is selected, 8th patch will activate, bank 3 will show (0011), with LED 4 on (2nd patch in bank 3)

PINS
----
7 leds (4 digital out, 3 analog out)
1 buttons (analog in)
8 relays (digital out)
1 usb/midi (tx)
