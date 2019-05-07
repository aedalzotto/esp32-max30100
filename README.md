# ESP32-MAX30100
ESP32 C Library driver for Maxim MAX30100 sensor.

## Authors

* **Raivis Strogonovs** - *[MAX30100](https://github.com/xcoder123/MAX30100)* - [xcoder123](https://github.com/xcoder123)
* **Angelo Elias Dalzotto** - *ESP32 port and enhancements* - [aedalzotto](https://github.com/aedalzotto)

# Adding to your project

You need to add this project as a submodule inside your ESP-IDF project "components" folder
From your project folder:
```
mkdir -p components && cd components
git submodule add https://github.com/aedalzotto/esp32-max30100.git
```

If you prefer you can clone instead of adding as submodule:
```
mkdir -p components && cd components
git clone https://github.com/aedalzotto/esp32-max30100.git
```

## Example

An example is available at https://github.com/aedalzotto/esp32-max30100-example

## License

This project is licensed under the MIT license - see the [LICENSE.md](LICENSE.md) file for details

## TODO

* Add interrupt-based I2C communication for better performance.
* Make a standard update task as library function that puts the detected beats to a queue.
