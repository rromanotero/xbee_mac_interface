# Xbee MAC Interface for SAM4S Xplained Pro

This is a MAC Interface (based on Xbee S1 Radios) for the SAM4S Xplained Pro. More than Xbee drivers, or an Xbee library, it is an actual IEEE 802.15.4 MAC interface. I call it interface, because the entire MAC functionality is handled in hardware from within the Xbee. I simply wrote drivers for the Xbee, and added a MAC and message abstractions.

## Architecture 

<p align="center">
  <img src="https://github.com/rromanotero/xbee_mac_interface/blob/master/architecture.png" width="480"/>
  <p align="center">It's supposed to be a "wireless" mug!</p>
</p>


## Setting up the radio

Using the XCTU utility make the following changes to the Xbee S1 radio:

- Reset to factory settings (to avoid any parameter being set to a value that might prevent communication),
- Set the baud rate to match RADIO_SPEED_RATE in mac_config.h
- Change to API mode
- Write changes and verify.

Then plug the radio’s Rx, Tx, Vcc and Gnd pins to the SAM4S Xpro board. Because the software uses USART1 you will need to use Rx (Pin 13), Tx (Pin 14), Gnd (Pin 19), and Vcc (Pin 20) in either EXT1 or EXT2. Alternatively, if you have an IO1 XPlained Pro, you could use the pins labeled Rx, Tx, Gnd, and VTG in the IO1 board. (Not sure why these pins? See here or check the SAM4s Xpro User Guide.)

[This is a video](https://youtu.be/72OjWygrqdo) showing the setting up steps.

## Demos

There are three demos. One  demonstrating unicast messages, and one demonstrating the use of RSSI. The unicasting demo consists of two nodes. Both send a 1-byte asynchronous message to each other approximately every half a second. Whenever a message is received, they toggle their respective LEDs. Likewise, the RSSI demo consists of two nodes. They both blink their LEDs depending on the RSSI of the last message received.

[This is a video](https://youtu.be/7ae6xg3zPZA) of the demoes.


## ASF and standalone

There's a stand alone version and an atmel studio. The stand alone version need to be ported for it to work first (see porting below). The atmel studio version I've tested it for ASFv3.34.1, but it should work for newer versions (the ASF doesn't changes that much, but hey who knows!).

## Concurrency

This is a bare-metal implementation, so do not access the radio concurrently! Meaning, do not access the radio from interrupt handlers (that includes msg_received and ack_received). This is because the radio is a shared resource. Some radio functions (e.g. radio_read_address) span two operations: asking something to the radio, and then receiving a response. You don’t want to use the radio in between. Or say execution is in msg_received, which is executed from the USART1 interrupt handler. Then you ask for radio_read_address which will send data to the radio and then busy-wait until it receives a response. Because execution is already in the USART1 Handler it’ll deadlock. In other words, odd things may occur. So don’t!

## Porting

To port to a different platform rewriting of xbee_cpu and xbee_uart modules should suffice. All data types used are stdint. h types (e.g. uint8_t).

## Limitations

Some functionality is NOT IMPLEMENTED. Namely,

- Networking-, security- and sleeping-related radio functions (e.g. node discover, sleep mode) .
- 64-bit MAC address support
- Verification of checksum when receiving API frames, as well as doing something when the checksum is wrong.
- Verification of status (OK Status) in received responses to Xbee commands
- Support for duplicate frames (maybe). The IEEE 802.15.4 standards defines a sequence number as part of the MAC header, to filter duplicate frames. This suggests this is all handled internally by the Xbee. Yet the Xbee has a MAC mode available to eliminate duplicate frames, hence suggesting otherwise.
- Processing of Modem Status frames. Though, with the current functionality, modem status frames are never sent by the Xbee (see data_received_callback in xbee.c).


