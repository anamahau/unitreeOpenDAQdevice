#include <coreobjects/callable_info_factory.h>
#include <coreobjects/unit_factory.h>
#include <fmt/format.h>
#include <opendaq/data_rule_factory.h>
#include <opendaq/packet_factory.h>
#include <opendaq/signal_factory.h>
#include <unitree_module/unitree_channel.h>
#include <chrono>

BEGIN_NAMESPACE_UNITREE_MODULE

UnitreeChannel::UnitreeChannel(const ContextPtr& context,
                               const ComponentPtr& parent,
                               const StringPtr& localId,
                               const UnitreeChannelInit& init)
    : ChannelImpl(FunctionBlockType("UnitreeChannel", fmt::format("AI{}", init.index + 1), ""), context, parent, localId)
    , index(init.index)
    , startTime(init.startTime)
    , microSecondsFromEpochToStartTime(init.microSecondsFromEpochToStartTime)
    , samplesGenerated(0)
    , counter(0)
    , sampleRate(init.sampleRate)
{
    initProperties();
    createSignals();
    buildSignalDescriptors();
}

void UnitreeChannel::initProperties()
{
}

void UnitreeChannel::publishSamples(std::chrono::microseconds curTime, std::vector<int16_t>& data)
{
    auto lock = this->getAcquisitionLock();

    // TODO
}

std::tuple<PacketPtr, PacketPtr> UnitreeChannel::generateSamples(int64_t curTime, uint64_t newSamples)
{
    auto domainPacket = DataPacket(timeSignal.getDescriptor(), newSamples, curTime);
    DataPacketPtr dataPacket = DataPacketWithDomain(domainPacket, valueSignal.getDescriptor(), newSamples);

    double* buffer = static_cast<double*>(dataPacket.getRawData());

    //for (uint64_t i = 0; i < newSamples; i++)
    //    buffer[i] = static_cast<double>(counter++) / sampleRate;

    for (uint64_t i = 0; i < newSamples; i++)
        buffer[i] = static_cast<double>(5);

    return {dataPacket, domainPacket};
}

std::string UnitreeChannel::getEpoch()
{
    const std::time_t epochTime = std::chrono::system_clock::to_time_t(std::chrono::time_point<std::chrono::system_clock>{});

    char buf[48];
    strftime(buf, sizeof buf, "%Y-%m-%dT%H:%M:%SZ", gmtime(&epochTime));

    return { buf };
}

RatioPtr UnitreeChannel::getResolution()
{
    return Ratio(1, 1000000);
}

Int UnitreeChannel::getDeltaT(const double sr) const
{
    const double tickPeriod = getResolution();
    const double samplePeriod = 1.0 / sr;
    return static_cast<Int>(std::round(samplePeriod / tickPeriod));
}

void UnitreeChannel::buildSignalDescriptors()
{
    const auto valueDescriptor = DataDescriptorBuilder().setSampleType(SampleType::Float64).setUnit(Unit("V", -1, "volts", "voltage"));

    // const auto valueDescriptorForce = DataDescriptorBuilder().setSampleType(SampleType::Int16).setUnit(Unit("N", -1));

    valueSignal.setDescriptor(valueDescriptor.build());
    deltaT = getDeltaT(sampleRate);

    // forceFLsignal.setDescriptor(valueDescriptorForce.build());
    // forceFRsignal.setDescriptor(valueDescriptorForce.build());
    // forceRLsignal.setDescriptor(valueDescriptorForce.build());
    // forceRRsignal.setDescriptor(valueDescriptorForce.build());

    // delta, start, tickResolution, unit, origin

    // PacketOffset
    // PacketOffset + rule.delta * sampleIndex + rule.start -> ticks since origin
    // absolute time = ticksSinceOrigin * tickResolution + origin

    const auto timeDescriptor = DataDescriptorBuilder()
                                    .setSampleType(SampleType::Int64)
                                    .setUnit(Unit("s", -1, "seconds", "time"))
                                    .setTickResolution(getResolution())
                                    .setRule(LinearDataRule(deltaT, 0))
                                    .setOrigin(getEpoch());

    timeSignal.setDescriptor(timeDescriptor.build());
}

void UnitreeChannel::createSignals()
{
    valueSignal = createAndAddSignal(fmt::format("AI{}", index));
    timeSignal = createAndAddSignal(fmt::format("AI{}Time", index), nullptr, false);
    valueSignal.setDomainSignal(timeSignal);

    forceFLsignal = createAndAddSignal(fmt::format("FL force"));
    forceFLsignal.setDomainSignal(timeSignal);

    forceFRsignal = createAndAddSignal(fmt::format("FR force"));
    forceFRsignal.setDomainSignal(timeSignal);

    forceRLsignal = createAndAddSignal(fmt::format("RL force"));
    forceRLsignal.setDomainSignal(timeSignal);

    forceRRsignal = createAndAddSignal(fmt::format("RR force"));
    forceRRsignal.setDomainSignal(timeSignal);
}

END_NAMESPACE_UNITREE_MODULE
