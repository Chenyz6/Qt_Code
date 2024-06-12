#ifndef OPENCL_DEMO_H
#define OPENCL_DEMO_H
#define _CRT_SECURE_NO_WARNINGS
#define NUM_FILES 2
#define PROGRAM_FILE_1 "good.cl"
#define PROGRAM_FILE_2 "bad.cl"

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
    void InitContext();      // 初始化上下文
    void InitProgram();      // 初始化程序

private:
    // 定义cl平台
    cl_platform_id* platforms;
    cl_uint num_platforms;      // 平台的数量
    char* platform_ext_data;    // 扩展数据  ext扩展
    size_t platform_ext_size;   // 扩展大小
    const char platform_icd_ext[11] = "cl_khr_icd";

    // 定义cl设备
    cl_device_id device;
    cl_uint device_addr_data;
    char device_name_data[48], device_ext_data[4096];

    // 定义上下文
    cl_context context;
    cl_uint ref_count;  // 上下文引用计数

    // 定义程序
    cl_program program;
    FILE *program_handle;
    char *program_buffer[NUM_FILES];
    char *program_log;
    const char *program_file_name[2] = {PROGRAM_FILE_1, PROGRAM_FILE_2};
    const char program_options[41] = "-cl-finite-math-only -cl-no-signed-zeros";
    size_t program_size[NUM_FILES];
    size_t program_log_size;

    // 定义命令队列
    cl_command_queue queue;
    // 定义错误信息
    cl_int err;

};

#endif // OPENCL_DEMO_H
