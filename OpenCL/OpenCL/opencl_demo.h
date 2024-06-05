#ifndef OPENCL_DEMO_H
#define OPENCL_DEMO_H
#define _CRT_SECURE_NO_WARNINGS
#define PROGRAM_FILE "demo.cl"
#define KERNEL_NAME "demo"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>

#ifdef MAC
#include <OpenCL/cl.h>
#else
#include <CL/cl.h>
#endif

using namespace std;

class opencl_demo
{
public:
    opencl_demo();
    void InitPlatForms();    // 初始化平台
    void InitDevices();      // 初始化设备

private:
    // 定义cl平台
    cl_platform_id* platforms;
    cl_uint num_platforms;      // 平台的数量
    char* platform_ext_data;             // 扩展数据
    size_t platform_ext_size;
    const char platform_icd_ext[11] = "cl_khr_icd";

    // 定义cl设备
    cl_device_id device;
    cl_uint device_addr_data;
    char device_name_data[48], device_ext_data[4096];

    // 定义上下文
    cl_context context;
    // 定义命令队列
    cl_command_queue queue;
    // 定义错误信息
    cl_int err;

    /* Program/kernel data structures */
    cl_program program;
    FILE *program_handle;
    char *program_buffer, *program_log;
    size_t program_size, log_size;
    cl_kernel kernel;

};

#endif // OPENCL_DEMO_H
