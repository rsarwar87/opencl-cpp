__kernel void helloworld(__global char* in, __global char* out)
{
  int num = get_global_id(0);
	out[num] = in[num] + 2;
}

__kernel void helloworld2(__global char* in, __global char* out)
{
  int num = get_global_id(0);
	out[num] = in[num] - 1;
}

