@rem write mbr for ota client:
@rem nrfjprog -f nrf52 -r --program ..\..\..\..\..\components\softdevice\mbr\hex\mbr_nrf52_2.4.1_mbr.hex --chiperase

@rem write bootloader for ota client combiled by armgcc:
@rem nrfjprog -f nrf52 -r --program ..\..\..\ota\bootloader\pca10059\mbr\armgcc\_build\nrf52840_xxaa_mbr.hex

@rem build and merge settings and firmware for ota client:
@rem nrfutil settings generate --family NRF52840 --application ..\efekta_mini_dev_board\mbr\ses\Output\Release\Exe\nrf52840_dimmer_rgbw_mbr.hex --application-version 0x01010101 --bootloader-version 1 --bl-settings-version 2 --app-boot-validation VALIDATE_ECDSA_P256_SHA256 --key-file priv.pem settings.hex
@rem mergehex -m ..\efekta_mini_dev_board\mbr\ses\Output\Release\Exe\nrf52840_dimmer_rgbw_mbr.hex settings.hex -o dfu_client.hex

@rem write ota client firmware:
@rem nrfjprog -f nrf52 -r --program dfu_client.hex --sectorerase

@rem prepare ota firmware update package for ota server:
@rem nrfutil pkg generate --hw-version 52 --sd-req 0x00 --application-version 0x01030101 --application ..\efekta_mini_dev_board\mbr\ses\Output\Release\Exe\nrf52840_dimmer_rgbw_mbr.hex --key-file priv.pem --app-boot-validation VALIDATE_ECDSA_P256_SHA256 app_dfu_package.zip --zigbee True --zigbee-manufacturer-id 123 --zigbee-image-type 321 --zigbee-comment good_image --zigbee-ota-hw-version 52 --zigbee-ota-fw-version 0x01030101

@rem write ota server + firmware update:
@rem nrfutil dfu zigbee -f 007B-0141-01030101-good_image.zigbee -snr 583648125 -chan 16
