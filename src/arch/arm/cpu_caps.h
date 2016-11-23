#pragma once


#if defined(__ARM_ARCH_6A__)
  #define __CONFIG_ARM_CP__
  #define __CONFIG_ARM_CPSR__
  #define __CONFIG_ARM_NO_xPSR__
  #define __CONFIG_ARM_NO_PRIMASK__
  #define __CONFIG_ARM_CLREX__
  #define __CONFIG_ARM_A__
#endif
#if defined(__ARM_ARCH_7A__)
  #define __CONFIG_ARM_CP__
  #define __CONFIG_ARM_CPSR__
  #define __CONFIG_ARM_NO_xPSR__
  #define __CONFIG_ARM_NO_PRIMASK__
  #define __CONFIG_ARM_CLREX__
  #define __CONFIG_ARM_A__
#endif
#if defined(__ARM_ARCH_6M__)
  #define __CONFIG_ARM_NO_CP__
  #define __CONFIG_ARM_NO_CPSR__
  #define __CONFIG_ARM_xPSR__
  #define __CONFIG_ARM_PRIMASK__
  #define __CONFIG_ARM_NO_CLREX__
  #define __CONFIG_ARM_M__
#endif

#if !defined(__CONFIG_ARM_CP__) && !defined(__CONFIG_ARM_NO_CP__)
  #error Cant determine if ARM CPU has a co-processor
#endif

#if !defined(__CONFIG_ARM_CPSR__) && !defined(__CONFIG_ARM_NO_CPSR__)
  #error Cant determine if ARM CPU has a special register CPSR
#endif

#if !defined(__CONFIG_ARM_xPSR__) && !defined(__CONFIG_ARM_NO_xPSR__)
  #error Cant determine if ARM CPU has a special register xPSR
#endif

#if !defined(__CONFIG_ARM_PRIMASK__) && !defined(__CONFIG_ARM_NO_PRIMASK__)
  #error Cant determine if ARM CPU has a special register PRIMASK
#endif

#if !defined(__CONFIG_ARM_CLREX__) && !defined(__CONFIG_ARM_NO_CLREX__)
  #error Cant determine if ARM CPU has clrex family of instructions.
#endif

#if !defined(__CONFIG_ARM_A__) && !defined(__CONFIG_ARM_M__)
#error Cant determine if ARM CPU profile (A/M)
#endif

