// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "Modules/ModuleManager.h"

/**
 * @brief AGPS plugin module.
 *
 * Implements the module lifecycle hooks for the AGPS plugin. Use StartupModule
 * to register editor extensions, commands, and other initialization logic, and
 * ShutdownModule to unregister and clean up resources when the module is unloaded.
 */
class FAGPSModule : public IModuleInterface
{
public:
    /**
     * @brief Called when the module is loaded into memory.
     *
     * Initialize plugin features, register menus, commands or editor extensions here.
     */
    virtual void StartupModule() override;

    /**
     * @brief Called before the module is unloaded.
     *
     * Clean up and unregister anything created in StartupModule to avoid leaks or dangling callbacks.
     */
    virtual void ShutdownModule() override;
};


// class FAGPSModule : public IModuleInterface
// {
// public:
//
// 	virtual void StartupModule() override;
// 	virtual void ShutdownModule() override;
// };
