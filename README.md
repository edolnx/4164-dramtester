# 464-dramtester
Arduino program to test 4164 DRAM chips

Designed to run with a [SparkFun RedBoard Qwiic](https://www.sparkfun.com/products/15123) but should work with any ATMEGA328 based board.
Also designed to run with a [SSD1306 based 128x64 OLED display](https://www.adafruit.com/product/938) connnected over I2C via the Qwiic header.

# Hookup

Here is a map showing where to connect the DRAM pins to the Arduino board:

```
           +---+--+---+
       N/C |1  +--+ 16| GND  -- GND
 8 --    D |2       15| !CAS -- 10
12 --  !WE |3       14| Q    -- 9
11 -- !RAS |4  4164 13| A6   -- 6
 0 --   A0 |5       12| A3   -- 3
 2 --   A2 |6       11| A4   -- 4
 1 --   A1 |7       10| A5   -- 5
5V --  VCC |8        9| A7   -- 7
           +----------+
```

If your OLED display needs the RESET line connected, it can be attached to A3 of the RedBoard. 

You can also optionally connect two LEDs to the board for status indicators:
```
A4 -- FAIL
A5 -- PASS
```
This status is also shown on the OLED display. 

# Usage

Connect the DRAM IC to test and then power the system. The 13 LED on the board is also used as a "BUSY" indicator, when lit the DRAM IC should not be removed.
The test will begin as soon as the RedBoard is powered on. The TX and RX LEDs show activity on the A0 and A1 address lines, however *DO NOT USE THE SERIAL MONITOR IN ARDUINO* as this will cause unpredictable test results.
The status is shown on the OLED display's second line:
` 00  11  01  10  RR `
Each group of letters represents a test: All Zeros, All Ones, Alternating 0, Alternating 1, and Random respectively.
As the test is being run, the selected test will be surrounded by arrows:
`>00< 11  01  10  RR `
Once a test is passed, brakets will be shown and the next test will be highlighted in arrows:
`[00]>11< 01  10  RR `
Any failure will immediately stop the test and say "FAIL" on the first line. The indicator will show which part of the test failed. If all the tests pass, "PASS" will be shown on the first line and the system will wait indefinately.
You can test again by pressing the RESET button on the board, or swap DRAM ICs and press RESET to test another chip.
