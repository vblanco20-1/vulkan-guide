
#include <nvcore/Library.h>

#include <cuda.h>
#include <stdio.h>

// Typedefs
typedef CUresult (CUDAAPI * cuInitPtr)( unsigned int Flags );	

typedef CUresult  (CUDAAPI * cuDeviceGetPtr)(CUdevice *device, int ordinal);
typedef CUresult  (CUDAAPI * cuDeviceGetCountPtr)(int *count);
typedef CUresult  (CUDAAPI * cuDeviceGetNamePtr)(char *name, int len, CUdevice dev);
typedef CUresult  (CUDAAPI * cuDeviceComputeCapabilityPtr)(int *major, int *minor, CUdevice dev);
typedef CUresult  (CUDAAPI * cuDeviceTotalMemPtr)(unsigned int *bytes, CUdevice dev);
typedef CUresult  (CUDAAPI * cuDeviceGetPropertiesPtr)(CUdevprop *prop, CUdevice dev);
typedef CUresult  (CUDAAPI * cuDeviceGetAttributePtr)(int *pi, CUdevice_attribute attrib, CUdevice dev);

typedef CUresult  (CUDAAPI * cuCtxCreatePtr)(CUcontext *pctx, unsigned int flags, CUdevice dev );
typedef CUresult  (CUDAAPI * cuCtxDestroyPtr)( CUcontext ctx );
typedef CUresult  (CUDAAPI * cuCtxAttachPtr)(CUcontext *pctx, unsigned int flags);
typedef CUresult  (CUDAAPI * cuCtxDetachPtr)(CUcontext ctx);
typedef CUresult  (CUDAAPI * cuCtxPushCurrentPtr)( CUcontext ctx );
typedef CUresult  (CUDAAPI * cuCtxPopCurrentPtr)( CUcontext *pctx );
typedef CUresult  (CUDAAPI * cuCtxGetDevicePtr)(CUdevice *device);
typedef CUresult  (CUDAAPI * cuCtxSynchronizePtr)(void);


// A compressor inits CUDA and creates a context for each device.
// 

struct CudaDevice
{
	CUdevice device;
	CUcontext context;
};

struct CudaContext
{
	CudaContext()
	{
		printf("CudaContext()\n");

	#if NV_OS_WIN32
		Library nvcuda("nvcuda.dll");
	#else
		Library nvcuda(NV_LIBRARY_NAME(cuda));
	#endif

		cuInit = (cuInitPtr)nvcuda.bindSymbol("cuInit");

		cuDeviceGet = (cuDeviceGetPtr)nvcuda.bindSymbol("cuDeviceGet");
		cuDeviceGetCount = (cuDeviceGetCountPtr)nvcuda.bindSymbol("cuDeviceGetCount");
		cuDeviceGetName = (cuDeviceGetNamePtr)nvcuda.bindSymbol("cuDeviceGetName");
		cuDeviceComputeCapability = (cuDeviceComputeCapabilityPtr)nvcuda.bindSymbol("cuDeviceComputeCapability");
		cuDeviceTotalMem = (cuDeviceTotalMemPtr)nvcuda.bindSymbol("cuDeviceTotalMem");
		cuDeviceGetProperties = (cuDeviceGetPropertiesPtr)nvcuda.bindSymbol("cuDeviceGetProperties");
		cuDeviceGetAttribute = (cuDeviceGetAttributePtr)nvcuda.bindSymbol("cuDeviceGetAttribute");
		
		cuCtxCreate = (cuCtxCreatePtr)nvcuda.bindSymbol("cuCtxCreate");
		cuCtxDestroy = (cuCtxDestroyPtr)nvcuda.bindSymbol("cuCtxDestroy");
		cuCtxAttach = (cuCtxAttachPtr)nvcuda.bindSymbol("cuCtxAttach");
		cuCtxDetach = (cuCtxDetachPtr)nvcuda.bindSymbol("cuCtxDetach");
		cuCtxPushCurrent = (cuCtxPushCurrentPtr)nvcuda.bindSymbol("cuCtxPushCurrent");
		cuCtxPopCurrent = (cuCtxPopCurrentPtr)nvcuda.bindSymbol("cuCtxPopCurrent");
		cuCtxGetDevice = (cuCtxGetDevicePtr)nvcuda.bindSymbol("cuCtxGetDevice");
		cuCtxSynchronize = (cuCtxSynchronizePtr)nvcuda.bindSymbol("cuCtxSynchronize");
	
		CUresult status = cuInit(0);
	
		if (status == CUDA_SUCCESS)
		{
			printf("cuInit succeeded.\n");
		}
		
		m_deviceCount = 0;
		cuDeviceGetCount(&m_deviceCount);
		
		printf("%d devices found.\n", m_deviceCount);
		
		if (m_deviceCount > 0)
		{
			m_devices = new CudaDevice[m_deviceCount];
			
			uint flags = CU_CTX_SCHED_AUTO;
			if (m_deviceCount > 1) flags = CU_CTX_SCHED_YIELD;
			
			for (int i = 0; i < m_deviceCount; i++)
			{
				cuDeviceGet(&m_devices[i].device, i);
				cuCtxCreate(&m_devices[i].context, flags, m_devices[i].device);
			
				cuCtxDestroy(m_devices[i].context);
			}
			
		}
	}
	
	~CudaContext()
	{
		printf("~CudaContext()\n");

		if (m_deviceCount > 0)
		{
			for (int i = 0; i < m_deviceCount; i++)
			{
				cuCtxDestroy(m_devices[i].context);
			}		

			delete [] m_devices;
		}
	}
	

public:	
	cuInitPtr cuInit;

	cuDeviceGetPtr cuDeviceGet;
	cuDeviceGetCountPtr cuDeviceGetCount;
	cuDeviceGetNamePtr cuDeviceGetName;
	cuDeviceComputeCapabilityPtr cuDeviceComputeCapability;
	cuDeviceTotalMemPtr cuDeviceTotalMem;
	cuDeviceGetPropertiesPtr cuDeviceGetProperties;
	cuDeviceGetAttributePtr cuDeviceGetAttribute;

	cuCtxCreatePtr cuCtxCreate;
	cuCtxDestroyPtr cuCtxDestroy;
	cuCtxAttachPtr cuCtxAttach;
	cuCtxDetachPtr cuCtxDetach;
	cuCtxPushCurrentPtr cuCtxPushCurrent;
	cuCtxPopCurrentPtr  cuCtxPopCurrent;
	cuCtxGetDevicePtr cuCtxGetDevice;
	cuCtxSynchronizePtr cuCtxSynchronize;

	int m_deviceCount;
	CudaDevice * m_devices;
};

int main(void)
{
	CudaContext ctx;
//	cuInit(0);

	return 0;
}
