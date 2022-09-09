const fz = require('zigbee-herdsman-converters/converters/fromZigbee');
const tz = require('zigbee-herdsman-converters/converters/toZigbee');
const exposes = require('zigbee-herdsman-converters/lib/exposes');
const reporting = require('zigbee-herdsman-converters/lib/reporting');
const extend = require('zigbee-herdsman-converters/lib/extend');
const ota = require('zigbee-herdsman-converters/lib/ota');
const tuya = require('zigbee-herdsman-converters/lib/tuya');
const e = exposes.presets;
const ea = exposes.access;

module.exports = [
	{
		zigbeeModel: [
			'RGBW Dimmer 1.2',
		],
		model: 'RGBW Dimmer 1.2',
		vendor: 'vke',
		description: 'RGBW Dimmer, 4 separate endpoints',
		fromZigbee: [
			fz.on_off,
			fz.brightness,
			fz.battery,
			fz.device_temperature,
		],
		toZigbee: [
			tz.light_onoff_brightness,
		],
		exposes: [
			e.light_brightness().withEndpoint('l1'),
			e.light_brightness().withEndpoint('l2'),
			e.light_brightness().withEndpoint('l3'),
			e.light_brightness().withEndpoint('l4'),
			e.battery().withEndpoint('l5'),
			e.device_temperature().withEndpoint('l5'),
		],
		endpoint: (device) => {
			return {'l1': 10, 'l2': 11, 'l3': 12, 'l4': 13, 'l5': 14, };
		},
		meta: {
			multiEndpoint: true,
		},
		configure: async (device, coordinatorEndpoint, logger) => {
			const endpoint10 = device.getEndpoint(10);
			await reporting.bind(endpoint10, coordinatorEndpoint, ['genOnOff', 'genLevelCtrl']);
			await reporting.onOff(endpoint10);
			await reporting.brightness(endpoint10);

			const endpoint11 = device.getEndpoint(11);
			await reporting.bind(endpoint11, coordinatorEndpoint, ['genOnOff', 'genLevelCtrl']);
			await reporting.onOff(endpoint11);
			await reporting.brightness(endpoint11);

			const endpoint12 = device.getEndpoint(12);
			await reporting.bind(endpoint12, coordinatorEndpoint, ['genOnOff', 'genLevelCtrl']);
			await reporting.onOff(endpoint12);
			await reporting.brightness(endpoint12);

			const endpoint13 = device.getEndpoint(13);
			await reporting.bind(endpoint13, coordinatorEndpoint, ['genOnOff', 'genLevelCtrl']);
			await reporting.onOff(endpoint13);
			await reporting.brightness(endpoint13);

			const endpoint14 = device.getEndpoint(14);
			await reporting.bind(endpoint14, coordinatorEndpoint, ['genDeviceTempCfg', 'haElectricalMeasurement', 'genPowerCfg']);
			await reporting.deviceTemperature(endpoint14, {min: 1, max: 15});
		},
	},
];
