__kernel void 
iterate(__global float4* ipPositions,
  __global float4* ipVelocities,
  __global int* ipLife,
  __local float2* shared,
  const int iNumParticules,
  __constant float4* iGravHoles,
  const int iNumGravHoles,
  const float iDeltaTsec)
{
  int gid = get_global_id(0);
	
  ipPositions[gid].xy += ipVelocities[gid].xy * iDeltaTsec;

  //Mon GPU ne supporte pas les boucles… bouette!
  float ghForce = iGravHoles[0].w;
  float2 vf = iGravHoles[0].xy - ipPositions[gid].xy;
  float norm = sqrt(mad(vf.x, vf.x, vf.y* vf.y));
  vf.xy /= norm;
  vf.xy *= (float2)(1.0 / norm * ghForce);
  ipVelocities[gid].xy += vf.xy * iDeltaTsec;
  ipVelocities[gid].zw = (float2)(0.0, 1.0);
  
  ipLife[gid] -= 1;
}