#include <example_module/example_channel.h>
#include <opendaq/signal_factory.h>
#include <opendaq/packet_factory.h>
#include <fmt/format.h>
#include <coreobjects/callable_info_factory.h>
#include <opendaq/data_rule_factory.h>
#include <coreobjects/unit_factory.h>
#include <chrono>

BEGIN_NAMESPACE_EXAMPLE_MODULE

ExampleChannel::ExampleChannel(const ContextPtr& context,
                               const ComponentPtr& parent,
                               const StringPtr& localId,
                               const RefChannelInit& init)
    : ChannelImpl(FunctionBlockType("ExampleChannel",  fmt::format("AI{}", init.index + 1), ""), context, parent, localId)
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

void ExampleChannel::initProperties()
{
}


uint64_t ExampleChannel::getSamplesSinceStart(std::chrono::microseconds time) const
{
    const uint64_t samplesSinceStart = static_cast<uint64_t>(std::trunc(static_cast<double>((time - startTime).count()) / 1000000.0 * sampleRate));
    return samplesSinceStart;
}

void ExampleChannel::collectSamples(std::chrono::microseconds curTime)
{
    auto lock = this->getAcquisitionLock();

    const uint64_t samplesSinceStart = getSamplesSinceStart(curTime);
    auto newSamples = samplesSinceStart - samplesGenerated;

    if (newSamples > 0)
    {
        const auto packetTime = samplesGenerated * deltaT + static_cast<uint64_t>(microSecondsFromEpochToStartTime.count());
        auto [dataPacket, domainPacket] = generateSamples(static_cast<int64_t>(packetTime), newSamples);

        valueSignal.sendPacket(std::move(dataPacket));
        timeSignal.sendPacket(std::move(domainPacket));

        samplesGenerated = samplesSinceStart;
    }
}

std::tuple<PacketPtr, PacketPtr> ExampleChannel::generateSamples(int64_t curTime, uint64_t newSamples)
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

std::string ExampleChannel::getEpoch()
{
    const std::time_t epochTime = std::chrono::system_clock::to_time_t(std::chrono::time_point<std::chrono::system_clock>{});

    char buf[48];
    strftime(buf, sizeof buf, "%Y-%m-%dT%H:%M:%SZ", gmtime(&epochTime));

    return { buf };
}

RatioPtr ExampleChannel::getResolution()
{
    return Ratio(1, 1000000);
}

Int ExampleChannel::getDeltaT(const double sr) const
{
    const double tickPeriod = getResolution();
    const double samplePeriod = 1.0 / sr;
    return static_cast<Int>(std::round(samplePeriod / tickPeriod));
}

void ExampleChannel::buildSignalDescriptors()
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

void ExampleChannel::createSignals()
{
    valueSignal = createAndAddSignal(fmt::format("AI{}", index));
    timeSignal = createAndAddSignal(fmt::format("AI{}Time", index), nullptr, false);
    valueSignal.setDomainSignal(timeSignal);

    // forceFLsignal = createAndAddSignal(fmt::format("FL force"));
    // forceFRsignal = createAndAddSignal(fmt::format("FR force"));
    // forceRLsignal = createAndAddSignal(fmt::format("RL force"));
    // forceRRsignal = createAndAddSignal(fmt::format("RR force"));
    // forceFLsignal.setDomainSignal(timeSignal);
    // forceFRsignal.setDomainSignal(timeSignal);
    // forceRLsignal.setDomainSignal(timeSignal);
    // forceRRsignal.setDomainSignal(timeSignal);
}

END_NAMESPACE_EXAMPLE_MODULE
