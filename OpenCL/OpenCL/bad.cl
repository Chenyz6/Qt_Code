__kernel void good1(__global float *a,
                   __global float *b,
                   __global float *c) {
   
   *c = *a + *b;
}
