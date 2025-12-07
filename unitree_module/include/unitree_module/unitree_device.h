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
#include <unitree_module/common.h>
#include <opendaq/channel_ptr.h>
#include <opendaq/device_impl.h>
#include <thread>
#include <condition_variable>

BEGIN_NAMESPACE_UNITREE_MODULE

class myDDSDevice
{
public:
    explicit myDDSDevice(const std::function<void(int16_t)>& function);

    void acqLoop();

    std::thread acqThread;
    std::function<void(int16_t)> function;
    std::condition_variable cv;
    std::mutex mutex;

    int16_t smpl;
    int sign;
};

class UnitreeDevice final : public Device
{
public:
    explicit UnitreeDevice(const ContextPtr& ctx, const ComponentPtr& parent);
    // ~UnitreeDevice() override;

    // void UnitreeDevice_fun(const std::function<void(int16_t)>& function);

    static DeviceInfoPtr CreateDeviceInfo();
    static DeviceTypePtr CreateType();

    // IDevice
    DeviceInfoPtr onGetInfo() override;
    uint64_t onGetTicksSinceOrigin() override;

    // std::condition_variable cv;
    // std::mutex mutex;

    // int16_t smpl;
    // int sign;

    std::shared_ptr<myDDSDevice> myDev;

private:
    // void initDomain();
    // void initChannels();
    void initSignals();
    // void acqLoop();
    void processData(int16_t data) const;
    // void processData(std::vector<int16_t> data) const;

    std::chrono::microseconds getMicroSecondsSinceDeviceStart() const;

    std::thread acqThread;
    std::condition_variable cv;

    std::chrono::steady_clock::time_point startTime;
    std::chrono::microseconds microSecondsFromEpochToDeviceStart;

    ChannelPtr channel1;
    ChannelPtr channel2;

    SignalConfigPtr sig_F_fl;
    SignalConfigPtr sig_F_fr;
    SignalConfigPtr sig_F_rl;
    SignalConfigPtr sig_F_rr;
    SignalConfigPtr sigTime;

    size_t acqLoopTime;
    bool stopAcq;
};

END_NAMESPACE_UNITREE_MODULE
