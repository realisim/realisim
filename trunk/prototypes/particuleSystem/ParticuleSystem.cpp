
#include <cassert>
#include "math/Vect.h"
#include "ParticuleSystem.h"
#include "qfile.h"
#include <stdio.h>
#include <stdlib.h>
#include <time.h>

#include <mach/mach_time.h>

double machcore(uint64_t endTime, uint64_t startTime){
	
	uint64_t difference = endTime - startTime;
    static double conversion = 0.0;
	double value = 0.0;
	
    if( 0.0 == conversion )
    {
        mach_timebase_info_data_t info;
        kern_return_t err = mach_timebase_info( &info );
        
        if( 0 == err ){
			/* seconds */
            conversion = 1e-9 * (double) info.numer / (double) info.denom;
			/* nanoseconds */
			//conversion = (double) info.numer / (double) info.denom;
		}
    }
    
	value = conversion * (double) difference;
	
	return value;
}


#include <sys/stat.h>
using namespace realisim;
  using namespace math;
  using namespace openCL;

ParticuleSystem::ParticuleSystem() : mComputingMode(cmCPU),
  mColor(255, 255, 255),
  mIsDecayEnabled(true), 
  mNumParticules(0),
  mEmitterPosition(0.0),
  mpPositions(0),
  mpVelocities(0),
  mpLife(0),
  mMaximumInitialVelocity(300.0),
  mMaximumInitialLife(1000),
  mGravityHoles(),
  mpGravityHoles(0),
  mIterationTimer(),
  mState(sStarted),
  mWorkGroupSize(1),
  mCmdQueue(0),
  mContext(0),
  mDevice(NULL),
  mpCLPositions(0),
  mpCLVelocities(0),
  mpCLLife(0),
  mpCLGravityHoles(0)
{
  mProgram[0] = 0;
  mKernel[0] = 0;
}

ParticuleSystem::ParticuleSystem(const ParticuleSystem& iPs) :
  mComputingMode(iPs.getComputingMode()),
  mColor(iPs.getColor()),
  mIsDecayEnabled(iPs.isDecayEnabled()),
  mNumParticules(iPs.getNumberOfParticules()),
  mEmitterPosition(iPs.getEmitterPosition()),
  mpPositions(0),
  mpVelocities(0),
  mpLife(0),
  mMaximumInitialVelocity(iPs.getMaximumInitialVelocity()),
  mMaximumInitialLife(iPs.getMaximumInitialLife()),
  mGravityHoles(iPs.getGravityHoles()),
  mpGravityHoles(0),
  mIterationTimer(),
  mState(sStarted),
  mCmdQueue(0),
  mContext(0),
  mDevice(NULL),
  mpCLPositions(0),
  mpCLVelocities(0),
  mpCLLife(0),
  mpCLGravityHoles(0)
{
  assert(0);
//  if(getNumberOfParticules() > 0)
//  {
//    size_t size = getNumberOfParticules() * 3 * sizeof(float);
//    mpPositions = (float*)malloc(size);
//    memcpy(mpPositions, iPs.mpPositions, size);
//    
//    mpVelocities = (float*)malloc(size);
//    memcpy(mpVelocities, iPs.mpVelocities, size);
//    
//    mpLife = (float*)malloc(size/3);
//    memcpy(mpLife, iPs.mpLife, size/3);
//  }
}

ParticuleSystem::~ParticuleSystem()
{
  free(mpPositions);
  free(mpVelocities);
  free(mpLife);
  free(mpGravityHoles);

}

ParticuleSystem& ParticuleSystem::operator=(const ParticuleSystem& iT)
{
  assert(0);
  return *this;
}

//----------------------------------------------------------------------------
void ParticuleSystem::addGravityHole(const math::Point3d& iPos, float iForce)
{
  mGravityHoles.push_back(GravityHole(iPos, iForce));
  initGravityHolesForOpenCL();
}

//----------------------------------------------------------------------------
const math::Point3d& ParticuleSystem::getEmitterPosition() const
{return mEmitterPosition;}

//----------------------------------------------------------------------------
const std::vector<ParticuleSystem::GravityHole>& ParticuleSystem::getGravityHoles() const
{ return mGravityHoles; }

//----------------------------------------------------------------------------
unsigned int ParticuleSystem::getMaximumInitialLife() const
{return mMaximumInitialLife;}

//----------------------------------------------------------------------------
unsigned int ParticuleSystem::getMaximumInitialVelocity() const
{return mMaximumInitialVelocity;}

//----------------------------------------------------------------------------
unsigned int ParticuleSystem::getNumberOfParticules() const
{return mNumParticules;}

//----------------------------------------------------------------------------
const float* ParticuleSystem::getParticulesPosition() const
{return mpPositions;}

//----------------------------------------------------------------------------
void ParticuleSystem::initGravityHolesForOpenCL()
{
	//early out
	if(getComputingMode() == cmCPU)
  	return;
    
	if(mpGravityHoles)
  {
    free(mpGravityHoles);
  }
    
  size_t size = mGravityHoles.size() * 4 * sizeof(float);
  mpGravityHoles = (float*)malloc(size);
  
  for(unsigned int i = 0; i < mGravityHoles.size(); ++i)
  {
    int index = i * 4;
    mpGravityHoles[index] = mGravityHoles[i].mPos.getX();
    mpGravityHoles[index+1] = mGravityHoles[i].mPos.getY();
    mpGravityHoles[index+2] = mGravityHoles[i].mPos.getZ();
    mpGravityHoles[index+3] = mGravityHoles[i].mForce;
  }
  
  //allocation du buffer openCL
  cl_int err = 0;
  if(mpCLGravityHoles)
	  clReleaseMemObject(mpCLGravityHoles);
  mpCLGravityHoles = clCreateBuffer(mContext, CL_MEM_READ_ONLY, size, NULL, NULL);
  err = clEnqueueWriteBuffer(mCmdQueue, mpCLGravityHoles, CL_TRUE, 0, size,
                   (void*)mpGravityHoles, 0, NULL, NULL);
  int gvSize = mGravityHoles.size();
  err |= clSetKernelArg(mKernel[0],  5, sizeof(cl_mem), &mpCLGravityHoles);
  err |= clSetKernelArg(mKernel[0],  6, sizeof(int), (int*)&gvSize);  
  assert(err == CL_SUCCESS);
  clFinish(mCmdQueue);
}

//----------------------------------------------------------------------------
void ParticuleSystem::initialize()
{
	//mParticule doit etre un multiple de mWorkgroupSize.
  if(mWorkGroupSize!=1)
  	mNumParticules = mNumParticules + (mWorkGroupSize - (mNumParticules & (mWorkGroupSize - 1)));

  if(mpPositions && mpVelocities && mpLife)
  { free(mpPositions); free(mpVelocities); free(mpLife);}
  
  if(getNumberOfParticules() == 0)
  {
    mpPositions = 0; mpVelocities = 0; mpLife = 0;
  }
  else
  {
    srand(time(NULL));
    
    size_t size = getNumberOfParticules() * 4 * sizeof(float);
    size_t sizeLife = getNumberOfParticules() * sizeof(int);
    mpPositions = (float*)malloc(size);
    mpVelocities = (float*)malloc(size);
    mpLife = (int*)malloc(sizeLife);
    
    memset(mpPositions, 0, size);
    memset(mpVelocities, 0, size);
    memset(mpLife, 0, sizeLife);
      
    //on génère des positions et des velocités pour chaque particules
    for(unsigned int i = 0; i < getNumberOfParticules(); i+=4)
    {
      initializeParticule(i);
    }
    
    if(getComputingMode() != cmCPU)
    {
    cl_int err = 0;
    #pragma mark Memory Allocation
      {       	
        if(mpCLPositions && mpCLVelocities && mpCLLife)
        {
          clReleaseMemObject(mpCLPositions);
          clReleaseMemObject(mpCLVelocities);
          clReleaseMemObject(mpCLLife);          
        }
      	
        // Allocate memory on the device to hold our data and store the results into
        size_t size = getNumberOfParticules() * 4 * sizeof(float);
        size_t sizeLife = getNumberOfParticules() * sizeof(int);
        
        // Input array
        mpCLPositions = clCreateBuffer(mContext, CL_MEM_READ_WRITE, size, NULL, NULL);
        err = clEnqueueWriteBuffer(mCmdQueue, mpCLPositions, CL_TRUE, 0, size,
                       (void*)mpPositions, 0, NULL, NULL);
                       
        // Input array
        mpCLVelocities = clCreateBuffer(mContext, CL_MEM_READ_WRITE, size, NULL, NULL);
        err |= clEnqueueWriteBuffer(mCmdQueue, mpCLVelocities, CL_TRUE, 0, size,
                       (void*)mpVelocities, 0, NULL, NULL);
       
        // Input array
        mpCLLife = clCreateBuffer(mContext, CL_MEM_READ_WRITE, sizeLife, NULL, NULL);
        err |= clEnqueueWriteBuffer(mCmdQueue, mpCLLife, CL_TRUE, 0, sizeLife,
                       (void*)mpLife, 0, NULL, NULL);                             
        assert(err == CL_SUCCESS);
      }
#pragma mark Kernel Arguments
      {
        // Now setup the arguments to our kernel
        err  = clSetKernelArg(mKernel[0],  0, sizeof(cl_mem), &mpCLPositions);
        err |= clSetKernelArg(mKernel[0],  1, sizeof(cl_mem), &mpCLVelocities);
        err |= clSetKernelArg(mKernel[0],  2, sizeof(cl_mem), &mpCLLife);
        size_t sharedSize =  2 * mWorkGroupSize * 2 * sizeof(float);
        err |= clSetKernelArg(mKernel[0],  3, sharedSize, NULL);
        err |= clSetKernelArg(mKernel[0],  4, sizeof(int), (int*)&mNumParticules);
        assert(err == CL_SUCCESS);
      }
      clFinish(mCmdQueue);
      initGravityHolesForOpenCL();
    }
  }
}

//----------------------------------------------------------------------------
void ParticuleSystem::initializeParticule(unsigned int iIndex)
{
	mpPositions[iIndex] = getEmitterPosition().getX();
  mpPositions[iIndex+1] = getEmitterPosition().getY();
  mpPositions[iIndex+2] = getEmitterPosition().getZ();
  mpPositions[iIndex+3] = 1.0;
  
  Vector3f v(rand() * (rand()% 2 == 0 ? 1.0 : -1.0),
    rand() * (rand() % 2 == 0 ? 1.0 : -1.0),
    /*rand() * (rand() % 2 == 0 ? 1.0 : -1.0)*/0.0);
  v = v.normalise() * (getMaximumInitialVelocity() > 0 ? rand() % getMaximumInitialVelocity() : 0.0);
  mpVelocities[iIndex] = v.getX();
  mpVelocities[iIndex+1] = v.getY();
  mpVelocities[iIndex+2] = 0.0; //v.getZ();
  mpVelocities[iIndex+2] = 1.0;
  
  mpLife[iIndex/4] = getMaximumInitialLife() > 0 ? rand() % (int)getMaximumInitialLife() : 0;
}

//----------------------------------------------------------------------------
void ParticuleSystem::initializeOpenCL()
{
	cl_int err = 0;
	size_t returned_size = 0;
	
#pragma mark Device Information
	{
    
    size_t returned_size;
    // Find the CPU CL device, as a fallback
  	if(getComputingMode() == cmOpenCLCPU)
    {    	
  		err = clGetDeviceIDs(NULL, CL_DEVICE_TYPE_CPU, 1, &mDevice, NULL);      	  	
    }
    else if(getComputingMode() == cmOpenCLGPU)
    {
      // Find the GPU CL device, this is what we really want
      // If there is no GPU device is CL capable, fall back to CPU
      err = clGetDeviceIDs(NULL, CL_DEVICE_TYPE_GPU, 1, &mDevice, NULL);
    }
		
		if(mDevice)
    {
      // Get some information about the returned device
      cl_char vendor_name[1024] = {0};
      cl_char device_name[1024] = {0};
      err |= clGetDeviceInfo(mDevice, CL_DEVICE_VENDOR, sizeof(vendor_name), 
                  vendor_name, &returned_size);
      err |= clGetDeviceInfo(mDevice, CL_DEVICE_NAME, sizeof(device_name), 
                  device_name, &returned_size);      
      assert(err == CL_SUCCESS);
      printf("Connecting to %s %s...\n", vendor_name, device_name);
      //get some info about the device
    unsigned int compute_units = 0;
    err |= clGetDeviceInfo(mDevice, CL_DEVICE_MAX_COMPUTE_UNITS, sizeof(unsigned int), &compute_units, &returned_size);
    err |= clGetDeviceInfo(mDevice, CL_DEVICE_MAX_WORK_GROUP_SIZE, sizeof(size_t), &mMaxWorkGroupSize, &returned_size);
    err |= clGetDeviceInfo(mDevice, CL_DEVICE_LOCAL_MEM_SIZE, sizeof(cl_ulong), &mLocalMemorySize, NULL);    
    printf("Max compute unit: %lu\n",compute_units);
    printf("Max workgroup size: %lu\n",mMaxWorkGroupSize);
    printf("local memory size: %lu\n",mLocalMemorySize); 
    }
	}
  
#pragma mark Context and Command Queue
	{
		// Now create a context to perform our calculation with the 
		// specified device 
		mContext = clCreateContext(0, 1, &mDevice, NULL, NULL, &err);
		assert(err == CL_SUCCESS);
		
		// And also a command queue for the context
		mCmdQueue = clCreateCommandQueue(mContext, mDevice, 0, NULL);
	}
  
#pragma mark Program and Kernel Creation
	{
		// Load the program source from disk
		// The kernel/program is the project directory and in Xcode the executable
		// is set to launch from that directory hence we use a relative path
    QFile programFile;
    if(getComputingMode() == cmOpenCLCPU)
    	programFile.setFileName(":/particuleSystemCPU_2D.cl");
    else if(getComputingMode() == cmOpenCLGPU)
      programFile.setFileName(":/particuleSystemGPU.cl");
        
    programFile.open(QIODevice::ReadOnly);
    QString progString = programFile.readAll();
    std::string s = progString.toStdString();
    const char* d = s.c_str();
    mProgram[0] = clCreateProgramWithSource(mContext, 1,
      (const char**)&d, NULL, &err);
		assert(err == CL_SUCCESS);
		
		err = clBuildProgram(mProgram[0], 0, NULL, NULL, NULL, NULL);
    
    size_t length = 0;
    char buildLog[2000];
    err |= clGetProgramBuildInfo(mProgram[0], mDevice, CL_PROGRAM_BUILD_LOG, 2000, buildLog, &length);
    fprintf(stderr, "Build Log for Device[%d]:\n%s\n", 0, buildLog);

		assert(err == CL_SUCCESS);
		
		// Now create the kernel "objects" that we want to use in the example file 
		mKernel[0] = clCreateKernel(mProgram[0], "iterate", &err);
    
    //get some info about the kernel
    err |= clGetKernelWorkGroupInfo(mKernel[0], mDevice, CL_KERNEL_WORK_GROUP_SIZE, sizeof(size_t), &mWorkGroupSize, NULL);
    printf("Recommended workgroup Size: %lu\n",mWorkGroupSize);

    assert(err == CL_SUCCESS);

	}
}

//----------------------------------------------------------------------------
//intervale en ms.
void ParticuleSystem::iterate()
{
	//early out
	if(getNumberOfParticules() == 0)
    return;
    
  if(getState() == sStoped)
    return;
	
  float deltaTsec = mIterationTimer.elapsed() / 1000.0;
  mIterationTimer.start();
	
	if(getComputingMode() == cmCPU)
  {
  	#pragma omp parallel for
    for(unsigned int i = 0; i < getNumberOfParticules(); i+=4)
    {
      //on modifie la position des particules
      mpPositions[i] = mpPositions[i] + mpVelocities[i] * deltaTsec;
      mpPositions[i+1] = mpPositions[i+1] + mpVelocities[i+1] * deltaTsec;
      //mpPositions[i+2] = mpPositions[i+2] + mpVelocities[i+2] * deltaTsec;
      
      //on applique les puits de gravité
      /*F = ma et la masse de chaque particule est 1 donc F = a, les puit de 
      gravité applique un force en direction de leur centre qui est inversement
      proportionel a la distance a la particule en question.*/
      math::Vector3d force(0.0);
      for(unsigned int j = 0; j < getGravityHoles().size(); ++j)
      {
        force.setXYZ(getGravityHoles()[j].mPos.getX() - mpPositions[i],
          getGravityHoles()[j].mPos.getY() - mpPositions[i+1],
          /*getGravityHoles()[j].mPos.getZ() - mpPositions[i+2]*/0.0);
        force = force.normalise() * (1.0 / force.fastNorm() * getGravityHoles()[j].mForce);
        
        //puisque f = a
        mpVelocities[i] += force.getX() * deltaTsec;
        mpVelocities[i+1] += force.getY() * deltaTsec;
        //mpVelocities[i+2] += force.getZ() * deltaTsec; 
      }
      
      //decrease life
      if(isDecayEnabled())
        mpLife[i/4] -= 1;
      if(mpLife[i/4] <= 0)
        initializeParticule(i);
    }
  }
  else
  {
#pragma mark Execution and Read
    {
      //CL_INVALID_PROGRAM
    	cl_int err = 0;
      err |= clSetKernelArg(mKernel[0],  7, sizeof(float), (void*)&deltaTsec);
      
      // Run the calculation by enqueuing it and forcing the 
      // command queue to complete the task
      size_t global_work_size = getNumberOfParticules();
      err = clEnqueueNDRangeKernel(mCmdQueue, mKernel[0], 1, NULL, 
                     &global_work_size, &mWorkGroupSize, 0, NULL, NULL);
      assert(err == CL_SUCCESS);
      
      size_t size = getNumberOfParticules() * 4 * sizeof(float);
      // Once finished read back the results from the answer 
      // array into the results array
      err |= clEnqueueReadBuffer(mCmdQueue, mpCLPositions, CL_FALSE, 0, size, 
				mpPositions, 0, NULL, NULL);

      assert(err == CL_SUCCESS);
      clFinish(mCmdQueue);
    }
  }
}

//----------------------------------------------------------------------------
void ParticuleSystem::removeGravityHole(unsigned int iIndex)
{
	mGravityHoles.erase(mGravityHoles.begin() + iIndex);
  if(mGravityHoles.empty())
  	addGravityHole(Point3d(0.0), 0);
  initGravityHolesForOpenCL();
}

//----------------------------------------------------------------------------
void ParticuleSystem::setComputingMode(ComputingMode iM)
{
	switch (iM) 
  {
    case cmCPU: mComputingMode = iM; mWorkGroupSize = 1; break;
    case cmOpenCLCPU:
    case cmOpenCLGPU:
      mComputingMode = iM;
      mWorkGroupSize = 0;
      terminateOpenCL(); initializeOpenCL(); initialize();
      break;
    default: break;
  }
}

//----------------------------------------------------------------------------
void ParticuleSystem::setEmitterPosition(const math::Point3d& p)
{mEmitterPosition = p;}

//----------------------------------------------------------------------------
void ParticuleSystem::setGravityHoleForce(unsigned int iIndex, float iF)
{
  mGravityHoles[iIndex].mForce = iF;
  initGravityHolesForOpenCL();
}

//----------------------------------------------------------------------------
void ParticuleSystem::setGravityHolePosition(unsigned int iIndex,
  const math::Point3d& iPos)
{
  mGravityHoles[iIndex].mPos = iPos;
  initGravityHolesForOpenCL();
}

//----------------------------------------------------------------------------
void ParticuleSystem::setMaximumInitialLife(unsigned int iV)
{ mMaximumInitialLife = iV; }

//----------------------------------------------------------------------------
void ParticuleSystem::setMaximumInitialVelocity(unsigned int iV)
{ mMaximumInitialVelocity = iV; }

//----------------------------------------------------------------------------
void ParticuleSystem::setNumberOfParticules(unsigned int i)
{ mNumParticules = i; }

//----------------------------------------------------------------------------
void ParticuleSystem::start()
{
	setState(sStarted);
  mIterationTimer.start();
}

//----------------------------------------------------------------------------
void ParticuleSystem::stop()
{ setState(sStoped); }

//----------------------------------------------------------------------------
void ParticuleSystem::terminateOpenCL()
{
#pragma mark Teardown
	{
		clReleaseMemObject(mpCLPositions);
		clReleaseMemObject(mpCLVelocities);
		clReleaseMemObject(mpCLLife);
    clReleaseMemObject(mpCLGravityHoles);
		clReleaseCommandQueue(mCmdQueue);
    clReleaseKernel(mKernel[0]);
		clReleaseContext(mContext);
    
    mpCLPositions = 0; mpCLVelocities = 0; mpCLLife = 0;
    mpCLGravityHoles = 0;
    mCmdQueue = 0;
    mContext = 0;
	}
}