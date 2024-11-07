#pragma once

#ifdef DLL_EXPORT
#define DLL_API __declspec(dllexport)
#else
#define DLL_API __declspec(dllimport)
#endif

template <typename T> class Singleton
{
  public:
    static T &GetInstance()
    {
        static T instance;
        return instance;
    }

    virtual ~Singleton()
    {
        // print debug info
    }

    Singleton(const Singleton &) = delete;
    Singleton &operator=(const Singleton &) = delete;

  protected:
    Singleton()
    {
        // print debug info
    }
};