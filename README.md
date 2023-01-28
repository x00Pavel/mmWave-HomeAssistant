# mmWave-HomeAssistant

This library is developed for combination of two devices: Wemos D1 mini board and mmWave 24Ghz Human stationary presence radar.

> Exact model of the sensor is `MR24D11C10`

Here I would put my observation from making this combo work.

## Wemos D1 mini

This device pretty common in IoT area, so I wouldn't go into deep with it, but here are some points that I discovered from practice.

### Serial bus (UART)

In the official documentation it is written that this board sharea UART with USB connection.
In other words, when USB is connected, communication via hardware serial bus becomes somehow tricky because Wemos D1 mini has it own logger on this bus.
The problem here is that on ESPHome documentation for [UART](https://esphome.io/components/uart.html) component and connected [logger](https://esphome.io/components/logger.html) component there is an information how to disable this logging.
I tried it and it didn't work.
With setting `baud_rate` parameter to 0 and with changing UART device for logging to `UART0_SWAP`, board still doesn't see communication from the radar on UART0 port.

### Radar

#### Usefull information

Most of the sensors of this radar have only tree values: someone is tationar (nothing happaning, just none), someone is staionar, someone is moving.
Description of sensors:

1. Heart beat sensor with standart values
2. Approaching away sensor. This sensor tells if someone is moving to the radar (someone is closing), or someone is going away from the radar
3. Motor signs (as thet name it). This sensor tells you have much movement are active on 0-100% scale, where 0 is just nothing, 1 is someone is tationar and higher values means more activity in the environment.
4. Human presence sensor (finnaly, uff). This sensor has standard 3 values in use. **BUT**... From the experiencing with this radar, I saw that it can't detect when there is noone in the room. It says if someone is moving, or someone is stationar. But nothing when the area is obviosly empty

So, the problem of not detecting absence might be that the radar is too sensative.
The communication protocl towards the radar is described in the documentation, but it very raw documentation.
Unfortenatly, untill now I didn't have enough time to make it work, so this would be in TODO list.

#### Guides & documentation

There is planty of similar guids on the official Seedo wiki

- <https://wiki.seeedstudio.com/Radar_MR24HPB1/> - this one I found by myself after googling
- <https://wiki.seeedstudio.com/Radar_MR24HPB1/#demo-2-sending-data-to-radar> & <https://wiki.seeedstudio.com/Radar_MR24HPC1/#demo3-send-data-to-sensor> - these two was sent to my on Discord channel by some guy that has some experience with this radars (I assume, that he is a developer for them)

The user manual is attached in the bottom of each guide.

## Summary

Overall experience from the radar is pretty sceptical for several reasons:

- the radar doesn't detect absence with default configuration
- several guides for similar radars (with similar name and look), but any of these guides is directly about the model I have. Confusing :(

But despiet of this, the radar is pretty accurate in deteting the movement in the environment with all of its sensors.
Guides provided on the Discord looks promissing, so hopefully they would realy help.

## TODO

1. ~~Establish the communication towards the radar~~ Done
2. Change the sensativity of the radar via Home Assitent


## Notes

- With fixing of sending commands to the radar, it appeared that geting protocol and hardware version causes fauiler of the whole microcontroller, so those function are disabled (commented in the code)
