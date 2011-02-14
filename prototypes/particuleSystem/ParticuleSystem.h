
#ifndef Realisim_openCL_Particule_System_hh
#define Realisim_openCL_Particule_System_hh

#include "math/Point.h"
#include <OpenCL/OpenCL.h>
#include <vector>
#include <QColor>
#include <QTime>

/* 
*/
namespace realisim
{
namespace openCL
{

class ParticuleSystem
{
public:
  ParticuleSystem();
  ParticuleSystem(const ParticuleSystem&);
  virtual ~ParticuleSystem();
  virtual ParticuleSystem& operator=(const ParticuleSystem&);
  
  enum ComputingMode
  { cmCPU, cmOpenCLCPU, cmOpenCLGPU };
  
  struct GravityHole
  {
  	GravityHole(const math::Point3d& iP, float iF) : mPos(iP), mForce(iF) {}
  	math::Point3d mPos;
    float mForce;
  };

	virtual void addGravityHole(const math::Point3d&, float);
  virtual const QColor& getColor() const {return mColor;} 
  virtual const ComputingMode getComputingMode() const {return mComputingMode;}
  virtual const math::Point3d& getEmitterPosition() const;
  virtual const std::vector<GravityHole>& getGravityHoles() const;
  virtual unsigned int getMaximumInitialLife() const;
  virtual unsigned int getMaximumInitialVelocity() const;
  virtual unsigned int getNumberOfParticules() const;
  virtual const float* getParticulesPosition() const;
  virtual void initialize();
  virtual void initializeOpenCL();
  virtual void iterate();
  virtual bool isDecayEnabled() const {return mIsDecayEnabled;}
  virtual void removeGravityHole(unsigned int);
  virtual void setColor(QColor iC) {mColor = iC;}
  virtual void setComputingMode(ComputingMode);
  virtual void setDecayEnabled(bool i) {mIsDecayEnabled = i;}
  virtual void setEmitterPosition(const math::Point3d&);
  virtual void setGravityHolePosition(unsigned int, const math::Point3d&);
  virtual void setGravityHoleForce(unsigned int, float);
  virtual void setMaximumInitialVelocity(unsigned int);
  virtual void setMaximumInitialLife(unsigned int);
	virtual void setNumberOfParticules(unsigned int);
  virtual void terminateOpenCL();
  virtual void start();
  virtual void stop();

protected:
	enum State{sStarted, sStoped};
  
  virtual const State getState() const{return mState;}
	virtual void initializeParticule(unsigned int);
  virtual void initGravityHolesForOpenCL();
  virtual void setState(State iS) {mState = iS;}

	ComputingMode mComputingMode;
	QColor mColor;
  bool mIsDecayEnabled;
  unsigned int mNumParticules;
  math::Point3d mEmitterPosition;
  float* mpPositions;
  float* mpVelocities;
  int* mpLife;
  unsigned int mMaximumInitialVelocity; // m/s
  unsigned int mMaximumInitialLife;
  std::vector<GravityHole> mGravityHoles;
  float* mpGravityHoles;
  QTime mIterationTimer;
  State mState;
  
  //openCL stuff
  size_t mMaxWorkGroupSize;
  size_t mWorkGroupSize;
  cl_ulong mLocalMemorySize;
  cl_program mProgram[1];
	cl_kernel mKernel[1];
	cl_command_queue mCmdQueue;
	cl_context mContext;
	cl_device_id mDevice;
  cl_mem mpCLPositions;
  cl_mem mpCLVelocities;
  cl_mem mpCLLife;
  cl_mem mpCLGravityHoles;
};

}//treeD
}//realisim

#endif