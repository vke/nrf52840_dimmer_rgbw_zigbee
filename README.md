--- Russian version below ---

# Steps to compile:

* download and install Segger Embedded Studio for ARM (SES) - free for Nodric Semiconductor hardware users
* download and unpack into any directory Nordic Semiconductor's ZigBee SDK 4.0.0
* download (clone) this repository into `\examples\zigbee\vke\nrf52840_dimmer_rgbw` of unpacked ZigBee SDK. main.c file should be located at `\examples\zigbee\vke\nrf52840_dimmer_rgbw\main.c`
* open `\examples\zigbee\vke\nrf52840_dimmer_rgbw\efekta_mini_dev_board\mbr\ses\nrf52840_dimmer_rgbw_mbr.emProject` project file in SES
* click on Build => Build nrf52840_dimmer_rgbw_mbr menu item inside SES to build project
* hex files for manual firmware download will be available at `\examples\zigbee\vke\nrf52840_dimmer_rgbw\efekta_mini_dev_board\mbr\ses\Output` subdirectories depending on Release/Debug setting and so on
* if you have J-Link connected you can simply click on Build => Build and run or Build => Build and debug menu items.


# Что нужно сделать, чтобы скомпилировать и запустить пример:

* скачать и установить Segger Embedded Studio for ARM (SES) - бесплатно для чипов от Nordic Semiconductor
* скачать и распаковать в любую директорию ZigBee SDK 4.0.0 с сайта Nordic Semiconductor
* скачать (или склонировать) этот репозитарий в директорию `\examples\zigbee\vke\nrf52840_dimmer_rgbw` от ZigBee SDK 4 так, чтобы файл main.c был по пути `\examples\zigbee\vke\nrf52840_dimmer_rgbw\main.c`
* открыть проект `\examples\zigbee\vke\nrf52840_dimmer_rgbw\efekta_mini_dev_board\mbr\ses\nrf52840_dimmer_rgbw_mbr.emProject` внутри SES
* нажать на Build => Build nrf52840_dimmer_rgbw_mbr в меню SES для сборки проекта
* hex-файлы для самостоятельной прошивки будут доступны в директории `\examples\zigbee\vke\nrf52840_dimmer_rgbw\efekta_mini_dev_board\mbr\ses\Output` в поддиректориях, соответствующих конфигурации (Release/Debug)
* если подключен дебаггер J-Link, то достаточно нажать в меню на Build => Build and run или Build => Build and debug, чтобы запустить прошивку
