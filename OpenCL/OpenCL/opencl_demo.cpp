#include "opencl_demo.h"

opencl_demo::opencl_demo() {

    InitPlatForms();    // 初始化平台
    InitDevices();      // 初始化设备

    free(platforms);    // 释放平台



    // /* Access the first GPU/CPU */
    // err = clGetDeviceIDs(platforms, CL_DEVICE_TYPE_GPU, 1, &device, NULL);
    // if(err == CL_DEVICE_NOT_FOUND) {
    //     err = clGetDeviceIDs(platforms, CL_DEVICE_TYPE_CPU, 1, &device, NULL);
    // }
    // if(err < 0) {
    //     perror("Couldn't find any devices");
    //     exit(1);
    // }

    // /* Create a context */
    // context = clCreateContext(NULL, 1, &device, NULL, NULL, &err);
    // if(err < 0) {
    //     perror("Couldn't create a context");
    //     exit(1);
    // }

    // /* Read program file and place content into buffer */
    // program_handle = fopen(PROGRAM_FILE, "r");
    // if(program_handle == NULL) {
    //     perror("Couldn't find the program file");
    //     exit(1);
    // }
    // fseek(program_handle, 0, SEEK_END);
    // program_size = ftell(program_handle);
    // rewind(program_handle);
    // program_buffer = (char*)malloc(program_size+1);
    // program_buffer[program_size] = '\0';
    // fread(program_buffer, sizeof(char), program_size, program_handle);
    // fclose(program_handle);

    // /* Create program from file */
    // program = clCreateProgramWithSource(context, 1,
    //                                     (const char**)&program_buffer, &program_size, &err);
    // if(err < 0) {
    //     perror("Couldn't create the program");
    //     exit(1);
    // }
    // free(program_buffer);

    // /* Build program */
    // err = clBuildProgram(program, 1, &device, NULL, NULL, NULL);
    // if(err < 0) {

    //     /* Find size of log and print to std output */
    //     clGetProgramBuildInfo(program, device, CL_PROGRAM_BUILD_LOG,
    //                           0, NULL, &log_size);
    //     program_log = (char*) malloc(log_size+1);
    //     program_log[log_size] = '\0';
    //     clGetProgramBuildInfo(program, device, CL_PROGRAM_BUILD_LOG,
    //                           log_size+1, program_log, NULL);
    //     printf("%s\n", program_log);
    //     free(program_log);
    //     exit(1);
    // }

    // /* Create the kernel */
    // kernel = clCreateKernel(program, KERNEL_NAME, &err);
    // if(err < 0) {
    //     perror("Couldn't create the kernel");
    //     exit(1);
    // }

    // /* Create the command queue */
    // queue = clCreateCommandQueue(context, device, 0, &err);
    // if(err < 0) {
    //     perror("Couldn't create the command queue");
    //     exit(1);
    // }

    // /* Enqueue the kernel execution command */
    // err = clEnqueueTask(queue, kernel, 0, NULL, NULL);
    // if(err < 0) {
    //     perror("Couldn't enqueue the kernel execution command");
    //     exit(1);
    // }
    // else
    //     printf("Successfully queued kernel.\n");

    // /* Deallocate resources */
    // clReleaseCommandQueue(queue);
    // clReleaseKernel(kernel);
    // clReleaseProgram(program);
    // clReleaseContext(context);
}

// 初始化平台
void opencl_demo::InitPlatForms()
{
    /*
    cl_int clGetPlatformIDs(cl_uint num_entries, cl_platform_id* platforms, cl_uint* num_platforms);
    参数1：监测平台数的上限值 --> 限制放到platforms的平台数量
    参数2：平台cl_platform_id
    参数3：返回主机上平台数量

    cl_int clGetPlatformInfo(cl_platform_id platform, cl_platform_info param_name,
                                size_t param_value_size, void* param_value,
                                size_t* param_value_size_ret);
    参数1：平台id
    参数2：所需信息类型  枚举类型
    eg:CL_PLATFORM_NAME--返回和平台相关的名字   CL_PLATFORM_EXTENSIONS--返回平台支持扩展名列表
    参数3：告诉函数所要保存的字节数
    参数4：char* 数组形式返回所要的数据  数组长度由最后一个参数决定
    参数5：数组长度
    */

    // 平台信息
    printf("\n平台信息:\n");

    // 第一遍查询平台的数量
    err = clGetPlatformIDs(5, NULL, &num_platforms);
    if(err < 0) {
        perror("Couldn't find any platforms.");
        exit(1);
    }

    // 第二遍创建平台
    platforms = (cl_platform_id*)malloc(sizeof(cl_platform_id) * num_platforms);
    clGetPlatformIDs(num_platforms, platforms, NULL);

    // 打印平台信息
    for(cl_int i = 0; i < num_platforms; i++) {
        // 第一遍 查询扩展数据 大小
        err = clGetPlatformInfo(platforms[i], CL_PLATFORM_EXTENSIONS, 0, NULL, &platform_ext_size);
        if(err < 0) {
            perror("Couldn't read extension data.");
            exit(1);
        }
        // 接受保存 打印扩展数据
        platform_ext_data = (char*)malloc(platform_ext_size);
        clGetPlatformInfo(platforms[i], CL_PLATFORM_EXTENSIONS,
                          platform_ext_size, platform_ext_data, NULL);
        printf("Platform %d supports extensions: %s\n", i, platform_ext_data);
        // 查找是否有icd字符
        if(strstr(platform_ext_data, platform_icd_ext) != NULL) {
            // C 库函数 char *strstr(const char *haystack, const char *needle)
            // 在字符串 haystack 中查找第一次出现字符串 needle 的位置，不包含终止符 '\0'
            free(platform_ext_data);
            // 显示是否支持ICD扩展
            if(i > -1) printf("Platform %d supports the %s extension.\n", i, platform_icd_ext);
            else printf("No platforms support the %s extension.\n", platform_icd_ext);
            break;
        }
        free(platform_ext_data);
    }
}

// 初始化设备
void opencl_demo::InitDevices()
{
    /*
    cl_int clGetDeviceIDs(cl_platform_id platform, cl_device_type device_type,
                            cl_uint num_entries, cl_device_id* devices,
                            cl_uint* num_devices)
    参数1：平台id
    参数2：设备类型 枚举类型
    eg: CL_DEVICE_TYPE_ALL--表示和平台相关的所有设备 CL_DEVICE_TYPE_CPU CL_DEVICE_TYPE_GPU
    参数3：限定设备数量
    参数4：设备id
    参数5：设备数量
    注：最后两个参数设定为NULL，可以确定所连的设备数量

    cl_int clGetDeviceIndo(cl_device_id device, cl_device_info param_name,
                            size_t param_value_size, void* param_value,
                            size_t* param_value_size_ret)
    同 clGetPlatformInfo()
    参数1：设备id
    参数2：所需信息类型  枚举类型
    eg:CL_DEVICE_NAME--返回设备的名字 CL_DEVICE_ADDRESS_BITS--返回设备地址空间大小 CL_DEVICE_EXTENSIONS--返回设备支持OpenCL扩展
    参数3：告诉函数所要保存的字节数
    参数4：char* 数组形式返回所要的数据  数组长度由最后一个参数决定
    参数5：数组长度
    */

    // 设备信息
    printf("\n设备信息:\n");

    // 如果有GPU设备则接受GPU设备 没有则寻找CPU设备
    err = clGetDeviceIDs(platforms[0], CL_DEVICE_TYPE_GPU, 1, &device, NULL);
    if(err == CL_DEVICE_NOT_FOUND) {
        err = clGetDeviceIDs(platforms[0], CL_DEVICE_TYPE_CPU, 1, &device, NULL);
    }
    if(err < 0) {
        perror("Couldn't access any devices");
        exit(1);
    }

    // 接收设备名字
    err = clGetDeviceInfo(device, CL_DEVICE_NAME, 48 * sizeof(char), device_name_data, NULL);
    if(err < 0) {
        perror("Couldn't read extension data");
        exit(1);
    }

    // 接收设备地址空间大小
    clGetDeviceInfo(device, CL_DEVICE_ADDRESS_BITS, sizeof(device_addr_data), &device_addr_data, NULL);

    // 接收设备扩展
    clGetDeviceInfo(device, CL_DEVICE_EXTENSIONS, 4096 * sizeof(char), device_ext_data, NULL);

    printf("NAME: %s\nADDRESS_WIDTH: %u\nEXTENSIONS: %s\n", device_name_data, device_addr_data, device_ext_data);
}






