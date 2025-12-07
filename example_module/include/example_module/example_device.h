/*
 * Copyright 2022-2024 openDAQ d.o.o.
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *     http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

#pragma once
#include <example_module/common.h>
#include <opendaq/channel_ptr.h>
#include <opendaq/device_impl.h>
#include <thread>
#include <condition_variable>

BEGIN_NAMESPACE_EXAMPLE_MODULE

class ExampleDevice final : public Device
{
public:
    explicit ExampleDevice(const ContextPtr& ctx, const ComponentPtr& parent);
    ~ExampleDevice() override;

    static DeviceInfoPtr CreateDeviceInfo();
    static DeviceTypePtr CreateType();

    // IDevice
    DeviceInfoPtr onGetInfo() override;
    uint64_t onGetTicksSinceOrigin() override;

private:
    void initDomain();
    void initChannels();
    void acqLoop();

    std::chrono::microseconds getMicroSecondsSinceDeviceStart() const;

    std::thread acqThread;
    std::condition_variable cv;

    std::chrono::steady_clock::time_point startTime;
    std::chrono::microseconds microSecondsFromEpochToDeviceStart;

    ChannelPtr channel1;
    ChannelPtr channel2;

    // ChannelPtr channelFLf;
    // ChannelPtr channelFRf;
    // ChannelPtr channelRLf;
    // ChannelPtr channelRRf;

    size_t acqLoopTime;
    bool stopAcq;
};

END_NAMESPACE_EXAMPLE_MODULE
