# Reconfigurable Device Access Interface (RDAI)

RDAI is a unified specification for accessing hardware accelerators on reconfigurable devices. Most RDAI-enabled environments will require 3 things in order to run accelerators:
- The RDAI API
- A RDAI Host Runtime
- A RDAI Platform Runtime

## RDAI API

The RDAI API provides data structures and function call specifications to create a unified hardware access interface. The files providing the RDAI API are:
- `rdai_types.h`
- `rdai_api.h`

### Execution Model
The execution model targeted by this interface is host-directed execution with parallel accelerator devices.
The host program runs on a CPU and manages the loading and offloading of accelerated tasks to/from attached accelerator devices.
Accelerator devices are configured within a hardware platform. A hardware platform can an FPGA device, a CGRA device, a CPU-emulated hardware platform, etc.
A runtime is provided in order for the host code/application to interface with a hardware platform. This runtime is called `host runtime`. 
Each supported hardware platform will:
- provide an implementation of required APIs (called `RDAI_PlatformOps`)
- have to be registered with the host runtime before the platform can be used

### Memory Model

The memory model consists of 3 defined address spaces:
- host address space: memory assumed to be visible only from the host program
- device address space: memory assumed to be visible only from an accelerator device
- shared address space: memory visible to both host and devices

Data transfer between two memory objects is carried by an address-space-aware API call.

## RDAI Host Runtime

The RDAI host runtime provides a thin indirection layer between the application-facing RDAI API and an RDAI platform runtime. The host runtime is responsible for the following:
- performing some sanity checks on API calls
- manage host and shared memory objects
- redirect API calls to proper platform runtimes

## RDAI Platform Runtime

This is the runtime that supports RDAI APIs for a particular hardware platform type.

## RDAI Project Folder Layout
- folder: rdai_api
- folder: host_runtimes: this contains host runtimes for different host environments (example: halide_linux)
- folder: platform_runtimes: this contains platform-specific runtimes (example: xilinx_fpga, aha_garnet, aha_clockwork_tb, unified_host_runtime, etc.)

## What's in a Host Runtime?
A host runtime interfaces with two domains:
- the host application through RDAI APIs
- backend hardware abstraction layers: drivers, wrappers, etc.

For backends that utilize an OS driver to communicate with underlying hardware runtimes, we want to specify a unified driver interface so that we can standardize host runtimes (i.e. FPGA and CGRA devices can use the same host runtime and driver)
