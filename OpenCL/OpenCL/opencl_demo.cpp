#include "opencl_demo.h"

// 检查返回值错误
#define CHECK_ERRORS(ERR) \
if(ERR != CL_SUCCESS){ \
        cerr << "OpenCL error code" << ERR \
        << "file: " << __FILE__ \
        << "line: " << __LINE__ \
        << ".\nExiting..." << endl; \
        exit(1); \
}

opencl_demo::opencl_demo() {
    cl_int err;
    cl_uint platformCount;
    std::vector<cl_platform_id> platforms;

    // 获取平台数量
    err = clGetPlatformIDs(0, nullptr, &platformCount);
    CHECK_ERRORS(err);

    // 获取所有平台
    platforms.resize(platformCount);
    err = clGetPlatformIDs(platformCount, platforms.data(), nullptr);
    CHECK_ERRORS(err);

    // 打印平台信息
    for (cl_platform_id platform : platforms) {
        char platformName[128];
        err = clGetPlatformInfo(platform, CL_PLATFORM_NAME, sizeof(platformName), platformName, nullptr);
        CHECK_ERRORS(err);
        std::cout << "Platform: " << platformName << std::endl;
    }

    // 获取设备
    cl_device_id device;
    err = clGetDeviceIDs(platforms[0], CL_DEVICE_TYPE_DEFAULT, 1, &device, nullptr);
    CHECK_ERRORS(err);

    // 打印设备信息
    char deviceName[128];
    err = clGetDeviceInfo(device, CL_DEVICE_NAME, sizeof(deviceName), deviceName, nullptr);
    CHECK_ERRORS(err);
    std::cout << "Device: " << deviceName << std::endl;
}

