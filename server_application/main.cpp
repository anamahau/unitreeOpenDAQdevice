/**
 * Server application
 */

/*
 * TODO:
 * - Copy files to existing ExampleModule -> DONE
 * - Rename files to properly represent what the module is doing -> DONE
 * - Fix CMake to find the files -> DONE
 * - Fix include paths to the new file names -> DONE
 * - Swap myDDSDevice class with Subscriber; call "function" in data callback of subscriber
 * - Provide vector of data through the callback arguments;
 *   - The argument type should be vector<uint16_t> instead of uint16_t
 * - Update signal names to properly represent their data -> DONE
 * - Set value signal packet data to the ones obtained from subscriber
 * - Create `main` function that
 *   - Sets the device as root,
 *   - Adds "mdns" discovery server,
 *   - Adds the "OpenDAQNativeStreaming" server
 *   - Enables discovery on said server
 */

#include <iostream>
#include <opendaq/opendaq.h>

using namespace daq;

void readSamples(const MirroredSignalConfigPtr signal)
{
    using namespace std::chrono_literals;
    StreamReaderPtr reader = StreamReader<double, uint64_t>(signal);
    DataDescriptorPtr descriptor = signal.getDomainSignal().getDescriptor();
    RatioPtr resolution = descriptor.getTickResolution();
    StringPtr origin = descriptor.getOrigin();
    StringPtr unitSymbol = descriptor.getUnit().getSymbol();

    std::cout << "\nReading signal: " << signal.getName() << "; active Streaming source: " << signal.getActiveStreamingSource() << std::endl;
    std::cout << "Origin: " << origin << std::endl;

    double samples[100];
    uint64_t domainSamples[100];
    for (int i = 0; i < 40; ++i)
    {
        std::this_thread::sleep_for(25ms);
        SizeT count = 100;
        reader.readWithDomain(samples, domainSamples, &count);
        if (count > 0)
        {
            Float domainValue = (Int) domainSamples[count - 1] * resolution;
            std::cout << "Value: " << samples[count - 1] << ", Domain: " << domainValue << unitSymbol << std::endl;
        }
    }
}

int main(int /*argc*/, const char* /*argv*/[])
{
    std::cout << ":)" << std::endl;

    // Create an Instance, loading modules at MODULE_PATH
    const auto instance = InstanceBuilder()
                            .addModulePath("C:/Program Files/openDAQ/bin/modules")
                            .setRootDevice("daq.dog://DewesoftRobotics_B42D4000O4N9D402")
                            .addDiscoveryServer("mdns")
                            .build();
    instance.addServer("OpenDAQNativeStreaming", nullptr).enableDiscovery();
    /*auto srv = instance.addServer("OpenDAQNativeStreaming", nullptr);
    srv.enableDiscovery();*/

    /*const auto instance = InstanceBuilder()
                            .addModulePath("C:/Program Files/openDAQ/bin/modules")
                            .setRootDevice("example://")
                            .addDiscoveryServer("mdns")
                            .build();
    instance.addServer("OpenDAQNativeStreaming", nullptr).enableDiscovery();*/

    std::cout << "Press \"enter\" to exit the application..." << std::endl;
    std::cin.get();
    return 0;
}
