#pragma once

#include "AsioCommon.h"
#include "AsioDriver.h"
#include "windows.h"

#include <memory>
#include <map>

class AsioDriverManager
{
    static AsioDriverManager *instance;
    static HANDLE asio_driver_semaphore;
public:
    // Only one instance of AsioDriverManager is allowed.
    static AsioDriverManager &Instance() {
        return instance ? (*instance) : *(new AsioDriverManager());
    }

    // Info required to start all ASIO drivers present get stored here.
    std::vector<AsioDriverInitInfo> driver_init_info;

    // Only one COM object of one ASIO driver is allowed
    std::vector<AsioDriver*> drivers;

    /*
        Probes all ASIO drivers.
        Initializes COM host if there are any ASIO driver.
        Saves the ASIO driver list and mark all ASIO drivers unused.
    */
    AsioDriverManager();

    //Cleanup any open ASIO driver and close stop COM hosting.
    ~AsioDriverManager();

    /*
        Creates COM object of specified ASIO driver.
        (variant of AsioDriverList::asioOpenDriver)
    */
    AsioDriver* OpenAsioDriver(int driver_index);
    
    void CloseAsioDriver(int driver_index);
};
