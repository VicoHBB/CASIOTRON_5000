# CASIOTRON_5000

This project is a configurable clock, with a temperature sensor,
configurable alarms, and a graphical interface. It uses the Real-Time
Clock(RTC) of a STM32GB01 microcontroller, which allows a counter
generation with a (percent of error). The SPI protocol was used for
sending information to an LCD screen: where the date, time, and
temperature were displayed. It also employs external interrupts to
detect the button on the card which serves to activate/deactivate
and display the alarm. Likewise, it uses I2C for communication with
a temperature sensor. The configuration of this clock is done with a
computer terminal by sending the commands through the UART protocol
and uses the WWDG module to reset the microcontroller in case of failure.

This project includes the use of a scheduler to manage tasks in a
deterministic way, as well as the use of state machines for concurrent
execution, thus emulating a real-time process. Finally, the MISRA
standard was followed for the code development.


---

<!--toc:start-->
- [CASIOTRON_5000](#casiotron5000)
  - [Dependencies](#dependencies)
<!--toc:end-->

---


## Dependencies
```sh
yay -S gcc-arm-none-eabi-bin cpcheck # Or use paru
```
