#include <gmock/gmock.h>
#include <testutils/testutils.h>
#include <opendaq/opendaq.h>


using namespace daq;
using ExampleModuleTest = testing::Test;

TEST_F(ExampleModuleTest, Test1)
{
    const auto instance = Instance();
    auto dev = instance.addDevice("example://device");

    auto reader = StreamReader<double>(dev.getSignalsRecursive()[0]);
    double buffer[1000];

    for (int x = 0; x < 100; x++)
    {
        auto avail = reader.getAvailableCount();
        auto data = reader.read(buffer, &avail);

        for (int i = 0; i < avail; i++)
        {
            std::cout << buffer[i] << std::endl;
        }
    }
}
