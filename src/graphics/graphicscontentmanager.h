/*
 * Copyright 2011 Scott MacDonald
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 * http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */
#ifndef SCOTT_HAILSTORM_GRAPHICS_CONTENT_MANAGER_H
#define SCOTT_HAILSTORM_GRAPHICS_CONTENT_MANAGER_H

#include <boost/utility.hpp>
#include <string>

#include "graphics/meshfactory.h"

// Forward declarations
struct ID3D10Device;

/**
 * Manages the creation, loading and unloading of graphical assets in the game.
 */
class GraphicsContentManager
{
public:
    GraphicsContentManager( ID3D10Device * pRenderDevice,
                            const std::string& mContentDir );
    ~GraphicsContentManager();

    // Get a reference to the mesh factory that constructs meshes on the fly
    MeshFactory& meshFactory();

private:
    MeshFactory mMeshFactory;
    std::string mContentDir;
};

#endif