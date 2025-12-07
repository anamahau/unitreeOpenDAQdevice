#include <unitree_module/unitree_device.h>
#include <opendaq/device_info_factory.h>
#include <coreobjects/unit_factory.h>
#include <unitree_module/unitree_channel.h>
#include <opendaq/device_type_factory.h>
#include <opendaq/device_domain_factory.h>
#include <opendaq/sync_component_private_ptr.h>
#include <chrono>
#include <random>
#include <opendaq/data_packet_ptr.h>
#include <opendaq/packet_factory.h>
#include <opendaq/thread_name.h>
#include <iostream>

// #include<LowState.hpp>

//#include<helloworld_unitree/subscriber.cpp>

//#include<>

// #include <dds/dds.hpp>
// #include <dds/domain/qos/DomainParticipantQos.hpp>
// #include <LowState.hpp>

/*
Additional Include Directories
D:\Razno\unitree_go2\cyclonedds-cxx-master\src\ddscxx\include
D:\Razno\unitree_go2\cyclonedds-cxx-master\build\src\ddscxx\include
D:\Razno\unitree_go2\cyclonedds-cxx-master\build\examples\helloworld
D:\Razno\unitree_go2\cyclonedds-master\cyclonedds-master\install\include
D:\Razno\unitree_go2\cyclonedds-master\cyclonedds-master\install\include\ddscxx

Windows PowerShell
Get-ChildItem -Path D:\Razno\unitree_go2 -Filter CycloneDDSConfig.cmake -Recurse
*/

BEGIN_NAMESPACE_UNITREE_MODULE




//
//class SensorDataListener : public virtual dds::sub::DataReaderListener<unitree_go::msg::dds_::LowState_>
//{
//public:
//    virtual void on_data_available(dds::sub::DataReader<unitree_go::msg::dds_::LowState_>& reader)
//    {
//        auto samples = reader.take();
//        for (const auto& sample : samples)
//        {
//            if (sample.info().valid())
//            {
//                const auto& state = sample.data();
//
//                auto forces = state.foot_force();
//                std::cout << "foot forces:"
//                            << " FL=" << forces[0] << " FR=" << forces[1] << " RL=" << forces[2] << " RR=" << forces[3] << std::endl;
//
//                // std::this_thread::sleep_for(std::chrono::milliseconds(200));
//            }
//        }
//    }
//
//    virtual void on_subscription_matched(dds::sub::DataReader<unitree_go::msg::dds_::LowState_>& reader,
//                                         const dds::core::status::SubscriptionMatchedStatus& status)
//    {
//        std::cout << "on_subscription_matched" << std::endl;
//    }
//
//    virtual void on_sample_lost(dds::sub::DataReader<unitree_go::msg::dds_::LowState_>& reader,
//                                const dds::core::status::SampleLostStatus& status)
//    {
//        std::cout << "on_sample_lost" << std::endl;
//    }
//
//    virtual void on_requested_deadline_missed(dds::sub::DataReader<unitree_go::msg::dds_::LowState_>& reader,
//                                              const dds::core::status::RequestedDeadlineMissedStatus& status)
//    {
//        std::cout << "on_requested_deadline_missed" << std::endl;
//    }
//
//    virtual void on_requested_incompatible_qos(dds::sub::DataReader<unitree_go::msg::dds_::LowState_>& reader,
//                                               const dds::core::status::RequestedIncompatibleQosStatus& status)
//    {
//        std::cout << "on_requested_incompatible_qos" << std::endl;
//    }
//
//    virtual void on_sample_rejected(dds::sub::DataReader<unitree_go::msg::dds_::LowState_>& reader,
//                                    const dds::core::status::SampleRejectedStatus& status)
//    {
//        std::cout << "on_sample_rejected" << std::endl;
//    }
//
//    virtual void on_liveliness_changed(dds::sub::DataReader<unitree_go::msg::dds_::LowState_>& reader,
//                                       const dds::core::status::LivelinessChangedStatus& status)
//    {
//        std::cout << "on_liveliness_changed" << std::endl;
//    }
//};
//






using namespace std::chrono_literals;
using  milli = std::chrono::milliseconds;

myDDSDevice::myDDSDevice(const std::function < void(int16_t)>& function)
    : function(function)
{
    smpl = 0;
    sign = 1;
    acqThread = std::thread{&myDDSDevice::acqLoop, this};
}

static std::random_device rd;
static std::mt19937 gen(rd());
static std::uniform_int_distribution<int> dist(10, 30);

void myDDSDevice::acqLoop()
{
    daqNameThread("RobotDog");

    auto lock = std::unique_lock(mutex);

    auto loopTime = milli(10);
    auto prevLoopTime = std::chrono::steady_clock::now();

    while (true)
    {
        const auto time = std::chrono::steady_clock::now();
        cv.wait_until(lock, prevLoopTime + loopTime);

        if (smpl == 0)
            sign = 1;
        else if (smpl == 10)
            sign = -1;

        smpl += sign;
        function(smpl);

        prevLoopTime = time;
        loopTime = milli(static_cast<int>(dist(gen)));
    }
}

UnitreeDevice::UnitreeDevice(const ContextPtr& ctx, const ComponentPtr& parent)
    : GenericDevice<>(ctx, parent, "DewesoftRobotics_B42D4000O4N9D402", nullptr, "RobotDog")
    //, microSecondsFromEpochToDeviceStart(0)
    //, acqLoopTime(50)
    //, stopAcq(false)
{
    /*this->loggerComponent = ctx.getLogger().getOrAddComponent(UNITREE_MODULE_NAME);
    initDomain();
    initChannels();
    acqThread = std::thread{ &UnitreeDevice::acqLoop, this };*/

    /****************************************/
    // Unitree DDS
    // dds::domain::DomainParticipant participant(0);
    // dds::topic::Topic<unitree_go::msg::dds_::LowState_> topic(participant, "rt/lowstate");
    // dds::sub::Subscriber subscriber(participant);
    // dds::sub::DataReader<unitree_go::msg::dds_::LowState_> reader(
    //     subscriber, topic, subscriber.default_datareader_qos(), new SensorDataListener(), dds::core::status::StatusMask::data_available());
    // std::cout << "Listening for Unitree Go2 sensor data on topic 'rt/lowstate'..." << std::endl;
    /****************************************/

    myDev = std::make_shared<myDDSDevice>([this](int16_t data) { processData(data); });
    initSignals();

    // create epoch
    const std::time_t epochTime = std::chrono::system_clock::to_time_t(std::chrono::time_point<std::chrono::system_clock>{});

    char buf[48];
    strftime(buf, sizeof buf, "%Y-%m-%dT%H:%M:%SZ", gmtime(&epochTime));

    std::string epoch = {buf};

    this->setDeviceDomain(
        DeviceDomain(Ratio(1, 1'000'000), epoch, UnitBuilder().setName("seconds").setSymbol("s").setQuantity("time").build()));
}

//UnitreeDevice::~UnitreeDevice()
//{
//    {
//        auto lock = this->getAcquisitionLock();
//        stopAcq = true;
//    }
//
//    acqThread.join();
//}

DeviceInfoPtr UnitreeDevice::CreateDeviceInfo()
{
    // auto devInfo = DeviceInfo("example://device");
    auto devInfo = DeviceInfo("daq.dog://DewesoftRobotics_B42D4000O4N9D402");

    // devInfo.setName("UnitreeDevice");
    devInfo.setName("Robot dog");
    devInfo.setManufacturer("Unitree");
    devInfo.setModel("Unitree GO2");
    devInfo.setSerialNumber("B42D4000O4N9D402");
    devInfo.setDeviceType(CreateType());

    return devInfo;
}

DeviceTypePtr UnitreeDevice::CreateType()
{
    return DeviceType("unitree_dev",
                      "Unitree device",
                      "Unitree device",
                      "unitree");
}

//void UnitreeDevice::initChannels()
//{
//    auto microSecondsSinceDeviceStart = getMicroSecondsSinceDeviceStart();
//
//    RefChannelInit init{0, 1000, microSecondsSinceDeviceStart, microSecondsFromEpochToDeviceStart};
//    channel1 = createAndAddChannel<ExampleChannel>(ioFolder, "ExCh1", init);
//
//    init.index = 1;
//    channel2 = createAndAddChannel<ExampleChannel>(ioFolder, "ExCh2", init);
//}

void UnitreeDevice::initSignals()
{
    sig_F_fl = Signal(context, this->signals, "Sig_F_fl");
    sig_F_fr = Signal(context, this->signals, "Sig_F_fr");
    sig_F_rl = Signal(context, this->signals, "Sig_F_rl");
    sig_F_rr = Signal(context, this->signals, "Sig_F_rr");
    sigTime = Signal(context, this->signals, "SigTime");

    this->signals.addItem(sig_F_fl);
    this->signals.addItem(sig_F_fr);
    this->signals.addItem(sig_F_rl);
    this->signals.addItem(sig_F_rr);
    this->signals.addItem(sigTime);

    // Create resolution
    auto resolution = Ratio(1, 1'000'000);

    // Create epoch
    const std::time_t epochTime = std::chrono::system_clock::to_time_t(std::chrono::time_point<std::chrono::system_clock>{});

    char buf[48];
    strftime(buf, sizeof buf, "%Y-%m-%dT%H:%M:%SZ", gmtime(&epochTime));

    std::string epoch = {buf};

    const auto timeDescriptor = DataDescriptorBuilder()
                                    .setSampleType(SampleType::Int64)
                                    .setUnit(Unit("s", -1, "seconds", "time"))
                                    .setTickResolution(resolution)
                                    .setRule(ExplicitDataRule())
                                    .setOrigin(epoch)
                                    .build();

    sigTime.setDescriptor(timeDescriptor);

    const auto forceDescriptor = DataDescriptorBuilder()
                                    .setSampleType(SampleType::Int16)
                                    .setUnit(Unit("N", -1, "Newton", "force"))
                                    .build();

    sig_F_fl.setDescriptor(forceDescriptor);
    sig_F_fr.setDescriptor(forceDescriptor);
    sig_F_rl.setDescriptor(forceDescriptor);
    sig_F_rr.setDescriptor(forceDescriptor);

    sig_F_fl.setDomainSignal(sigTime);
    sig_F_fr.setDomainSignal(sigTime);
    sig_F_rl.setDomainSignal(sigTime);
    sig_F_rr.setDomainSignal(sigTime);
}

DeviceInfoPtr UnitreeDevice::onGetInfo()
{
    return CreateDeviceInfo();
}

std::chrono::microseconds UnitreeDevice::getMicroSecondsSinceDeviceStart() const
{
    auto currentTime = std::chrono::steady_clock::now();
    return std::chrono::duration_cast<std::chrono::microseconds>(currentTime - startTime);
}

uint64_t UnitreeDevice::onGetTicksSinceOrigin()
{
    /*auto ticksSinceEpoch = microSecondsFromEpochToDeviceStart + getMicroSecondsSinceDeviceStart();
    return static_cast<SizeT>(ticksSinceEpoch.count());*/

    using namespace std::chrono;
    return duration_cast<microseconds>(system_clock::now().time_since_epoch()).count();
}

//void UnitreeDevice::initDomain()
//{
//    startTime = std::chrono::steady_clock::now();
//    auto startAbsTime = std::chrono::system_clock::now();
//
//    microSecondsFromEpochToDeviceStart = std::chrono::duration_cast<std::chrono::microseconds>(startAbsTime.time_since_epoch());
//
//    this->setDeviceDomain(
//        DeviceDomain(ExampleChannel::getResolution(),
//                     ExampleChannel::getEpoch(),
//                     UnitBuilder().setName("second").setSymbol("s").setQuantity("time").build()));
//}

//void UnitreeDevice::UnitreeDevice_fun(const std::function<void(int16_t)>& function)
//{
//    : function(function)
//    smpl = 0;
//    sign = 1;
//    acqThread = std::thread{&UnitreeDevice::acqLoop, this};
//}

//static std::random_device rd;                            // non-deterministic random source
//static std::mt19937 gen(rd());                           // Mersenne Twister engine seeded once
//static std::uniform_int_distribution<int> dist(10, 30);  // inclusive range

//void UnitreeDevice::acqLoop()
//{
//    const auto loopTime = milli(acqLoopTime);
//    auto lock = getUniqueLock();
//
//    while (!stopAcq)
//    {
//        cv.wait_for(lock, loopTime);
//        if (!stopAcq)
//        {
//            auto curTime = getMicroSecondsSinceDeviceStart();
//            channel1.asPtr<IRefChannel>()->collectSamples(curTime);
//            channel2.asPtr<IRefChannel>()->collectSamples(curTime);
//        }
//    }
//}

void UnitreeDevice::processData(int16_t data) const
// void UnitreeDevice::processData(std::vector<int16_t> data) const
{
    using namespace std::chrono;
    int64_t time = duration_cast<microseconds>(system_clock::now().time_since_epoch()).count();

    DataPacketPtr domainPacket = DataPacket(sigTime.getDescriptor(), 1);
    int64_t* domainData = static_cast<int64_t*>(domainPacket.getRawData());
    *domainData = time;

    DataPacketPtr dp_F_fl = DataPacketWithDomain(domainPacket, sig_F_fl.getDescriptor(), 1);
    DataPacketPtr dp_F_fr = DataPacketWithDomain(domainPacket, sig_F_fr.getDescriptor(), 1);
    DataPacketPtr dp_F_rl = DataPacketWithDomain(domainPacket, sig_F_rl.getDescriptor(), 1);
    DataPacketPtr dp_F_rr = DataPacketWithDomain(domainPacket, sig_F_rr.getDescriptor(), 1);

    int16_t* data_F_fl = static_cast<int16_t*>(dp_F_fl.getRawData());
    int16_t* data_F_fr = static_cast<int16_t*>(dp_F_fr.getRawData());
    int16_t* data_F_rl = static_cast<int16_t*>(dp_F_rl.getRawData());
    int16_t* data_F_rr = static_cast<int16_t*>(dp_F_rr.getRawData());

    // std::cout << "data: " << data_F_fl << " " << data_F_fr << " " << data_F_rl << " " << data_F_rr << std::endl;

    *data_F_fl = data;
    *data_F_fr = data;
    *data_F_rl = data;
    *data_F_rr = data;

    sigTime.sendPacket(domainPacket);
    sig_F_fl.sendPacket(dp_F_fl);
    sig_F_fr.sendPacket(dp_F_fr);
    sig_F_rl.sendPacket(dp_F_rl);
    sig_F_rr.sendPacket(dp_F_rr);
}


END_NAMESPACE_UNITREE_MODULE
