#include "opencl_demo.h"

opencl_demo::opencl_demo() {

    InitPlatForms();    // 初始化平台
    InitDevices();      // 初始化设备
    InitContext();      // 初始化上下文
    InitProgram();      // 初始化程序


    free(platforms);    // 释放平台
    clReleaseContext(context);	// 使用完之后释放上下文
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

// 初始化上下文
void opencl_demo::InitContext()
{
    /*
    cl_context clCreateContext(const cl_context_properties* properties, cl_uint num_devices,
                                const cl_device_id* devices, (void CL_CALLBACK* notify_func)(...),
                                void* user_data, cl_int* error)
    clCreateContext()  直接通过确定设备的方式
    参数1：指向由一个属性名和属性值所组成的数组，并且数组以0元素结尾 -- 可以为NULL
    参数2：设备数量
    参数3：设备id
    参数4：回调函数  上下文发生运行错误时 被调用
    参数5：可以指向任何类型的数据，提供报错信息 -- 可以为NULL
    参数6：错误值

    clgetContextInfo(cl_context context, cl_context_info param_name,
                        size_t param_value_size, void* param_value,
                        size_t* param_value_size_ret)
    与clGetPlatformInfo()  clGetDeviceInfo() 相似
    参数2：枚举类型
    eg: CL_CONTEXT_REFERENCE_COUNT -- 返回上下文设备数  CL_CONTEXT_REFERENCE_COUNT -- 返回上下文的引用计数

    上下文引用次数：返回的是cl_context OpenCL需要对cl_context的访问次数进行管理，也称为保留计数/引用计数，
                  在创建的时候初始化为1,当计数为0的时候程序会释放相应的内存空间。
    clRetainContext() 增加计数
    clReleaseContext() 减少计数
    */

    // 上下文信息
    printf("\n上下文信息:\n");

    // 创建一个上下文
    context = clCreateContext(NULL, 1, &device, NULL, NULL, &err);
    if(err < 0) {
        perror("Couldn't create a context");
        exit(1);
    }

    // 上下文引用计数
    err = clGetContextInfo(context, CL_CONTEXT_REFERENCE_COUNT, sizeof(ref_count), &ref_count, NULL);
    if(err < 0) {
        perror("Couldn't read the reference count.");
        exit(1);
    }
    printf("Initial reference count: %u\n", ref_count);

    // 增加一次计数
    clRetainContext(context);
    clGetContextInfo(context, CL_CONTEXT_REFERENCE_COUNT,
                     sizeof(ref_count), &ref_count, NULL);
    printf("Reference count: %u\n", ref_count);

    // 减少一次计数
    clReleaseContext(context);
    clGetContextInfo(context, CL_CONTEXT_REFERENCE_COUNT,
                     sizeof(ref_count), &ref_count, NULL);
    printf("Reference count: %u\n", ref_count);
}

// 初始化程序
void opencl_demo::InitProgram()
{
    /*
    clCreateProgramWithSource(cl_context context, cl_uint src_num,
                                const char** src_strings, const size_t* src_sizes,
                                cl_int* err_code)
    参数1：上下文对象
    参数2：函数的文件数量 与参数3对应
    参数3：每个文件的内容都需要保存到一个由3字符串组成的指针数组(char**)中
    参数4：表示每个文本字符串的大小
    参数5：错误值
    */

    // 程序信息
    printf("\n程序信息:\n");

    // for每次读取一个文本到char*中
    for(cl_int i = 0; i < NUM_FILES; i++) {
        program_handle = fopen(program_file_name[i], "r");
        if(program_handle == NULL) {
            perror("Couldn't find the program file");
            exit(1);
        }
        fseek(program_handle, 0, SEEK_END); // 文件指针移到文件末尾
        program_size[i] = ftell(program_handle);    // 查询文件指针指向当前的位置
        rewind(program_handle);
        program_buffer[i] = (char*)malloc(program_size[i]+1);
        program_buffer[i][program_size[i]] = '\0';
        fread(program_buffer[i], sizeof(char), program_size[i],
              program_handle);
        fclose(program_handle);
    }

    /* Create a program containing all program content */
    program = clCreateProgramWithSource(context, NUM_FILES,
                                        (const char**)program_buffer, program_size, &err);
    if(err < 0) {
        perror("Couldn't create the program");
        exit(1);
    }

    /* Build program */
    err = clBuildProgram(program, 1, &device, program_options, NULL, NULL);
    if(err < 0) {

        /* Find size of log and print to std output */
        clGetProgramBuildInfo(program, device, CL_PROGRAM_BUILD_LOG,
                              0, NULL, &program_log_size);
        program_log = (char*) malloc(program_log_size+1);
        program_log[program_log_size] = '\0';
        clGetProgramBuildInfo(program, device, CL_PROGRAM_BUILD_LOG,
                              program_log_size+1, program_log, NULL);
        printf("%s\n", program_log);
        free(program_log);
        exit(1);
    }

    /* Deallocate resources */
    for(cl_int i=0; i<NUM_FILES; i++) {
        free(program_buffer[i]);
    }
    clReleaseProgram(program);
    clReleaseContext(context);

}






