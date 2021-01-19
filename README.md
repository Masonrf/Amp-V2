# Amp V2
The purpose of this project is to add touchscreen control to my original [class D amplifier project](https://www.hackster.io/masonrf/2x260w-4ohm-audio-amplifier-6eb9b6) along with finishing up the overall design of the amplifier.

The [Teensy 4.0](https://www.pjrc.com/store/teensy40.html) MCU used for control can be programmed using the arduino IDE with [Teensyduino](https://www.pjrc.com/teensy/teensyduino.html)

The touchscreen can be programmed with VisualLcdStudioIDE (can't find a download for this on their website, I had to get it through the Display's eBay seller. Not sure if I can redistribute the installer or not).

## Functionality
- Read and display when the amplifier module is clipping or when it's in a fault state
- Reset the amplifier when it's in a fault state
- Change fan speed and read in the current fan speed
- Switch between balanced XLR and single ended 3.5mm inputs
- Switch between SpeakOn and binding post outputs
- Display an audio RTA for both audio channels
- Display a VU meter for both audio channels on the home page
