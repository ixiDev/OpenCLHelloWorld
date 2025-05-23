/**
 * Author : Abdelmajid ID ALI
 * Date : 07/10/2021
 * Email : abdelmajid.idali@gmail.com
 */
#define  CL_HPP_MINIMUM_OPENCL_VERSION 120
#define  CL_HPP_TARGET_OPENCL_VERSION 120

#include <iostream>
#include <CL/opencl.hpp> // or #include <CL/cl2.hpp>
#include <fstream>

using namespace cl;
using namespace std;

string loadKernelSourceFromFile(const char *path);

int main() {
    // create a context its contains the platform inside
    const auto context = Context(CL_DEVICE_TYPE_GPU);

    // get all available devices
    const auto devices = context.getInfo<CL_CONTEXT_DEVICES>();

    if (devices.empty()) {
        cout << "No device found ! " << endl;
        exit(1);
    }

    const auto &targetDevice = devices.front();

    cout << "Selected device : " << endl;
    cout << "Name : " << targetDevice.getInfo<CL_DEVICE_NAME>() << endl;
    cout << "Vendor : " << targetDevice.getInfo<CL_DEVICE_VENDOR>() << endl;
    cout << "Version : " << targetDevice.getInfo<CL_DEVICE_VERSION>() << endl;


    const auto path = "../hello_kernel.cl";
    const string source = loadKernelSourceFromFile(path);

    const auto program = Program(context, source);

    if (program.build(devices) != CL_SUCCESS) {
        cout << "Failed to build source file (" << path << ")" << endl;
        auto log = program.getBuildInfo<CL_PROGRAM_BUILD_LOG>();

        if (!log.empty()) {
            for (const auto &item: log) {
                cout << "Log : " << item.first.getInfo<CL_DEVICE_NAME>() << " : " << item.second.c_str() << endl;
            }
        }
        exit(1);
    }

    // number of chars in 'Hello World'
    constexpr int count = 13;
    constexpr size_t size = sizeof(char) * count;

    // our hello text
    char hostTxt[count];

    // initialize text
    for (char &i: hostTxt) {
        i = '-';
    }

    // buffer to get text from device
    //CL_MEM_USE_HOST_PTR
    const auto textBuffer = Buffer(context, CL_MEM_WRITE_ONLY | CL_MEM_USE_HOST_PTR, size, &hostTxt);

    const CommandQueue commandQueue(context, targetDevice, CL_QUEUE_PROFILING_ENABLE);

    // Create kernel with name of kernel function see 'hello_kernel.cl'
    Kernel kernel(program, "say_hello");
    kernel.setArg(0, textBuffer);

    // enqueue  the kernel  and wait until execution finished
    Event event;
    commandQueue.enqueueNDRangeKernel(
        kernel,
        NullRange,
        NDRange(1),
        NDRange(1),
        nullptr,
        &event
    );
    event.wait();

    const unsigned long start_time = event.getProfilingInfo<CL_PROFILING_COMMAND_START>();
    const unsigned long end_time = event.getProfilingInfo<CL_PROFILING_COMMAND_END>();
    const double milliseconds = (static_cast<double>(end_time) - static_cast<double>(start_time)) / 1000000.0;

    cout << "OpenCl Execution time is: " << milliseconds << " ms" << endl;


    // read the text buffer from queue
    const auto result = commandQueue.enqueueReadBuffer(textBuffer, CL_TRUE, 0, size, &hostTxt);
    if (result != CL_SUCCESS)
        cout << "Cannot read buffer" << endl;

    cout << "Text from device  = " << hostTxt << endl;

    return 0;
}

string loadKernelSourceFromFile(const char *path) {
    std::ifstream stream(path);
    std::string str((std::istreambuf_iterator<char>(stream)), std::istreambuf_iterator<char>());
    stream.close();
    return str;
}
