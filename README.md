# attiny13-circuit-checker
Importing Arduino IDE 1.8.5 sketch From In-Circuit-Checker by ChaN (http://elm-chan.org)

## notice

Original Design:
http://elm-chan.org/works/cch/report.html

I tried attiny13's fuse "H:FB L:3B", with Using USBASP(Slow-mode).

## setting Arduino IDE (1.8.5)

### add board manager URL:
https://mcudude.github.io/MicroCore/package_MCUdude_MicroCore_index.json

### installing board manager:
MicroCore

### burning bootloader(once):
* Tools -> board -> ATtiny13
* Tools -> Clock -> 128 KHz internal osc.
* Tools -> Writer -> USBasp (Set Slow JP on USBasp board)

### building
Let's enjoy!


## License

designed by ChaN (elm-chan.org)

CreativeCommon BY 3.0 / Adapted.
