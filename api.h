#ifndef HLC_API_H
#define HLC_API_H

#ifdef __cplusplus
  #define HLC_DECLARATIONS_BEGIN extern "C" {
  #define HLC_DECLARATIONS_END }
#else
  #define HLC_DECLARATIONS_BEGIN
  #define HLC_DECLARATIONS_END
#endif

#ifdef HLC_EXPORTS
  #if defined(__GNUC__)
    #define HLC_API __attribute__((visibility("default")))
  #elif defined(_MSC_VER)
    #define HLC_API __declspec(dllexport)
  #else
    #define HLC_API
  #endif
#else
  #if defined(__GNUC__)
    #define HLC_API
  #elif defined(_MSC_VER)
    #define HLC_API __declspec(dllimport)
  #else
    #define HLC_API
  #endif
#endif

#endif
