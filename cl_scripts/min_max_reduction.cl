  __kernel void min_max_reduction ( __global const float *input, 
                         __global float *partialMin,
                         __global float *partialMax,
                         __global float *partialSums,
                         __local float *localMin,
                         __local float *localMax,
                         __local float *localSums
                         )
 {
  uint local_id = get_local_id(0);
  uint group_size = get_local_size(0);

  // Copy from global to local memory
  localSums[local_id] = input[get_global_id(0)];
  localMin[local_id] = input[get_global_id(0)];
  localMax[local_id] = input[get_global_id(0)];
  barrier(CLK_LOCAL_MEM_FENCE);

  // Loop for computing localSums : divide WorkGroup into 2 parts
  for (uint stride = group_size >> 1; stride>0; stride >>= 1)
  {
    // Add elements 2 by 2 between local_id and local_id + stride
    if (local_id < stride)
    {
      localSums[local_id] += localSums[local_id + stride];
      localMin[local_id] = fmin(localMin[local_id], localMin[local_id + stride]);
      localMax[local_id] = fmax(localMax[local_id], localMax[local_id + stride]);
    }
    // Waiting for each 2x2 addition into given workgroup
    barrier(CLK_LOCAL_MEM_FENCE);
  }

  // Write result into partialSums[nWorkGroups]
  if (local_id == 0)
  {
    partialSums[get_group_id(0)] = localSums[0];
    partialMax[get_group_id(0)] = localMax[0];
    partialMin[get_group_id(0)] = localMin[0];
  }
 } 
