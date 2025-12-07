# Example device module

Simple example that builds an openDAQ module giving access to an example device. Said device contains 2 channels, each with a value and time signal. The value signal outputs a counter signal, increasing at a rate of 1 per second.

## Testing the module

To test the module, enable the `OPENDAQ_DEVICE_EXAMPLE_ENABLE_SERVER_APP` cmake flag. Doing so will add an openDAQ native server module to your build targets, and add an additional "server_application" executable. Running the executable will create a device, add an openDAQ server, and enable discovery.

To connect to the device, the openDAQ Python GUI application can be used (Latest Python version is recommended):

```
py -m pip install openDAQ
py -m openDAQ
```