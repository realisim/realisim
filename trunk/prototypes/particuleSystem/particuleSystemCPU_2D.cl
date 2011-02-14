
__kernel void 
iterate(__global float4* ipPositions,
  __global float4* ipVelocities,
  __global int* ipLife,
  __local float4* shared,
  const int iNumParticules,
  __constant float4* iGravHoles,
  const int iNumGravHoles,
  const float iDeltaTsec)
{
  int gid = get_global_id(0);
  int workItem_id = get_local_id(0);
  int tile_size = get_local_size(0);
  
  ipPositions[gid] += ipVelocities[gid] * iDeltaTsec;
  ipPositions[gid].zw = (float2)(0.0, 1.0);
 
  float ghForce = 0.0;
  float4 vf = (float4)(0.0, 0.0, 0.0, 0.0);
  float norm = 0.0;

  //Mon GPU ne supporte pas les boucles… bouette!
  for(int i = 0; i < iNumGravHoles; ++i)
  {
    ghForce = iGravHoles[i].w;
    vf = iGravHoles[i] - ipPositions[gid];
    norm = sqrt(mad( vf.x, vf.x, mad( vf.y, vf.y, vf.z*vf.z) ));
    vf /= norm;
    vf *= (float4)(1.0 / norm * ghForce);
    ipVelocities[gid] += vf * iDeltaTsec;
    ipVelocities[gid].zw = (float2)(0.0, 1.0);
  }
  
  ipLife[gid] -= 1;
}