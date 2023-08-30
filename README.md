# Simplified ASIO Interface

---

**Simplified ASIO Interface is a wrapper on top of the ASIO SDK interface that allows simple and intuitive ASIO manipulation.**

---

### Build Instructions

This library supports windows platform. Download  <a herf = http://www.steinberg.net/asiosdk>ASIO SDK</a> and unzip to ```./asio_sdk```, 
then build with CMake.

---

### File Structure

`./asio` : Simplified ASIO interface implementation.

`./asio_sdk` : Put ASIO SDK here.

`./log`: Console logging utility.

`./example_application` : A console application that outputs a test tone, built with this library.

---

### Usage Notes

- For library usage, see `./example_application/main.cpp : CallSequenceDemo()`. The interface should be pretty self-explanatory.


- This library currently does not support driver change during streaming. More functionality and bugfixes will be done in future.