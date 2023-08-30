#include "AsioDriverManager.h"

AsioDriverManager* AsioDriverManager::instance = nullptr;

AsioDriverManager::AsioDriverManager()
{
    AsioDriverList drvlist;
    driver_init_info.resize(drvlist.numdrv);
    drivers.resize(drvlist.numdrv);

    asiodrvstruct *ite = drvlist.lpdrvlist;
    for (int i = 0; i < drvlist.numdrv; i++)
    {
        driver_init_info[i] = AsioDriverInitInfo (*ite);
        ite = ite->next;
    }
    CoInitialize(0);
}

AsioDriverManager::~AsioDriverManager()
{
    for(int i=0; i<drivers.size();i++)
    {
        if (drivers[i])
        {
            drivers[i]->DisposeBuffers();
            CloseAsioDriver(i);
        }
    }
    CoUninitialize();
}

AsioDriver* AsioDriverManager::OpenAsioDriver(int driver_index)
{
    if(drivers[driver_index]) return drivers[driver_index];

    AsioDriver* result = new AsioDriver;
    long rc;
    rc = CoCreateInstance(driver_init_info[driver_index].clsid, 0, CLSCTX_INPROC_SERVER, driver_init_info[driver_index].clsid, (void**)(&result->driver));
    if (!rc == S_OK) return nullptr;

    drivers[driver_index] = result;
    return drivers[driver_index];
}

void AsioDriverManager::CloseAsioDriver(int driver_index)
{
    if(!drivers[driver_index]) return;
    drivers[driver_index]->DisposeBuffers();
    drivers[driver_index]->driver->Release();
    drivers[driver_index] = nullptr;
}