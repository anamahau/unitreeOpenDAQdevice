#include <example_module/example_device.h>
#include <opendaq/device_info_factory.h>
#include <coreobjects/unit_factory.h>
#include <example_module/example_channel.h>
#include <opendaq/device_type_factory.h>
#include <opendaq/device_domain_factory.h>
#include <opendaq/sync_component_private_ptr.h>
#include <chrono>

BEGIN_NAMESPACE_EXAMPLE_MODULE

using namespace std::chrono_literals;
using  milli = std::chrono::milliseconds;

ExampleDevice::ExampleDevice(const ContextPtr& ctx, const ComponentPtr& parent)
    : GenericDevice<>(ctx, parent, "openDAQ_ExampleDB1234", nullptr, "Example Device")
    , microSecondsFromEpochToDeviceStart(0)
    , acqLoopTime(50)
    , stopAcq(false)
{
    this->loggerComponent = ctx.getLogger().getOrAddComponent(EXAMPLE_MODULE_NAME);
    
    initDomain();
    initChannels();

    acqThread = std::thread{ &ExampleDevice::acqLoop, this };
}

ExampleDevice::~ExampleDevice()
{
    {
        auto lock = this->getAcquisitionLock();
        stopAcq = true;
    }

    acqThread.join();
}

DeviceInfoPtr ExampleDevice::CreateDeviceInfo()
{
    auto devInfo = DeviceInfo("example://device");

    devInfo.setName("ExampleDevice");
    devInfo.setManufacturer("openDAQ");
    devInfo.setModel("Reference device");
    devInfo.setSerialNumber("ExampleDB1234");
    devInfo.setDeviceType(CreateType());

    return devInfo;
}

DeviceTypePtr ExampleDevice::CreateType()
{
    return DeviceType("example_dev",
                      "Example device",
                      "Example device",
                      "example");
}

DeviceInfoPtr ExampleDevice::onGetInfo()
{
    return CreateDeviceInfo();
}

void ExampleDevice::initChannels()
{
    auto microSecondsSinceDeviceStart = getMicroSecondsSinceDeviceStart();

    RefChannelInit init{0, 1000, microSecondsSinceDeviceStart, microSecondsFromEpochToDeviceStart};
    channel1 = createAndAddChannel<ExampleChannel>(ioFolder, "ExCh1", init);

    init.index = 1;
    channel2 = createAndAddChannel<ExampleChannel>(ioFolder, "ExCh2", init);

    // channelFLf = createAndAddChannel<ExampleChannel>(ioFolder, "FLfCh", init);
    // channelFRf = createAndAddChannel<ExampleChannel>(ioFolder, "FRfCh", init);
    // channelRLf = createAndAddChannel<ExampleChannel>(ioFolder, "RLfCh", init);
    // channelRRf = createAndAddChannel<ExampleChannel>(ioFolder, "RRfCh", init);
}

std::chrono::microseconds ExampleDevice::getMicroSecondsSinceDeviceStart() const
{
    auto currentTime = std::chrono::steady_clock::now();
    return std::chrono::duration_cast<std::chrono::microseconds>(currentTime - startTime);
}

uint64_t ExampleDevice::onGetTicksSinceOrigin()
{
    auto ticksSinceEpoch = microSecondsFromEpochToDeviceStart + getMicroSecondsSinceDeviceStart();
    return static_cast<SizeT>(ticksSinceEpoch.count());
}

void ExampleDevice::initDomain()
{
    startTime = std::chrono::steady_clock::now();
    auto startAbsTime = std::chrono::system_clock::now();

    microSecondsFromEpochToDeviceStart = std::chrono::duration_cast<std::chrono::microseconds>(startAbsTime.time_since_epoch());

    this->setDeviceDomain(
        DeviceDomain(ExampleChannel::getResolution(),
                     ExampleChannel::getEpoch(),
                     UnitBuilder().setName("second").setSymbol("s").setQuantity("time").build()));
}

void ExampleDevice::acqLoop()
{
    const auto loopTime = milli(acqLoopTime);
    auto lock = getUniqueLock();

    while (!stopAcq)
    {
        cv.wait_for(lock, loopTime);
        if (!stopAcq)
        {
            auto curTime = getMicroSecondsSinceDeviceStart();
            channel1.asPtr<IRefChannel>()->collectSamples(curTime);
            channel2.asPtr<IRefChannel>()->collectSamples(curTime);
        }
    }
}


END_NAMESPACE_EXAMPLE_MODULE
