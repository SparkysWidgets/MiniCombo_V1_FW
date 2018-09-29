# MiniCombo_V1_FW
Public FW repo for MiniCombo HW V1  


Welcome To: MiniCombo Firmware version 1!!
================================

##### Note: This is for the MiniCombo Hardware Version 1

This is the Firmware which runs on the Kinetis processor and controls MiniCombos hardware :sunglasses:.
MiniCombo is a combination pH and eC interface with an I2C interface to get data out, with an onboard processor to handle both the pH and eC calculations and allow for programmable I2C address this is the next step in out hardware roadmap.

Interfacing with both pH and eC sensors has never been easier!
-------------------------

ToDo: finishing adding in information about project here

Whats in the firmware?
-------------------------

This repo contains a KDS 3.0 project for the MKE04Z8VFK4 MCU which contains all the code needed to run MiniCombo V1 hardware (Please see the Hardware repo for more details about that portion), this build uses the Qfplib Library ( https://www.quinapalus.com/qfplib.html ) :aus:


Basic Usage
-------------------------

ToDO: Add all this fun stuff in soon


Hardware: Schematics and Layouts
-------------------------

One repo is public will add back in links

Known issues and Plannes updates
-------------------------

- Currently on HWv1 designs the pH analog power is defaulted to on state, when HW is configured for 3.3v operation pH front end can brown out when power up, default to on ensure that it will be on even if a brown out occurs. (Symptoms sending an analog on command Response isnt :AFEN, instead random chars due to brown out)

License Info
-------------------------

<p>This is a fully open source project released under the CC BY SA license</p>
<a rel="license" href="http://creativecommons.org/licenses/by-sa/4.0/deed.en_US"><img alt="Creative Commons License" style="border-width: 0px;" src="http://i.creativecommons.org/l/by-sa/4.0/88x31.png" /></a><br />
<span xmlns:dct="http://purl.org/dc/terms/" property="dct:title">MiniCombo FW</span> by <a xmlns:cc="http://creativecommons.org/ns#" href="www.sparkyswidgets.com" property="cc:attributionName" rel="cc:attributionURL">Ryan Edwards, Sparky's Widgets</a> is licensed under a <a rel="license" href="http://creativecommons.org/licenses/by-sa/4.0/deed.en_US">Creative Commons Attribution-ShareAlike 4.0 Unported License</a>.<br />
Based on a work at <a xmlns:dct="http://purl.org/dc/terms/" href="https://www.sparkyswidgets.com" rel="dct:source">https://www.sparkyswidgets.com</a>