#include <coretypes/version_info_factory.h>
#include <opendaq/custom_log.h>
#include <example_module/example_device.h>
#include <example_module/example_module.h>
#include <example_module/version.h>

BEGIN_NAMESPACE_EXAMPLE_MODULE

ExampleModule::ExampleModule(ContextPtr context)
    : Module("ReferenceDeviceModule",
             VersionInfo(EXAMPLE_MODULE_MAJOR_VERSION, EXAMPLE_MODULE_MINOR_VERSION, EXAMPLE_MODULE_PATCH_VERSION),
             std::move(context),
             EXAMPLE_MODULE_NAME)
      , deviceAdded(false)
{
}

ListPtr<IDeviceInfo> ExampleModule::onGetAvailableDevices()
{
    ListPtr<IDeviceInfo> availableDevices = List<IDeviceInfo>();

    availableDevices.pushBack(ExampleDevice::CreateDeviceInfo());

    return availableDevices;
}

DictPtr<IString, IDeviceType> ExampleModule::onGetAvailableDeviceTypes()
{
    auto result = Dict<IString, IDeviceType>();

    auto deviceType = ExampleDevice::CreateType();
    result.set(deviceType.getId(), deviceType);

    return result;
}

DevicePtr ExampleModule::onCreateDevice(const StringPtr& connectionString,
                                        const ComponentPtr& parent,
                                        const PropertyObjectPtr& /*config*/)
{
    std::scoped_lock lock(sync);

    std::string connStr = connectionString;
    if (connStr.find("example://") != 0)
        throw std::runtime_error("Invalid connection string prefix");

    if (deviceAdded)
        throw std::runtime_error("Only one device can be created");

    auto devicePtr = createWithImplementation<IDevice, ExampleDevice>(context, parent);

    deviceAdded = true;
    return devicePtr;
}

END_NAMESPACE_EXAMPLE_MODULE
